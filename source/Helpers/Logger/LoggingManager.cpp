#include "LoggingManager.h"

#include <Helpers/Functions.h>
#include <Helpers/Settings/Settings.h>

constexpr auto LOGFLAG_WRITE_TO_LOG_FILE = 1 << 0;
constexpr auto LOGFLAG_WRITE_TO_DEBUGGER = 1 << 1;

LoggingManager::~LoggingManager(void)
{
	m_messengerCriticalSection.Lock();

	for (auto messenger : m_errorMessengers)
		delete messenger;
	m_errorMessengers.clear();

	m_messengerCriticalSection.Unlock();
}

void LoggingManager::Init()
{
	auto root = Settings::Tag("Logging");

	// set display flags for each tag
	for (auto node = root->FirstChildElement(); node; node = node->NextSiblingElement())
	{
		unsigned char flags = 0;
		std::string tag = node->Attribute("tag");

		if (node->Attribute("debugger", "1"))
			flags |= LOGFLAG_WRITE_TO_DEBUGGER;

		if (node->Attribute("file", "1"))
			flags |= LOGFLAG_WRITE_TO_LOG_FILE;

		SetDisplayFlags(tag, flags);
	}

	_Output = root->Attribute("output");

	// init output file - either keep data (a+) or reset it (w+)
	if (root->Attribute("mode", "clear"))
		ClearFile(_Output.c_str());
}

void LoggingManager::SetDisplayFlags(const std::string& tag, unsigned char flags)
{
	m_tagCriticalSection.Lock();

	if (flags != 0)
	{
		auto findIt = m_tags.find(tag);

		// if tag doesn't exist in map, create it; Else update it
		if (findIt == m_tags.end())
			m_tags.insert(std::make_pair(tag, flags));
		else
			findIt->second = flags;
	}
	else
	{
		m_tags.erase(tag);
	}

	m_tagCriticalSection.Unlock();
}

//------------------------------------------------------------------------------------------------------------------------------------
// This function builds up the log string and outputs it to various places based on the display flags (m_displayFlags).
//------------------------------------------------------------------------------------------------------------------------------------
void LoggingManager::Log(
	const std::string tag,
	const std::string message,
	const std::string funcName,
	const std::string sourceFile,
	unsigned int lineNum
)
{
	m_tagCriticalSection.Lock();

	auto findIt = m_tags.find(tag);
	auto tagExists = findIt != m_tags.end();

	m_tagCriticalSection.Unlock();

	// TODO: if INFO enabled, log everything
	if (tagExists || MustLogEverything())
	{
		std::string buffer;
		GetOutputBuffer(buffer, tag, message, funcName, sourceFile, lineNum);
		OutputFinalBufferToLogs(buffer, findIt->second);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------
// Fills outOutputBuffer with the find error string.
//------------------------------------------------------------------------------------------------------------------------------------
void LoggingManager::GetOutputBuffer(
	std::string& outOutputBuffer,
	const std::string& tag,
	const std::string& message,
	const std::string& funcName,
	const std::string& sourceFile,
	unsigned int lineNum
)
{
	if (!tag.empty())
		outOutputBuffer = "[" + tag + "] " + message;
	else
		outOutputBuffer = message;

	if (funcName != "")
		outOutputBuffer += "\nFunction: " + funcName;

	if (sourceFile != "")
		outOutputBuffer += "\nFunction: " + sourceFile;

	if (lineNum != 0)
		outOutputBuffer += "\nLine: " + std::to_string(lineNum);

	outOutputBuffer += "\n";
}

//------------------------------------------------------------------------------------------------------------------------------------
// This is a helper function that checks all the display flags and outputs the passed in finalBuffer to the appropriate places.
// 
// IMPORTANT: The two places this function is called from wrap the code in the tag critical section (m_pTagCriticalSection), 
// so that makes this call thread safe.  If you call this from anywhere else, make sure you wrap it in that critical section.
//------------------------------------------------------------------------------------------------------------------------------------
void LoggingManager::OutputFinalBufferToLogs(const std::string& finalBuffer, unsigned char flags)
{
	// Write the log to each display based on the display flags
	if ((flags & LOGFLAG_WRITE_TO_LOG_FILE) > 0)  // log file
		WriteToLogFile(finalBuffer);
	if ((flags & LOGFLAG_WRITE_TO_DEBUGGER) > 0)  // debugger output window
		OutputDebugStringA(finalBuffer.c_str());
}

void LoggingManager::WriteToLogFile(const std::string& data)
{
	FILE* pLogFile = NULL;
	fopen_s(&pLogFile, _Output.c_str(), "a+");

	if (!pLogFile)
		return;  // can't write to the log file for some reason

	fprintf_s(pLogFile, data.c_str());

	fclose(pLogFile);
}

//------------------------------------------------------------------------------------------------------------------------------------
// Adds an error messenger to the list
//------------------------------------------------------------------------------------------------------------------------------------
void LoggingManager::AddErrorMessenger(Logger::ErrorMessenger* pMessenger)
{
	m_messengerCriticalSection.Lock();
	m_errorMessengers.push_back(pMessenger);
	m_messengerCriticalSection.Unlock();
}

//------------------------------------------------------------------------------------------------------------------------------------
// Helper for ErrorMessenger.
//------------------------------------------------------------------------------------------------------------------------------------
LoggingManager::ErrorDialogResult LoggingManager::Error(
	const std::string& errorMessage,
	bool isFatal,
	const char* funcName,
	const char* sourceFile,
	unsigned int lineNum
)
{
	std::string tag = isFatal ? "FATAL" : "ERROR";

	// buffer for our final output string
	std::string buffer;
	GetOutputBuffer(buffer, tag, errorMessage, funcName, sourceFile, lineNum);

	m_tagCriticalSection.Lock();

	// write the final buffer to all the various logs
	auto findIt = m_tags.find(tag);
	if (findIt != m_tags.end())
		OutputFinalBufferToLogs(buffer, findIt->second);

	m_tagCriticalSection.Unlock();

	// show the dialog box
	int result = MessageBoxA(NULL, buffer.c_str(), tag.c_str(), MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_DEFBUTTON3);

	// act upon the choice
	switch (result)
	{
		case IDIGNORE:
			return ErrorDialogResult::LOGMGR_ERROR_IGNORE;

		case IDABORT:
			__debugbreak(); // assembly language instruction to break into the debugger
			return ErrorDialogResult::LOGMGR_ERROR_RETRY;

		case IDRETRY:
			return ErrorDialogResult::LOGMGR_ERROR_RETRY;

		default:
			return ErrorDialogResult::LOGMGR_ERROR_RETRY;
	}
}