%define api.pure
%parse-param {ParseResult* pResult}
%locations
%param { yyscan_t scanner }
%code requires {
typedef void* yyscan_t;
#include "BuildPlan.h"
#include <stdint.h>
}

%union{
	ParseNode* pNode;
}

%{
#include <array>

#include "SqlParser.lex.h"

#define YYLEX_PARAM pResult->m_scanInfo

extern void yyerror(YYLTYPE* yylloc, ParseResult* p, yyscan_t scanner,  const std::string& msg);

struct DbPlanBuilder
{
	const char* m_db;
	BuildPlanFunc m_pfnSelect;
	BuildPlanFunc m_pfnInsert;
	BuildPlanFunc m_pfnDelete;
	
	bool valid() const {return m_db != nullptr;}
};

static std::array<DbPlanBuilder,1> planBuilders = {
	{"file", buildPlanForFileSelect, nullptr, nullptr},
};

static DbPlanBuilder getPlanBuilder(ParseResult* pResult, ParseNode** ppTable)
{
		assert(ppTable);
		ParseNode* pTable = *ppTable;
		if(pTable->m_type == NodeType::OP)
		{
				int i = 0;
				assert(pTable->children() == 2);
				assert(pTable->m_children[0]->m_type == NodeType::NAME);
				assert(pTable->m_children[1]->m_type == NodeType::NAME);
				ParseNode* pDB = pTable->m_children[0];
				*ppTable = pTable = pTable->m_children[1];
				for(auto& builder : planBuilders)
				{
					if(strcasecmp(pDB->m_sValue.c_str(), builder.m_db) == 0)
					{
						return builder;
					}
				}
				return DbPlanBuilder{};
		}
		else
		{
				return planBuilders[0];
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
		$$ = newParentNode(pResult, "ExplainStmt", @$.first_column, @$.last_column, { $2 });
		$$->m_fnBuildPlan = buildPlanForExplain;
		pResult->m_pResult = $$;
		YYACCEPT;
	}
	;

merge_stmt: '(' get_stmt ')' UNION ALL '(' get_stmt ')'
	{
		$$ = newParentNode(pResult, "UnionAll", @$.first_column, @$.last_column, { $2, $7 });
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
	| NAME '.' NAME {$$ = newExprNode(pResult, '.', @$.first_column, @$.last_column,{ $1, $3});}
	| ERROR {$$ = 0;YYERROR;}
	;

expr: expr '+' expr {$$ = newExprNode(pResult, '+', @$.first_column, @$.last_column, {$1, $3});}
	| expr '-' expr {$$ = newExprNode(pResult, '-', @$.first_column, @$.last_column, { $1, $3});}
	| expr '*' expr {$$ = newExprNode(pResult, '*', @$.first_column, @$.last_column, { $1, $3});}
	| expr '/' expr {$$ = newExprNode(pResult, '/', @$.first_column, @$.last_column, { $1, $3});}
	| expr '%' expr {$$ = newExprNode(pResult, '%', @$.first_column, @$.last_column, { $1, $3});}
	| expr MOD expr {$$ = newExprNode(pResult, '%', @$.first_column, @$.last_column, { $1, $3});}
	| '-' expr %prec UMINUS {
		if($2->m_type == NodeType::INT)
		{
			$2 = new ParseNode(pResult, NodeType::INT,@$.first_column, @$.last_column);
			$2->m_iValue = - $2->m_iValue;
			$$ = $2;
		}
		else
		{
			$$ = newExprNode(pResult, '-',@$.first_column, @$.last_column, { $2 });
		}
	}
	| '+' expr %prec UMINUS {
		$$ = $2;
	}
	| expr COMP_LE expr {$$ = newExprNode(pResult, COMP_LE, @$.first_column, @$.last_column, {$1, $3});}
	| expr COMP_LT expr {$$ = newExprNode(pResult, COMP_LT, @$.first_column, @$.last_column, { $1, $3});}
	| expr COMP_EQ expr {$$ = newExprNode(pResult, COMP_EQ, @$.first_column, @$.last_column, { $1, $3});}
	| expr COMP_GE expr {$$ = newExprNode(pResult, COMP_GE, @$.first_column, @$.last_column, { $1, $3});}
	| expr COMP_GT expr {$$ = newExprNode(pResult, COMP_GT, @$.first_column, @$.last_column, { $1, $3});}
	| expr COMP_NE expr {$$ = newExprNode(pResult, COMP_NE, @$.first_column, @$.last_column, { $1, $3});}
	| expr LIKE STRING {
		auto len =  $3->m_sValue.length();
		if($3->m_sValue[0] != '%' || $3->m_sValue[len - 1] != '%')
		{
			yyerror(&@3,pResult, nullptr, ConcateToString("missing %% for like ", $3->m_sValue));
			YYERROR;
		}
		$3->m_sValue =  $3->m_sValue.substr(1, len -2);
		$$ = newExprNode(pResult, LIKE, @$.first_column, @$.last_column, { $1, $3});
	}
	| expr ANDOP expr {$$ = newExprNode(pResult, ANDOP, @$.first_column, @$.last_column, { $1, $3});}
	| expr OR expr {$$ = newExprNode(pResult, OR, @$.first_column, @$.last_column, { $1, $3});}
	| '(' expr ')' { $$ = $2;}
	;


expr: expr IS NULLX {
		$$ = newExprNode(pResult,COMP_EQ, @$.first_column, @$.last_column, { $1, $3}); 
	}
	| expr IS NOT NULLX {
		$$ = newExprNode(pResult, COMP_NE, @$.first_column, @$.last_column, { $1, $4}); 
	}
	;

expr: expr IN '(' val_list ')' {
		$4 = $4->merge(pResult,"ValueList", "ValueList");
		$$ = newExprNode(pResult, IN, @$.first_column, @$.last_column, { $1, $4});
		}
	| expr NOT IN '(' val_list ')' { 
		$5 = $5->merge(pResult,"ValueList", "ValueList");
		$$ = newExprNode(pResult, NOT_IN, @$.first_column, @$.last_column, { $1, $5});
	}
	;
	
expr: NAME '(' expr ')' {
        $$ = newFuncNode(pResult, $1->m_sValue, @$.first_column, @$.last_column, { $3 });
}       
;

val_list: expr {$$ = $1;}
	| expr ',' val_list { $$ = newParentNode(pResult, "ValueList",@$.first_column, @$.last_column,  {$1, $3});}
	;

delete_stmt: DELETE FROM table_factor opt_where
	{
		ParseNode* pTable = $3;
		auto builder = getPlanBuilder(pResult, &pTable);
		if(builder.m_pfnDelete == nullptr)
		{
		  yyerror(&@3,pResult,nullptr, "Delete is not supported for current database");
		  YYERROR;
		}
		$$ = newParentNode(pResult, "DeleteStmt", @$.first_column, @$.last_column, {pTable, $4 });
		$$->m_fnBuildPlan = builder.m_pfnDelete;
	}

update_stmt: UPDATE table_factor SET update_asgn_list opt_where
	{
		$4 = $4->merge(pResult,"AssignValueList", "AssignValueList");
		yyerror(&@1,pResult,nullptr, "Update is not supported for current database");
		YYERROR;
	}
	;

update_asgn_list:NAME COMP_EQ expr 
	{
		$$ = newParentNode(pResult, "AssignValue",@$.first_column, @$.last_column, {$1, $3}); 
	}
	| update_asgn_list ',' NAME COMP_EQ expr
	{
		ParseNode* pNode = newParentNode(pResult, "AssignValue",@$.first_column, @$.last_column,  {$3, $5 }); 
		$$ = newParentNode(pResult, "AssignValueList",@$.first_column, @$.last_column,  {$1, pNode });

	}
	;
values_stmt:VALUES value_list
	{
		$2 = $2->merge(pResult,"ValueList","ValueList");
		$$ = $2;
		$$->m_fnBuildPlan = buildPlanForConst;
	}
	;

insert_stmt: INSERT INTO table_factor opt_col_names select_stmt
	{
	  ParseNode* pTable = $3;
		auto builder = getPlanBuilder(pResult, &pTable);
		if(builder.m_pfnInsert == nullptr)
		{
			yyerror(&@3,pResult,nullptr, "Insert is not supported for current database");
			YYERROR;
		}
		$$ = newParentNode(pResult, "InsertStmt",@$.first_column, @$.last_column,  { pTable,$4,$5 });
		$$->m_fnBuildPlan = builder.m_pfnInsert;
	}
	| INSERT INTO table_factor opt_col_names values_stmt
	{
	  ParseNode* pTable = $3;
		auto builder = getPlanBuilder(pResult, &pTable);
		if(builder.m_pfnInsert == nullptr)
		{
			yyerror(&@3,pResult,nullptr, "Insert is not supported for current database");
			YYERROR;
		}
		$$ = newParentNode(pResult, "InsertStmt",  @$.first_column, @$.last_column, { pTable,$4,$5 });
		$$->m_fnBuildPlan = builder.m_pfnInsert;
	}
	;

show_tables_stmt:SHOW TABLES
	{
		$$ = newInfoNode(pResult,  SHOW,  @$.first_column, @$.last_column);
		$$->m_fnBuildPlan = buildPlanForShowTables;
	}
	;
	
desc_table_stmt:DESC table_factor
	{
		$$ = newParentNode(pResult, "DescStmt", @$.first_column, @$.last_column, {$2 });
		$$->m_fnBuildPlan = buildPlanForDesc;
	}
	; 

workload_stmt:WORKLOAD
	{
		$$ = newInfoNode(pResult,  WORKLOAD,  @$.first_column, @$.last_column);
		$$->m_fnBuildPlan = buildPlanForWorkload;	
	}
 	;
 	
load_stmt: LOAD DATA INFILE STRING INTO TABLE table_factor opt_col_names FIELDS TERMINATED BY STRING
	{
		ParseNode* pTable = $7;
		auto builder = getPlanBuilder(pResult, &pTable);
		if(builder.m_pfnInsert == nullptr)
		{
			yyerror(&@3,pResult,nullptr, "Insert is not supported for current database");
			YYERROR;
		}

		ParseNode* pFileNode = newParentNode(pResult, "FileNode", @$.first_column, @$.last_column, { $4, pTable, $8, $12 });
		pFileNode->m_fnBuildPlan = buildPlanForReadFile;	

		$$ = newParentNode(pResult, "Loadtmt", @$.first_column, @$.last_column, { pTable, $8, pFileNode });
		$$->m_fnBuildPlan = builder.m_pfnInsert;
	}
	;
	
opt_col_names: /* empty */{$$ = 0;}
	| '(' column_list ')' {
		$2 = $2->merge(pResult,"ColumnList", "ColumnList");
		$$ = $2;
	}
	;

value_list: '(' row_value ')' { 
		$2 = $2->merge(pResult,"ExprList", "ExprList");
		$$ = $2;
	}
	| value_list ',' '(' row_value ')' {
		$4 = $4->merge(pResult,"ExprList", "ExprList");
		$$ = newParentNode(pResult, "ValueList",@$.first_column, @$.last_column,  { $1, $4 });
	}

row_value: expr {$$ = $1;}
	| row_value ',' expr { 
	$$ = newParentNode(pResult, "ExprList", @$.first_column, @$.last_column,{ $1, $3 });}
	;

column_list: NAME { $$ = $1;}
	| column_list ',' NAME {
		$$ = newParentNode(pResult, "ColumnList", @$.first_column, @$.last_column,{ $1, $3 });
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
		$2 = $2->merge(pResult, "SelectExprList","ExprList");
		ParseNode* pProject = $2;
		ParseNode* pTable = $4;
		ParseNode* pAlias = $5;
		ParseNode* pPredicate = $6;
		ParseNode* pJoin = $11;

		int hasSubquery = (pTable->m_type != NodeType::NAME && pTable->m_type != NodeType::OP);
		if(pJoin != 0)
		{
			//This is a left join statement
			pJoin = pJoin->merge(pResult,"JoinList", "JoinList");
			if(pAlias == nullptr)
			{
				yyerror(&@5,pResult,nullptr, "table in left join statement  must have a alias name");
				YYERROR;
			}
			if(!hasSubquery)
			{
				yyerror(&@4,pResult,nullptr, "left join target must be subquery");
				YYERROR;
			}

			$$ = newParentNode(pResult, "LeftJoinStmt",@$.first_column, @$.last_column, { pProject, pTable, pAlias, pJoin });
			$$->m_fnBuildPlan = buildPlanForLeftJoin;
		}	
		else
		{
			pProject->m_fnBuildPlan = buildPlanForProjection;
			if(pAlias != nullptr)
			{
				yyerror(&@5,pResult,nullptr, "table alias name in non-join statement  is not supported");
				YYERROR;
			}
			if(hasSubquery)
			{
				//this is a select statement with subquery
				// children order is important, it is the BuildPlan order
				$$ = newParentNode(pResult, "SubQueryStmt", @$.first_column, @$.last_column, { pTable, pPredicate, $7, $8, $9, $10, pProject});
				$$->m_fnBuildPlan = buildPlanDefault;
			}
			else
			{
				auto builder = getPlanBuilder(pResult, &pTable);
			    if(builder.m_pfnSelect == nullptr)
			    {
			      yyerror(&@3,pResult,nullptr, "Select is not supported for current database");
			      YYERROR;
			    }
	
					$$ = newParentNode(pResult, "SelectStmt", @$.first_column, @$.last_column, { pProject, pTable, pPredicate, $7, $8, $9, $10 });
					$$->m_fnBuildPlan = builder.m_pfnSelect;
				}
		}
	}
	;

join_clause : LEFT JOIN table_factor USING '(' column_list ')'
	{
		$6 = $6->merge(pResult,"Using", "ColumnList");
    	$$ = newParentNode(pResult, "LeftJoin", @$.first_column, @$.last_column, { $3, $6});
	}
	;

join_list : join_clause
	{
		$$ = newParentNode(pResult, "JoinList", @$.first_column, @$.last_column, {$1 });
	}
	| join_list  join_clause
	{
		$$ = newParentNode(pResult, "JoinList", @$.first_column, @$.last_column, { $1, $2 });
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
	       $$ = newParentNode(pResult, "Limit",@$.first_column, @$.last_column, { $2, $4}); 
		$$->m_fnBuildPlan = buildPlanForLimit;
	}
	| LIMIT INTNUM ',' INTNUM 
	{
	       $$ = newParentNode(pResult, "Limit",@$.first_column, @$.last_column,{ $4, $2 }); 
		$$->m_fnBuildPlan = buildPlanForLimit;
	}

opt_groupby:{$$ = 0;}
	| GROUP BY column_list {
		$3 = $3->merge(pResult,"GroupBy",  "ColumnList");
		$$ = $3;
		$$->m_fnBuildPlan = buildPlanForGroupBy;
	}
	;

sort_list: expr opt_asc_desc {
			$$ = newParentNode(pResult, "SortItem", @$.first_column, @$.last_column,{ $1, $2 }); 
		}
	| sort_list ',' expr opt_asc_desc { 
			auto pChild =  newParentNode(pResult, "SortItem",@$.first_column, @$.last_column, { $3, $4 });
			$$ = newParentNode(pResult, "SortList",@$.first_column, @$.last_column, { $1,pChild });
		}
	;

opt_asc_desc:{$$ = newInfoNode(pResult, ASC,  @$.first_column, @$.last_column);}
	| ASC {$$ = newInfoNode(pResult, ASC,  @$.first_column, @$.last_column);}
	| DESC {$$ = newInfoNode(pResult, DESC,  @$.first_column, @$.last_column);}
	;

opt_having:{$$ = 0;}
	| HAVING expr {
		$$ = $2;
		$$->m_fnBuildPlan = buildPlanForFilter;
	}

	;
	
opt_orderby:{$$ = 0;}
	| ORDER BY sort_list {
		$3 = $3->merge(pResult,"OrderBy", "SortList");
		$$ = $3;
		$$->m_fnBuildPlan = buildPlanForOrderBy;
	}
	;

projection: expr {
		$$ = $1; 
	} | expr AS NAME { 
		$$ = newExprNode(pResult, AS, @$.first_column, @$.last_column, { $1, $3 }); 
	}

select_expr_list: projection { 
		$$ = $1;
	}
	| select_expr_list ',' projection {
		$$ = newParentNode(pResult, "ExprList", @$.first_column, @$.last_column, { $1, $3 });
	}
	| '*' {
		$$ = newInfoNode(pResult, ALL_COLUMN,  @$.first_column, @$.last_column);
	}
	;

table_factor: NAME { 
		$$ = $1;
	}
	| NAME '.' NAME {$$ = newExprNode(pResult, '.', @$.first_column, @$.last_column, { $1, $3});}
	;

%%

void yyerror(YYLTYPE* yylloc, ParseResult* p, yyscan_t scanner,const std::string& msg)
{
	p->m_pResult = 0;
	p->m_sError = msg;
	p->m_iStartCol = yylloc->first_column;
	p->m_iEndCol = yylloc->last_column;
	p->m_iLine = yylloc->first_line;
}

int parseInit(ParseResult* p)
{
    p->m_nodes.clear();
	p->m_yycolumn = 1;
	p->m_yylineno = 1;
	return yylex_init_extra(p, &(p->m_scanInfo));
}

int parseTerminate(ParseResult* p)
{
	p->m_nodes.clear();
	return yylex_destroy(p->m_scanInfo);
}

void parseSql(ParseResult* p, const std::string_view sql)
{
	p->m_pResult = nullptr;
	p->m_sSql.assign(sql.data(), sql.length());
	p->m_sError = "";
	
	p->m_yycolumn = 1;
	p->m_yylineno = 1;

	YY_BUFFER_STATE bp;

	bp = yy_scan_string(p->m_sSql.c_str(), p->m_scanInfo);
	yy_switch_to_buffer(bp, p->m_scanInfo);
	yyparse(p, p->m_scanInfo);
	yy_delete_buffer(bp, p->m_scanInfo);
	//printTree(p->m_pResult, 0);
}
