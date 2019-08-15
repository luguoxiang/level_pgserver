#include <string>

#include <gflags/gflags.h>

#include "PgServer.h"
#include "common/IOException.h"
#include "common/Log.h"
#include "common/MetaConfig.h"
#include "config.h"

DEFINE_string(logLevel, "info", "Log level, can be DEBUG,WARN,INFO,ERROR.");
DEFINE_int32(port, 5433, "Server listen port.");
DEFINE_int32(workernum, 20, "Working process number.");
DEFINE_int32(timeout, 60, "socket read write timeout.");
DEFINE_string(metaConfigPath, "meta.conf", "configure file path.");

int main(int argc, char** argv) {

	google::ParseCommandLineFlags(&argc, &argv, true);

	LogLevel level = LogLevel::INFO;
	if (FLAGS_logLevel == "debug") {
		level = LogLevel::DEBUG;
	} else if (FLAGS_logLevel ==  "warn") {
		level = LogLevel::WARN;
	} else if (FLAGS_logLevel == "info") {
		level = LogLevel::INFO;
	} else if (FLAGS_logLevel ==  "error") {
		level = LogLevel::ERROR;
	} else {
		fprintf(stderr, "Unknown log level %s!", FLAGS_logLevel.c_str());
		return 1;
	}
	printf("Log on %s, level %s.\n", "stdout", FLAGS_logLevel.c_str());

	MetaConfig::getInstance().setTimeout(FLAGS_timeout);
	MetaConfig::getInstance().setWorkerNum(FLAGS_workernum);

	Log::getLogger().init("", level);
	LOG(INFO, "csv2pgserver version %d.%d started, config file %s.",
			VERSION_MAJOR, VERSION_MINOR, FLAGS_metaConfigPath.c_str());
	LOG(INFO, "Network buffer %d, timeout %d, Execution buffer %d, ",
			MetaConfig::getInstance().getNetworkBuffer(),
			MetaConfig::getInstance().getTimeout(),
			MetaConfig::getInstance().getExecutionBuffer());

	try {
		MetaConfig::getInstance().load(FLAGS_metaConfigPath);
		PgServer server(FLAGS_port);
		server.run();
	} catch (Exception* pe) {
		LOG(ERROR, "start server failed:%s", pe->what().c_str());
		delete pe;
		return 1;
	}
	return 0;
}
