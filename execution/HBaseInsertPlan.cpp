#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common/ParseException.h"
#include "common/ConfigException.h"
#include "common/Log.h"
#include "common/MetaConfig.h"
#include "execution/ParseTools.h"
#include "execution/HBaseInsertPlan.h"
#include "execution/ExecutionException.h"
#include "execution/ObConnection.h"
#include "WorkThreadInfo.h"

#define LOAD_CACHE_NUM 1000

HBaseInsertPlan::HBaseInsertPlan(TableInfo* pTableInfo) :
		ExecutionPlan(HBaseInsert), m_pTableInfo(pTableInfo), m_mutations(LOAD_CACHE_NUM), m_iInsertRows(0), m_pDataPlan(0)
{
	assert(pTableInfo);

	m_pInfo = (WorkThreadInfo*) pthread_getspecific(WorkThreadInfo::tls_key);
	assert(m_pInfo);
	if(m_pTableInfo->hasAttribute("cf"))
	{
		m_cf = m_pTableInfo->getAttribute("cf") + ":";
	}
	else
	{
		throw new ConfigException("hbase table missing cf attribute", false);
	}
}

HBaseInsertPlan::~HBaseInsertPlan()
{
	if (m_pDataPlan != NULL) {
		delete m_pDataPlan;
		m_pDataPlan = NULL;
	}
}

void HBaseInsertPlan::setValuePlan(ExecutionPlan* pDataPlan)
{
	if (pDataPlan->getResultColumns() != m_columns.size()) {
		throw new ExecutionException(
				"the numbers of insert columns and values are not matched",
				false);
	}
	m_pDataPlan = pDataPlan;
}

void HBaseInsertPlan::explain(std::vector<std::string>& rows)
{
	if (m_pDataPlan->getType() == ExecutionPlan::Const) {
		m_pDataPlan->begin();
		prepareParseNode();
		while (m_pDataPlan->next()) {
			ImplicitRowKeyRange rowkey(m_pTableInfo);
			buildRowKey(rowkey);

			doExplain(rowkey, m_pTableInfo->getName(), rows);
		}
		m_pDataPlan->end();
	} else {
		m_pDataPlan->explain(rows);
		std::string s = "hbase:insert ";
		s.append(m_pTableInfo->getName());
		s.append("(");
		for (size_t i = 0; i < m_columns.size(); ++i) {
			s.append(m_columns[i]->m_sName);
			s.append(",");
		}
		s.append(")");
		rows.push_back(s);
	}

}

void HBaseInsertPlan::addColumn(DBColumnInfo* pColumn)
{
	m_columns.push_back(pColumn);
}

void HBaseInsertPlan::cancel()
{
	m_pDataPlan->cancel();
}

void HBaseInsertPlan::begin()
{
	assert(m_pDataPlan);

	m_iInsertRows = 0;

	if (m_columns.empty()) {
		throw new ParseException("no insert columns!");
	}

	m_pDataPlan->begin();
	prepareParseNode();
}

void HBaseInsertPlan::prepareParseNode()
{
	m_pRow = new (*m_pInfo) ParseNode();
	m_pRow->m_iType = PARENT_NODE;
	m_pRow->m_iChildNum = m_columns.size();
	m_pRow->m_pszExpr = NULL;
	m_pRow->m_fnBuildPlan = NULL;
	m_pRow->m_children = new (*m_pInfo) ParseNode*[m_columns.size()];
	for (size_t i = 0; i < m_pRow->m_iChildNum; ++i) { 
		ParseNode* pNode = new(*m_pInfo) ParseNode();
		m_pRow->m_children[i] = pNode;
		pNode->m_iChildNum = 0;
		pNode->m_pszExpr = NULL;
		pNode->m_fnBuildPlan = NULL;
	}
}

void HBaseInsertPlan::commitData()
{
	if(m_mutations.empty()) return;
	try
	{
		HBaseConnection& conn = m_pInfo->getHBaseConnection();
		HbaseClient* pClient = conn.getClient();
		std::string sTableName(m_pTableInfo->getName());
		pClient->mutateRows(sTableName, m_mutations);
		m_mutations.clear();
	}
	catch(const IOError& e)
	{
			throw new ExecutionException(e.message.c_str(), false);
	}
	catch(const TException& ioe)
	{
			throw new ExecutionException(ioe.what(), false);
	}
}

void HBaseInsertPlan::end()
{

	m_pDataPlan->end();
	commitData();
}

void HBaseInsertPlan::checkValueForInteger(DBColumnInfo* pColumn, ResultInfo& info, DBDataType type)
{
	switch(type)
	{
	case TYPE_INT64:
	case TYPE_INT32:
	case TYPE_INT16:
	case TYPE_INT8:
		break;
	default:
		char msg[200];
		const char* pszName = pColumn->m_sName.c_str();
		snprintf(
				msg,
				200,
				"Insert data type is not matched with column %s's schema, int expected!",
				pszName);
		throw new ExecutionException(msg, 200);
	}
}

void HBaseInsertPlan::checkValueForString(DBColumnInfo* pColumn, ResultInfo& info, DBDataType type)
{
	if (type != TYPE_BYTES && type != TYPE_STRING) {
		char msg[200];
		const char* pszName = pColumn->m_sName.c_str();
		snprintf(msg, 200,
				"Insert data type is not matched with column %s's schema, string expected!",
				pszName);
		throw new ExecutionException(msg, 200);
	}
}

void HBaseInsertPlan::doExplain(ImplicitRowKeyRange& rowkey, const char* pszTableName, std::vector<std::string>& rows)
{
	const char* pszRowKey = rowkey.getStartRowKey();
	assert(pszRowKey && rowkey.getEndRowKey());
	int64_t iRowKeyLen = rowkey.getStartLength();
	if (rowkey.isRangeSearch()) {
		throw new ExecutionException("Missing rowkey columns!", false);
	}

	ResultInfo info;

	char msg[200];
	const char* pszExplainRowKey = Tools::byteToString(pszRowKey, iRowKeyLen);
	snprintf(msg, 200, "hbase:insert table=%s,rowkey=%s", pszTableName, pszExplainRowKey);
	rows.push_back(msg);

	for (size_t i = 0; i < m_columns.size(); ++i) {

		const char* pszName = m_columns[i]->m_sName.c_str();

		m_pDataPlan->getResult(i, &info);
		DBDataType type = m_pDataPlan->getResultType(i);
		switch (m_columns[i]->m_type) {
		case TYPE_INT8:
		case TYPE_INT16:
		case TYPE_INT32:
		case TYPE_INT64:{
			checkValueForInteger(m_columns[i], info, type);
			snprintf(msg, 200, "hbase:insert_int column=%s,value=%lu",
					pszName, info.m_value.m_lResult);
			break;
		}
		case TYPE_BYTES:
		case TYPE_STRING: {
		  checkValueForString(m_columns[i], info, type);
			snprintf(msg, 200, "hbase:insert_string column=%s,value=%s",
					pszName, info.m_value.m_pszResult);
			break;
		}
		default:
			throw new ExecutionException("Unsupported data type!", false);
		} //switch
		rows.push_back(msg);
	} //for
}

void HBaseInsertPlan::doInsert(ImplicitRowKeyRange& rowkey, const char* pszTableName)
{
	const char* pszRowKey = rowkey.getStartRowKey();
	assert(pszRowKey && rowkey.getEndRowKey());
	int64_t iRowKeyLen = rowkey.getStartLength();
	if (rowkey.isRangeSearch()) {
		throw new ExecutionException("Missing rowkey columns!", false);
	}

	BatchMutation batch_mutation;
	batch_mutation.row.assign(pszRowKey, iRowKeyLen);
	ResultInfo info;
	for (size_t i = 0; i < m_columns.size(); ++i) {

		const char* pszName = m_columns[i]->m_sName.c_str();

		m_pDataPlan->getResult(i, &info);
		DBDataType type = m_pDataPlan->getResultType(i);
		Mutation mutation;
		mutation.column = m_cf + pszName;
		switch (m_columns[i]->m_type) {
		case TYPE_INT8:{
			checkValueForInteger(m_columns[i], info, type);
			char data = info.m_value.m_lResult & 0xff;
			mutation.value.assign(&data, 1); 
			break;
		}
		case TYPE_INT16:{
			checkValueForInteger(m_columns[i], info, type);
			char data[2];
			data[0] = (info.m_value.m_lResult >> 8) & 0xff;
			data[1] = info.m_value.m_lResult & 0xff;
			mutation.value.assign(data, 2); 
			break;
		}
		case TYPE_INT32:{
			checkValueForInteger(m_columns[i], info, type);
			char data[4];
			data[0] = (info.m_value.m_lResult >> 24) & 0xff;
			data[1] = (info.m_value.m_lResult >> 16) & 0xff;
			data[2] = (info.m_value.m_lResult >> 8) & 0xff;
			data[3] = info.m_value.m_lResult & 0xff;
			mutation.value.assign(data, 4); 
			break;
		}
		case TYPE_INT64:{
			checkValueForInteger(m_columns[i], info, type);
			char data[8];
			data[0] = (info.m_value.m_lResult >> 56) & 0xff;
			data[1] = (info.m_value.m_lResult >> 48) & 0xff;
			data[2] = (info.m_value.m_lResult >> 40) & 0xff;
			data[3] = (info.m_value.m_lResult >> 32) & 0xff;
			data[4] = (info.m_value.m_lResult >> 24) & 0xff;
			data[5] = (info.m_value.m_lResult >> 16) & 0xff;
			data[6] = (info.m_value.m_lResult >> 8) & 0xff;
			data[7] = info.m_value.m_lResult & 0xff;
			mutation.value.assign(data, 8); 
			break;
		}
		case TYPE_BYTES:
		case TYPE_STRING: {
			checkValueForString(m_columns[i], info, type);
			mutation.value.assign(info.m_value.m_pszResult, info.m_len);
			break;
		}
		default:
			throw new ExecutionException("Unsupported data type!", false);
		} //switch
		batch_mutation.mutations.push_back(mutation);
	} //for
	m_mutations.push_back(batch_mutation);
}

void HBaseInsertPlan::buildRowKey(ImplicitRowKeyRange& rowkey)
{
	ResultInfo info;
	for (size_t i = 0; i < m_columns.size(); ++i) {
		if (m_columns[i]->m_iKeyIndex >= 0) {
			m_pDataPlan->getResult(i, &info);
			ParseNode* pChild = m_pRow->m_children[i];
			switch (m_pDataPlan->getResultType(i)) {
			case TYPE_INT64:
				pChild->m_iType = INT_NODE;
				pChild->m_iValue = info.m_value.m_lResult;
				break;
			case TYPE_DATE:
			case TYPE_DATETIME:
				pChild->m_iType = DATE_NODE;
				pChild->m_iValue = info.m_value.m_time.tv_sec * 1000000
						+ info.m_value.m_time.tv_usec;
				break;
			case TYPE_STRING:
				pChild->m_iType = STR_NODE;
				pChild->m_iValue = info.m_len;
				pChild->m_pszValue = info.m_value.m_pszResult;
				break;
			case TYPE_BYTES:
				pChild->m_iType = BINARY_NODE;
				pChild->m_pszValue = info.m_value.m_pszResult;
				pChild->m_iValue = info.m_len;
				break;
			default:
				throw new ExecutionException("Unsupport insert rowkey type!",
						false);
			}
			rowkey.setColumnValue(m_columns[i], m_pRow->m_children[i]);
		}
	}
	rowkey.done();
}

bool HBaseInsertPlan::next()
{
	if (!m_pDataPlan->next())
		return false;

	ImplicitRowKeyRange rowkey(m_pTableInfo);
	buildRowKey(rowkey);

	doInsert(rowkey, m_pTableInfo->getName());
	++m_iInsertRows;
	if((m_iInsertRows % LOAD_CACHE_NUM) == 0)
	{
		LOG(INFO, "ob_insert: table=%s,count=%d",
				m_pTableInfo->getName(), m_iInsertRows);
		commitData();
	}
	return true;
}
