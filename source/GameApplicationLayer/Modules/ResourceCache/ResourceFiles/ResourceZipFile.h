#pragma once

#include <Helpers/ZipFile/ZipFile.h>

#include "IResourceFile.h"

class ResourceZipFile : public IResourceFile
{
	ZipFile* m_pZipFile;
	std::wstring m_resFileName;

	std::optional<int> GetResourceId(const Resource& r) const;

public:
	ResourceZipFile(const std::wstring resFileName);
	~ResourceZipFile();

	bool Open() override;
	std::string GetResourceFilename(const int id) const override;
	int GetRawResourceSize(const Resource& r) override;
	bool GetRawResource(const Resource& r, char* buffer) override;
	int GetNumResources() const override;

	// TODO: implement this option
	bool IsUsingDevelopmentDirectories(void) const override { return false; }
};