#include "LevelDBHandler.h"
#include "DBException.h"
#include "execution/DataRow.h"
#include <sstream>
KeyComparator::KeyComparator(const TableInfo* pTable) {
	std::ostringstream os;
	for(size_t i = 0; i< pTable->getKeyCount(); ++i) {
		auto pColumn = pTable->getKeyColumn(i);
		m_keyTypes.push_back(pColumn->m_type);
		os<< GetTypeName(pColumn->m_type);
		os << ",";
	}
	m_sName = os.str();
}

int KeyComparator::Compare(const leveldb::Slice& a, const leveldb::Slice& b) const{
	DataRow rowa(reinterpret_cast<const std::byte*>(a.data()),m_keyTypes, a.size());
	DataRow rowb(reinterpret_cast<const std::byte*>(b.data()),m_keyTypes, b.size());
	return rowa.compare(rowb);
}

const char* KeyComparator::Name() const {
	return m_sName.c_str();
}

LevelDBHandler::LevelDBHandler(const TableInfo* pTable) {
	leveldb::DB* pDB;
	leveldb::Options options;
	options.create_if_missing = true;

	m_pCompare.reset(new KeyComparator(pTable));

	options.comparator = m_pCompare.get();
	std::string_view sPath = pTable->getAttribute("path");

	leveldb::Status status = leveldb::DB::Open(options, std::string(sPath.data(), sPath.length()), &pDB);
	if(!status.ok()) {
		DB_ERROR("Failed to open leveldb " , sPath, ": ", status.ToString());
	}
	m_pDB.reset(pDB);
}


