#include "runner.hpp"
#include "../common/pipe.hpp"
#include "../common/processMonitor.hpp"
#include "../common/utils.hpp"
#include "sandBox.hpp"

namespace njudger
{
void checkerRunner(string srcFile, string inFile, string outFile,
				   vector<string> &extFiles, int &result, string &judgerLog)
{
	Pipe _pipe;
	if (_pipe.init(O_NONBLOCK))
		_logger.Nyan(logger, ERR_CREATE_PIPE_ERROR,
					 "Error while creating pipe: " + string(strerror(errno)));

	int checkerProcess = fork();
	if (checkerProcess)
	{
		// parent process
		ProcessMonitor processMonitor(checkerProcess,
									  conf.checker.maxRunningTime,
									  conf.checker.maxRunningMemory);
		ProcessStatus processStatus = processMonitor.getStatus();

		if (processStatus.err)
			_logger.Nyan(logger, ERR_GET_RESOURCE_USAGE_FAILED,
						 "Failed to get resource usage: " +
							 string(strerror(errno)));
		if (processStatus.signal == SIGUSR1) exit(0); // NOTHING TO DO HERE.
		if (processMonitor.isTLE())
		{
			result = CHECKER_ERROR;
			judgerLog = "Checker Error: checker time limit excceeded.";
		}
		else if (processMonitor.isMLE())
		{
			result = CHECKER_ERROR;
			judgerLog = "Checker Error: checker memory limit excceeded.";
		}
		else if (processMonitor.isOLE())
		{
			result = CHECKER_ERROR;
			judgerLog = "Checker Error: checker output limit excceeded.";
		}
		else if (processStatus.signal)
		{
			result = CHECKER_ERROR;
			judgerLog = "Checker Error: checker was exited by the signal " +
						itoa(processStatus.signal);
		}
		else if (processStatus.status > 2)
		{
			result = CHECKER_ERROR;
			judgerLog = "Checker Error: checker was exited with code " +
						itoa(processStatus.status) + " (must be 0, 1 or 2)";
		}
		else
		{
			result = processStatus.status;
			judgerLog = _pipe.readStr();
		}
	}
	else
	{
		// checkerProcess
		char *args[255];
		string command =
			judgement.checker + " " + srcFile + " " + inFile + " " + outFile;
		for (int i = 0; i < extFiles.size(); i++) command += " " + extFiles[i];
		commandArgsToArray(command, args);
		_pipe.redirect(1);

		// Checker should also run in the sandbox
		SandBox sandBox;
		sandBox.init(args);
		if (sandBox.setResLimit(conf.checker.maxRunningTime,
								conf.checker.maxRunningMemory * 2,
								conf.checker.maxRunningOutput))
			_logger.Nyan(logger, ERR_SANDBOX_SET_RLIMIT_ERROR,
						 "Failed to set resource limit for sand box", 1);
		if (sandBox.loadSeccomp())
			_logger.Nyan(logger, ERR_SANDBOX_LOAD_SECCOMP_ERROR,
						 "Failed to load seccomp for sand box", 1);

		sandBox.execute();
		_logger.Nyan(logger, ERR_RUN_CHECKER_ERROR,
					 "Error while running checker: " + string(strerror(errno)),
					 1);
	}
}

void fin(int result, int testId)
{
	int maxCpuTime = 0, maxRealTime = 0, maxMemoryUsage = 0;
	for (int i = 0; i < logger["test"].GetArraySize(); i++)
	{
		int cpuTime, realTime, memoryUsage;
		logger["test"][i].Get("cpuTime", cpuTime);
		logger["test"][i].Get("realTime", realTime);
		logger["test"][i].Get("memoryUsage", memoryUsage);
		maxCpuTime = max(maxCpuTime, cpuTime);
		maxRealTime = max(maxRealTime, realTime);
		maxMemoryUsage = max(maxMemoryUsage, memoryUsage);
	}
	logger.Add("maxCpuTime", maxCpuTime);
	logger.Add("maxRealTime", maxRealTime);
	logger.Add("maxMemoryUsage", maxMemoryUsage);
	cout << "Judgement result: ";
	if (result == ACCEPTED) _logger.addResult(logger, ACCEPTED);
	else
		_logger.addResult(logger, result, testId);
	_logger.Nyan(logger);
}

void runOnce(string inFile, vector<string> &extFiles, int testId, int testCount)
{
	cout << "Running on test " << testId << "(" << testId << "/" << testCount
		 << ")..." << endl;

	string outFile = string(TMP_DIR) + "/" + randomString(15);

	if (!logger["test"].IsArray()) logger.Add("test", CJsonObject("[]"));
	CJsonObject test("{}");

	int runnerProcess = fork();
	if (runnerProcess)
	{
		// parent process
		ProcessMonitor processMonitor(runnerProcess, judgement.timeLimit,
									  judgement.memoryLimit);
		ProcessStatus processStatus = processMonitor.getStatus();

		if (processStatus.err)
			_logger.Nyan(logger, ERR_GET_RESOURCE_USAGE_FAILED,
						 "Failed to get resource usage: " +
							 string(strerror(errno)));
		if (processStatus.signal == SIGUSR1) exit(0); // NOTHING TO DO HERE.

		test.Add("cpuTime", processStatus.cpuTime);
		test.Add("realTime", processStatus.realTime);
		test.Add("memoryUsage", processStatus.memoryUsage);

		int result = 0;
		string judgerLog = "";
		if (processMonitor.isTLE()) result = TIME_LIMIT_EXCCEEDED;
		else if (processMonitor.isMLE())
			result = MEMORY_LIMIT_EXCCEEDED;
		else if (processMonitor.isOLE())
			result = OUTPUT_LIMIT_EXCCEEDED;
		else if (processStatus.signal)
		{
			result = RUNTIME_ERROR;
			judgerLog =
				"Runtime Error: user's program was exited by the signal " +
				itoa(processStatus.signal);
		}
		else if (processStatus.status)
		{
			result = RUNTIME_ERROR;
			judgerLog = "Runtime Error: user's program was exited with code " +
						itoa(processStatus.status);
		}
		if (!result)
			checkerRunner(judgement.srcFile, inFile, outFile, extFiles, result,
						  judgerLog);

		cout << "Judgement result of test " << testId << "(" << testId << "/"
			 << testCount << "): ";
		_logger.addResult(test, result, judgerLog), logger["test"].Add(test);

		if (result && !conf.strategy) fin(result, testId);
	}
	else
	{
		// runnerProcess

		int infd, outfd;
		if ((infd = open(inFile.c_str(), O_RDONLY)) == -1)
			_logger.Nyan(
				logger, ERR_OPEN_INPUT_FILE_ERROR,
				"Failed to open input file " + inFile + strerror(errno), 1);
		if ((outfd = open(outFile.c_str(), O_WRONLY | O_CREAT, 0644)) == -1)
			_logger.Nyan(
				logger, ERR_OPEN_INPUT_FILE_ERROR,
				"Failed to open output file " + outFile + strerror(errno), 1);

		char *runArgs[1 << 8];
		commandArgsToArray(conf.language[judgement.lang].runCommand, runArgs);

		// Redirect STDIN/STDOUT

		dup2(infd, 0), dup2(outfd, 1);
		// Enter sand box and execute
		SandBox sandBox;
		sandBox.init(runArgs);
		if (sandBox.setResLimit(judgement.timeLimit, judgement.memoryLimit * 2,
								conf.maxOutput))
			_logger.Nyan(logger, ERR_SANDBOX_SET_RLIMIT_ERROR,
						 "Failed to set resource limit for sand box", 1);
		if (sandBox.loadSeccomp())
			_logger.Nyan(logger, ERR_SANDBOX_LOAD_SECCOMP_ERROR,
						 "Failed to load seccomp for sand box", 1);

		sandBox.execute();
		_logger.Nyan(logger, ERR_RUN_TEST_ERROR,
					 "Error while running on test " + inFile + ": " +
						 strerror(errno),
					 1);
	}
}

void run()
{
	int testCount = 0;
	for (; getFileType(judgement.dataDir + "/" + itoa(testCount + 1) + ".in") ==
		   2;)
		testCount++;
	cout << "Enumrating tests... there are " << testCount
		 << " test(s) in total." << endl;

	for (int i = 1; i <= testCount; i++)
	{
		vector<string> extFiles;
		string inFile = judgement.dataDir + "/" + itoa(i) + ".in";
		for (int j = 1;; j++)
		{
			string current = judgement.dataDir + "/" + itoa(i) + ".ext" + itoa(j);
			if (getFileType(current) != 2) break;
			extFiles.push_back(current);
		}
		runOnce(inFile, extFiles, i, testCount);
	}

	for (int i = 0; i < logger["test"].GetArraySize(); i++)
	{
		int resultCode;
		logger["test"][i].Get("resultCode", resultCode);
		if (resultCode)
			_logger.addResult(logger, resultCode, i + 1), _logger.Nyan(logger);
	}

	fin(ACCEPTED, 0);
}
}; // namespace njudger
