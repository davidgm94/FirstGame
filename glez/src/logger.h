#pragma once
#include "type.h"

#if GLEZ_BUILD_DLL
#define LOG_CONTEXT "[ GLEZ ] "
#else
#define LOG_CONTEXT "[ GAME ] "
#endif

typedef enum LOG_LEVEL {
	LOG_LEVEL_QUIET = 0x00,
	LOG_LEVEL_DEBUG = 0x01,
	LOG_LEVEL_INFO = 0x02,
	LOG_LEVEL_WARN = 0x03,
	LOG_LEVEL_ERROR = 0x04,
	LOG_LEVEL_FATAL = 0x06,
} LOG_LEVEL;

#define LOGS 1
#if LOGS
#define logDebug(...) logger(LOG_CONTEXT, COUNT_OF(LOG_CONTEXT), LOG_LEVEL_DEBUG, __FILE__, COUNT_OF(__FILE__), __LINE__, __VA_ARGS__)
#define logInfo(...)  logger(LOG_CONTEXT, COUNT_OF(LOG_CONTEXT), LOG_LEVEL_INFO, __FILE__,  COUNT_OF(__FILE__), __LINE__, __VA_ARGS__)
#define logWarn(...)  logger(LOG_CONTEXT, COUNT_OF(LOG_CONTEXT), LOG_LEVEL_WARN, __FILE__,  COUNT_OF(__FILE__), __LINE__, __VA_ARGS__)
#define logError(...) logger(LOG_CONTEXT, COUNT_OF(LOG_CONTEXT), LOG_LEVEL_ERROR, __FILE__, COUNT_OF(__FILE__), __LINE__, __VA_ARGS__)
#define logFatal(...) logger(LOG_CONTEXT, COUNT_OF(LOG_CONTEXT), LOG_LEVEL_FATAL, __FILE__, COUNT_OF(__FILE__), __LINE__, __VA_ARGS__)

void initLogger(LOG_LEVEL logLevel);
GLEZ_API void logger(const char* logContext, u32 charCount, LOG_LEVEL level, const char* file, s32 fileCharCount, s32 line, const char* fmt, ...);
char** getPointerToPrintBuffer(void);
const char* getPrintBuffer(void);
#else
#define logDebug(...) 
#define logInfo(...)  
#define logWarn(...)  
#define logError(...) 
#define logFatal(...) 
#endif

