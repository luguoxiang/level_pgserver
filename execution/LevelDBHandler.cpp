#include "LevelDBHandler.h"
#include "execution/DataRow.h"
#include "execution/ExecutionException.h"
#include <sstream>
#include <cassert>
#include <set>


namespace {
class KeyComparator: public leveldb::Comparator {
public:
	KeyComparator(const TableInfo* pTable) {
		std::ostringstream os;
		for (size_t i = 0; i < pTable->getKeyCount(); ++i) {
			auto pColumn = pTable->getKeyColumn(i);
			m_keyTypes.push_back(pColumn->m_type);
			os << GetTypeName(pColumn->m_type);
			os << ",";
		}
		m_sName = os.str();
	}
	virtual ~KeyComparator() = default;

	virtual int Compare(const leveldb::Slice& a, const leveldb::Slice& b)
			const override {
		DataRow rowa(reinterpret_cast<const std::byte*>(a.data()), m_keyTypes,	a.size());
		DataRow rowb(reinterpret_cast<const std::byte*>(b.data()), m_keyTypes,	b.size());
		return rowa.compare(rowb);
	}

	virtual const char* Name() const override {
		return m_sName.c_str();
	}

	virtual void FindShortestSeparator(std::string* start,
			const leveldb::Slice& limit) const override {
	}

	virtual void FindShortSuccessor(std::string* key) const override {
	}
private:
	std::vector<DBDataType> m_keyTypes;
	std::string m_sName;
};

static std::mutex s_mutex;
static std::map<const TableInfo*, std::unique_ptr<LevelDBHandler>> s_handlers;
}


LevelDBHandler* LevelDBHandler::getHandler(const TableInfo* pTable){
	std::lock_guard < std::mutex > lock(s_mutex);

	auto iter = s_handlers.find(pTable);
	if (iter == s_handlers.end()) {
		auto [iter, newValue] = s_handlers.insert(
				std::make_pair(pTable, std::make_unique<LevelDBHandler>(pTable)));
		assert(newValue);
		return iter->second.get();
	} else {
		return iter->second.get();
	}
}

LevelDBHandler::LevelDBHandler(const TableInfo* pTable) : m_pTable(pTable), m_pCompare(new KeyComparator(pTable)){
	leveldb::DB* pDB;
	leveldb::Options options;
	options.create_if_missing = true;


	options.comparator = m_pCompare.get();
	std::string_view sPath = pTable->getAttribute("path");

	leveldb::Status status = leveldb::DB::Open(options,
			std::string(sPath.data(), sPath.length()), &pDB);
	if (!status.ok()) {
		EXECUTION_ERROR("Failed to open leveldb ", sPath, ": ", status.ToString());
	}
	m_pDB.reset(pDB);
}

void LevelDBBatch::insert(const std::string_view key,	const std::string_view value) {
	m_batch.Put(leveldb::Slice(key.data(), key.length()), leveldb::Slice(value.data(), value.length()));
}

void LevelDBHandler::commit(LevelDBBatch& batch) {
	leveldb::Status status = m_pDB->Write(leveldb::WriteOptions(), &batch.m_batch);
	if (!status.ok()) {
		EXECUTION_ERROR("Failed to write to leveldb", status.ToString());
	}
}
