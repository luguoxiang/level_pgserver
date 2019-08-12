%option noyywrap nodefault yylineno case-insensitive 
%option reentrant bison-bridge bison-locations
%option header-file="SqlParser.lex.h"
%{
#include "ParseNode.h"
#include "SqlParser.tab.h"
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define STR_MAX_LEN     (16318)  /* 16KB length */

extern void yyerror(YYLTYPE* yylloc, ParseResult* p, yyscan_t scanner, const char* s,...);

#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = ((ParseResult*)yyextra)->m_yylineno;\
	yylloc->first_column = ((ParseResult*)yyextra)->m_yycolumn;\
	yylloc->last_column = yylloc->first_column + yyleng - 1;\
	((ParseResult*)yyextra)->m_yycolumn += yyleng;

extern ParseNode* newNode(ParseResult* pResult, int type, int num);

static size_t parseBinary(const char* pszSrc, char* pszDest, size_t iLen)
{
	size_t i,j;
	j = 0;
	for(i = 0; i< iLen; i+=2)
	{
		char szBuf[3];
		szBuf[0] = pszSrc[i];
		szBuf[1] = pszSrc[i+1];
		szBuf[2] = 0;
		unsigned char a = strtol(szBuf,0, 16);
		pszDest[j++] = (char)a;
	}
	return j;
}

%}

%%
ADD {return ADD;}
ALL { return ALL;}
AND {return ANDOP;}
ANY {return ANY;}
AS {return AS;}
ASC {return ASC;}
CHAR(ACTER)? {return CHAR;}
CREATE {return CREATE;}
DATABASE {return DATABASE;}
DEFAULT { return DEFAULT;}
DELETE { return DELETE;}
DISTINCT { return DISTINCT;}
FLOAT4 { return FLOAT;}
FLOAT8 | DOUBLE { return DOUBLE;}
DROP { return DROP;}
EXPLAIN { return EXPLAIN;}
FROM { return FROM;}
GROUP { return GROUP;}
HAVING { return HAVING;}
IN { return IN;}
INSERT { return INSERT;}
INT { return INTEGER;}
INT4 { return INTEGER;}
INTEGER { return INTEGER;}
INTO { return INTO;}
LIKE {return LIKE;}
LIMIT { return LIMIT;}
LOAD {return LOAD;}
WHERE {return WHERE;}
BY {return BY;}
DATA {return DATA;}
WORKLOAD {return WORKLOAD;}
SHOW {return SHOW;}
DESC {return DESC;}
INFILE {return INFILE;}
TABLES {return TABLES;}
FIELDS {return FIELDS;}
TERMINATED {return TERMINATED;}
IS {return IS;}
NOT 	{return NOT;}
NULL 	{ 
	yylval->pNode = newNode((ParseResult*)yyextra, NULL_NODE,0);
	return NULLX;
}
ON 	{ return ON;}
OFFSET	{ return OFFSET;}
OR 	{ return OR;}
ORDER 	{ return ORDER;}
SELECT 	{ return SELECT;}
INT2 	{ return SMALLINT;}
SMALLINT { return SMALLINT;}
TABLE 	{return TABLE;}
TO 	{ return TO;}
UNION 	{ return UNION;}
UNIQUE 	{return UNIQUE;}
UNSIGNED { return UNSIGNED;}
UPDATE 	{ return UPDATE;}
SET	{ return SET;}
VALUES 	{ return VALUES;}
VARCHAR(ACTER)? {return VARCHAR;}
XOR {return XOR;}
IF {return IF;}
EXISTS {return EXISTS;}
KEY {return KEY;}
PRIMARY { return PRIMARY;}
JOIN {return JOIN;}
LEFT { return LEFT;}
USING {return USING;}

[0-9]+ {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = newNode(pResult, INT_NODE,0);
	yylval->pNode = pNode;
	pNode->m_pszValue = my_strdup(pResult, yytext);
	pNode->m_pszExpr = pNode->m_pszValue;
	errno = 0;
	pNode->m_iValue = strtol(pNode->m_pszValue, 0, 10);
	if (errno != 0)
	{
		yyerror(yylloc, (ParseResult*)yyextra, NULL, "Integer Out of Range: %s", yytext);
		return ERROR;
	}
	else return INTNUM;
}
[0-9]+"."[0-9]* | 
"."[0-9]+ |
[0-9]+E[-+]?[0-9]+ |
[0-9]+"."[0-9]*E[-+]?[0-9]+ |
"."[0-9]+E[-+]?[0-9]+ {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = newNode(pResult, FLOAT_NODE,0);
	yylval->pNode = pNode;
	pNode->m_pszValue = my_strdup(pResult, yytext);
	pNode->m_pszExpr = pNode->m_pszValue;
	return APPROXNUM;
}

TRUE {
	ParseNode* pNode = newNode((ParseResult*)yyextra, BOOL_NODE,0);
	pNode->m_iValue = 1;
	yylval->pNode = pNode;
	pNode->m_pszExpr = "true";
	return BOOL;
}
UNKNOWN {
	ParseNode* pNode = newNode((ParseResult*)yyextra, BOOL_NODE,0);
	pNode->m_iValue = -1;
	yylval->pNode = pNode;
	pNode->m_pszExpr = "unknown";
	return BOOL;
}
FALSE {
	ParseNode* pNode = newNode((ParseResult*)yyextra, BOOL_NODE,0);
	pNode->m_iValue = 0;
	yylval->pNode = pNode;
	pNode->m_pszExpr = "false";
	return BOOL;
}

'(\\.|''|[^'\n])*'|\"(\\.|\"\"|[^"\n])*\" {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = newNode(pResult, STR_NODE,0);
	yylval->pNode = pNode;

	char* pszSrc = yytext+1;
	int len = strlen(pszSrc) - 1; //remove last quote charactor
	if(len > STR_MAX_LEN)
	{
		yyerror(yylloc, (ParseResult*)yyextra, NULL, "String is too large: %s", yytext);
		return ERROR;
	}

	char* pszDest = (char*) memPoolAlloc(len + 1, pResult);

	pNode->m_pszValue = pszDest;
	pNode->m_pszExpr = pszDest;
	pNode->m_iValue = parseString(pszSrc, pszDest, len);

	return STRING;
}

'(\\.|[^'\n])*$ {yyerror(yylloc, (ParseResult*)yyextra,NULL, "Unterminated string %s", yytext);}

\"(\\.|[^"\n])*$ {yyerror(yylloc, (ParseResult*)yyextra, NULL,"Unterminated string %s", yytext);}

X'([0-9A-F]{2})+'|0X([0-9A-F]{2})+ {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = newNode(pResult, BINARY_NODE,0);
	char* pszSrc = yytext + 2; 
	char* pszDest = 0;
	
	size_t len = strlen(pszSrc);
	if(pszSrc[len - 1] == '\'')
	{
		--len;
	}
	assert((len % 2) == 0);

	pszDest = (char*) memPoolAlloc(len/2, pResult);
	pNode->m_pszValue = pszDest;
	pNode->m_iValue = parseBinary(pszSrc, pszDest, len);
	pNode->m_pszExpr = my_strdup(pResult, yytext);

	yylval->pNode = pNode;
	return BINARY;
}

D'[0-9]{4}(-[0-9]{2}){2}' {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = newNode(pResult, DATE_NODE,0);
	char* pszDest =  my_strdup(pResult, yytext + 2);
	size_t len = strlen(pszDest);
	--len;
	pszDest[len] = '\0';//remove final '

	pNode->m_pszValue = pszDest;
	pNode->m_pszExpr = pszDest;

	pNode->m_iValue = parseTime(pszDest);
	if(pNode->m_iValue == 0)
	{
		yyerror(yylloc, (ParseResult*)yyextra,NULL, "Wrong date: %s", yytext);
		return ERROR;
	}
	
	yylval->pNode = pNode;
	return DATE_VALUE;
}
D'[0-9]{4}(-[0-9]{2}){2}[ ][0-9]{2}(:[0-9]{2}){2}' {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = newNode(pResult, DATE_NODE,0);
	char* pszDest =  my_strdup(pResult, yytext + 2);
	size_t len = strlen(pszDest);
	--len;
	pszDest[len] = '\0';//remove final '
	pNode->m_pszValue = pszDest;
	pNode->m_pszExpr = pszDest;

	pNode->m_iValue = parseTime(pszDest);
	if(pNode->m_iValue == 0)
	{
		yyerror(yylloc, (ParseResult*)yyextra,NULL, "Wrong date: %s", yytext);
		return ERROR;
	}

	yylval->pNode = pNode;
	return DATE_VALUE;
}

\/\*[^*]+\*\/	  { }

[-+&~|^/%*(),.;!] { return yytext[0];}

"%%" { return ANDOP;}
"||" {return OR;}
"=" {return COMP_EQ;}
">=" {return COMP_GE;}
">" {return COMP_GT;}
"<=" {return COMP_LE;}
"<" {return COMP_LT;}
"!="|"<>" {return COMP_NE;}

[A-Za-z_][A_Za-z0-9_]* {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = newNode(pResult, NAME_NODE,0);
	yylval->pNode = pNode;
	pNode->m_pszValue = my_strdup(pResult,yytext);
	pNode->m_pszExpr = pNode->m_pszValue;
	pNode->m_iValue = 0;
	return NAME;
}

$[0-9]+ {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = newNode(pResult, PARAM_NODE,0);
	yylval->pNode = pNode;
	pNode->m_pszValue = "unbound";
	int index = strtol(yytext + 1, 0, 10);
	if (errno != 0 || index <= 0 || index > 64)
	{
		yyerror(yylloc, (ParseResult*)yyextra,NULL, "Illegal prepared statement param: %s", yytext);
		return ERROR;
	}
	pNode->m_iValue = index;
	return PARAM;
}

[ \t\r\n]		{}
"--"[ \t].*;

<<EOF>>		{return ';';}
.		{
	yyerror(yylloc, (ParseResult*)yyextra,NULL, "mystery charactor '%c'", *yytext);
	return ERROR;
}
%%

