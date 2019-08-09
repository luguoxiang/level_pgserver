%define api.pure
%parse-param {ParseResult* pResult}
%locations
%{
#include "BuildPlan.h"
#include <stdint.h>
%}

%union{
	struct _ParseNode* pNode;
}

%{
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "SqlParser.lex.h"

#define YYLEX_PARAM pResult->m_scanInfo

extern void yyerror(YYLTYPE* yylloc, ParseResult* p, char* s,...);

extern ParseNode* mergeTree(ParseResult* pResult, const char* pszRootName, 
			ParseNode* pRoot, 
			const char* pszRemove);

extern ParseNode* newIntNode(ParseResult* pResult, int type, int value, int num, ...);

extern ParseNode* newParentNode(ParseResult* pResult, const char* pszName, int childNum, ...);

extern ParseNode* newExprNode(ParseResult *pResult, 
				int value, int firstColumn, int lastColumn, int num, ...);
extern ParseNode* newFuncNode(ParseResult *p, 
				const char* pszName, int firstColumn, int lastColumn, int num, ...);

struct DbPlanBuilder
{
	const char* m_db;
	BuildPlanFunc m_pfnSelect;
	BuildPlanFunc m_pfnInsert;
	BuildPlanFunc m_pfnDelete;
};

static struct DbPlanBuilder g_dbPlanBuilder[] = {
	{"hbase", buildPlanForHBaseSelect, buildPlanForHBaseInsert, buildPlanForHBaseDelete},
	{"file", buildPlanForFileSelect, NULL, NULL},
};

static struct DbPlanBuilder* getPlanBuilder(ParseResult* pResult, ParseNode** ppTable)
{
		struct DbPlanBuilder* pBuilder = NULL;
		assert(ppTable);
		ParseNode* pTable = *ppTable;
		if(pTable->m_iType == OP_NODE)
		{
				int i = 0;
				assert(pTable->m_iChildNum == 2);
				assert(pTable->m_children[0]->m_iType == NAME_NODE);
				assert(pTable->m_children[1]->m_iType == NAME_NODE);
				ParseNode* pDB = pTable->m_children[0];
				*ppTable = pTable = pTable->m_children[1];
				for(i=0 ; ; ++i)
				{
					if(i == sizeof(g_dbPlanBuilder)/sizeof(struct DbPlanBuilder))
					{
						break;
					}
					if(strcasecmp(pDB->m_pszValue, g_dbPlanBuilder[i].m_db) == 0)
					{
						return g_dbPlanBuilder+ i;
					}
				}
				return NULL;
		}
		else
		{
				return g_dbPlanBuilder;
		}
}

%}

%token <pNode> NAME
%token <pNode> STRING
%token <pNode> BINARY
%token <pNode> INTNUM
%token <pNode> DATE_VALUE
%token <pNode> HINT_VALUE
%token <pNode> BOOL
%token <pNode> APPROXNUM
%token <pNode> NULLX
%token <pNode> PARAM

%right ASSIGN
%left OR
%left XOR
%left ANDOP
%nonassoc IN IS LIKE REGEXP
%left NOT '!'
%left '&'
%left COMP_LE COMP_LT COMP_EQ COMP_GT COMP_GE COMP_NE
%left '|'
%left '+' '-'
%left '*' '/' '%' MOD
%left '^'
%nonassoc UMINUS
%nonassoc UPLUS

%token DATA
%token FIELDS
%token INFILE
%token SHOW
%token TABLES
%token TERMINATED
%token WORKLOAD
%token ERROR
%token ADD
%token ALL
%token AND
%token ANDOP
%token ANY
%token AS
%token ASC 
%token BETWEEN 
%token CHAR
%token CREATE 
%token DATABASE 
%token DATE 
%token DATETIME 
%token DEFAULT 
%token DELETE 
%token DESC 
%token DISTINCT 
%token FLOAT 
%token DOUBLE
%token DROP 
%token EXPLAIN 
%token FROM 
%token GROUP 
%token HAVING 
%token IN 
%token INSERT 
%token INTEGER 
%token INTO 
%token LIKE 
%token LIMIT 
%token LOAD 
%token NOT 
%token ON 
%token OR 
%token ORDER 
%token SELECT 
%token WHERE
%token BY
%token IS 
%token SMALLINT 
%token TABLE 
%token TO 
%token UNION 
%token UNIQUE 
%token UNSIGNED 
%token UPDATE 
%token SET
%token VALUES 
%token VARCHAR
%token OFFSET
%token NOT_IN		//not in grammer
%token NOT_LIKE		//not in grammer
%token ALL_ROW		//not in grammer
%token ALL_COLUMN	//not in grammer
%token PRIMARY_KEY	//not in grammer
%token NOT_NULL		//not in grammer
%token KEY
%token PRIMARY
%token IF
%token EXISTS
%token IF_EXISTS
%token JOIN
%token LEFT
%token USING

%type <pNode> expr
%type <pNode> val_list
%type <pNode> select_stmt select_expr_list projection 
%type <pNode> join_list join_clause
%type <pNode> opt_where opt_groupby opt_having opt_orderby opt_limit
%type <pNode> table_factor
%type <pNode> sort_list opt_asc_desc
%type <pNode> sql_stmt stmt
%type <pNode> insert_stmt load_stmt opt_col_names value_list 

%type <pNode> column_list row_value
%type <pNode> update_stmt update_asgn_list 
%type <pNode> delete_stmt
%type <pNode> get_stmt merge_stmt values_stmt
%type <pNode> show_tables_stmt desc_table_stmt workload_stmt
%type <pNode> table_or_query opt_alias opt_join

%start sql_stmt
%%

sql_stmt: stmt ';'
	{
		pResult->m_pResult = $$ = $1;
		YYACCEPT;
	}
	| EXPLAIN stmt ';'
	{
		$$ = newParentNode(pResult, "ExplainStmt", 1, $2);
		$$->m_fnBuildPlan = buildPlanForExplain;
		pResult->m_pResult = $$;
		YYACCEPT;
	}
	;

merge_stmt: '(' get_stmt ')' UNION ALL '(' get_stmt ')'
	{
		$$ = newParentNode(pResult, "UnionAll", 2, $2, $7);
		$$->m_fnBuildPlan = buildPlanForUnionAll;
	}
  ;
get_stmt: select_stmt {$$ = $1;}
	| merge_stmt {$$ = $1;}
	| values_stmt {$$ = $1;}
	| workload_stmt { $$ = $1;}
	| show_tables_stmt { $$ = $1;}
	| desc_table_stmt { $$ = $1;}
	;

stmt: get_stmt {$$ = $1;}
	| insert_stmt { $$ = $1;}
	| load_stmt { $$ = $1;}	
	| update_stmt {$$ = $1;}
	| delete_stmt {$$ = $1;}
	;

expr: NAME { $$ = $1;}
	| STRING {$$ = $1;}
	| BINARY {$$ = $1;}
	| DATE_VALUE {$$ = $1;}
	| INTNUM {$$ = $1;}
	| APPROXNUM {$$ = $1;}
	| BOOL {$$ = $1;}
	| NAME '.' NAME {$$ = newExprNode(pResult, '.', @$.first_column, @$.last_column, 2, $1, $3);}
	| ERROR {$$ = 0;YYERROR;}
	;

expr: expr '+' expr {$$ = newExprNode(pResult, '+', @$.first_column, @$.last_column,  2, $1, $3);}
	| expr '-' expr {$$ = newExprNode(pResult, '-', @$.first_column, @$.last_column, 2, $1, $3);}
	| expr '*' expr {$$ = newExprNode(pResult, '*', @$.first_column, @$.last_column, 2, $1, $3);}
	| expr '/' expr {$$ = newExprNode(pResult, '/', @$.first_column, @$.last_column, 2, $1, $3);}
	| expr '%' expr {$$ = newExprNode(pResult, '%', @$.first_column, @$.last_column, 2, $1, $3);}
	| expr MOD expr {$$ = newExprNode(pResult, '%', @$.first_column, @$.last_column, 2, $1, $3);}
	| '-' expr %prec UMINUS {
		if($2->m_iType == INT_NODE)
		{
			$2->m_iValue = - $2->m_iValue;
			char szBuf[20];
			snprintf(szBuf,20, "%lld", $2->m_iValue);
			$2->m_pszValue = my_strdup(pResult, szBuf);
			$2->m_pszExpr = $2->m_pszValue;
			$$ = $2;
		}
		else
		{
			$$ = newExprNode(pResult, '-',@$.first_column, @$.last_column, 1, $2);
		}
	}
	| '+' expr %prec UMINUS {
		$$ = $2;
	}
	| expr COMP_LE expr {$$ = newExprNode(pResult, COMP_LE, @$.first_column, @$.last_column, 2, $1, $3);}
	| expr COMP_LT expr {$$ = newExprNode(pResult, COMP_LT, @$.first_column, @$.last_column, 2, $1, $3);}
	| expr COMP_EQ expr {$$ = newExprNode(pResult, COMP_EQ, @$.first_column, @$.last_column, 2, $1, $3);}
	| expr COMP_GE expr {$$ = newExprNode(pResult, COMP_GE, @$.first_column, @$.last_column, 2, $1, $3);}
	| expr COMP_GT expr {$$ = newExprNode(pResult, COMP_GT, @$.first_column, @$.last_column, 2, $1, $3);}
	| expr COMP_NE expr {$$ = newExprNode(pResult, COMP_NE, @$.first_column, @$.last_column, 2, $1, $3);}
	| expr LIKE STRING {
		if($3->m_pszValue[0] != '%' || $3->m_pszValue[$3->m_iValue - 1] != '%')
		{
			yyerror(&@3,pResult, "missing %% for like '%s'", $3->m_pszValue);
			YYERROR;
		}
		$3->m_iValue = $3->m_iValue - 2;
		strncpy((char*)$3->m_pszValue, $3->m_pszValue + 1, $3->m_iValue);
		$$ = newExprNode(pResult, LIKE, @$.first_column, @$.last_column, 2, $1, $3);
	}
	| expr ANDOP expr {$$ = newExprNode(pResult, ANDOP, @$.first_column, @$.last_column, 2, $1, $3);}
	| expr OR expr {$$ = newExprNode(pResult, OR, @$.first_column, @$.last_column, 2, $1, $3);}
	| '(' expr ')' { $$ = $2;}
	;


expr: expr IS NULLX {
		$$ = newExprNode(pResult,COMP_EQ, @$.first_column, @$.last_column, 2, $1, $3); 
	}
	| expr IS NOT NULLX {
		$$ = newExprNode(pResult, COMP_NE, @$.first_column, @$.last_column, 2, $1, $4); 
	}
	;

expr: expr IN '(' val_list ')' {
		$$ = newExprNode(pResult, IN, @$.first_column, @$.last_column, 2, $1, 
			mergeTree(pResult, "ValueList", $4, "ValueList"));
		}
	| expr NOT IN '(' val_list ')' { 
		$$ = newExprNode(pResult, NOT_IN, @$.first_column, @$.last_column, 2, $1, 
			mergeTree(pResult, "ValueList", $5, "ValueList"));
	}
	;
	
expr: NAME '(' expr ')' {
        $$ = newFuncNode(pResult, $1->m_pszValue, @$.first_column, @$.last_column, 1, $3);
}       
;

val_list: expr {$$ = $1;}
	| expr ',' val_list { $$ = newParentNode(pResult, "ValueList", 2, $1, $3);}
	;

delete_stmt: DELETE FROM table_factor opt_where
	{
		ParseNode* pTable = $3;
		struct DbPlanBuilder* pBuilder = getPlanBuilder(pResult, &pTable);
		if(pBuilder == NULL || pBuilder->m_pfnDelete == NULL)
		{
		  yyerror(&@3,pResult, "Delete is not supported for current database");
		  YYERROR;
		}
		$$ = newParentNode(pResult, "DeleteStmt", 2, pTable, $4);
		$$->m_fnBuildPlan = pBuilder->m_pfnDelete;
	}

update_stmt: UPDATE table_factor SET update_asgn_list opt_where
	{
		$4 = mergeTree(pResult, "AssignValueList", $4, "AssignValueList");
		yyerror(&@1,pResult, "Update is not supported for current database");
		YYERROR;
	}
	;

update_asgn_list:NAME COMP_EQ expr 
	{
		$$ = newParentNode(pResult, "AssignValue", 2, $1, $3); 
	}
	| update_asgn_list ',' NAME COMP_EQ expr
	{
		ParseNode* pNode = newParentNode(pResult, "AssignValue", 2, $3, $5); 
		$$ = newParentNode(pResult, "AssignValueList", 2, $1, pNode);

	}
	;
values_stmt:VALUES value_list
	{
		$$ = mergeTree(pResult, "ValueList", $2, "ValueList");
		$$->m_fnBuildPlan = buildPlanForConst;
	}
	;

insert_stmt: INSERT INTO table_factor opt_col_names select_stmt
	{
	  ParseNode* pTable = $3;
		struct DbPlanBuilder* pBuilder = getPlanBuilder(pResult, &pTable);
		if(pBuilder == NULL || pBuilder->m_pfnInsert == NULL)
		{
			yyerror(&@3,pResult, "Insert is not supported for current database");
			YYERROR;
		}
		$$ = newParentNode(pResult, "InsertStmt", 3, pTable,$4,$5);
		$$->m_fnBuildPlan = pBuilder->m_pfnInsert;
	}
	| INSERT INTO table_factor opt_col_names values_stmt
	{
	  ParseNode* pTable = $3;
		struct DbPlanBuilder* pBuilder = getPlanBuilder(pResult, &pTable);
		if(pBuilder == NULL || pBuilder->m_pfnInsert == NULL)
		{
			yyerror(&@3,pResult, "Insert is not supported for current database");
			YYERROR;
		}
		$$ = newParentNode(pResult, "InsertStmt", 3, pTable,$4,$5);
		$$->m_fnBuildPlan = pBuilder->m_pfnInsert;
	}
	;

show_tables_stmt:SHOW TABLES
	{
		$$ = newIntNode(pResult, INFO_NODE, SHOW, 0);
		$$->m_fnBuildPlan = buildPlanForShowTables;
	}
	;
	
desc_table_stmt:DESC table_factor
	{
		$$ = newParentNode(pResult, "DescStmt", 1, $2);
		$$->m_fnBuildPlan = buildPlanForDesc;
	}
	; 

workload_stmt:WORKLOAD
	{
		$$ = newIntNode(pResult, INFO_NODE, WORKLOAD, 0);
		$$->m_fnBuildPlan = buildPlanForWorkload;	
	}
 	;
 	
load_stmt: LOAD DATA INFILE STRING INTO TABLE table_factor opt_col_names FIELDS TERMINATED BY STRING
	{
		ParseNode* pTable = $7;
		struct DbPlanBuilder* pBuilder = getPlanBuilder(pResult, &pTable);
		if(pBuilder == NULL || pBuilder->m_pfnInsert == NULL)
		{
			yyerror(&@3,pResult, "Insert is not supported for current database");
			YYERROR;
		}

		ParseNode* pFileNode = newParentNode(pResult, "FileNode", 4, $4, pTable, $8, $12);
		pFileNode->m_fnBuildPlan = buildPlanForReadFile;	

		$$ = newParentNode(pResult, "Loadtmt", 3, pTable, $8, pFileNode);
		$$->m_fnBuildPlan = pBuilder->m_pfnInsert;
	}
	;
	
opt_col_names: /* empty */{$$ = 0;}
	| '(' column_list ')' { $$ = mergeTree(pResult, "ColumnList", $2, "ColumnList");}
	;

value_list: '(' row_value ')' { 
		$$ = mergeTree(pResult, "ExprList",$2,"ExprList");
	}
	| value_list ',' '(' row_value ')' {
		$4 = mergeTree(pResult, "ExprList", $4, "ExprList");
		$$ = newParentNode(pResult, "ValueList", 2, $1, $4);
	}

row_value: expr {$$ = $1;}
	| row_value ',' expr { 
	$$ = newParentNode(pResult, "ExprList", 2, $1, $3);}
	;

column_list: NAME { $$ = $1;}
	| column_list ',' NAME {
		$$ = newParentNode(pResult, "ColumnList", 2, $1, $3);
	}
	;

table_or_query: table_factor {$$ = $1;}
	|  '(' get_stmt ')' {$$ = $2;}
	;

opt_alias: {$$ = 0;}
	|  AS NAME {$$ = $2;}
	;

opt_join: {$$ = 0;}
	| join_list {$$ = $1;}
	;
select_stmt: SELECT select_expr_list FROM table_or_query opt_alias 
			opt_where opt_groupby opt_having opt_orderby opt_limit opt_join
	{
		ParseNode* pProject = mergeTree(pResult, "SelectExprList", $2, "ExprList");
		ParseNode* pTable = $4;
		ParseNode* pAlias = $5;
		ParseNode* pPredicate = $6;
		ParseNode* pJoin = $11;

		int hasSubquery = (pTable->m_iType != NAME_NODE && pTable->m_iType != OP_NODE);
		if(pJoin != 0)
		{
			//This is a left join statement
			ParseNode* pJoinList = mergeTree(pResult, "JoinList", pJoin, "JoinList");
			if(pAlias == NULL)
			{
				yyerror(&@5,pResult, "table in left join statement  must have a alias name");
				YYERROR;
			}
			if(!hasSubquery)
			{
				yyerror(&@4,pResult, "left join target must be subquery");
				YYERROR;
			}

			$$ = newParentNode(pResult, "LeftJoinStmt", 4, pProject, pTable, pAlias, pJoinList);
			$$->m_fnBuildPlan = buildPlanForLeftJoin;
		}	
		else
		{
			pProject->m_fnBuildPlan = buildPlanForProjection;
			if(pAlias != NULL)
			{
				yyerror(&@5,pResult, "table alias name in non-join statement  is not supported");
				YYERROR;
			}
			if(hasSubquery)
			{
				//this is a select statement with subquery
				// children order is important, it is the BuildPlan order
				$$ = newParentNode(pResult, "SubQueryStmt", 7, pTable, pPredicate, $7, $8, $9, $10, pProject);
				$$->m_fnBuildPlan = buildPlanDefault;
			}
			else
			{
				struct DbPlanBuilder* pBuilder = getPlanBuilder(pResult, &pTable);
		    if(pBuilder == NULL || pBuilder->m_pfnSelect == NULL)
		    {
		      yyerror(&@3,pResult, "Select is not supported for current database");
		      YYERROR;
		    }

				$$ = newParentNode(pResult, "SelectStmt", 7, pProject, pTable, pPredicate, $7, $8, $9, $10);
				$$->m_fnBuildPlan = pBuilder->m_pfnSelect;
			}
		}
	}
	;

join_clause : LEFT JOIN table_factor USING '(' column_list ')'
	{
		$6 = mergeTree(pResult, "Using", $6, "ColumnList");
    $$ = newParentNode(pResult, "LeftJoin", 2, $3, $6);
	}
	;

join_list : join_clause
	{
		$$ = newParentNode(pResult, "JoinList", 1, $1);
	}
	| join_list  join_clause
	{
		$$ = newParentNode(pResult, "JoinList", 2, $1, $2);
	}
	;
opt_where:{$$ = 0;}
	| WHERE expr {
		$$ = $2;
		$$->m_fnBuildPlan = buildPlanForFilter;
	}
	;

opt_limit:{$$ = 0;}
	| LIMIT INTNUM OFFSET INTNUM 
	{
	       $$ = newParentNode(pResult, "Limit", 2, $2, $4); 
		$$->m_fnBuildPlan = buildPlanForLimit;
	}
	| LIMIT INTNUM ',' INTNUM 
	{
	       $$ = newParentNode(pResult, "Limit", 2, $4, $2); 
		$$->m_fnBuildPlan = buildPlanForLimit;
	}

opt_groupby:{$$ = 0;}
	| GROUP BY column_list {
		$$ = mergeTree(pResult, "GroupBy", $3, "ColumnList");
		$$->m_fnBuildPlan = buildPlanForGroupBy;
	}
	;

sort_list: expr opt_asc_desc {
			$$ = newParentNode(pResult, "SortItem", 2, $1, $2); 
		}
	| sort_list ',' expr opt_asc_desc { 
			$$ = newParentNode(pResult, "SortList", 2, $1,
				newParentNode(pResult, "SortItem", 2, $3, $4));
		}
	;

opt_asc_desc:{$$ = newIntNode(pResult, INFO_NODE, ASC, 0);}
	| ASC {$$ = newIntNode(pResult, INFO_NODE, ASC, 0);}
	| DESC {$$ = newIntNode(pResult, INFO_NODE, DESC, 0);}
	;

opt_having:{$$ = 0;}
	| HAVING expr {
		$$ = $2;
		$$->m_fnBuildPlan = buildPlanForFilter;
	}

	;
	
opt_orderby:{$$ = 0;}
	| ORDER BY sort_list {
		$$ = mergeTree(pResult, "OrderBy", $3, "SortList");
		$$->m_fnBuildPlan = buildPlanForOrderBy;
	}
	;

projection: expr {
		$$ = $1; 
	} | expr AS NAME { 
		$$ = newIntNode(pResult, OP_NODE, AS, 2, $1, $3); 
	}

select_expr_list: projection { 
		$$ = $1;
	}
	| select_expr_list ',' projection {
		$$ = newParentNode(pResult, "ExprList", 2, $1, $3);
	}
	| '*' {
		$$ = newIntNode(pResult, INFO_NODE, ALL_COLUMN, 0);
	}
	;

table_factor: NAME { 
		$$ = $1;
	}
	| NAME '.' NAME {$$ = newExprNode(pResult, '.', @$.first_column, @$.last_column, 2, $1, $3);}
	;

%%

void yyerror(YYLTYPE* yylloc, ParseResult* p, char* s, ...)
{
	p->m_pResult = 0;
	va_list ap;
	va_start(ap, s);
	vsnprintf(p->m_szErrorMsg, MAX_ERROR_MSG, s, ap);
	p->m_iStartCol = yylloc->first_column;
	p->m_iEndCol = yylloc->last_column;
	p->m_iLine = yylloc->first_line;
}

int parseInit(ParseResult* p)
{
	p->m_yycolumn = 1;
	p->m_yylineno = 1;
	p->m_pPoolHead = NULL;
	p->m_pPoolTail = NULL;
	return yylex_init_extra(p, &(p->m_scanInfo));
}

int parseTerminate(ParseResult* p)
{
	memPoolClear(p, 1);
	return yylex_destroy(p->m_scanInfo);
}

void parseSql(ParseResult* p, const char* pszBuf, size_t iLen)
{
	p->m_pResult = 0;
	p->m_pszSql = pszBuf;
	p->m_szErrorMsg[0]=0;
	
	p->m_yycolumn = 1;
	p->m_yylineno = 1;
	
	memPoolClear(p, 0);

	YY_BUFFER_STATE bp;

	bp = yy_scan_string(pszBuf, p->m_scanInfo);
	yy_switch_to_buffer(bp, p->m_scanInfo);
	yyparse(p);
	yy_delete_buffer(bp, p->m_scanInfo);
}
