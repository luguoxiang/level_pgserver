%option noyywrap nodefault yylineno case-insensitive 
%option reentrant bison-bridge bison-locations
%option header-file="common/SqlParser.lex.h"
%option nounistd
%{
#include "ParseResult.h"
#include "SqlParser.tab.h"
#include <cassert>
#include <time.h>
#include <absl/strings/str_cat.h>


extern void yyerror(YYLTYPE* yylloc, ParseResult* p, yyscan_t scanner, const std::string& msg);

#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = ((ParseResult*)yyextra)->m_yylineno;\
	yylloc->first_column = ((ParseResult*)yyextra)->m_yycolumn;\
	yylloc->last_column = yylloc->first_column + yyleng - 1;\
	((ParseResult*)yyextra)->m_yycolumn += yyleng;

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
SHOW {return SHOW;}
DESC {return DESC;}
INFILE {return INFILE;}
TABLES {return TABLES;}
FIELDS {return FIELDS;}
TERMINATED {return TERMINATED;}
IS {return IS;}
NOT 	{return NOT;}
NULL 	{ 
		yyerror(yylloc, (ParseResult*)yyextra, nullptr, "NULL is not supported");
		return ERROR;
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

TRUE {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = pResult->newSimpleNode( NodeType::INT,yylloc->first_column, yylloc->last_column);
	yylval->pNode = pNode;
	pNode->setString("1");
	return BOOL;
}
FALSE {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = pResult->newSimpleNode( NodeType::INT,yylloc->first_column, yylloc->last_column);
	yylval->pNode = pNode;
	pNode->setString("0");
	return BOOL;
}

[0-9]+ {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = pResult->newSimpleNode( NodeType::INT,yylloc->first_column, yylloc->last_column);
	yylval->pNode = pNode;
	pNode->setString(pNode->m_sExpr);
	return INTNUM;

}
[0-9]+"."[0-9]* | 
"."[0-9]+ |
[0-9]+E[-+]?[0-9]+ |
[0-9]+"."[0-9]*E[-+]?[0-9]+ |
"."[0-9]+E[-+]?[0-9]+ {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = pResult->newSimpleNode( NodeType::FLOAT, yylloc->first_column, yylloc->last_column);
	yylval->pNode = pNode;
	pNode->setString(pNode->m_sExpr);
	return APPROXNUM;
}

'(\\.|''|[^'\n])*'|\"(\\.|\"\"|[^"\n])*\" {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = pResult->newSimpleNode( NodeType::STR, yylloc->first_column, yylloc->last_column);
	yylval->pNode = pNode;

	//ignore last quote charactors
	size_t len =  pNode->m_sExpr.length() - 1;
	char* pValue = pResult->alloc(len);

	size_t resultSize = 0;
	char szBuf[3];
	for (size_t i = 1; i< len ; ++i) {
		char c = pNode->m_sExpr[i];
		if (c == '\\') {
			if (i+3 < len && IS_DIGIT(pNode->m_sExpr[i + 1]) && IS_DIGIT(pNode->m_sExpr[i + 2]) && IS_DIGIT(pNode->m_sExpr[i + 3])) {
					c = (pNode->m_sExpr[++i] - '0') * 64;
					c += (pNode->m_sExpr[++i] - '0') * 8;
					c += pNode->m_sExpr[++i] - '0';
			} else if (i + 1 < len){
				c = pNode->m_sExpr[++i];

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
				case 'x':
					szBuf[0] = pNode->m_sExpr[++i];
					szBuf[1] = pNode->m_sExpr[++i];
					szBuf[2] = 0;
					c= strtol(szBuf,0, 16);
					break;
				default:
					break;
				}
			}
		}
		pValue[resultSize++] = c;
	}

	pNode->setString(std::string_view(pValue, resultSize));
	return STRING;
}

'(\\.|[^'\n])*$ {yyerror(yylloc, (ParseResult*)yyextra,nullptr, absl::StrCat("Unterminated string ", yytext));}

\"(\\.|[^"\n])*$ {yyerror(yylloc, (ParseResult*)yyextra, nullptr,absl::StrCat("Unterminated string ", yytext));}

X'([0-9A-F]{2})+'|0X([0-9A-F]{2})+ {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = pResult->newSimpleNode( NodeType::BINARY, yylloc->first_column, yylloc->last_column);
	yylval->pNode = pNode;
	
	size_t len =  pNode->m_sExpr.length();
	if (pNode->m_sExpr[pNode->m_sExpr.length() - 1] == '\'') {
		--len; //remove last quote
	}
	char* pValue = pResult->alloc(len - 2);

	size_t resultSize = 0;
	char szBuf[3];
	for (size_t i = 2; i + 1  < len ; i += 2) {
		szBuf[0] = pNode->m_sExpr[i];
		szBuf[1] = pNode->m_sExpr[i+1];
		szBuf[2] = 0;
		unsigned char a = strtol(szBuf,0, 16);
		pValue[resultSize++] = (char)a;
	}
	pNode->setString(std::string_view(pValue, resultSize));

	return BINARY;
}

D'[0-9]{4}(-[0-9]{2}){2}' {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = pResult->newSimpleNode( NodeType::DATE,yylloc->first_column, yylloc->last_column);
	pNode->setString(pNode->m_sExpr.substr(2, pNode->m_sExpr.size() - 3));
	yylval->pNode = pNode;
	return DATE_VALUE;
}
D'[0-9]{4}(-[0-9]{2}){2}[ ][0-9]{2}(:[0-9]{2}){2}' {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = pResult->newSimpleNode( NodeType::DATE, yylloc->first_column, yylloc->last_column);
	pNode->setString(pNode->m_sExpr.substr(2, pNode->m_sExpr.size() - 3));
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
	ParseNode* pNode = pResult->newSimpleNode( NodeType::NAME, yylloc->first_column, yylloc->last_column);
	pNode->setString(pNode->m_sExpr);
	yylval->pNode = pNode;
	return NAME;
}

$[0-9]+ {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = pResult->newSimpleNode( NodeType::PARAM, yylloc->first_column, yylloc->last_column, Operation::UNBOUND_PARAM);
	yylval->pNode = pNode;
	pResult->m_bindParamNodes.push_back(pNode);
	return PARAM;
}

[ \t\r\n]		{}
"--"[ \t].*;

<<EOF>>		{return ';';}
.		{
	std::string_view s(yytext, 1);
	yyerror(yylloc, (ParseResult*)yyextra,nullptr, absl::StrCat("mystery charactor ", s));
	return ERROR;
}
%%

