#include "server.hpp"
#include "general.hpp"
#include "jobController.hpp"
#include "../common/utils.hpp"

namespace njudgerd
{
    SocketServer Server::socketServer;
    int Server::requestCount = 0;

    void Server::init()
    {
        if (socketServer.init(conf.server.port))
        {
            logger.print("Error while creating server: ", true);
            terminatePro(ERR_CREATE_SERVER_ERROR);
        }
    }

    void Server::response(CJsonObject response, int id)
    {
        if (socketServer.sendMessage(response.ToFormattedString(), conf.listener.host, conf.listener.port))
            logger.print("Sending response for request (ID = " + itoa(id) + ") to " + conf.listener.host + ":" + itoa(conf.listener.port) + " failed", true);
        else
            logger.print("Successfully send response for request (ID = " + itoa(id) + ") to " + conf.listener.host + ":" + itoa(conf.listener.port));
    }

    void invalidRequest(int id)
    {
        logger.print("Invalid request (ID = " + itoa(id) + ")");
        CJsonObject response("{}");
        response.Add("type", "general-response");
        response.Add("information", "Invalid request format");

        thread _response(Server::response, response, id);
        _response.detach();
    }

    void Server::parser(string message, int id)
    {
        CJsonObject request = CJsonObject(message);

        string type;
        request.Get("type", type);
        if (type == "judgement-request")
        {
            if (request["judgement"].IsNull("srcFile") || request["judgement"].IsNull("dataDir"))
                invalidRequest(id);
            else
            {
                thread _addJob(JobController::addJob,PCI(message,id));
                _addJob.detach();
            }
        }
        else
            invalidRequest(id);
    }

    // The _server thread.
    void Server::run()
    {
        logger.print("Njudgerd starts at localhost:" + itoa(conf.server.port));

        string message;
        sockaddr_in from;
        
        for (string message;;)
        {
            if (socketServer.receiveMessage(message, from))
                logger.print("Error while receiving message", true);
            else
            {
                requestCount++;
                logger.print(string("Request received from ") + inet_ntoa(from.sin_addr) +
                             ":" + itoa(from.sin_port) + "(ID = " + itoa(requestCount) + "): " + message);

                thread _parser(parser, message, requestCount);
                _parser.detach();
            }
        }
    }
}; // namespace njudgerd
