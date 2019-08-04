#ifndef HBASE_CONNECTION_H
#define HBASE_CONNECTION_H

#include "common/ParseNode.h"
#include "oceanbase.h"
#include "ExecutionException.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>
#include "Hbase.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::hadoop::hbase::thrift;

class HBaseConnection
{
public:
	HBaseConnection();
	~HBaseConnection();

	HbaseClient* getClient()
	{
		return m_pClient.get();
	}

private:
  boost::shared_ptr<TSocket> m_pSocket;
  boost::shared_ptr<TTransport> m_pTransport;
  boost::shared_ptr<TProtocol> m_pProtocol;
  boost::shared_ptr<HbaseClient> m_pClient;


};

#endif  // HBASE_CONNECTION_H
