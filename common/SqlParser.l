%option noyywrap nodefault yylineno case-insensitive 
%option reentrant bison-bridge bison-locations
%option header-file="common/SqlParser.lex.h"
%{
#include "ParseNode.h"
#include "SqlParser.tab.h"
#include <string.h>
#include <assert.h>
#include <time.h>


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
	yylval->pNode = new ParseNode((ParseResult*)yyextra, NodeType::NONE, "NULL");
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
	ParseNode* pNode = new ParseNode(pResult, NodeType::INT,yylloc->first_column, yylloc->last_column);
	yylval->pNode = pNode;
	errno = 0;
	pNode->m_iValue = strtol(yytext, 0, 10);
	if (errno != 0)
	{
		yyerror(yylloc, pResult, NULL, ConcateToString("Integer Out of Range:", yytext));
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
	ParseNode* pNode = new ParseNode(pResult, NodeType::FLOAT, yylloc->first_column, yylloc->last_column);
	yylval->pNode = pNode;
	pNode->m_sValue = yytext;
	return APPROXNUM;
}

TRUE {
	ParseNode* pNode = new ParseNode((ParseResult*)yyextra, NodeType::BOOL, "true");
	pNode->m_iValue = 1;
	yylval->pNode = pNode;
	return BOOL;
}
UNKNOWN {
	ParseNode* pNode = new ParseNode((ParseResult*)yyextra, NodeType::BOOL,"unknown");
	pNode->m_iValue = -1;
	yylval->pNode = pNode;
	return BOOL;
}
FALSE {
	ParseNode* pNode = new ParseNode((ParseResult*)yyextra, NodeType::BOOL,"false");
	pNode->m_iValue = 0;
	yylval->pNode = pNode;
	return BOOL;
}

'(\\.|''|[^'\n])*'|\"(\\.|\"\"|[^"\n])*\" {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = new ParseNode(pResult, NodeType::STR, yylloc->first_column, yylloc->last_column);
	yylval->pNode = pNode;

	//remove first and last quote charactors
	pNode->m_sValue.assign(yytext + 1, strlen(yytext) - 2);

	return STRING;
}

'(\\.|[^'\n])*$ {yyerror(yylloc, (ParseResult*)yyextra,NULL, ConcateToString("Unterminated string ", yytext));}

\"(\\.|[^"\n])*$ {yyerror(yylloc, (ParseResult*)yyextra, NULL,ConcateToString("Unterminated string ", yytext));}

X'([0-9A-F]{2})+'|0X([0-9A-F]{2})+ {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = new ParseNode(pResult, NodeType::BINARY, yylloc->first_column, yylloc->last_column);

	pNode->m_sValue = parseBinary(yytext + 1);

	yylval->pNode = pNode;
	return BINARY;
}

D'[0-9]{4}(-[0-9]{2}){2}' {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = new ParseNode(pResult, NodeType::DATE,yylloc->first_column, yylloc->last_column);

	pNode->m_iValue = parseTime(yytext + 2);
	if(pNode->m_iValue == 0)
	{
		yyerror(yylloc, pResult ,NULL, ConcateToString("Wrong date: ", yytext));
		return ERROR;
	}
	
	yylval->pNode = pNode;
	return DATE_VALUE;
}
D'[0-9]{4}(-[0-9]{2}){2}[ ][0-9]{2}(:[0-9]{2}){2}' {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = new ParseNode(pResult, NodeType::DATE, yylloc->first_column, yylloc->last_column);

	pNode->m_iValue = parseTime(yytext + 2);
	if(pNode->m_iValue == 0)
	{
		yyerror(yylloc, pResult, NULL, ConcateToString("Wrong date: ", yytext));
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
	ParseNode* pNode = new ParseNode(pResult, NodeType::NAME, yylloc->first_column, yylloc->last_column);
	pNode->m_sValue = yytext;
	yylval->pNode = pNode;
	return NAME;
}

$[0-9]+ {
	ParseResult* pResult = (ParseResult*)yyextra;
	ParseNode* pNode = new ParseNode(pResult, NodeType::PARAM, "unbound");
	yylval->pNode = pNode;
	int index = strtol(yytext + 1, 0, 10);
	if (errno != 0 || index <= 0 || index > 64)
	{
		yyerror(yylloc, pResult,NULL,ConcateToString( "Illegal prepared statement param: ", yytext));
		return ERROR;
	}
	pNode->m_iValue = index;
	return PARAM;
}

[ \t\r\n]		{}
"--"[ \t].*;

<<EOF>>		{return ';';}
.		{
	yyerror(yylloc, (ParseResult*)yyextra,NULL, ConcateToString("mystery charactor ", *yytext));
	return ERROR;
}
%%

