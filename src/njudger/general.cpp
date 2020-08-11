#include "../common/utils.hpp"
#include "general.hpp"

namespace njudger
{
	const string VERSION = "0.0.1";

	Conf conf;
	Logger _logger;
	Language language;
	Judgement judgement;
	CJsonObject logger("{}");

	void commandArgsToArray(string command, char *args[])
	{
		auto commandArgs = getCommandArgs(command);
		for (int i = 0; i < commandArgs.size(); i++)
		{
			if (commandArgs[i] == "%SRCFILE%")
				commandArgs[i] = judgement.srcFile;
			else if (commandArgs[i] == "%DESTFILE%")
				commandArgs[i] = judgement.destFile;
			args[i] = new char[commandArgs[i].length() + 1];
			strcpy(args[i], commandArgs[i].c_str());
		}
		args[commandArgs.size()] = NULL;
	}

	void terminatePro(int code)
	{
		cerr << "Terminated." << endl;
		exit(code);
	}

} // namespace njudger
