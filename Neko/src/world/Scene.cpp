#include "pch.h"
#include "Scene.h"

#include "Entity.h"
#include "Component.h"
#include "ScriptableEntity.h"

#include "core/UUID.h"
#include "core/renderer/Shader.h"
#include "core/renderer/Texture.h"
#include "core/renderer/Renderer2D.h"
#include "core/renderer/Renderer3D.h"
#include "core/renderer/RenderCommand.h"
#include "core/renderer/FrameBuffer.h"

#include "core/renderer/VertexArray.h"
#include "core/renderer/Buffer.h"

#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

#include <glad/glad.h>

namespace Neko {

	static std::unordered_map<Rigidbody2DComponent::BodyType, b2BodyType> s_nekoBodyTypeBoxBodyTypeMap = {
		{Rigidbody2DComponent::BodyType::Static, b2_staticBody},
		{Rigidbody2DComponent::BodyType::Dynamic, b2_dynamicBody},
		{Rigidbody2DComponent::BodyType::Kinematic, b2_kinematicBody},
	};

	Scene::Scene() {
		m_depthShader = Shader::Create("assets/shaders/ShaderDepth.glsl");
		m_testShader = Shader::Create("assets/shaders/DebugDepth.glsl");
		m_texture = Texture2D::Create("assets/textures/testpic.png");

		float vertices[] = {
			-0.5f, -0.5f,   0.0f, 0.0f,
			 0.5f, -0.5f,   1.0f, 0.0f,
			 0.5f,  0.5f,   1.0f, 1.0f,
			-0.5f,  0.5f,   0.0f, 1.0f,
		};
		uint32_t indices[] = {
			 0, 1, 2,
			 2, 3, 0,
		};
		m_vao = VertexArray::Create();
		m_vbo = VertexBuffer::Create(vertices, sizeof(vertices));
		m_vao->Bind();
		m_vbo->SetLayout({
			{"a_position", ShaderDataType::Float2},
			{"a_texcoord", ShaderDataType::Float2},
		});
		m_vao->AddVertexBuffer(m_vbo);
		m_ibo = IndexBuffer::Create(indices, sizeof(indices));
		m_vao->SetIndexBuffer(m_ibo);

		m_testShader->Bind();
		m_testShader->SetInt("u_shadowMap", 0);
	}

	Scene::~Scene() {
	}

	template<typename Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap) {
		auto view = src.view<Component>();
		for (auto e : view) {
			UUID uuid = src.get<IDComponent>(e).id;
			NEKO_CORE_ASSERT(enttMap.count(uuid));
			entt::entity dstId = enttMap.at(uuid);

			auto& component = src.get<Component>(e);
			dst.emplace_or_replace<Component>(dstId, component);
		}
	}

	template<typename Component>
	static void CopyComponentIfExists(Entity dst, Entity src) {
		if (src.HasComponent<Component>())
			dst.AddorReplaceComponent<Component>(src.GetComponent<Component>());
	}

	std::shared_ptr<Scene> Scene::CopyFrom(std::shared_ptr<Scene> other) {
		auto newScene = std::make_shared<Scene>();

		newScene->m_viewportWidth = other->m_viewportWidth;
		newScene->m_viewportHeight = other->m_viewportHeight;

		auto& srcSceneRegistry = other->m_registry;
		auto& dstSceneRegistry = newScene->m_registry;

		std::unordered_map<UUID, entt::entity> enttMap;

		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView) {
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).id;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CircleCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<MeshComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<DirectionalLightComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<PointLightComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name) {
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID id, const std::string& name) {
		Entity entity = { m_registry.create(), this };
		entity.AddComponent<IDComponent>(id);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.tag = name.empty() ? "Entity" : name;
		return entity;
		return Entity();
	}

	void Scene::DestroyEntity(Entity entity) {
		m_registry.destroy(entity);
	}

	void Scene::OnRuntimeStart() {
		OnPhysics2DStart();
	}

	void Scene::OnRuntimeStop() {
		OnPhysics2DStop();
	}

	void Scene::OnSimulationStart() {
		OnPhysics2DStart();
	}

	void Scene::OnSimulationStop() {
		OnPhysics2DStop();
	}

	void Scene::OnShadow(EditorCamera& camera) {
		constexpr uint32_t shadowMapSlot = 8;
		// shadow map
		
		auto view = m_registry.view<TransformComponent, DirectionalLightComponent>();
		for (auto entity : view) {
			auto [transform, light] = view.get<TransformComponent, DirectionalLightComponent>(entity);
			auto position = transform.translation;
			glm::vec3 lightDir = glm::normalize(glm::mat3(transform.GetTransformMatrix()) * glm::vec3(0.0f, 0.0f, -1.0f));
			auto focal = position + lightDir;
			auto view = glm::lookAt(position, focal, glm::vec3(0.0f, 1.0f, 0.0f));
			// auto projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, camera.GetNearPlane(), camera.GetFarPlane());
			auto projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
			auto viewProj = projection * view;
			m_depthShader->Bind();
			m_depthShader->SetMat4("u_viewProjection", viewProj);
			break;
		}
		Renderer3D::s_shadowFbo->Bind();
		Neko::RenderCommand::SetClearColor({ 0.2f, 0.3f, 0.3f, 1.0f });
		Neko::RenderCommand::Clear();
		Renderer3D::BeginScene(camera);
		m_depthShader->Bind();
		auto meshview = m_registry.view<TransformComponent, MeshComponent>();
		for (auto entity : meshview) {
			auto [transform, mesh] = meshview.get<TransformComponent, MeshComponent>(entity);
			mesh.mesh->Draw(transform.GetTransformMatrix(), m_depthShader, (int)entity);
		}
		Renderer3D::EndScene();
		Renderer3D::s_shadowFbo->Unbind();
	}

	void Scene::OnUpdateRuntime(TimeStep dt) {
		// scripts
		{
			m_registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
				if (!nsc.instance) {
					nsc.instance = nsc.InstantiateScript();
					nsc.instance->m_entity = Entity{ entity, this };
					nsc.instance->OnCreate();
				}

				nsc.instance->OnUpdate(dt);
			});
		}

		// render 2d
		Projection* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			// find main camera
			auto view = m_registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view) {
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.primary) {
					mainCamera = &camera.camera;
					cameraTransform = transform.GetTransformMatrix();
					break;
				}
			}
		}

		if (mainCamera) {
			Renderer2D::BeginScene(*mainCamera, cameraTransform);

			auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group) {
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawSprite(transform.GetTransformMatrix(), sprite, (int)entity);
			}

			auto view = m_registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view) {
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(transform.GetTransformMatrix(), circle.color, circle.thickness, circle.fade, (int)entity);
			}

			Renderer2D::EndScene();
		}

	}

	void Scene::OnUpdateSimulation(TimeStep dt, EditorCamera& camera) {
		// physics
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_physicsWorld->Step(dt, velocityIterations, positionIterations);

			auto view = m_registry.view<Rigidbody2DComponent>();
			for (auto e : view) {
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

				b2Body* body = (b2Body*)rb2d.runtimeBody;
				const auto& position = body->GetPosition();
				transform.translation.x = position.x;
				transform.translation.y = position.y;
				transform.rotation.z = body->GetAngle();
			}
		}

		RenderScene(camera);
	}

	void Scene::OnUpdateEditor(TimeStep dt, EditorCamera& camera) {
		RenderScene(camera);
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height) {
		m_viewportWidth = width;
		m_viewportHeight = height;

		auto view = m_registry.view<CameraComponent>();
		for (auto entity : view) {
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.fixedAspectRatio) {
				cameraComponent.camera.SetViewportSize(width, height);
			}
		}
	}

	void Scene::DuplicateEntity(Entity entity) {
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);

		CopyComponentIfExists<TransformComponent>(newEntity, entity);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CircleRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CameraComponent>(newEntity, entity);
		CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
		CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<CircleCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<MeshComponent>(newEntity, entity);
		CopyComponentIfExists<DirectionalLightComponent>(newEntity, entity);
		CopyComponentIfExists<PointLightComponent>(newEntity, entity);
	}

	Entity Scene::GetPrimaryCameraEntity() {
		auto view = m_registry.view<CameraComponent>();
		for (auto entity : view) {
			if (view.get<CameraComponent>(entity).primary) {
				return Entity{ entity, this };
			}
		}
		return {};
	}

	void Scene::OnPhysics2DStart() {
		m_physicsWorld = std::make_shared<b2World>(b2Vec2{ 0.0f, -9.8f });

		auto view = m_registry.view<Rigidbody2DComponent>();
		for (auto e : view) {
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = s_nekoBodyTypeBoxBodyTypeMap[rb2d.type];
			bodyDef.position.Set(transform.translation.x, transform.translation.y);
			bodyDef.angle = transform.rotation.z;

			b2Body* body = m_physicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.fixedRotation);
			rb2d.runtimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>()) {
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.size.x * transform.scale.x, bc2d.size.y * transform.scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.density;
				fixtureDef.friction = bc2d.friction;
				fixtureDef.restitution = bc2d.restitution;
				fixtureDef.restitutionThreshold = bc2d.restitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<CircleCollider2DComponent>()) {
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.offset.x, cc2d.offset.y);
				circleShape.m_radius = transform.scale.x * cc2d.radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.density;
				fixtureDef.friction = cc2d.friction;
				fixtureDef.restitution = cc2d.restitution;
				fixtureDef.restitutionThreshold = cc2d.restitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnPhysics2DStop() {
		m_physicsWorld = nullptr;
	}

	void Scene::RenderScene(EditorCamera& camera) {

		{
			// depth map debug
			m_vao->Bind();
			m_testShader->Bind();
			m_testShader->SetInt("u_shadowMap", 8);
			// Renderer3D::s_shadowFbo->BindDepthTexture(8);
			glBindTextureUnit(8, Renderer3D::s_shadowFbo->GetDepthAttachmentId());
			// glBindTextureUnit(8, Renderer3D::s_shadowFbo->GetColorAttachmentId(0));
			// m_texture->Bind(8);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		}
		Renderer2D::BeginScene(camera);
		{
			// sprite / quad
			auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group) {
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawSprite(transform.GetTransformMatrix(), sprite, (int)entity);
			}
		}
		{
			// circle
			auto view = m_registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view) {
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(transform.GetTransformMatrix(), circle.color, circle.thickness, circle.fade, (int)entity);
			}
		}
		// Renderer2D::DrawLine(glm::vec3(0.0f), glm::vec3(5.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
		Renderer2D::EndScene();

		Renderer3D::BeginScene(camera);
		{
			// directional light
			{
				auto view = m_registry.view<TransformComponent, DirectionalLightComponent>();
				for (auto entity : view) {
					auto [transform, light] = view.get<TransformComponent, DirectionalLightComponent>(entity);
					glm::vec3 lightDir = glm::mat3(transform.GetTransformMatrix()) * glm::vec3(0.0f, 0.0f, -1.0f);
					// Renderer3D::SetDirectionalLight(glm::normalize(glm::eulerAngles(glm::quat(transform.rotation))), light.radiance);
					Renderer3D::SetDirectionalLight(glm::normalize(lightDir), light.radiance);
					break;
				}
			}
			// point lights
			{
				auto view = m_registry.view<TransformComponent, PointLightComponent>();
				int i = 0;
				for (auto entity : view) {
					auto [transform, light] = view.get<TransformComponent, PointLightComponent>(entity);
					Renderer3D::SetPointLight(transform.translation, light.radiance, i);
					i++;
				}
				Renderer3D::SetPointLightNum(i);
			}

			// mesh3d
			auto view = m_registry.view<TransformComponent, MeshComponent>();
			for (auto entity : view) {
				auto [transform, mesh] = view.get<TransformComponent, MeshComponent>(entity);
				Renderer3D::DrawMesh(transform.GetTransformMatrix(), mesh.mesh, (int)entity);
			}
			//for (auto entity : view) {
			//	auto [transform, mesh] = view.get<TransformComponent, MeshComponent>(entity);
			//	mesh.mesh->Draw(transform.GetTransformMatrix(), m_depthShader, (int)entity);
			//}
		}
		Renderer3D::EndScene();
		
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component) {
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component) {
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component) {

	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component) {
		component.camera.SetViewportSize(m_viewportWidth, m_viewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component) {

	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component) {

	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component) {
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component) {
	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component) {
	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component) {
	}

	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component) {
	}

	template<>
	void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component) {
	}

	template<>
	void Scene::OnComponentAdded<DirectionalLightComponent>(Entity entity, DirectionalLightComponent& component) {
	}

	template<>
	void Scene::OnComponentAdded<PointLightComponent>(Entity entity, PointLightComponent& component) {
	}
}