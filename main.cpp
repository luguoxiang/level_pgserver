#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "PgServer.h"
#include "common/IOException.h"
#include "common/MetaConfig.h"
#include "config.h"

DEFINE_string(logLevel, "info", "Log level, can be DEBUG,WARN,INFO,ERROR.");
DEFINE_int32(port, 5433, "Server listen port.");
DEFINE_int32(workernum, 20, "Working process number.");
DEFINE_int32(timeout, 60, "socket read write timeout.");
DEFINE_string(metaConfigPath, "meta.conf", "configure file path.");

int main(int argc, char** argv) {

	google::ParseCommandLineFlags(&argc, &argv, true);
	google::InitGoogleLogging(argv[0]);

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
		LOG(ERROR)<< "Unknown log level << FLAGS_logLevel;
		return 1;
	}

	MetaConfig::getInstance().setTimeout(FLAGS_timeout);
	MetaConfig::getInstance().setWorkerNum(FLAGS_workernum);

	LOG(INFO)<< "csv2pgserver version "<<VERSION_MAJOR<<"."<<VERSION_MINOR<<" started, config file " <<FLAGS_metaConfigPath;
	LOG(INFO) << "timeout "<<MetaConfig::getInstance().getTimeout();

	try {
		MetaConfig::getInstance().load(FLAGS_metaConfigPath);
		PgServer server(FLAGS_port);
		server.run();
	} catch (Exception* pe) {
		LOG(ERROR) << "start server failed:" << pe->what();
		delete pe;
		return 1;
	}
	return 0;
}
