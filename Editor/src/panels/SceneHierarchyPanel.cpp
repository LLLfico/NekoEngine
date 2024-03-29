#include "pch.h"
#include "SceneHierarchyPanel.h"

#include "core/Core.h"
#include "core/utils/PlatformUtils.h"
#include "world/Scene.h"
#include "world/Component.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Neko {

	extern const std::filesystem::path g_assetPath;

	SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene>& scene) {
		SetContext(scene);
	}

	void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene>& scene) {
		m_context = scene;
		m_selectionContext = {};
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity) {
		m_selectionContext = entity;
	}

	void SceneHierarchyPanel::OnImGuiRender() {
		ImGui::Begin("Scene Hierarchy");

		if (m_context) {
			m_context->m_registry.each([&](auto entityId) {
				Entity entity{ entityId, m_context.get() };
				DrawEntityMode(entity);
				});
			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_selectionContext = {};

			if (ImGui::BeginPopupContextWindow(0, 1, false)) {
				if (ImGui::MenuItem("Create Empty Entity"))
					m_context->CreateEntity("Empty Entity");

				ImGui::EndPopup();
			}
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_selectionContext) {
			DrawComponents(m_selectionContext);
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityMode(Entity entity) {
		auto& tag = entity.GetComponent<TagComponent>().tag;

		ImGuiTreeNodeFlags flags = ((m_selectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked()) {
			m_selectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;
			ImGui::EndPopup();
		}

		if (opened) {
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9817237, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}
		if (entityDeleted) {
			m_context->DestroyEntity(entity);
			if (m_selectionContext == entity)
				m_selectionContext = {};
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f) {
		// draw translation,rotation,scale UI, and typesetting
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.5f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);

		if (ImGui::Button("X", buttonSize)) {
			values.x = resetValue;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);

		if (ImGui::Button("Y", buttonSize)) {
			values.y = resetValue;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize)) {
			values.z = resetValue;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uifunction) {
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (!entity.HasComponent<T>()) {
			return;
		}
		auto& component = entity.GetComponent<T>();
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
		ImGui::PopStyleVar();

		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
		if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight })) {
			ImGui::OpenPopup("ComponentSettings");
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup("ComponentSettings")) {
			if (ImGui::MenuItem("Remove Component"))
				removeComponent = true;
			ImGui::EndPopup();
		}
		if (open) {
			uifunction(component);
			ImGui::TreePop();
		}

		if (removeComponent) {
			entity.RemoveComponent<T>();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity) {
		if (entity.HasComponent<TagComponent>()) {
			auto& tag = entity.GetComponent<TagComponent>().tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");
		if (ImGui::BeginPopup("AddComponent")) {
			if (ImGui::MenuItem("Camera")) {
				if (!m_selectionContext.HasComponent<CameraComponent>()) {
					m_selectionContext.AddComponent<CameraComponent>();
				}
				else {
					NEKO_CORE_WARN("Entity already has camera component!");
				}
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Sprit Renderer")) {
				if (!m_selectionContext.HasComponent<SpriteRendererComponent>()) {
					m_selectionContext.AddComponent<SpriteRendererComponent>();
				}
				else {
					NEKO_CORE_WARN("Entity already has sprite renderer component!");
				}
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Circle Renderer")) {
				if (!m_selectionContext.HasComponent<CircleRendererComponent>()) {
					m_selectionContext.AddComponent<CircleRendererComponent>();
				}
				else {
					NEKO_CORE_WARN("Entity already has circle renderer component!");
				}
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Rigidbody 2D")) {
				if (!m_selectionContext.HasComponent<Rigidbody2DComponent>()) {
					m_selectionContext.AddComponent<Rigidbody2DComponent>();
				}
				else {
					NEKO_CORE_WARN("Entity already has rigidbody 2d component!");
				}
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Box Collider 2D")) {
				if (!m_selectionContext.HasComponent<BoxCollider2DComponent>()) {
					m_selectionContext.AddComponent<BoxCollider2DComponent>();
				}
				else {
					NEKO_CORE_WARN("Entity already has box collider 2d component!");
				}
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Circle Collider 2D")) {
				if (!m_selectionContext.HasComponent<CircleCollider2DComponent>()) {
					m_selectionContext.AddComponent<CircleCollider2DComponent>();
				}
				else {
					NEKO_CORE_WARN("Entity already has circle collider 2d component!");
				}
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Mesh")) {
				if (!m_selectionContext.HasComponent<MeshComponent>()) {
					m_selectionContext.AddComponent<MeshComponent>();
				}
				else {
					NEKO_CORE_WARN("Entity already has mesh component!");
				}
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Directional Light")) {
				if (!m_selectionContext.HasComponent<DirectionalLightComponent>()) {
					m_selectionContext.AddComponent<DirectionalLightComponent>();
				}
				else {
					NEKO_CORE_WARN("Entity already has directional light component!");
				}
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Point Light")) {
				if (!m_selectionContext.HasComponent<PointLightComponent>()) {
					m_selectionContext.AddComponent<PointLightComponent>();
				}
				else {
					NEKO_CORE_WARN("Entity already has point light component!");
				}
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component) {
			DrawVec3Control("Translation", component.translation);

			auto rotation = glm::degrees(component.rotation);
			DrawVec3Control("Rotation", rotation);
			component.rotation = glm::radians(rotation);

			DrawVec3Control("Scale", component.scale);
			});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component) {
			auto& camera = component.camera;

			ImGui::Checkbox("Primary", &component.primary);

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
			if (ImGui::BeginCombo("Projection", currentProjectionTypeString)) {
				for (int i = 0; i < (int)SceneCamera::ProjectionType::Count; i++) {
					bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.SetProjectionType(SceneCamera::ProjectionType(i));
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {
				float fov = glm::degrees(camera.GetPerspectiveFov());
				if (ImGui::DragFloat("FOV", &fov)) {
					camera.SetPerspectiveFov(glm::radians(fov));
				}

				float zNear = camera.GetPerspectiveNearClip();
				if (ImGui::DragFloat("zNear", &zNear)) {
					camera.SetPerspectiveNearClip(zNear);
				}

				float zFar = camera.GetPerspectiveFarClip();
				if (ImGui::DragFloat("zFar", &zFar)) {
					camera.SetPerspectiveFarClip(zFar);
				}
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic) {
				float orthoSize = camera.GetOrthographicSize();
				if (ImGui::DragFloat("OrthoSize", &orthoSize)) {
					camera.SetOrthographicSize(orthoSize);
				}

				float zNear = camera.GetOrthographicNearClip();
				if (ImGui::DragFloat("zNear", &zNear)) {
					camera.SetOrthographicNearClip(zNear);
				}

				float zFar = camera.GetOrthographicFarClip();
				if (ImGui::DragFloat("zFar", &zFar)) {
					camera.SetOrthographicFarClip(zFar);
				}

				ImGui::Checkbox("Fixed Aspect Ratio", &component.fixedAspectRatio);
			}
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component) {
			ImGui::ColorEdit4("Color", glm::value_ptr(component.color));

			ImGui::Button("Texture", ImVec2{ 100.0f, 0.0f });
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
					const wchar_t* path = (const wchar_t*)payload->Data;
					auto texturePath = g_assetPath / path;
					auto texture = Texture2D::Create(texturePath.string());
					if (texture->IsLoaded())
						component.texture = texture;
					else
						NEKO_WARN("Could not load texture {0}", texturePath.filename().string());
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::DragFloat("Tiling Factor", &component.tilingFactor, 0.1f, 0.0f, 100.0f);
			});

		DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component) {
			ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
			ImGui::DragFloat("Thickness", &component.thickness, 0.025, 0.0f, 1.0f);
			ImGui::DragFloat("Fade", &component.fade, 0.00025, 0.0f, 1.0f);
			});

		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component) {
			const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeStrings[(int)component.type];
			if (ImGui::BeginCombo("Body Type", currentBodyTypeString)) {
				for (int i = 0; i < 2; i++) {
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
					if (ImGui::Selectable(bodyTypeStrings[i], isSelected)) {
						currentBodyTypeString = bodyTypeStrings[i];
						component.type = Rigidbody2DComponent::BodyType(i);
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::Checkbox("Fixed Rotation", &component.fixedRotation);
			});

		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component) {
			ImGui::DragFloat2("Offset", glm::value_ptr(component.offset));
			ImGui::DragFloat2("Size", glm::value_ptr(component.size));
			ImGui::DragFloat("Density", &component.density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.restitutionThreshold, 0.01f, 0.0f);
			});

		DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component) {
			ImGui::DragFloat2("Offset", glm::value_ptr(component.offset));
			ImGui::DragFloat("Radius", &component.radius);
			ImGui::DragFloat("Density", &component.density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.restitutionThreshold, 0.01f, 0.0f);
			});

		DrawComponent<MeshComponent>("Mesh", entity, [](auto& component) {
			ImGui::Columns(2, nullptr, false);
			ImGui::SetColumnWidth(0, 100.0f);
			ImGui::Text("Mesh Path");
			ImGui::NextColumn();

			ImGui::Button("Mesh", ImVec2{ 100.0f, 0.0f });
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
					const wchar_t* path = (const wchar_t*)payload->Data;
					auto meshPath = g_assetPath / path;
					component.mesh = std::make_shared<Mesh>(meshPath.string());
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::SameLine();
			if (ImGui::Button("...")) {
				auto file = FileDialogs::OpenFile("Model (*.obj *.fbx)\0");
				auto filepath = *file;
				if (filepath.find("assets") != std::string::npos) {
					filepath = filepath.substr(filepath.find("assets"));
				}
				else {
					// TODO
					NEKO_CORE_ASSERT(false, "Not support import model from other directory currently");
					return;
				}
				component.mesh = std::make_shared<Mesh>(filepath);
			}
			ImGui::EndColumns();

			auto& materials = component.mesh->GetMaterialsRef();
			for (size_t materialIndex = 0; materialIndex < component.mesh->GetSubMeshNum(); materialIndex++) {
				std::string label = std::string("Material") + std::to_string(materialIndex);
				ImGui::PushID(label.c_str());
				if (ImGui::TreeNode((void*)label.c_str(), label.c_str())) {
					{
						// albedo
						std::string nodeName = "Albedo";
						std::string nodeLabel = nodeName + std::to_string(materialIndex);
						ImGui::PushID(nodeLabel.c_str());

						if (ImGui::TreeNode((void*)nodeName.c_str(), nodeName.c_str())) {
							ImGui::SameLine();
							ImGui::Checkbox("UseMap", &materials[materialIndex].m_useAlbedoMap);
							if (materials[materialIndex].m_useAlbedoMap) {
								ImGui::Image((ImTextureID)materials[materialIndex].m_albedoMap->GetId(), ImVec2{ 64, 64 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
								if (ImGui::BeginDragDropTarget()) {
									if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
										const wchar_t* path = (const wchar_t*)payload->Data;
										auto texturePath = g_assetPath / path;
										auto texture = TextureLibrary::GetInstance().Load(texturePath.string());
										materials[materialIndex].m_albedoMap = texture;
									}
									ImGui::EndDragDropTarget();
								}
							}
							else {
								if (ImGui::ColorEdit4("##albedo", glm::value_ptr(materials[materialIndex].m_color))) {
									unsigned char color[4];
									for (size_t i = 0; i < 4; i++)
										color[i] = (unsigned char)(materials[materialIndex].m_color[i] * 255.0f);
									materials[materialIndex].m_albedoSprite->SetData(color, 4 * sizeof(unsigned char));
								}
							}
							ImGui::TreePop();
						}
						ImGui::PopID();
					}

					{
						// metallic 
						std::string nodeName = "Metallic";
						std::string nodeLabel = nodeName + std::to_string(materialIndex);
						ImGui::PushID(nodeLabel.c_str());

						if (ImGui::TreeNode((void*)nodeName.c_str(), nodeName.c_str())) {
							ImGui::SameLine();
							ImGui::Checkbox("UseMap", &materials[materialIndex].m_useMetallicMap);
							if (materials[materialIndex].m_useMetallicMap) {
								ImGui::Image((ImTextureID)materials[materialIndex].m_metallicMap->GetId(), ImVec2{ 64, 64 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
								if (ImGui::BeginDragDropTarget()) {
									if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
										const wchar_t* path = (const wchar_t*)payload->Data;
										auto texturePath = g_assetPath / path;
										auto texture = TextureLibrary::GetInstance().Load(texturePath.string());
										materials[materialIndex].m_metallicMap = texture;
									}
									ImGui::EndDragDropTarget();
								}
							}
							else {
								if (ImGui::DragFloat("##metallic", &materials[materialIndex].m_metalllic, 0.01f, 0.0f, 1.0f)) {
									unsigned char color[4];
									for (size_t i = 0; i < 3; i++)
									color[i] = (unsigned char)(materials[materialIndex].m_metalllic * 255.0f);
									color[3] = 255.0f;
									materials[materialIndex].m_metallicSprite->SetData(color, 4 * sizeof(unsigned char));
								}
							}
							ImGui::TreePop();
						}
						ImGui::PopID();
					}

					{
						// roughness
						std::string nodeName = "Roughness";
						std::string nodeLabel = nodeName + std::to_string(materialIndex);
						ImGui::PushID(nodeLabel.c_str());

						if (ImGui::TreeNode((void*)nodeName.c_str(), nodeName.c_str())) {
							ImGui::SameLine();
							ImGui::Checkbox("UseMap", &materials[materialIndex].m_useRoughnessMap);
							if (materials[materialIndex].m_useRoughnessMap) {
								ImGui::Image((ImTextureID)materials[materialIndex].m_roughnessMap->GetId(), ImVec2{ 64, 64 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
								if (ImGui::BeginDragDropTarget()) {
									if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
										const wchar_t* path = (const wchar_t*)payload->Data;
										auto texturePath = g_assetPath / path;
										auto texture = TextureLibrary::GetInstance().Load(texturePath.string());
										materials[materialIndex].m_roughnessMap = texture;
									}
									ImGui::EndDragDropTarget();
								}
							}
							else {
								if (ImGui::DragFloat("##roughness", &materials[materialIndex].m_roughness, 0.01f, 0.0f, 1.0f)) {
									unsigned char color[4];
									for (size_t i = 0; i < 3; i++)
										color[i] = (unsigned char)(materials[materialIndex].m_roughness * 255.0f);
									color[3] = 255.0f;
									materials[materialIndex].m_roughnessSprite->SetData(color, 4 * sizeof(unsigned char));
								}
							}
							ImGui::TreePop();
						}
						ImGui::PopID();
					}

					ImGui::TreePop();
				}
				ImGui::PopID();
			}
		});

		DrawComponent<DirectionalLightComponent>("Directional Light", entity, [](auto& component) {
			ImGui::DragFloat3("Radiance", glm::value_ptr(component.radiance));
			});

		DrawComponent<PointLightComponent>("Point Light", entity, [](auto& component) {
			ImGui::DragFloat3("Radiance", glm::value_ptr(component.radiance));
			});
	}

}