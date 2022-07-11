#include "pch.h"
#include "Entity.h"

namespace Neko {

	Entity::Entity(entt::entity handle, Scene* scene) : m_handle(handle), m_scene(scene) {}

}