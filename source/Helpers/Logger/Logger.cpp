#include "Logger.h"
#include "LoggingManager.h"

#include <macros.h>

// singleton
static LoggingManager* LogMgrSingleton = NULL;

Logger::ErrorMessenger::ErrorMessenger(void)
{
	LogMgrSingleton->AddErrorMessenger(this);
	m_enabled = true;
}

void Logger::ErrorMessenger::Show(const std::string& errorMessage, bool isFatal, const char* funcName, const char* sourceFile, unsigned int lineNum)
{
	if (m_enabled)
	{
		auto result = LogMgrSingleton->Error(errorMessage, isFatal, funcName, sourceFile, lineNum);
		if (result == LoggingManager::ErrorDialogResult::LOGMGR_ERROR_IGNORE)
			m_enabled = false;
	}
}

namespace Logger
{
	void Init(const char* loggingConfigFilename)
	{
		if (!LogMgrSingleton)
		{
			LogMgrSingleton = new LoggingManager;
			LogMgrSingleton->Init(loggingConfigFilename);

			LOG_INFO("Logger initialized");
		}
	}

	void Destroy(void)
	{
		LOG_INFO("Logger destroyed");

		delete LogMgrSingleton;
		LogMgrSingleton = NULL;
	}

	void Log(
		const std::string& tag, 
		const std::string& message, 
		const char* funcName, 
		const char* sourceFile, 
		unsigned int lineNum
	)
	{
		LogMgrSingleton->Log(tag, message, funcName, sourceFile, lineNum);
	}

}