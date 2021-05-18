#pragma once

#include <const.h>
#include <pch.h>


namespace Settings
{
	static tinyxml2::XMLDocument s_document;
	static std::unordered_map<std::string, _bstr_t> s_cache;

	inline bool IsLoaded()
	{
		return s_document.RootElement() != nullptr;
	}

	inline tinyxml2::XMLDocument* Load()
	{
		if (!IsLoaded() && s_document.LoadFile(SETTINGS_FILENAME) != tinyxml2::XML_SUCCESS)
			throw std::exception("Unable to load logging config file");

		return &s_document;
	}

	inline tinyxml2::XMLElement* Tag(const std::string tag)
	{
		auto element = Load()->RootElement();

		while(element != nullptr && element->Name() != tag)
			element = element->FirstChildElement(tag.c_str());

		if (!element)
			throw std::exception("Tag not found");

		return element;
	}

	inline _bstr_t Get(const std::string attr)
	{
		if(!s_cache.contains(attr))
			s_cache[attr] = _bstr_t(Tag("Config")->Attribute(attr.c_str()));
		return s_cache[attr];
	}

	inline i32 GetInt(const std::string attr)
	{
		return std::stoi((char*)Get(attr));
	}
}

