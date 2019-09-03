#pragma once

#include "leveldb/db.h"
#include "leveldb/slice.h"
#include "leveldb/comparator.h"
#include "leveldb/write_batch.h"

#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include "common/ConfigInfo.h"
#include "execution/DataRow.h"
#include "execution/ExecutionResult.h"

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

class LevelDBBatch {
	friend class LevelDBHandler;
public:
	void insert(const std::vector<std::byte>& key, const std::vector<std::byte>& value);
private:
	leveldb::WriteBatch m_batch;
};
class LevelDBIterator {
	friend class LevelDBHandler;
public:
	~LevelDBIterator() ;
	void seek(const DataRow& key);
	void first() {
		m_pIter->SeekToFirst();
	}
	void last() {
		m_pIter->SeekToLast();
	}
	void next() {
		m_pIter->Next();
	}

	void prev() {
		m_pIter->Prev();
	}
	DataRow key( const std::vector<DBDataType>& types) {
		auto data = m_pIter->key();
		return DataRow(reinterpret_cast<const std::byte*>(data.data()), types,	data.size());
	}
	DataRow value( const std::vector<DBDataType>& types) {
		auto data = m_pIter->value();
		return DataRow(reinterpret_cast<const std::byte*>(data.data()), types,	data.size());
	}
	bool valid() {
		return m_pIter->Valid();
	}

private:
	leveldb::ReadOptions m_options;
	std::unique_ptr<leveldb::Iterator> m_pIter;
	leveldb::DB* m_pDB;
};

using LevelDBIteratorPtr = std::shared_ptr<LevelDBIterator>;

class LevelDBHandler {
public:
	LevelDBHandler(const TableInfo* pTable);

	void commit(LevelDBBatch& batch);
	LevelDBIteratorPtr createIterator();
	static LevelDBHandler* getHandler(const TableInfo* pTable);

	uint64_t getCost(const DataRow& start,	const DataRow& end);
private:
	std::unique_ptr<leveldb::DB> m_pDB;
	const TableInfo* m_pTable;

	KeyComparator m_compare;
};
