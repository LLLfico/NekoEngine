#pragma once

// for use by neko applications
#include "core/Application.h"
#include "core/Layer.h"
#include "core/Log.h"
#include "core/Time.h"
#include "core/imgui/ImGuiLayer.h"

#include "core/Input.h"
#include "core/KeyCodes.h"
#include "core/MouseButtonCodes.h"

// render
#include "core/Renderer/Renderer.h"
#include "core/Renderer/RenderCommand.h"

#include "core/Renderer/Buffer.h"
#include "core/Renderer/VertexArray.h"
#include "core/Renderer/Shader.h"
#include "core/Renderer/Texture.h"

#include "core/Renderer/Camera.h"

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Entry point
#include "core/EntryPoint.h"