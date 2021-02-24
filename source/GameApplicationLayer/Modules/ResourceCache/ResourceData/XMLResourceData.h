#pragma once

#include <pch.h>

#include "IResourceData.h"

namespace ResourceData
{
    class XML : public IResourceData
    {
        tinyxml2::XMLDocument m_XMLDocument;

    public:
        std::string GetName() override { return "XMLResourceData"; }
        void Parse(char* buffer, unsigned int size) override;

        tinyxml2::XMLElement* GetRoot(void) { return m_XMLDocument.RootElement(); }
    };
}