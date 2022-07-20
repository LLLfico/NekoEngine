#pragma once

#include <xhash>

namespace Neko {

	class UUID {
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_uuid; }
	private:
		uint64_t m_uuid;
	};
}

namespace std {
	template<>
	struct hash<Neko::UUID> {
		std::size_t operator()(const Neko::UUID& uuid) const {
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}