#include "Resource.h"

#include <helpers.h>

Resource::Resource(const std::string &name)
{
	m_name = Convert::StringToLower(name);
}
