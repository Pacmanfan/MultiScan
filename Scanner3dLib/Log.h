
#ifndef LOG_H
#ifdef __cplusplus
extern "C" {
#endif __cplusplus
void Log(char const *fmt, ...);
void SetLogPath(char *path);

#ifdef __cplusplus
};
#endif __cplusplus
/*
#ifndef NDEBUG
#define DLog Log
#else
static inline void DLog(char const * , ...) {};
#endif
*/
#define LOG_H

#endif
