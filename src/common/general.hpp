#ifndef __COMMON_GENERAL
#define __COMMON_GENERAL

/*
    Global variables, functions, etc.
*/

#include "headers.hpp"

extern const int ERR_INVALID_ARGUMENT;
extern const int ERR_INVALID_COMMAND_FORMAT;
extern const int ERR_LOAD_CONF_ERROR;
extern const int ERR_INVALID_CONF;
extern const int ERR_OPEN_LOGGER_FILE_ERROR;
extern const int ERR_CREATE_TEMP_DIR_ERROR;
extern const int ERR_GET_FILE_SIZE_ERROR;
extern const int ERR_LOAD_SECCOMP_ERROR;
extern const int ERR_OPEN_INPUT_FILE_ERROR;
extern const int ERR_OPEN_OUTPUT_FILE_ERROR;
extern const int ERR_RUN_TEST_ERROR;
extern const int ERR_COMPILATION_FAILED;
extern const int ERR_SANDBOX_SET_RLIMIT_ERROR;
extern const int ERR_SANDBOX_LOAD_SECCOMP_ERROR;
extern const int ERR_GET_RESOURCE_USAGE_FAILED;
extern const int ERR_RUN_CHECKER_ERROR;
extern const int ERR_CREATE_PIPE_ERROR;
extern const int ERR_CREATE_SERVER_ERROR;
extern const int ERR_CREATE_DAEMON_ERROR;

extern const int ACCEPTED;
extern const int WRONG_ANSWER;
extern const int PRESENTATION_ERROR;
extern const int TIME_LIMIT_EXCCEEDED;
extern const int MEMORY_LIMIT_EXCCEEDED;
extern const int OUTPUT_LIMIT_EXCCEEDED;
extern const int CODE_LENGTH_LIMIT_EXCCEEDED;
extern const int RUNTIME_ERROR;
extern const int COMPILATION_ERROR;
extern const int CHECKER_ERROR;
extern const int SYSTEM_ERROR;

extern const string TMP_DIR;

void createTempDir();

#endif
