#include "pch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Neko {

	static GLenum OpenGLShaderDataBaseType(ShaderDataType type) {
		static std::map<ShaderDataType, GLenum> m = {
			{ShaderDataType::Float, GL_FLOAT}, {ShaderDataType::Float2, GL_FLOAT}, {ShaderDataType::Float3, GL_FLOAT}, {ShaderDataType::Float4, GL_FLOAT},
			{ShaderDataType::Mat3, GL_FLOAT}, {ShaderDataType::Mat4, GL_FLOAT},
			{ShaderDataType::Int, GL_INT}, {ShaderDataType::Int2, GL_INT}, {ShaderDataType::Int3, GL_INT}, {ShaderDataType::Int4, GL_INT},
			{ShaderDataType::Bool, GL_BOOL},
		};
		if (m.count(type)) {
			return m[type];
		}
		NEKO_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray() {
		glCreateVertexArrays(1, &m_id);
	}

	OpenGLVertexArray::~OpenGLVertexArray() {
		glDeleteVertexArrays(1, &m_id);
	}

	void OpenGLVertexArray::Bind() const {
		glBindVertexArray(m_id);
	}

	void OpenGLVertexArray::Unbind() const {
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
		NEKO_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		this->Bind();
		vertexBuffer->Bind();

		auto& layout = vertexBuffer->GetLayout();
		for (auto& element : layout) {
			switch (element.type) {
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4: {
					glEnableVertexAttribArray(m_vertexArrayIndex);
					glVertexAttribPointer(m_vertexArrayIndex, element.Count(), OpenGLShaderDataBaseType(element.type),
						element.normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)element.offset);
					m_vertexArrayIndex++;
					break;
				}
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Bool: {
					// glVertexAttrib I Pointer用的接口和上面的不一样，上面的输入的int型会被转为浮点型
					glEnableVertexAttribArray(m_vertexArrayIndex);
					glVertexAttribIPointer(m_vertexArrayIndex, element.Count(), OpenGLShaderDataBaseType(element.type),
						layout.GetStride(), (const void*)element.offset);
					m_vertexArrayIndex++;
					break;
				}
				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4: {
					uint8_t count = element.Count();
					for (uint8_t i = 0; i < count; i++) {
						glEnableVertexAttribArray(m_vertexArrayIndex);
						glVertexAttribPointer(m_vertexArrayIndex,
							count,
							OpenGLShaderDataBaseType(element.type),
							element.normalized ? GL_TRUE : GL_FALSE,
							layout.GetStride(),
							(const void*)(element.offset + sizeof(float) * count * i));
						glVertexAttribDivisor(m_vertexArrayIndex, 1);
						m_vertexArrayIndex++;
					}
					break;
				}

			}
		}
		
		m_vertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
		this->Bind();
		indexBuffer->Bind();

		m_indexBuffer = indexBuffer;
	}

}