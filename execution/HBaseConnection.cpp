#include "HBaseConnection.h"
#include "ExecutionException.h"
#include <assert.h>
#include "common/Log.h"
#include "common/MetaConfig.h"
#include "execution/ParseTools.h"

HBaseConnection::HBaseConnection()
{
	ServerInfo* pServerInfo = MetaConfig::getInstance().getServerInfo("hbase");
	if(pServerInfo == NULL)
	{
		throw new ExecutionException("hbase connection is not defined!", false);
	}
	const char* pszHost = pServerInfo->getAttribute("host").c_str();
	const char* pszPort = pServerInfo->getAttribute("port").c_str();
	int port = atoi(pszPort);

	LOG(INFO, "start hbase thrift connection to %s:%d", pszHost, port);
  m_pSocket.reset(new TSocket(pszHost, port));
  m_pTransport.reset(new TBufferedTransport(m_pSocket));
  m_pProtocol.reset(new TBinaryProtocol(m_pTransport));
  m_pClient.reset(new HbaseClient(m_pProtocol));
	m_pTransport->open();
}

HBaseConnection::~HBaseConnection()
{
	m_pTransport->close();
}

