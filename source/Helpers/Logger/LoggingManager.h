#pragma once
#include "Logger.h"

#include <pch.h>

#include "../Concurrency/CriticalSection.h"

class LoggingManager
{
	// log helpers
	void OutputFinalBufferToLogs(const std::string& finalBuffer, unsigned char flags);
	void WriteToLogFile(const std::string& data);
	void GetOutputBuffer(std::string& outOutputBuffer, const std::string& tag, const std::string& message, const std::string& funcName, const std::string& sourceFile, unsigned int lineNum);

public:
	enum class ErrorDialogResult
	{
		LOGMGR_ERROR_ABORT,
		LOGMGR_ERROR_RETRY,
		LOGMGR_ERROR_IGNORE
	};

	typedef std::map<std::string, unsigned char> Tags;
	typedef std::list<Logger::ErrorMessenger*> ErrorMessengerList;

	Tags m_tags;
	ErrorMessengerList m_errorMessengers;

	// thread safety
	CriticalSection m_tagCriticalSection;
	CriticalSection m_messengerCriticalSection;

	std::string _Output;

	// construction
	LoggingManager() = default;
	~LoggingManager();
	void Init();

	// conditions
	inline bool MustLogEverything() { return m_tags.find("INFO") != m_tags.end(); }

	// logs
	// TODO: add timestamp
	void SetDisplayFlags(const std::string& tag, unsigned char flags);
	void Log(const std::string tag, const std::string message, const std::string funcName, const std::string sourceFile, unsigned int lineNum);

	// error messengers
	void AddErrorMessenger(Logger::ErrorMessenger* pMessenger);
	ErrorDialogResult Error(const std::string& errorMessage, bool isFatal, const char* funcName, const char* sourceFile, unsigned int lineNum);
};