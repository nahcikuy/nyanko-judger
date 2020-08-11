#include "general.hpp"
#include "utils.hpp"

const int ERR_INVALID_ARGUMENT = 1;
const int ERR_INVALID_COMMAND_FORMAT = 2;
const int ERR_LOAD_CONF_ERROR = 3;
const int ERR_INVALID_CONF = 4;
const int ERR_OPEN_LOGGER_FILE_ERROR = 5;
const int ERR_CREATE_TEMP_DIR_ERROR = 6;
const int ERR_GET_FILE_SIZE_ERROR = 7;
const int ERR_LOAD_SECCOMP_ERROR = 8;
const int ERR_OPEN_INPUT_FILE_ERROR = 9;
const int ERR_OPEN_OUTPUT_FILE_ERROR = 10;
const int ERR_RUN_TEST_ERROR = 11;
const int ERR_COMPILATION_FAILED = 12;
const int ERR_SANDBOX_SET_RLIMIT_ERROR = 13;
const int ERR_SANDBOX_LOAD_SECCOMP_ERROR = 14;
const int ERR_GET_RESOURCE_USAGE_FAILED = 15;
const int ERR_RUN_CHECKER_ERROR = 16;
const int ERR_CREATE_PIPE_ERROR = 17;
const int ERR_CREATE_SERVER_ERROR = 18;
extern const int ERR_CREATE_DAEMON_ERROR = 19;

const int ACCEPTED = 0;
const int WRONG_ANSWER = 1;
const int PRESENTATION_ERROR = 2;
const int TIME_LIMIT_EXCCEEDED = 3;
const int MEMORY_LIMIT_EXCCEEDED = 4;
const int OUTPUT_LIMIT_EXCCEEDED = 5;
const int CODE_LENGTH_LIMIT_EXCCEEDED = 6;
const int RUNTIME_ERROR = 7;
const int COMPILATION_ERROR = 8;
const int CHECKER_ERROR = 9;
const int SYSTEM_ERROR = 10;

const string TMP_DIR = "/tmp/njudger";

void createTempDir()
{
	if (getFileType("/tmp") == -1)
	{
		if (mkdir("/tmp", 0755) == -1)
		{
			perror("Create temporary directory failed.");
			cerr << "Terminated." << endl;
			exit(ERR_CREATE_TEMP_DIR_ERROR);
		}
	}
	if (getFileType("/tmp/njudger") == -1)
	{
		if (mkdir("/tmp/njudger", 0755) == -1)
		{
			perror("Create temporary directory failed.");
			cerr << "Terminated." << endl;
			exit(ERR_CREATE_TEMP_DIR_ERROR);
		}
	}
}
