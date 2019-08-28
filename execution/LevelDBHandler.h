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

	KeyComparator m_compare;
};
