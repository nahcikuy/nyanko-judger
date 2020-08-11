#include "utils.hpp"
#include "confLoader.hpp"

bool ConfLoader::isInited()
{
	return _isInited;
}

string ConfLoader::getFileName()
{
	return fileName;
}

CJsonObject ConfLoader::getConf()
{
	return _conf;
}

int ConfLoader::load(string str, bool isPath, string &errorMsg)
{
	if (isPath)
	{
		if (getFileType(str) != 2)
		{
			errorMsg = "Can't find file: " + str;
			return 1;
		}
		else
			fileName = str;
	}
	else
	{
		if (getFileType("./" + str + ".conf") == 2)
			fileName = "./" + str + ".conf";
		else if (getFileType("~/." + str + ".conf") == 2)
			fileName = "~/." + str + ".conf";
		else if (getFileType("/etc/" + str + ".conf") == 2)
			fileName = "/etc/" + str + ".conf";
		else
		{
			errorMsg = "Can't find configuration file.";
			return 1;
		}
	}
	char content[1 << 15];
	memset(content, 0, sizeof(content));
	FILE *file = fopen(fileName.c_str(), "r");
	if (!file)
	{
		errorMsg = "Error while loading configuration file " + fileName + strerror(errno);
		return 1;
	}
	fread(content, 1, getFileSize(fileName), file);
	_conf = CJsonObject(content);
	_isInited = true;
	return 0;
}
