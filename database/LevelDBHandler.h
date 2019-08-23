#pragma once

#include "leveldb/db.h"
#include "leveldb/comparator.h"

#include <memory>
#include <vector>
#include "common/ConfigInfo.h"

class KeyComparator: public leveldb::Comparator {
public:
	KeyComparator(const TableInfo* pTable);
	virtual ~KeyComparator() {
	}

	virtual int Compare(const leveldb::Slice& a, const leveldb::Slice& b) const
			override;

	virtual const char* Name() const override;

	virtual void FindShortestSeparator(std::string* start,
			const leveldb::Slice& limit) const override {
	}

	virtual void FindShortSuccessor(std::string* key) const override {
	}
private:
	std::vector<DBDataType> m_keyTypes;
	std::string m_sName;
};

class LevelDBHandler {
public:
	LevelDBHandler(const TableInfo* pTable);
	~LevelDBHandler() {
	}
private:
	std::unique_ptr<leveldb::DB> m_pDB;
	std::unique_ptr<KeyComparator> m_pCompare;

};
