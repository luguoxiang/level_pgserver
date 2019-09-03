#pragma once

#include <set>
#include <vector>
#include "common/ParseNode.h"
#include "ExecutionResult.h"
#include "DataRow.h"

class KeySearchRange {
public:
	KeySearchRange(std::set<std::string_view>& solved,
			std::vector<DBDataType>& types,
			const TableInfo* pTable,
			const ParseNode* pNode);

	KeySearchRange(const KeySearchRange&) = delete;
	KeySearchRange& operator =(const KeySearchRange&) = delete;

	const DataRow getStartRow() const {
		return DataRow((const std::byte*)m_sStartRow.data(), m_keyTypes, m_sStartRow.size());
	}
	const DataRow getEndRow() const {
		return DataRow((const std::byte*)m_sEndRow.data(), m_keyTypes, m_sEndRow.size());
	}

	const std::vector<ExecutionResult>& getStartResults() {return m_startKeyResults;}
	const std::vector<ExecutionResult>& getEndResults() {return m_endKeyResults;}

	bool startInclusive() const {
		return m_bStartInclusive;
	}
	bool endInclusive() const {
		return m_bEndInclusive;
	}

private:
	struct KeyPredicateInfo {
		int m_iKeyIndex = -1;
		const ParseNode* m_pValue = nullptr;;
		Operation m_op = Operation::NONE;
		std::string_view m_sExpr;
	};

	const TableInfo* m_pTable;

	std::vector<KeyPredicateInfo> m_predicates;
	KeyPredicateInfo m_endPredicate;
	std::set<std::string_view>& m_solved;

	std::string m_sStartRow;
	std::string m_sEndRow;

	bool m_bStartInclusive = true;
	bool m_bEndInclusive = true;

	std::vector<ExecutionResult> m_startKeyResults;
	std::vector<ExecutionResult> m_endKeyResults;

	std::vector<DBDataType>& m_keyTypes;

	void doSetPredicate(const ParseNode* pPredicate);
	void setPredicateInfo(Operation op, const ParseNode* pKey, const ParseNode* pValue, const ParseNode* pPredicate);
};
