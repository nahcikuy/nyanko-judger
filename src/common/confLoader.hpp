#ifndef __COMMON_CONFLOADER
#define __COMMON_CONFLOADER

#include "headers.hpp"

class ConfLoader
{
	bool _isInited = false;
	string fileName;
	CJsonObject _conf;

public:
	bool isInited();
	string getFileName();
	CJsonObject getConf();
	int load(string str, bool isPath, string &errorMsg);
};

#endif
