#include "utils.hpp"

bool getNumber(string s, int lower, int upper, int &val)
{
	int start = 0, sgn = 1;
	int len = s.length();

	if (!s.length() || s == "-")
		return 0;
	if (s[0] == '-')
		sgn = -1, start = 1;

	LL res = 0;
	for (int i = start; i < len; i++)
	{
		if (s[i] < 48 || s[i] > 57)
			return 0;
		res = res * 10 + s[i] - 48;
		if ((sgn == -1 && res * sgn < lower) || (sgn == 1 && res * sgn > upper))
			return 0;
	}
	res *= sgn;
	if (res >= lower && res <= upper)
	{
		val = res;
		return 1;
	}
	return 0;
}

int getFileType(string file)
{
	struct stat buffer;
	if (stat(file.c_str(), &buffer) == -1)
		return -1;
	else
	{
		if (buffer.st_mode & S_IFREG)
			return 2;
		else if (buffer.st_mode & S_IFDIR)
			return 1;
		return 3;
	}
}

int getFileSize(string file)
{
	struct stat buffer;
	if (stat(file.c_str(), &buffer) == -1)
		return -1;
	else
		return buffer.st_size;
}

string itoa(LL val)
{
	char str[32];
	sprintf(str, "%lld", val);
	return string(str);
}

string randomString(int length)
{
	string res = "";
	for (int i = 0; i < length; i++)
	{
		int val = rand() % 62;
		if (val < 10)
			res.append(1, val + 48);
		else if (val < 36)
			res.append(1, val + 55);
		else
			res.append(1, val + 61);
	}
	return res;
}

vector<string> getCommandArgs(string command)
{
	string current = "";
	vector<string> res;
	for (int i = 0; i < command.length(); i++)
	{
		if (command[i] != ' ')
			current += command[i];
		else
		{
			res.push_back(current);
			current = "";
		}
	}
	if (current != "")
		res.push_back(current);
	return res;
}

string trim(string str)
{
	int l = 0, r = str.length() - 1;
	for (; l < str.length() && str[l] == '\r' || str[l] == '\n' || str[l] == '\t' || str[l] == ' '; l++)
		;
	for (; r >= 0 && str[r] == '\r' || str[r] == '\n' || str[r] == '\t' || str[r] == ' '; r--)
		;
	return str.substr(l, r - l + 1);
}

string removeEndlines(string str)
{
	string res = "";
	for (int i = 0; i < str.length(); i++)
		if (str[i] != '\r' && str[i] != '\n')
			res += str[i];
	return res;
}

string getTimestamp()
{
	time_t now = time(0);
	return trim(ctime(&now));
}

void randomize()
{
	timeval tv;
	gettimeofday(&tv, 0);
	srand(tv.tv_sec * 1000000 + tv.tv_usec);
}
