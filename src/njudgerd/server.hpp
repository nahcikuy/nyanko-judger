#ifndef __NJUDGERD_SERVER
#define __NJUDGERD_SERVER

#include "../common/udpSocket.hpp"

namespace njudgerd
{
	class Server
	{
		static SocketServer socketServer;
		static int requestCount;

	public:
		static void init();
		static void run();
		static void parser(string message, int id);
		static void response(CJsonObject message, int id);
	};
} // namespace njudgerd

#endif
