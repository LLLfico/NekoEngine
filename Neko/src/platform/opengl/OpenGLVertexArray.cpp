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
		auto& elements = layout.GetElements();
		for (uint32_t i = 0; i < elements.size(); i++) {
			auto& element = elements[i];
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, element.Count(), OpenGLShaderDataBaseType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)element.offset);
		}

		m_vertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
		this->Bind();
		indexBuffer->Bind();

		m_indexBuffer = indexBuffer;
	}

}