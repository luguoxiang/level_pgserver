#include <limits>

#include <glog/logging.h>

#include "common/MetaConfig.h"
#include "execution/DBDataTypeHandler.h"
#include "server/PgServer.h"
#include "server/PgMessageWriter.h"
#include "config.h"

int main(int argc, char** argv) {
	google::InitGoogleLogging(argv[0]);

	LOG(INFO)<< "level_pgserver version "<<VERSION_MAJOR<<"."<<VERSION_MINOR<<" started";
	DBDataTypeHandler::init();
	PgMessageWriter::init();

	static_assert(std::numeric_limits<float>::is_iec559);
	static_assert(std::numeric_limits<double>::is_iec559);

	try {
		const char* pszConfigPath = "meta.conf";
		if(argc > 1) {
			pszConfigPath = argv[1];
			LOG(INFO) << "config: "<<pszConfigPath;
		}
		MetaConfig::getInstance().load(pszConfigPath);

		PgServer::getInstance().run();
	} catch (std::exception& e) {
		LOG(ERROR)<< "start server failed:" << e.what();
		return 1;
	}
	return 0;
}
