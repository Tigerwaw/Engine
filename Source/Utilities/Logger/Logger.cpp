#include "Logger.h"
#pragma region WindowsIncludes
#define	WIN32_LEAN_AND_MEAN

#define NOGDICAPMASKS     
#define NOVIRTUALKEYCODES 
#define NOWINMESSAGES     
#define NOWINSTYLES       
#define NOSYSMETRICS      
#define NOMENUS           
#define NOICONS           
#define NOKEYSTATES       
#define NOSYSCOMMANDS     
#define NORASTEROPS       
#define NOSHOWWINDOW      
#define OEMRESOURCE       
#define NOATOM            
#define NOCLIPBOARD       
#define NOCOLOR           
#define NOCTLMGR          
#define NODRAWTEXT        
#define NOGDI             
#define NOKERNEL          
//#define NOUSER            
//#define NONLS - Required for CP_ACP and WideCharToMultiByte
#define NOMB              
#define NOMEMMGR          
#define NOMETAFILE        
#define NOMINMAX          
#define NOMSG             
#define NOOPENFILE        
#define NOSCROLL          
#define NOSERVICE         
#define NOSOUND           
#define NOTEXTMETRIC      
#define NOWH              
#define NOWINOFFSETS      
#define NOCOMM            
#define NOKANJI           
#define NOHELP            
#define NOPROFILER        
#define NODEFERWINDOWPOS  
#define NOMCX
#include <Windows.h>  
#pragma endregion

#include <fstream>
#include <chrono>
#include <iostream>

Logger::LogStream::~LogStream()
{
	if(File.is_open())
	{
		File.flush();
		File.close();
	}
}

Logger::LogCategoryBase::LogCategoryBase(const std::string& aName, LogVerbosity::Type aVerbosity): Name(aName), Verbosity(aVerbosity)
{  }

Logger::Logger()
	: myStdErrHandle(GetStdHandle(STD_ERROR_HANDLE)), myIsRunning(true)
{
	myLogThread = std::thread(&Logger::Heartbeat, this);
}

Logger::~Logger()
{
	myIsRunning = false;
	myLogThread.join();
}

std::string Logger::Timestamp(bool aIncludeDate /*= false*/) const
{
	static std::string dateFormat = "%Y-%m-%d %H:%M:%S";
	static std::string noDateFormat = "%H:%M:%S";

	const std::chrono::time_point now = std::chrono::system_clock::now();
	const std::time_t time = std::chrono::system_clock::to_time_t(now);

	struct tm timeInfo{};
	const int error = localtime_s(&timeInfo, &time);
	error;

	char buffer[20]{};
	const size_t wcsTimeErr = strftime(buffer, 20, aIncludeDate ? dateFormat.c_str() : noDateFormat.c_str(), &timeInfo);
	wcsTimeErr;
	return buffer;
}

void Logger::LogIntl(const LogCategoryBase& aCategory, LogVerbosity::Type aVerbosity, const char* aMessage)
{
	// [hh:mm:ss][   LOG   ][ModelViewer] ModelViewer starting...
	SetConsoleTextAttribute(myStdErrHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);	

	myStream << "[" << Timestamp() << "]";

	switch(aVerbosity)
	{
	case LogVerbosity::None:
		break;
	case LogVerbosity::Fatal:
	case LogVerbosity::Error:
		{
			SetConsoleTextAttribute(myStdErrHandle, BACKGROUND_RED);
			myStream << "[  ERROR  ]";
			SetConsoleTextAttribute(myStdErrHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
		}
		break;
	case LogVerbosity::Warning:
		{
			SetConsoleTextAttribute(myStdErrHandle, BACKGROUND_RED | BACKGROUND_GREEN);
			myStream << "[ WARNING ]";
			SetConsoleTextAttribute(myStdErrHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		}
		break;
	case LogVerbosity::Log:
	case LogVerbosity::Verbose:
		{
			SetConsoleTextAttribute(myStdErrHandle, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
			myStream << "[   LOG   ]";
			SetConsoleTextAttribute(myStdErrHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		}
		break;
	}

	myStream << "[" << aCategory.Name << "]";

	myStream << " " << aMessage << LogStream::endl;

	SetConsoleTextAttribute(myStdErrHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void Logger::Heartbeat()
{
	char imagePath[MAX_PATH]{};
	GetModuleFileNameA(NULL, imagePath, MAX_PATH);

	std::string imageFileName(imagePath);
	imageFileName.shrink_to_fit();

	const size_t lastSlashPos = imageFileName.find_last_of('\\');
	std::string inFileNameOnly = imageFileName;
	if(lastSlashPos != std::string::npos)
	{
		inFileNameOnly = inFileNameOnly.substr(lastSlashPos + 1);
	}

	inFileNameOnly = inFileNameOnly.substr(0, inFileNameOnly.size() - 4);

	myLogFilePath = LOGGING_PATH;
	if(!myLogFilePath.has_filename())
	{
		TCHAR exeFileName[MAX_PATH];
		GetModuleFileName(NULL, exeFileName, MAX_PATH);
		const std::filesystem::path exePath = exeFileName;
		myLogFilePath = myLogFilePath / (exePath.stem().string() + ".log");
	}

	const std::string timeStamp = Timestamp(true);

	myStream.File = std::ofstream(myLogFilePath, std::ios_base::app);
	myStream.File << std::string(100, '*') << std::endl;
	myStream.File << "Logging has started at " << timeStamp << "." << std::endl;
	myStream.File << std::string(100, '*') << std::endl;

	while(myIsRunning.load())
	{
		while(!myLogQueue.empty())
		{
			std::lock_guard<std::mutex> logLock(myLogMutex);
			const LogEntry& entry = myLogQueue.front();
			LogIntl(*entry.Category, entry.Verbosity, entry.Message.c_str());
			myLogQueue.pop();
		}

		std::this_thread::yield();
	}
}

void Logger::Log(const LogCategoryBase& aCategory, LogVerbosity::Type aVerbosity, const char* aMessage)
{
	if(aCategory.Verbosity >= aVerbosity)
	{
		LogEntry entry;
		entry.Category = &aCategory;
		entry.Verbosity = aVerbosity;
		entry.Message = aMessage;

		std::lock_guard<std::mutex> logLock(Get().myLogMutex);
		Get().myLogQueue.emplace(std::move(entry));
	}
}

void Logger::Flush()
{
	Get().myStream.File.flush();
}