#include "pch.h"
#include "Material.h"

namespace Neko {

	void Material::SetAlbedoMap(std::shared_ptr<Texture2D> texture) {
		m_useAlbedoMap = true;
		m_albedoMap = texture;
	}

}