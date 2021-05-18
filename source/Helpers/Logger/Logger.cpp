#include "Logger.h"
#include "LoggingManager.h"

#include <macros.h>

// singleton
static LoggingManager* LogMgrSingleton = nullptr;

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

void LogSync(
	const std::string tag,
	const std::string message,
	const std::string funcName = "",
	const std::string sourceFile = "",
	unsigned int lineNum = 0
)
{
	LogMgrSingleton->Log(tag, message, funcName, sourceFile, lineNum);
}

void FailSync(
	const std::string errorMessage,
	bool isFatal,
	const std::string funcName,
	const std::string sourceFile,
	unsigned int lineNum
)
{
	static Logger::ErrorMessenger em;
	em.Show(errorMessage, isFatal, funcName.c_str(), sourceFile.c_str(), lineNum);
}

namespace Logger
{
	void Init()
	{
		if (!LogMgrSingleton)
		{
			LogMgrSingleton = NEW LoggingManager;
			LogMgrSingleton->Init();
		}
		LOG_INFO("Logger initialized");
	}

	void Destroy()
	{
		LogSync("INFO", "Logger destroyed");
		SAFE_DELETE(LogMgrSingleton);
	}

	void Log(
		const std::string tag,
		const std::string message,
		const std::string funcName,
		const std::string sourceFile,
		unsigned int lineNum
	)
	{
		std::thread(LogSync, tag, message, funcName, sourceFile, lineNum).detach();
	}

	void Fail(
		const std::string& errorMessage,
		bool isFatal, 
		const char* funcName, 
		const char* sourceFile, 
		unsigned int lineNum
	)
	{
		std::thread(FailSync, errorMessage, isFatal, funcName, sourceFile, lineNum).detach();
	}

}