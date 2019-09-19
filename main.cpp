#include <glog/logging.h>

#include "common/MetaConfig.h"
#include "execution/DBDataTypeHandler.h"
#include "server/PgServer.h"
#include "server/PostgresProtocol.h"
#include "config.h"

int main(int argc, char** argv) {
	google::InitGoogleLogging(argv[0]);

	LOG(INFO)<< "level_pgserver version "<<VERSION_MAJOR<<"."<<VERSION_MINOR<<" started";
	DBDataTypeHandler::init();
	PostgresProtocol::init();

	try {
		const char* pszConfigPath = "meta.conf";
		if(argc > 1) {
			pszConfigPath = argv[1];
			LOG(INFO) << "config: "<<pszConfigPath;
		}
		MetaConfig::getInstance().load(pszConfigPath);

		PgServer server(MetaConfig::getInstance().getPort());
		server.run();
	} catch (std::exception& e) {
		LOG(ERROR)<< "start server failed:" << e.what();
		return 1;
	}
	return 0;
}
