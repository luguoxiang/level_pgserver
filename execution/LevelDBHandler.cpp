#include "LevelDBHandler.h"

#include "execution/ExecutionException.h"
#include <sstream>
#include <cassert>
#include <set>


namespace {
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

LevelDBHandler::LevelDBHandler(const TableInfo* pTable) : m_pTable(pTable), m_compare(pTable){
	leveldb::DB* pDB;
	leveldb::Options options;
	options.create_if_missing = true;


	options.comparator = &m_compare;
	std::string_view sPath = pTable->getAttribute("path");

	leveldb::Status status = leveldb::DB::Open(options,
			std::string(sPath.data(), sPath.length()), &pDB);
	if (!status.ok()) {
		EXECUTION_ERROR("Failed to open leveldb ", sPath, ": ", status.ToString());
	}
	m_pDB.reset(pDB);
}

void LevelDBBatch::insert(const std::vector<std::byte>& key, const std::vector<std::byte>& value) {
	m_batch.Put(leveldb::Slice((const char*)key.data(), key.size()), leveldb::Slice((const char*)value.data(), value.size()));
}

void LevelDBBatch::remove(const std::string_view key) {
	m_batch.Delete(leveldb::Slice((const char*)key.data(), key.size()));
}

void LevelDBHandler::commit(LevelDBBatch& batch) {
	leveldb::Status status = m_pDB->Write(leveldb::WriteOptions(), &batch.m_batch);
	if (!status.ok()) {
		EXECUTION_ERROR("Failed to write to leveldb", status.ToString());
	}
}
void LevelDBIterator::seek(const std::vector<std::byte>& key) {
	m_pIter->Seek(leveldb::Slice((const char*)key.data(), key.size()));
}
LevelDBIterator::~LevelDBIterator() {
	m_pIter.reset(nullptr);
	m_pDB->ReleaseSnapshot(m_options.snapshot);
}
LevelDBIteratorPtr LevelDBHandler::createIterator() {
	LevelDBIteratorPtr pIter = std::make_shared<LevelDBIterator>();
	pIter->m_options.snapshot = m_pDB->GetSnapshot();

	pIter->m_pIter.reset(m_pDB->NewIterator(pIter->m_options));
	pIter->m_pDB = m_pDB.get();
	return pIter;
}

uint64_t LevelDBHandler::getCost(const std::vector<std::byte>& start, const std::vector<std::byte>& end) {
	leveldb::Range ranges[2];
	ranges[0] = leveldb::Range(leveldb::Slice((const char*)start.data(), start.size()), leveldb::Slice((const char*)end.data(), end.size()));
	uint64_t sizes[1];
	m_pDB->GetApproximateSizes(ranges, 2, sizes);
	return sizes[0];
}
