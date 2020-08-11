#include "general.hpp"
#include "server.hpp"
#include "initializer.hpp"
#include "jobController.hpp"
#include "../common/utils.hpp"
#include "../common/confLoader.hpp"

namespace njudgerd
{
    ConfLoader confLoader;

    void showHelp()
    {
        cout << "Usage: judgerd [OPTIONS]" << endl
             << endl;
        cout << "Start njudgerd server." << endl
             << endl;
        cout << "Options: " << endl;
        cout << "  -d, --daemon          Run as daemon." << endl;
        cout << "  -h  --help            Display this help and exit." << endl;
        cout << "      --conf=FILE       Load the file as the configuration file." << endl;
        exit(0);
    }

    void argParser(int argc, char **argv)
    {
        opterr = 0;
        option long_options[] =
            {
                {"daemon", no_argument, 0, 'd'},
                {"conf", required_argument, 0, 'c'},
                {"help", no_argument, 0, 'h'},
                {0, 0, 0, 0}};

        int c, option_index;
        for (; (c = getopt_long(argc, argv, "dh", long_options, &option_index)) != -1;)
        {
            if (c == 'h')
                showHelp();
            if (c == 'd')
                conf.daemon = true;
            if (c == 'c')
            {
                string errorMsg;
                if (confLoader.load(optarg, 1, errorMsg))
                {
                    logger.print(errorMsg);
                    terminatePro(ERR_LOAD_CONF_ERROR);
                }
            }
            if (c == '?')
            {
                logger.print("Command format error");
                terminatePro(ERR_INVALID_COMMAND_FORMAT);
            }
        }
        if (!confLoader.isInited())
        {
            string errorMsg;
            if (confLoader.load("njudgerd", 0, errorMsg))
            {
                logger.print(errorMsg);
                terminatePro(ERR_LOAD_CONF_ERROR);
            }
        }
    }

    void loadConf()
    {
        CJsonObject _conf = confLoader.getConf();
        // description
        if (_conf["description"].ToString() != "\"Njudgerd Configuration File\"")
        {
            logger.print("Invalid configuration file: " + confLoader.getFileName() +
                         " The file must be in JSON format containing a field of \"description\" with value " +
                         "\"Njudgerd Configuration File\"");
            terminatePro(ERR_LOAD_CONF_ERROR);
        }

        // server.port
        _conf["server"].Get("port", conf.server.port);
        if (conf.server.port <= 0 || conf.server.port > 65535)
        {
            logger.print("Invalid configuration for server.port: the port must be between 1 to 65535");
            terminatePro(ERR_INVALID_CONF);
        }

        // job Controller
        _conf["jobs"].Get("maxCount", conf.jobs.maxCount);
        _conf["jobs"].Get("maxThreads", conf.jobs.maxThreads);

        if (conf.jobs.maxCount < 0)
        {
            logger.print("Invalid configuration for jobs.maxCount");
            terminatePro(ERR_INVALID_CONF);
        }
        if (conf.jobs.maxThreads < 0)
        {
            logger.print("Invalid configuration for jobs.maxThreads");
            terminatePro(ERR_INVALID_CONF);
        }

        if (conf.jobs.maxCount == 0)
            conf.jobs.maxCount = 1 << 30;
        if (conf.jobs.maxThreads == 0)
            conf.jobs.maxThreads = 1 << 30;
            
        // listener
        _conf["listener"].Get("host", conf.listener.host);
        _conf["listener"].Get("port", conf.listener.port);

        // njudger.location
        _conf["njudger"].Get("location", conf.njudger.location);
        if (getFileType(conf.njudger.location) != 2)
        {
            logger.print("Can't find njudger in " + conf.njudger.location);
            terminatePro(ERR_INVALID_CONF);
        }
    }

    void initialize(int argc, char **argv)
    {
        randomize();  // Initialize randomize seed
        createTempDir(); // Create temp directory

        // Parse arguments
        argParser(argc, argv), loadConf();

        // Create daemon
        if (conf.daemon)
        {
            if (daemon(1, 0))
            {
                logger.print("Create daemon failed", true);
                terminatePro(ERR_CREATE_DAEMON_ERROR);
            }
        }

        // Start server
        Server::init();
        thread _server(Server::run);
        thread _jobController(JobController::boss);

        _server.join();
        _jobController.join();
    }
} // namespace njudgerd
