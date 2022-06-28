#pragma once

#include "core/Core.h"

namespace Neko {

	enum class ShaderDataType {
		None = 0,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool,
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type) {

		static std::map<ShaderDataType, int> m = {
			{ShaderDataType::Float, 4}, {ShaderDataType::Float2, 4*2}, {ShaderDataType::Float3, 4*3}, {ShaderDataType::Float4, 4*4},
			{ShaderDataType::Mat3, 4*3*3}, {ShaderDataType::Mat4, 4*4*4},
			{ShaderDataType::Int, 4}, {ShaderDataType::Int2, 4*2}, {ShaderDataType::Int3, 4*3}, {ShaderDataType::Int4, 4*4},
			{ShaderDataType::Bool, 1},
		};

		if (m.count(type)) {
			return m[type];
		}

		NEKO_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement {
		std::string name;
		ShaderDataType type;
		uint32_t size;
		uint32_t offset;
		bool normalized;

		BufferElement() = default;
		BufferElement(const std::string& _name, ShaderDataType _type, bool _normalized = false) :
			name(_name), type(_type), size(ShaderDataTypeSize(type)), offset(0), normalized(_normalized) {}

		uint32_t Count() const {

			static std::map<ShaderDataType, int> m = {
				{ShaderDataType::Float, 1}, {ShaderDataType::Float2, 2}, {ShaderDataType::Float3, 3}, {ShaderDataType::Float4, 4},
				{ShaderDataType::Mat3, 3 * 3}, {ShaderDataType::Mat4, 4 * 4},
				{ShaderDataType::Int, 1}, {ShaderDataType::Int2, 2}, {ShaderDataType::Int3, 3}, {ShaderDataType::Int4, 4},
				{ShaderDataType::Bool, 1},
			};

			if (m.count(type)) {
				return m[type];
			}

			NEKO_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout {
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& elements) : m_elements(elements) {
			CaculateOffsetAndStride();
		}

		inline uint32_t GetStride() const { return m_stride; }
		inline const std::vector<BufferElement>& GetElements() const { return m_elements; }

		// ranging
		std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); }

	private:
		void CaculateOffsetAndStride() {
			uint32_t offset = 0;
			m_stride = 0;
			for (auto& element : m_elements) {
				element.offset = offset;
				offset += element.size;
				m_stride += element.size;
			}
		}
	private:
		std::vector<BufferElement> m_elements;
		uint32_t m_stride = 0;
	};

	class NEKO_API VertexBuffer {
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		static VertexBuffer* Create(float* vertices, uint32_t size);
	};

	class NEKO_API IndexBuffer {
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t Count() const = 0;

		static IndexBuffer* Create(unsigned int* indices, uint32_t size);
	};

}