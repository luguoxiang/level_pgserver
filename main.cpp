#include <limits>

#include <glog/logging.h>

#include "PgServer.h"
#include "common/Exception.h"
#include "common/MetaConfig.h"
#include "execution/DBDataTypeHandler.h"
#include "config.h"

int main(int argc, char** argv) {
	google::InitGoogleLogging(argv[0]);

	LOG(INFO)<< "level_pgserver version "<<VERSION_MAJOR<<"."<<VERSION_MINOR<<" started";
	DBDataTypeHandler::init();

	static_assert(std::numeric_limits<float>::is_iec559);
	static_assert(std::numeric_limits<double>::is_iec559);

	try {
		MetaConfig::getInstance().load("meta.conf");
		PgServer::getInstance().run();
	} catch (Exception* pe) {
		LOG(ERROR)<< "start server failed:" << pe->what();
		delete pe;
		return 1;
	}
	return 0;
}
