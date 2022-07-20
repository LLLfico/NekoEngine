#include "pch.h"
#include "UUID.h"

#include <random>

namespace Neko {

	static std::random_device s_randomDevice;
	static std::mt19937_64 s_generator(s_randomDevice());
	static std::uniform_int_distribution<uint64_t> s_uniformDistribution;

	UUID::UUID() : m_uuid(s_uniformDistribution(s_generator)) {}
	UUID::UUID(uint64_t uuid) : m_uuid(uuid) {}
}