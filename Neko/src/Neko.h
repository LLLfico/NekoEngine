#pragma once

// for use by neko applications
#include "core/Core.h"
#include "core/Assert.h"
#include "core/Application.h"
#include "core/Layer.h"
#include "core/Log.h"
#include "core/TimeStep.h"
#include "core/imgui/ImGuiLayer.h"

#include "core/Input.h"
#include "core/KeyCodes.h"
#include "core/MouseCodes.h"

// render
#include "core/renderer/Renderer.h"
#include "core/renderer/Renderer2D.h"
#include "core/renderer/RenderCommand.h"
#include "core/renderer/Buffer.h"
#include "core/renderer/FrameBuffer.h"
#include "core/renderer/VertexArray.h"
#include "core/renderer/Shader.h"
#include "core/renderer/Texture.h"
#include "core/renderer/Camera.h"
#include "core/renderer/EditorCamera.h"

#include "core/CameraController.h"

// ecs
#include "world/Scene.h"
#include "world/Entity.h"
#include "world/Component.h"
#include "world/ScriptableEntity.h"

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Entry point