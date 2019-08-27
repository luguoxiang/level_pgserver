#pragma once

#include "leveldb/db.h"
#include "leveldb/slice.h"
#include "leveldb/write_batch.h"

#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include "common/ConfigInfo.h"
#include "execution/ExecutionResult.h"

namespace leveldb {
class Comparator {
public:
	virtual ~Comparator() = default;

	virtual int Compare(const leveldb::Slice& a, const leveldb::Slice& b)
			const = 0;

	virtual const char* Name() const = 0;

	virtual void FindShortestSeparator(std::string* start,
			const leveldb::Slice& limit) const = 0;

	virtual void FindShortSuccessor(std::string* key) const = 0;
};
}

class LevelDBBatch {
	friend class LevelDBHandler;
public:
	void insert(const std::string_view key,	const std::string_view value);
private:
	leveldb::WriteBatch m_batch;
};

class LevelDBHandler {
public:
	LevelDBHandler(const TableInfo* pTable);

	void commit(LevelDBBatch& batch);

	static LevelDBHandler* getHandler(const TableInfo* pTable);
private:
	std::unique_ptr<leveldb::DB> m_pDB;
	const TableInfo* m_pTable;

	std::unique_ptr<leveldb::Comparator> m_pCompare;
};
