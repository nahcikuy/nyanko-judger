#ifndef __NJUDGER_SANDBOX
#define __NJUDGER_SANDBOX

#include "general.hpp"

namespace njudger
{
	class SandBox
	{
		char *_argv[1 << 8];

	public:
		void init(char *argv[]);
		int setResLimit(int maxCpuTime, int maxMemory, int maxOutput);
		int loadSeccomp();
		int execute();
	};
} // namespace njudger

#endif
