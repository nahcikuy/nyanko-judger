#include <bits/stdc++.h>
#include <unistd.h>

using namespace std;

int main()
{
	int s = 0;
	for (int i = 0; i < 1 << 27; i++) s += i;
	cout << s << endl;
}
