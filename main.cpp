#include <limits>

#include <glog/logging.h>

#include "PgServer.h"
#include "IOException.h"
#include "common/MetaConfig.h"
#include "execution/DataRow.h"
#include "execution/ExecutionResult.h"
#include "config.h"

int main(int argc, char** argv) {
	google::InitGoogleLogging(argv[0]);

	LOG(INFO)<< "csv2pgserver version "<<VERSION_MAJOR<<"."<<VERSION_MINOR<<" started";
	DataRow::init();
	ExecutionResult::init();

	static_assert(std::numeric_limits<float>::is_iec559);
	static_assert(std::numeric_limits<double>::is_iec559);

	try {
		MetaConfig::getInstance().load("meta.conf");
		PgServer server(MetaConfig::getInstance().getPort());
		server.run();
	} catch (Exception* pe) {
		LOG(ERROR)<< "start server failed:" << pe->what();
		delete pe;
		return 1;
	}
	return 0;
}
