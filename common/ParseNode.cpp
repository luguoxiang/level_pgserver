#include "ParseNode.h"
#include <stdio.h>
#include <time.h>

extern const char* getTypeName(int type);

void buildPlanDefault(ParseNode* pNode) {
	size_t i;
	for (i = 0; i < pNode->m_iChildNum; ++i) {
		ParseNode* pChild = pNode->m_children[i];
		if (pChild && pChild->m_fnBuildPlan) {
			BUILD_PLAN(pChild);
		}
	};
}

void printTree(ParseNode* pRoot, int level) {
	int i;
	const char* pszTypeName;
	for (i = 0; i < level; ++i)
		printf("\t");
	if (pRoot == 0) {
		printf("NULL\n");
		return;
	}
	switch (pRoot->m_iType) {
	case NodeType::OP:
	case NodeType::FUNC:
	case NodeType::INFO:
	case NodeType::DATATYPE:
		pszTypeName = getTypeName(pRoot->m_iValue);
		if (pszTypeName == 0)
			printf("%c\n", (char) pRoot->m_iValue);
		else
			printf("%s\n", pszTypeName);
		break;
	case NodeType::BINARY:
		printf("\\x");
		for (i = 0; i < pRoot->m_iValue; ++i) {
			printf("%02x", (unsigned char) pRoot->m_pszValue[i]);
		}
		printf("\n");
		break;
	default:
		printf("%s\n", pRoot->m_pszValue);
		break;
	}
	for (i = 0; i < pRoot->m_iChildNum; ++i) {
		printTree(pRoot->m_children[i], level + 1);
	}
}

ParseNode* newNode(ParseResult *p, NodeType type, int num) {
	ParseNode* pNode = (ParseNode*) memPoolAlloc(sizeof(ParseNode), p);
	memset(pNode, 0, sizeof(ParseNode));

	pNode->m_iType = type;
	pNode->m_iChildNum = num;
	pNode->m_pszExpr = NULL;
	pNode->m_fnBuildPlan = buildPlanDefault;
	if (num > 0) {
		size_t allocSize = sizeof(ParseNode*) * num;
		pNode->m_children = (ParseNode**) memPoolAlloc(allocSize, p);
		memset(pNode->m_children, 0, allocSize);
	} else {
		pNode->m_children = 0;
	}
	return pNode;
}

static int _countChild(ParseNode* pRoot, const char* pszRemove) {
	if (pRoot == 0)
		return 0;

	int count = 0;
	if (pRoot->m_iType != NodeType::PARENT
			|| strcmp(pRoot->m_pszValue, pszRemove) != 0) {
		return 1;
	}
	int i;
	for (i = 0; i < pRoot->m_iChildNum; ++i) {
		count += _countChild(pRoot->m_children[i], pszRemove);
	}
	return count;
}

static void _mergeChild(ParseNode* pNode, ParseNode* pSource, int* pIndex,
		const char* pszRemove) {
	assert(pNode);
	if (pSource == 0)
		return;

	if (pSource->m_iType == NodeType::PARENT
			&& strcmp(pSource->m_pszValue, pszRemove) == 0) {
		int i;
		for (i = 0; i < pSource->m_iChildNum; ++i) {
			_mergeChild(pNode, pSource->m_children[i], pIndex, pszRemove);
			pSource->m_children[i] = 0;
		}
	} else {
		assert(*pIndex >= 0 && *pIndex < pNode->m_iChildNum);
		pNode->m_children[*pIndex] = pSource;
		++(*pIndex);
	}
}

ParseNode* mergeTree(ParseResult *p, const char* pszRootName, ParseNode* pRoot,
		const char* pszRemove) {
	int index, num;
	ParseNode* pNode;
	assert(pRoot);
	num = _countChild(pRoot, pszRemove);
	pNode = newNode(p, NodeType::PARENT, num);
	pNode->m_pszValue = pszRootName;
	index = 0;
	_mergeChild(pNode, pRoot, &index, pszRemove);
	assert(index == num);
	return pNode;
}

static const char* trim_dup(ParseResult *p, int firstColumn, int lastColumn) {
	size_t i = 0;
	size_t j = 0;
	size_t iLen = lastColumn - firstColumn + 1;
	char* pszColumn = memPoolAlloc(iLen + 1, p);
	for (i = firstColumn; i <= lastColumn; ++i) {
		char c = p->m_pszSql[i - 1];
		switch (c) {
		case ' ':
		case '\r':
		case '\n':
		case '\t':
			continue;
		default:
			break;
		}
		pszColumn[j++] = c;
	}
	pszColumn[j] = '\0';
	return pszColumn;
}

ParseNode* newFuncNode(ParseResult *p, const char* pszName, int firstColumn,
		int lastColumn, int num, ...) {
	va_list va;
	size_t i, j;
	ParseNode* pNode = newNode(p, NodeType::FUNC, num);
	pNode->m_pszExpr = trim_dup(p, firstColumn, lastColumn);
	pNode->m_pszValue = pszName;
	pNode->m_iValue = 0;
	va_start(va, num);
	for (i = 0; i < num; ++i) {
	pNode->m_children[i] = va_arg(va, ParseNode*);
}
va_end(va);
return pNode;
}

ParseNode* newExprNode(ParseResult *p, int value, int firstColumn,
	int lastColumn, int num, ...) {
va_list va;
size_t i, j;
ParseNode* pNode = newNode(p, NodeType::OP, num);
pNode->m_pszExpr = trim_dup(p, firstColumn, lastColumn);
pNode->m_iValue = value;
va_start(va, num);
for (i = 0; i < num; ++i) {
pNode->m_children[i] = va_arg(va, ParseNode*);
}
va_end(va);
return pNode;
}

ParseNode* newIntNode(ParseResult *p, NodeType type, int value, int num, ...) {
va_list va;
size_t i;
ParseNode* pNode = newNode(p, type, num);

pNode->m_iValue = value;
va_start(va, num);
for (i = 0; i < num; ++i) {
pNode->m_children[i] = va_arg(va, ParseNode*);
}
va_end(va);
return pNode;
}

ParseNode* newParentNode(ParseResult *p, const char* pszName, int num, ...) {
assert(num > 0);
va_list va;
int i;
ParseNode* pNode = newNode(p, NodeType::PARENT, num);
pNode->m_pszValue = pszName;
va_start(va, num);
for (i = 0; i < num; ++i) {
pNode->m_children[i] = va_arg(va, ParseNode*);
}
va_end(va);
return pNode;
}

int64_t parseTime(const char* pszTime) {
int iYear = 0;
int iMonth = 0;
int iDay = 0;
int iHour = 0;
int iMinute = 0;
int iSecond = 0;

struct tm time;
int ret = sscanf(pszTime, "%4d-%2d-%2d %2d:%2d:%2d", &iYear, &iMonth, &iDay,
&iHour, &iMinute, &iSecond);
if (ret != 3 && ret != 6) {
return 0;
}

memset(&time, 0, sizeof(struct tm));
time.tm_year = iYear - 1900;
time.tm_mon = iMonth - 1;
time.tm_mday = iDay;
time.tm_hour = iHour;
time.tm_min = iMinute;
time.tm_sec = iSecond;

time_t iRetTime = mktime(&time);
if (iYear - 1900 != time.tm_year || iMonth - 1 != time.tm_mon
|| iDay != time.tm_mday) {
return 0;
}
return iRetTime * 1000000; // unit is microseconds
}

char* memPoolAlloc(size_t iSize, ParseResult *p) {
assert(iSize < POOL_BLOCK_SIZE);
MemoryPoolBlock* pBlock = NULL;
if (p->m_pPoolHead == NULL) {
pBlock = (MemoryPoolBlock*) malloc(sizeof(MemoryPoolBlock));
pBlock->m_pNext = NULL;
pBlock->m_iUsed = 0;
p->m_pPoolHead = pBlock;
p->m_pPoolTail = pBlock;
} else {
pBlock = p->m_pPoolTail;
if (pBlock->m_iUsed + iSize > POOL_BLOCK_SIZE) {
pBlock->m_pNext = (MemoryPoolBlock*) malloc(sizeof(MemoryPoolBlock));
pBlock = pBlock->m_pNext;
pBlock->m_iUsed = 0;
pBlock->m_pNext = NULL;
p->m_pPoolTail = pBlock;
}
}
char* pRet = pBlock->m_szData + pBlock->m_iUsed;
pBlock->m_iUsed += iSize;
return pRet;
}

size_t memPoolUsed(ParseResult *p) {
MemoryPoolBlock* pBlock = p->m_pPoolHead;
if (pBlock == NULL)
return 0;
size_t iSize = 0;
while (pBlock != NULL) {
iSize += pBlock->m_iUsed;
pBlock = pBlock->m_pNext;
}
return iSize;

}
void memPoolClear(ParseResult *p, int8_t bReleaseAll) {
MemoryPoolBlock* pBlock = p->m_pPoolHead;
if (pBlock == NULL)
return;

if (!bReleaseAll) {
p->m_pPoolHead = pBlock;
p->m_pPoolTail = pBlock;
pBlock->m_iUsed = 0;
pBlock = pBlock->m_pNext;
p->m_pPoolHead->m_pNext = NULL;
} else {
p->m_pPoolHead = NULL;
p->m_pPoolTail = NULL;
}
while (pBlock != NULL) {
MemoryPoolBlock* pLast = pBlock;
pBlock = pBlock->m_pNext;
free(pLast);
}
}

char* my_memdup(ParseResult *p, const char* pszSrc, size_t iLen) {
assert(p && pszSrc);
char* pszDest = memPoolAlloc(iLen, p);
memcpy(pszDest, pszSrc, iLen);
return pszDest;
}

char* my_strdup(ParseResult *p, const char* pszSrc) {
assert(p && pszSrc);
size_t iLen = strlen(pszSrc) + 1;
char* pszDest = memPoolAlloc(iLen, p);
strncpy(pszDest, pszSrc, iLen);
return pszDest;
}

#define IS_DIGIT(c) (c >='0' && c<='9')
size_t parseString(const char* pszSrc, char* pszDest, size_t iLen) {
int i, copyIndex = 0;
for (i = 0; i < iLen; ++i) {
char c = pszSrc[i];
if (c == '\\') {
if (i + 3
	< iLen&& IS_DIGIT(pszSrc[i + 1]) && IS_DIGIT(pszSrc[i + 2]) && IS_DIGIT(pszSrc[i + 3])) {
c = (pszSrc[++i] - '0') * 64;
c += (pszSrc[++i] - '0') * 8;
c += pszSrc[++i] - '0';
} else {
++i;
c = pszSrc[i];
switch (c) {
case 'n':
	c = '\n';
	break;
case 'r':
	c = '\r';
	break;
case 't':
	c = '\t';
	break;
default:
	break;
}
}
}
pszDest[copyIndex++] = c;
}
assert(copyIndex <= iLen);
pszDest[copyIndex] = '\0';
return copyIndex;
}

