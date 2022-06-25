#pragma once

#include "core/Core.h"

namespace Neko {

	class NEKO_API Shader {
	public:
		Shader(const std::string& vertexDesc, const std::string& fragDesc);
		~Shader();

		void Bind() const;
		void Unbind() const;
	private:
		unsigned int m_id;
	};

}

