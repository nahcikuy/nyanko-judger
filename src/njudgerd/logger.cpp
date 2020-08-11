#include "logger.hpp"
#include "../common/general.hpp"
#include "../common/utils.hpp"

namespace njudgerd
{
	void Logger::print(string message, bool attachErr)
	{
		loggerMtx.lock();

		if (attachErr)
			message += string(": ") + strerror(errno);
		cerr << message << endl;
		FILE *logFile = fopen((TMP_DIR + "/njudgerd.log").c_str(), "a");
		message = "[" + getTimestamp() + "] " + message + "\n";
		fwrite(message.c_str(), message.length(), 1, logFile), fclose(logFile);

		loggerMtx.unlock();
	}
} // namespace njudgerd
