#include <string>
#include <getopt.h>
#include <unistd.h>

#include "PgServer.h"
#include "common/IOException.h"
#include "common/Log.h"
#include "common/MetaConfig.h"
#include "config.h"

void printHelp() {
	printf("csv2pgserver version %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
	printf("\t-p, --port=PORT\t\tserver port.\n");
	printf(
			"\t-l, --loglevel=LEVEL\tlog level, can be DEBUG,WARN,INFO,ERROR.\n");
	printf("\t--workernum=NUM\t\tWorking process number.\n");
	printf("\t--timeout=NUM\t\tsocket read write timeout.\n");
	printf(
			"\t--logpath=PATH\t\tlog path, print on standard screen if not specified.\n");
	printf("\t--config=PATH\t\tconfigure file path.\n");
	printf(
			"\t--netbuffer=NUM\tnetwork send buffer size(bytes), default is 1M.\n");
	printf(
			"\t--execbuffer=NUM\texecution buffer size(bytes), default is 64M\n");
	printf("\t--help\t\t\tPrint this message.\n\n");
	exit(1);
}

static struct option long_options[] = { { "port", 1, 0, 'p' }, { "logpath", 1,
		0, 0 }, { "config", 1, 0, 0 }, { "loglevel", 1, 0, 'l' }, { "workernum",
		1, 0, 0 }, { "timeout", 1, 0, 0 }, { "execbuffer", 1, 0, 0 }, {
		"netbuffer", 1, 0, 0 }, { "floatmultiply", 1, 0, 0 },
		{ "help", 0, 0, 0 }, { 0, 0, 0, 0 }, };

int main(int argc, char** argv) {
	std::string sPort = "5433";
	int opt;
	std::string sLogPath = "";
	std::string sLogLevel = "info";
	int option_index;

	std::string sMetaConfigPath ="meta.conf";

	while ((opt = getopt_long(argc, argv, "h:p:l:s", long_options,
			&option_index)) != EOF) {
		switch (opt) {
		case 0: {
			std::string sArgName = long_options[option_index].name;
			if (sArgName == "logpath") {
				sLogPath = optarg;
			} else if (sArgName == "workernum") {
				MetaConfig::getInstance().setWorkerNum(atoi(optarg));
			} else if (sArgName == "timeout") {
				MetaConfig::getInstance().setTimeout(atoi(optarg));
			} else if (sArgName == "netbuffer") {
				MetaConfig::getInstance().setNetworkBuffer(atoi(optarg));
			} else if (sArgName == "execbuffer") {
				MetaConfig::getInstance().setExecutionBuffer(atoi(optarg));
			} else if (sArgName == "config") {
				sMetaConfigPath = optarg;
			} else {
				printHelp();
				return 1;
			}
			break;
		}
		case 'p':
			sPort = optarg;
			break;
		case 'l':
			sLogLevel = optarg;
			for(auto& c : sLogLevel)
			{
			   c = std::tolower(c);
			}
			break;
		default:
			printHelp();
			return 1;
		}
	}

	LogLevel level = LogLevel::INFO;
	if (sLogLevel == "debug") {
		level = LogLevel::DEBUG;
	} else if (sLogLevel ==  "warn") {
		level = LogLevel::WARN;
	} else if (sLogLevel == "info") {
		level = LogLevel::INFO;
	} else if (sLogLevel ==  "error") {
		level = LogLevel::ERROR;
	} else {
		fprintf(stderr, "Unknown log level %s!", sLogLevel.c_str());
		return 1;
	}
	printf("Log on %s, level %s.\n", sLogPath == "" ? "stdout" : sLogPath.c_str(), sLogLevel.c_str());

	Log::getLogger().init(sLogPath, level);
	LOG(INFO, "csv2pgserver version %d.%d started, config file %s.",
			VERSION_MAJOR, VERSION_MINOR, sMetaConfigPath.c_str());
	LOG(INFO, "Network buffer %d, timeout %d, Execution buffer %d, ",
			MetaConfig::getInstance().getNetworkBuffer(),
			MetaConfig::getInstance().getTimeout(),
			MetaConfig::getInstance().getExecutionBuffer());

	try {
		MetaConfig::getInstance().load(sMetaConfigPath);
		PgServer server(sPort);
		server.run();
	} catch (Exception* pe) {
		LOG(ERROR, "start server failed:%s", pe->what().c_str());
		delete pe;
		return 1;
	}
	return 0;
}
