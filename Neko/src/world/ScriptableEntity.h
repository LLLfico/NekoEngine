#pragma once
#include "Entity.h"

namespace Neko {

	class ScriptableEntity {
		friend class Scene;
	public:
		virtual ~ScriptableEntity() {}
		
		template<typename T>
		T& GetComponent() {
			return m_entity.GetComponent<T>();
		}
	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(TimeStep dt) {}
	private:
		Entity m_entity;
	};

}