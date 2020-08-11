#include "../common/confLoader.hpp"
#include "../common/utils.hpp"
#include "general.hpp"

namespace njudger
{
ConfLoader confLoader;

void showHelp()
{
	cout << "Usage:" << endl;
	cout << " njudger [OPTIONS] <SRCFILE> <DATADIR> <LOGGERILE>" << endl;
	cout << " njudger -h|--help" << endl << endl;
	cout << "Run a specified judgement." << endl << endl;
	cout << "Njudger will run a judgement with the source code of SRCFILE "
			"using DATADIR as the data directory."
		 << endl;
	cout << "For each case of test data, the data files in DATADIR should be "
			"in the following format:"
		 << endl;
	cout << "(testIndex).in (testIndex).ext1 (testIndex).ext2 ..." << endl;
	cout << "where testIndex indicates the index of the case of tests (started "
			"from 1). For instance, a group of"
		 << endl;
	cout << "files of test data containing 3 cases of test may look like this: "
		 << endl;
	cout << "1.in 1.ext1 2.in 2.ext1 3.in 3.ext1" << endl;
	cout << "The file with extension \".in\" will be regarded as input file, "
			"and njudger will redirect the standard"
		 << endl;
	cout << "input of the judged program to these files. The extended files "
			"with extension \".ext1\", \".ext2\", ..."
		 << endl;
	cout << "won't be used when running the judged program, but it will be "
			"used for the checker to check the output of"
		 << endl;
	cout << "it. In most of case (unless you need a special judge or you want "
			"to provide multiple solutions), there is"
		 << endl;
	cout << "only one extended file for each case of test data and it should "
			"be the correct output for the case of the"
		 << endl;
	cout << "corresponding test." << endl;
	cout << "The result of the judgement will be saved into LOGGERFILE, in "
			"JSON format."
		 << endl
		 << endl;
	cout << "Options:" << endl;
	cout << " -l, --language           Specify the language ID of SRCFILE. "
			"Default to 0."
		 << endl;
	cout << " -t, --time-limit         Specify the time limit when running(in "
			"milliseconds). Default to 1000."
		 << endl;
	cout << " -m, --memory-limit       Specify the memory limit when "
			"running(in kilobytes). Default to 131072."
		 << endl;
	cout << "     --code-length-limit  Specify the size limit of SRCFILE(in "
			"bytes). Default to 65535."
		 << endl;
	cout << " -c, --checker            Specify the checker judger will use. If "
			"not specified, judger will use "
		 << endl;
	cout << "                           the default checker. " << endl;
	cout << "     --conf=FILE          Load the file as the configuration file." << endl;
	cout << " -h, --help               Display this help and exit." << endl;

	exit(0);
}

/*
	Parse arguments. Load configuration file.
*/

void argParser(int argc, char **argv)
{
	option long_options[] = {{"language", required_argument, 0, 'l'},
							 {"time-limit", required_argument, 0, 't'},
							 {"memory-limit", required_argument, 0, 'm'},
							 {"code-length-limit", required_argument, 0, 'x'},
							 {"checker", required_argument, 0, 'c'},
							 {"conf", required_argument, 0, 'f'},
							 {"help", no_argument, 0, 'h'},
							 {0, 0, 0, 0}};
	int c, option_index;

	for (; (c = getopt_long(argc, argv, "l:t:m:c:h", long_options,
							&option_index)) != -1;)
	{
		if (c == 'l')
		{
			if (!getNumber(optarg, 0, 1023, judgement.lang))
			{
				cerr << "Invalid value for language: " << optarg << endl;
				terminatePro(ERR_INVALID_ARGUMENT);
			}
		}
		if (c == 't')
		{
			if (!getNumber(optarg, 100, 100000, judgement.timeLimit))
			{
				cerr << "Invalid value for time-limit(100~100000): " << optarg
					 << endl;
				terminatePro(ERR_INVALID_ARGUMENT);
			}
		}
		if (c == 'm')
		{
			if (!getNumber(optarg, 4096, 2097152, judgement.memoryLimit))
			{
				cerr << "Invalid value for memory-limit(4096~2097152): "
					 << optarg << endl;
				terminatePro(ERR_INVALID_ARGUMENT);
			}
		}
		if (c == 'x')
		{
			if (!getNumber(optarg, 100, 65535, judgement.codeLengthLimit))
			{
				cerr << "Invalid value for code-length-limit(100~65535): "
					 << optarg << endl;
				terminatePro(ERR_INVALID_ARGUMENT);
			}
		}
		if (c == 'c')
		{
			if (string(optarg) != "default")
			{
				int fileType = getFileType(optarg);
				if (fileType == -1)
				{
					perror(optarg);
					terminatePro(ERR_INVALID_ARGUMENT);
				}
				else if (fileType != 2)
				{
					cerr << optarg << ": not a file" << endl;
					terminatePro(ERR_INVALID_ARGUMENT);
				}
			}
		}
		if (c == 'h') showHelp();
		if (c == 'f')
		{
			string errorMsg;
			if (confLoader.load(optarg, 1, errorMsg))
			{
				cerr << errorMsg << endl;
				terminatePro(ERR_LOAD_CONF_ERROR);
			}
		}
		if (c == '?') terminatePro(ERR_INVALID_COMMAND_FORMAT);
	}

	if (argc - optind != 3)
	{
		cerr << argv[0] << ": Command format error. " << endl;
		cerr << "Use njudger --help to get more details." << endl;
		terminatePro(ERR_INVALID_ARGUMENT);
	}

	judgement.srcFile = argv[optind];
	judgement.dataDir = argv[optind + 1];
	judgement.loggerFile = argv[optind + 2];

	if (!confLoader.isInited())
	{
		string errorMsg;
		if (confLoader.load("njudger", 0, errorMsg))
		{
			cerr << errorMsg << endl;
			terminatePro(ERR_LOAD_CONF_ERROR);
		}
	}
}

/*
	Load configurations.
*/

void loadConf()
{
	// description
	CJsonObject _conf = confLoader.getConf();
	if (_conf["description"].ToString() != "\"Njudger Configuration File\"")
	{
		cerr << "Invalid configuration file: " << confLoader.getFileName()
			 << endl
			 << "The file must be in JSON format containing a field of "
				"\"description\" with value "
				"\"Njudger Configuration File\""
			 << endl;
		terminatePro(ERR_INVALID_CONF);
	}

	// maxOutput
	_conf.Get("maxOutput", conf.maxOutput);
	if (conf.maxOutput < 16384 || conf.maxOutput > 134217728)
	{
		cerr << "Invalid configuration for maxOutput: " << conf.maxOutput
			 << " (must be between 16384 to 134217728(bytes))" << endl;
		terminatePro(ERR_INVALID_CONF);
	}

	// strategy
	string strategy;
	_conf.Get("strategy", strategy);
	if (strategy == "default") conf.strategy = 0;
	else if (strategy == "force")
		conf.strategy = 1;
	else
	{
		cerr << "Invalid configuration for strategy: " << strategy
			 << " (must be default or force)" << endl;
		terminatePro(ERR_INVALID_CONF);
	}

	// languages

	if (!_conf["lang"].IsArray())
	{
		cerr << "Invalid configuration for language: you must define at least "
				"one language."
			 << endl;
		terminatePro(ERR_INVALID_CONF);
	}
	for (int i = 0; i < _conf["lang"].GetArraySize(); i++)
	{
		auto curLang = _conf["lang"][i];

		int id;
		curLang.Get("id", id);
		if (id < 0 || id >= 1024)
		{
			cerr << "Invalid configuration for language ID: " << id
				 << " (must be between 0 to 1023)" << endl;
			terminatePro(ERR_INVALID_CONF);
		}
		if (conf.language[id].name != "")
		{
			cerr << "Invalid configuration. Duplicate definition for language "
					"ID: "
				 << id << endl;
			terminatePro(ERR_INVALID_CONF);
		}

		curLang.Get("name", conf.language[id].name);
		if (conf.language[id].name == "")
		{
			cerr << "Invalid configuration for language ID: " << id << endl
				 << "Name of the language is invalid." << endl;
			terminatePro(ERR_INVALID_CONF);
		}

		curLang.Get("needCompile", conf.language[id].needCompile);
		if (curLang.GetErrMsg() != "")
		{
			cerr << "Invalid configuration for language: "
				 << conf.language[id].name << endl
				 << "The needCompile property must be true/false." << endl;
			terminatePro(ERR_INVALID_CONF);
		}

		if (conf.language[id].needCompile)
		{
			curLang.Get("compileCommand", conf.language[id].compileCommand);
			if (conf.language[id].compileCommand == "")
			{
				cerr << "Invalid configuration for language: "
					 << conf.language[id].name << endl
					 << "The compileCommand must be set." << endl;
				terminatePro(ERR_INVALID_CONF);
			}

			curLang.Get("maxCompileTime", conf.language[id].maxCompileTime);
			if (conf.language[id].maxCompileTime < 2000 ||
				conf.language[id].maxCompileTime > 100000)
			{
				cerr << "Invalid configuration for language: "
					 << conf.language[id].name << endl
					 << "The maxCompileTime must be between 2000 to 100000(ms)."
					 << endl;
				terminatePro(ERR_INVALID_CONF);
			}

			curLang.Get("maxCompileMemory", conf.language[id].maxCompileMemory);
			if (conf.language[id].maxCompileMemory < 4096 ||
				conf.language[id].maxCompileMemory > 16777216)
			{
				cerr << "Invalid configuration for language: "
					 << conf.language[id].name << endl
					 << "The maxCompileMemory must be between 4096 to "
						"16777216(kilobytes)."
					 << endl;
				terminatePro(ERR_INVALID_CONF);
			}

			curLang.Get("maxCompileOutput", conf.language[id].maxCompileOutput);
			if (conf.language[id].maxCompileOutput < 4096 ||
				conf.language[id].maxCompileOutput > 134217728)
			{
				cerr << "Invalid configuration for language: "
					 << conf.language[id].name << endl
					 << "The maxCompileOutput must be between 4096 to "
						"134217728(bytes)."
					 << endl;
				terminatePro(ERR_INVALID_CONF);
			}
		}
		curLang.Get("runCommand", conf.language[id].runCommand);
		if (conf.language[id].runCommand == "")
		{
			cerr << "Invalid configuration for language ID: " << id << endl
				 << "The runCommand must be set." << endl;
			terminatePro(ERR_INVALID_CONF);
		}
	}

	// checker
	_conf["checker"].Get("default", conf.checker._default);
	if (conf.checker._default == "")
	{
		cerr << "Invalid configuration for checker: you must specify a default "
				"checker."
			 << endl;
		terminatePro(ERR_INVALID_CONF);
	}
	_conf["checker"].Get("maxRunningTime", conf.checker.maxRunningTime);
	if (conf.checker.maxRunningTime < 2000 ||
		conf.checker.maxRunningTime > 100000)
	{
		cerr << "Invalid configuration for checker: The maxRunningTime must be "
				"between 2000 to 100000(ms)."
			 << endl;
		terminatePro(ERR_INVALID_CONF);
	}
	_conf["checker"].Get("maxRunningMemory", conf.checker.maxRunningMemory);
	if (conf.checker.maxRunningMemory < 4096 ||
		conf.checker.maxRunningMemory > 16777216)
	{
		cerr << "Invalid configuration for checker: The maxRunningMemory must "
				"be between 4096 to 16777216(kilobytes)."
			 << endl;
		terminatePro(ERR_INVALID_CONF);
	}
	_conf["checker"].Get("maxRunningOutput", conf.checker.maxRunningOutput);
	if (conf.checker.maxRunningOutput < 2 ||
		conf.checker.maxRunningOutput >
			1024) // Huge output of checker is not permitted.
	{
		cerr << "Invalid configuration for checker: The maxRunningOuput must "
				"be between 2 to 1024(bytes)."
			 << endl;
		terminatePro(ERR_INVALID_CONF);
	}
	if (judgement.checker == "default")
		judgement.checker = conf.checker._default;
}

/*
	Check lang, SRCFILE, DATADIR.
*/

void paramChecker()
{
	if (conf.language[judgement.lang].name == "")
	{
		cerr << "Invalid value for lang ID:" << judgement.lang << endl;
		terminatePro(ERR_INVALID_ARGUMENT);
	}

	int srcFileType = getFileType(judgement.srcFile);
	if (srcFileType == -1)
	{
		perror(judgement.srcFile.c_str());
		terminatePro(ERR_INVALID_ARGUMENT);
	}
	if (srcFileType != 2)
	{
		cerr << judgement.srcFile << ": not a file" << endl;
		terminatePro(ERR_INVALID_ARGUMENT);
	}

	int dataDirType = getFileType(judgement.dataDir);
	if (dataDirType == -1)
	{
		perror(judgement.dataDir.c_str());
		terminatePro(ERR_INVALID_ARGUMENT);
	}
	if (dataDirType != 1)
	{
		cerr << judgement.dataDir << ": not a directory" << endl;
		terminatePro(ERR_INVALID_ARGUMENT);
	}
}

/*
	Create temporary directory: /tmp/njudger
*/

void initLogger()
{
	if (_logger.init(judgement.loggerFile))
	{
		perror(("Failed while openning logger file " + judgement.loggerFile)
				   .c_str());
		terminatePro(ERR_OPEN_LOGGER_FILE_ERROR);
	}
	logger.Add("description", "Njudger Logger File");
	logger.Add("version", VERSION);
	logger.Add("srcFile", judgement.srcFile);
	logger.Add("dataDir", judgement.dataDir);
}

void initialize(int argc, char **argv)
{
	randomize(); // initialize seed
	// cout.setf(ios::unitbuf); // close cout flush
	createTempDir();
	argParser(argc, argv), loadConf(), paramChecker(), initLogger();
}
} // namespace njudger
