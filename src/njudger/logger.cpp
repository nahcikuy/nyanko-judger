#include "logger.hpp"
#include "../common/utils.hpp"

namespace njudger
{
	int Logger::init(string loggerFileStr)
	{
		if (!(loggerFile = fopen(loggerFileStr.c_str(), "w")))
			return errno;
		return 0;
	}

	void Logger::addResult(CJsonObject &logger, int resultCode, string judgerLog)
	{
		if (judgerLog == "")
			judgerLog = results[resultCode];
		if (resultCode != 10)
			cout << judgerLog << endl;
		logger.Add("result", results[resultCode]);
		logger.Add("resultCode", resultCode);
		logger.Add("judgerLog", judgerLog);
	}

	void Logger::addResult(CJsonObject &logger, int resultCode)
	{
		addResult(logger, resultCode, "");
	}

	void Logger::addResult(CJsonObject &logger, int resultCode, int testId)
	{
		addResult(logger, resultCode, results[resultCode] + " on test " + itoa(testId));
	}

	void Logger::Nyan(CJsonObject &logger, int exitCode, string errorMessage, bool sendSIGUSR1)
	{
		if (exitCode)
		{
			addResult(logger, 10);
			cerr << errorMessage << endl
				 << "Terminated." << endl;
		}
		logger.Add("exitCode", exitCode);
		logger.Add("errorMessage", errorMessage);
		logger.Add("etc", "Nyan~");

		fprintf(loggerFile, logger.ToFormattedString().c_str()), fclose(loggerFile);

		if (sendSIGUSR1)
			raise(SIGUSR1);
		// ALL IS DONE HERE! Nyannnnnnnnnn~
		exit(exitCode);
	}

	void Logger::Nyan(CJsonObject &logger)
	{
		Nyan(logger, 0, "SUCCESS");
	}
}; // namespace njudger
