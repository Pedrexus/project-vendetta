#include "XMLResourceData.h"

#include <macros.h>

void ResourceData::XML::Parse(char* buffer, size size)
{
	if (m_XMLDocument.Parse(buffer, size) != tinyxml2::XML_SUCCESS)
		LOG_ERROR("Unable to parse XML file");
}
