#ifndef __NJUDGER_GENERAL
#define __NJUDGER_GENERAL

#include "../common/general.hpp"
#include "logger.hpp"

namespace njudger
{
	extern const string VERSION;
	struct Judgement
	{
		string srcFile = "";
		string dataDir = "";
		string destFile = "";
		string loggerFile = "";
		string checker = "default";

		int lang = 0;
		int timeLimit = 1000;
		int memoryLimit = 131072;
		int codeLengthLimit = 65535;
	};

	struct Language
	{
		string name;
		string runCommand;
		string compileCommand;

		bool needCompile;
		int maxCompileTime;
		int maxCompileMemory;
		int maxCompileOutput;
	};

	struct Conf
	{
		int maxOutput;
		int strategy;
		Language language[1024];
		struct
		{
			string _default;
			int maxRunningTime;
			int maxRunningMemory;
			int maxRunningOutput;
		} checker;
	};

	extern Conf conf;
	extern Logger _logger;
	extern CJsonObject logger;
	extern Judgement judgement;

	void commandArgsToArray(string command, char *args[]);
	void terminatePro(int code);
} // namespace njudger

#endif
