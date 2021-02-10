#include "ResourceZipFile.h"

#include <macros.h>

ResourceZipFile::ResourceZipFile(const std::wstring resFileName) :
	m_pZipFile(nullptr),
	m_resFileName(resFileName)
{}

ResourceZipFile::~ResourceZipFile()
{
	SAFE_DELETE(m_pZipFile);
}

bool ResourceZipFile::Open()
{
	m_pZipFile = NEW ZipFile;
	return m_pZipFile ? m_pZipFile->Init(m_resFileName.c_str()) : false;
}

// retrives the resource index in the zip file
std::optional<int> ResourceZipFile::GetResourceId(const Resource& r) const
{
	return m_pZipFile->Find(r.GetName().c_str());
}

int ResourceZipFile::GetRawResourceSize(const Resource& r)
{
	auto id = GetResourceId(r);
	return id.has_value() ? m_pZipFile->GetFileLen(id.value()) : 0;
}

bool ResourceZipFile::GetRawResource(const Resource& r, char* buffer)
{
	auto id = GetResourceId(r);
	return id.has_value() ? m_pZipFile->ReadFile(id.value(), buffer) : false;
}

int ResourceZipFile::GetNumResources() const
{
	return m_pZipFile ? m_pZipFile->GetNumFiles() : 0;
}

std::string ResourceZipFile::GetResourceFilename(const int id) const
{
	return m_pZipFile ? m_pZipFile->GetFilename(id) : std::string();
}
