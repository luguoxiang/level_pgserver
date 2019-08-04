#include <string.h>
#include <getopt.h>
#include <unistd.h>

#include "PgServer.h"
#include "common/IOException.h"
#include "common/Log.h"
#include "common/MetaConfig.h"
#include "config.h"

void printHelp()
{
#ifdef UPDATE_SUPPORT
	const char* pszUpdateInfo = "Update enabled";
#else
	const char* pszUpdateInfo = "Update disabled";
#endif
	printf("AnyConnector version %s(%s)\n", VERSION, pszUpdateInfo);
	printf("\t-p, --port=PORT\t\tOBConnector server port.\n");
	printf("\t-l, --loglevel=LEVEL\tlog level, can be DEBUG,WARN,INFO,ERROR.\n");
	printf("\t--workernum=NUM\t\tWorking process number.\n");
	printf("\t--timeout=NUM\t\tsocket read write timeout.\n");
	printf("\t--logpath=PATH\t\tlog path, print on standard screen if not specified.\n");
	printf("\t--config=PATH\t\tconfigure file path.\n");
	printf("\t--netbuffer=NUM\tnetwork send buffer size(bytes), default is 1M.\n");
	printf("\t--execbuffer=NUM\texecution buffer size(bytes), default is 64M\n");
	printf("\t--help\t\t\tPrint this message.\n\n");
	exit(1);
}

static struct option long_options[] = {
		{ "port", 1, 0, 'p' },
		{ "logpath", 1,	0, 0 },
		{ "config", 1, 0, 0 },
		{ "loglevel", 1, 0, 'l' },
		{ "workernum", 1, 0, 0 },
		{ "timeout", 1, 0, 0 },
		{ "execbuffer", 1, 0, 0 },
		{"netbuffer", 1, 0, 0 },
		{"floatmultiply", 1, 0, 0 },
		{ "help", 0, 0, 0 },
		{ 0, 0, 0, 0 },
};

int main(int argc, char** argv)
{
	const char* pszPort = "5433";
	int opt;
	const char* pszLogPath = 0;
	const char* pszLogLevel = "info";
	int option_index;

	std::string sMetaConfigPath("/usr/local/obconnector/conf/sell.conf");
	while ((opt = getopt_long(argc, argv, "h:p:l:s", long_options,
			&option_index)) != EOF)
	{
		switch (opt)
		{
		case 0: {
			std::string sArgName = long_options[option_index].name;
			if (sArgName == "logpath")
			{
				pszLogPath = optarg;
			}
			else if (sArgName == "workernum")
			{
				MetaConfig::getInstance().setWorkerNum(atoi(optarg));
			}
			else if (sArgName == "timeout")
			{
				MetaConfig::getInstance().setTimeout(atoi(optarg));
			}
			else if (sArgName == "netbuffer")
			{
				MetaConfig::getInstance().setNetworkBuffer(atoi(optarg));
			}
			else if (sArgName == "execbuffer")
			{
				MetaConfig::getInstance().setExecutionBuffer(atoi(optarg));
			}
			else if (sArgName == "config")
			{
				sMetaConfigPath = optarg;
			}
			else
			{
				printHelp();
				return 1;
			}
			break;
		}
		case 'p':
			pszPort = optarg;
			break;
		case 'l':
			pszLogLevel = optarg;
			break;
		default:
			printHelp();
			return 1;
		}
	}
	int64_t iValue = 1;
	bool bLittleEnd = ((*(char*) &iValue) == 1);
	if(!bLittleEnd)
	{
		fprintf(stderr, "little end is not support!");
		return 1;
	}

	Log::LogLevel level = Log::INFO;
	if (strcasecmp(pszLogLevel, "debug") == 0)
	{
		level = Log::DEBUG;
	}
	else if (strcasecmp(pszLogLevel, "warn") == 0)
	{
		level = Log::WARN;
	}
	else if (strcasecmp(pszLogLevel, "info") == 0)
	{
		level = Log::INFO;
	}
	else if (strcasecmp(pszLogLevel, "error") == 0)
	{
		level = Log::ERROR;
	}
	else
	{
		fprintf(stderr, "Unknown log level %s!", pszLogLevel);
		return 1;
	}
	printf("Log on %s, level %s.\n", pszLogPath == 0 ? "stdout" : pszLogPath,
			pszLogLevel);

	Log::getLogger().init(pszLogPath, level);
	LOG(INFO, "OBConnector version %s started, config file %s.",
			VERSION, sMetaConfigPath.c_str());
	LOG(INFO, "Network buffer %d, timeout %d, Execution buffer %d, ",
			MetaConfig::getInstance().getNetworkBuffer(),
			MetaConfig::getInstance().getTimeout(),
			MetaConfig::getInstance().getExecutionBuffer());

	try
	{
		MetaConfig::getInstance().load(sMetaConfigPath.c_str());
		PgServer server(pszPort);
		server.run();
	} catch (Exception* pe)
	{
		LOG(ERROR, "start server failed:%s", pe->what());
		delete pe;
		return 1;
	}
	return 0;
}
