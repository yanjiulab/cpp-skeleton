
#ifndef LIBLOG_H
#define LIBLOG_H
#include <string.h>

#define LIBLOG_VERSION "0.1.0"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define DIR_SEPARATOR     '\\'
#define DIR_SEPARATOR_STR "\\"
#else
#define DIR_SEPARATOR     '/'
#define DIR_SEPARATOR_STR "/"
#endif

#ifndef __FILENAME__
#define __FILENAME__ (strrchr(DIR_SEPARATOR_STR __FILE__, DIR_SEPARATOR) + 1)
#endif

#define CRLF            "\r\n"

#define CLR_CLR         "\033[0m"  /* 恢复颜色 */
#define CLR_BLACK       "\033[30m" /* 黑色字 */
#define CLR_RED         "\033[31m" /* 红色字 */
#define CLR_GREEN       "\033[32m" /* 绿色字 */
#define CLR_YELLOW      "\033[33m" /* 黄色字 */
#define CLR_BLUE        "\033[34m" /* 蓝色字 */
#define CLR_PURPLE      "\033[35m" /* 紫色字 */
#define CLR_SKYBLUE     "\033[36m" /* 天蓝字 */
#define CLR_WHITE       "\033[37m" /* 白色字 */

#define CLR_BLK_WHT     "\033[40;37m" /* 黑底白字 */
#define CLR_RED_WHT     "\033[41;37m" /* 红底白字 */
#define CLR_GREEN_WHT   "\033[42;37m" /* 绿底白字 */
#define CLR_YELLOW_WHT  "\033[43;37m" /* 黄底白字 */
#define CLR_BLUE_WHT    "\033[44;37m" /* 蓝底白字 */
#define CLR_PURPLE_WHT  "\033[45;37m" /* 紫底白字 */
#define CLR_SKYBLUE_WHT "\033[46;37m" /* 天蓝底白字 */
#define CLR_WHT_BLK     "\033[47;30m" /* 白底黑字 */

#define CLR_YELLOW_BOLD "\033[33m\033[1m" /* 黄色字加粗 */
#define CLR_RED_BOLD    "\033[31m\033[1m" /* 红色字加粗 */
#define CLR_BOLD_ONRED  "\033[1m\033[41m" /* 加粗红背景 */

// XXX(id, str, clr)
#define LOG_LEVEL_MAP(XXX)                        \
    XXX(LOG_LEVEL_DEBUG, "DEBUG", CLR_SKYBLUE)    \
    XXX(LOG_LEVEL_INFO, "INFO ", CLR_GREEN)       \
    XXX(LOG_LEVEL_WARN, "WARN ", CLR_YELLOW_BOLD) \
    XXX(LOG_LEVEL_ERROR, "ERROR", CLR_RED_BOLD)   \
    XXX(LOG_LEVEL_FATAL, "FATAL", CLR_BOLD_ONRED)

typedef enum {
    LOG_LEVEL_VERBOSE = 0,
#define XXX(id, str, clr) id,
    LOG_LEVEL_MAP(XXX)
#undef XXX
        LOG_LEVEL_SILENT
} log_level_e;

#define DEFAULT_LOG_FILE         "libyjc"
#define DEFAULT_LOG_LEVEL        LOG_LEVEL_INFO
#define DEFAULT_LOG_FORMAT       "%y-%m-%d %H:%M:%S.%z %L %s"
#define DEFAULT_LOG_REMAIN_DAYS  1
#define DEFAULT_LOG_MAX_BUFSIZE  (1 << 14) // 16k
#define DEFAULT_LOG_MAX_FILESIZE (1 << 24) // 16M

// logger: default file_logger
typedef void (*logger_handler)(int loglevel, const char* buf, int len);

void stdout_logger(int loglevel, const char* buf, int len);
void stderr_logger(int loglevel, const char* buf, int len);
void file_logger(int loglevel, const char* buf, int len);
void syslog_logger(int loglevel, const char* buf, int len);
void net_logger(int loglevel, const char* buf, int len);

typedef struct logger_s logger_t;
logger_t* logger_create();
void logger_destroy(logger_t* logger);

void logger_set_handler(logger_t* logger, logger_handler fn);
logger_handler logger_get_handler(logger_t* logger);
void logger_set_level(logger_t* logger, int level);
// level = [VERBOSE,DEBUG,INFO,WARN,ERROR,FATAL,SILENT]
void logger_set_level_by_str(logger_t* logger, const char* level);
int logger_get_level(logger_t* logger);
const char* logger_get_level_str(logger_t* logger);
/*
 * format  = "%y-%m-%d %H:%M:%S.%z %L %s"
 * message = "2020-01-02 03:04:05.067 DEBUG message"
 * %y year
 * %m month
 * %d day
 * %H hour
 * %M min
 * %S sec
 * %z ms
 * %Z us
 * %l First character of level
 * %L All characters of level
 * %s message
 * %% %
 */
void logger_set_format(logger_t* logger, const char* format);
const char* logger_get_format(logger_t* logger);
void logger_set_max_bufsize(logger_t* logger, unsigned int bufsize);
void logger_enable_color(logger_t* logger, int on);
int logger_print(logger_t* logger, int level, const char* fmt, ...);

// below for file logger
void logger_set_file(logger_t* logger, const char* filepath);
void logger_set_max_filesize(logger_t* logger, unsigned long long filesize);
// 16, 16M, 16MB
void logger_set_max_filesize_by_str(logger_t* logger, const char* filesize);
void logger_set_remain_days(logger_t* logger, int days);
void logger_enable_fsync(logger_t* logger, int on);
void logger_fsync(logger_t* logger);
const char* logger_get_cur_file(logger_t* logger);

// below for net logger
void logger_set_net_fd(logger_t* logger, int fd);

// log: default logger instance
logger_t* default_logger();
void destroy_default_logger(void);

// macro log*
#define dftlog                                default_logger()
#define log_destory()                         destroy_default_logger()
#define log_disable()                         logger_set_level(dftlog, LOG_LEVEL_SILENT)
#define log_set_file(filepath)                logger_set_file(dftlog, filepath)
#define log_set_level(level)                  logger_set_level(dftlog, level)
#define log_set_level_by_str(level)           logger_set_level_by_str(dftlog, level)
#define log_get_level()                       logger_get_level(dftlog)
#define log_get_level_str()                   logger_get_level_str(dftlog)
#define log_set_handler(fn)                   logger_set_handler(dftlog, fn)
#define log_get_handler(fn)                   logger_get_handler(dftlog)
#define log_set_format(format)                logger_set_format(dftlog, format)
#define log_get_format(format)                logger_get_format(dftlog)
#define log_set_max_filesize(filesize)        logger_set_max_filesize(dftlog, filesize)
#define log_set_max_filesize_by_str(filesize) logger_set_max_filesize_by_str(dftlog, filesize)
#define log_set_remain_days(days)             logger_set_remain_days(dftlog, days)
#define log_set_net_fd(fd)                    logger_set_net_fd(dftlog, fd)
#define log_enable_color()                    logger_enable_color(dftlog, 1)
#define log_disable_color()                   logger_enable_color(dftlog, 0)
#define log_enable_fsync()                    logger_enable_fsync(dftlog, 1)
#define log_disable_fsync()                   logger_enable_fsync(dftlog, 0)
#define log_fsync()                           logger_fsync(dftlog)
#define log_get_cur_file()                    logger_get_cur_file(dftlog)

#define logd(fmt, ...)                        logger_print(dftlog, LOG_LEVEL_DEBUG, fmt " [%s:%d:%s]" CRLF, ##__VA_ARGS__, __FILENAME__, __LINE__, __FUNCTION__)
#define logi(fmt, ...)                        logger_print(dftlog, LOG_LEVEL_INFO, fmt " [%s:%d:%s]" CRLF, ##__VA_ARGS__, __FILENAME__, __LINE__, __FUNCTION__)
#define logw(fmt, ...)                        logger_print(dftlog, LOG_LEVEL_WARN, fmt " [%s:%d:%s]" CRLF, ##__VA_ARGS__, __FILENAME__, __LINE__, __FUNCTION__)
#define loge(fmt, ...)                        logger_print(dftlog, LOG_LEVEL_ERROR, fmt " [%s:%d:%s]" CRLF, ##__VA_ARGS__, __FILENAME__, __LINE__, __FUNCTION__)
#define logc(fmt, ...)                        logger_print(dftlog, LOG_LEVEL_FATAL, fmt " [%s:%d:%s]" CRLF, ##__VA_ARGS__, __FILENAME__, __LINE__, __FUNCTION__)

// below for android
#if defined(ANDROID) || defined(__ANDROID__)
#include <android/log.h>
#define LOG_TAG "JNI"
#undef logd
#undef logi
#undef logw
#undef loge
#undef logf
#define logd(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define logi(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define logw(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define loge(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define logf(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)
#endif

// macro alias
#if !defined(LOGD) && !defined(LOGI) && !defined(LOGW) && !defined(LOGE) && !defined(LOGF)
#define LOGD logd
#define LOGI logi
#define LOGW logw
#define LOGE loge
#define LOGF logf
#endif

#ifdef __cplusplus
}
#endif
#endif
