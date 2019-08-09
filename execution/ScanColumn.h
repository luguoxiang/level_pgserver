#ifndef SCANCOLUMN_H
#define SCANCOLUMN_H

#include <string>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include "common/ConfigInfo.h"

class ScanColumn
{
public:
	ScanColumn(bool bProject, const char* pszName)
			: m_bProject(bProject), m_pszName(pszName), m_type(TYPE_UNKNOWN)
	{
	}

	ScanColumn(bool bProject, const char* pszBase, size_t id);

	virtual ~ScanColumn()
	{
	}

	virtual std::string explain() = 0;

	bool m_bProject;

	const char* getName() const
	{
		return m_pszName;
	}

	void setName(const char* pszName)
	{
		m_pszName = pszName;
	}

	DBDataType getType() { return m_type;}
	void setType(DBDataType type) {m_type = type;}
private:
	ScanColumn(const ScanColumn&);
	ScanColumn& operator =(const ScanColumn&);

	DBDataType m_type;
	const char* m_pszName;
};

class ScanColumnVector
{
public:
	ScanColumnVector()
	{
	}

	~ScanColumnVector()
	{
		clear();
	}

	void explain(std::vector<std::string>& rows)
	{
		for (size_t i = 0; i < m_columns.size(); ++i)
		{
			if (m_columns[i] == NULL)
				continue;
			std::string s = m_columns[i]->explain();
			rows.push_back(s);
		}
	}

	void clear()
	{
		for (size_t i = 0; i < m_columns.size(); ++i)
		{
			if (m_columns[i] == NULL)
				continue;
			delete m_columns[i];
		}
		m_columns.clear();
	}
	size_t size() const
	{
		return m_columns.size();
	}

	bool empty() const
	{
		return m_columns.empty();
	}

	void set(size_t iIndex, ScanColumn* pColumn)
	{
		m_columns[iIndex] = pColumn;
	}

	ScanColumn* get(size_t iIndex)
	{
		return m_columns[iIndex];
	}

	void push_back(ScanColumn* pColumn)
	{
		m_columns.push_back(pColumn);
	}

private:
	std::vector<ScanColumn*> m_columns;
};

#endif //SCANCOLUMN_H
