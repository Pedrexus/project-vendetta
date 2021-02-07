#pragma once

#include <ApplicationLayer/ResourceCache/ResourceFiles/IResourceFile.h>
#include <Helpers/ZipFile/ZipFile.h>

class ResourceZipFile : public IResourceFile
{
	ZipFile* m_pZipFile;
	std::wstring m_resFileName;

public:
	ResourceZipFile(const std::wstring resFileName) { m_pZipFile = NULL; m_resFileName = resFileName; }
	virtual ~ResourceZipFile();

	virtual bool Open();
	virtual int GetRawResourceSize(const Resource& r);
	virtual int GetRawResource(const Resource& r, char* buffer);
	virtual int GetNumResources() const;
	virtual std::string GetResourceName(int num) const;
	virtual bool IsUsingDevelopmentDirectories(void) const { return false; }
};