#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "PgServer.h"
#include "common/IOException.h"
#include "common/MetaConfig.h"
#include "config.h"

DEFINE_int32(port, 5433, "Server listen port.");
DEFINE_int32(workernum, 20, "Working process number.");
DEFINE_int32(timeout, 60, "socket read write timeout.");
DEFINE_string(metaConfigPath, "meta.conf", "configure file path.");

int main(int argc, char** argv) {
	google::SetUsageMessage("help message");

	std::ostringstream version;
	version <<VERSION_MAJOR<<"."<<VERSION_MINOR;
	google::SetVersionString(version.str());

	google::ParseCommandLineFlags(&argc, &argv, true);
	google::InitGoogleLogging(argv[0]);

	MetaConfig::getInstance().setTimeout(FLAGS_timeout);
	MetaConfig::getInstance().setWorkerNum(FLAGS_workernum);

	LOG(INFO)<< "csv2pgserver version "<<version.str()<<" started, config file " <<FLAGS_metaConfigPath;
	LOG(INFO)<< "timeout "<<MetaConfig::getInstance().getTimeout();

	try {
		MetaConfig::getInstance().load(FLAGS_metaConfigPath);
		PgServer server(FLAGS_port);
		server.run();
	} catch (Exception* pe) {
		LOG(ERROR)<< "start server failed:" << pe->what();
		delete pe;
		return 1;
	}
	return 0;
}
