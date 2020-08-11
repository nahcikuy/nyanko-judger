#ifndef __COMMON_UTILS
#define __COMMON_UTILS

#include "headers.hpp"

bool getNumber(string s, int lower, int upper, int &val);
int getFileType(string file);
int getFileSize(string file);
string itoa(LL val);
string randomString(int length);
vector<string> getCommandArgs(string command);
string trim(string str);
string removeEndlines(string str);
string getTimestamp();
void randomize();
#endif
