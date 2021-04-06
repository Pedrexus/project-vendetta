#include "Resource.h"

#include <Helpers/Functions.h>

Resource::Resource(const std::string &name)
{
	m_name = Convert::StringToLower(name);
}
