#pragma once

#include <set>
#include <vector>
#include "common/ParseNode.h"
#include "ExecutionResult.h"
#include "DataRow.h"
#include "LevelDBHandler.h"

class KeySearchRange {
public:
	KeySearchRange(std::vector<DBDataType>& types,
			const TableInfo* pTable,
			const ParseNode* pNode,
			std::set<std::string_view>* pSolved);

	KeySearchRange(const KeySearchRange&) = delete;
	KeySearchRange& operator =(const KeySearchRange&) = delete;

	const DataRow getStartRow() const {
		return DataRow(m_sStartRow.data(), m_keyTypes, m_sStartRow.size());
	}
	const DataRow getEndRow() const {
		return DataRow(m_sEndRow.data(), m_keyTypes, m_sEndRow.size());
	}

	const std::vector<ExecutionResult>& getStartResults() {
		return m_startKeyResults;
	}
	const std::vector<ExecutionResult>& getEndResults() {
		return m_endKeyResults;
	}

	bool startInclusive() const {
		return m_bStartInclusive;
	}
	bool endInclusive() const {
		return m_bEndInclusive;
	}

	void seekStart(LevelDBIteratorPtr& pDBIter);
	void seekStartReversed(LevelDBIteratorPtr& pDBIter);

	bool exceedEnd(const std::vector<ExecutionResult>& keyValues);
	bool exceedEndReversed(const std::vector<ExecutionResult>& keyValues);

	int compareStart(KeySearchRange* pRange);
	bool startAfterEnd(KeySearchRange* pRange);
private:
	struct KeyPredicateInfo {
		int m_iKeyIndex = -1;
		const ParseNode* m_pValue = nullptr;
		Operation m_op = Operation::NONE;
		std::string_view m_sExpr;
	};

	const TableInfo* m_pTable;

	std::vector<KeyPredicateInfo> m_predicates;
	KeyPredicateInfo m_endPredicate;

	std::vector<std::byte> m_sStartRow;
	std::vector<std::byte> m_sEndRow;

	bool m_bStartInclusive = true;
	bool m_bEndInclusive = true;

	bool m_bSeekToFirst = true;
	bool m_bSeekToLast = true;

	std::vector<ExecutionResult> m_startKeyResults;
	std::vector<ExecutionResult> m_endKeyResults;

	std::vector<DBDataType>& m_keyTypes;

	void doSetPredicate(const ParseNode* pPredicate);
	void setPredicateInfo(Operation op, const ParseNode* pKey, const ParseNode* pValue, const ParseNode* pPredicate);
};
