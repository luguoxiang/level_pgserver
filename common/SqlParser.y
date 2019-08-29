%define api.pure
%parse-param {ParseResult* pResult}
%locations
%param { yyscan_t scanner }
%code requires {
typedef void* yyscan_t;
#include "ParseResult.h"
#include "BuildPlan.h"
#include "MetaConfig.h"
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
	BuildPlanFunc m_pfnSelect;
	BuildPlanFunc m_pfnInsert;
	BuildPlanFunc m_pfnDelete;
};


static DbPlanBuilder getPlanBuilder(ParseResult* pResult, ParseNode* pTable)
{
		assert(pTable);
		auto pTableInfo = MetaConfig::getInstance().getTableInfo(pTable->m_sValue);
		if (pTableInfo == nullptr) {
			PARSE_ERROR("table ", pTable->m_sValue, " not found");
		}
		if (pTableInfo->getKeyCount() == 0){
			return DbPlanBuilder{buildPlanForFileSelect, nullptr, nullptr};
		} else {
			return DbPlanBuilder{nullptr, buildPlanForLevelDBInsert, nullptr};
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
%type <pNode> opt_where opt_groupby opt_having opt_orderby opt_limit
%type <pNode> table_factor
%type <pNode> sort_list opt_asc_desc
%type <pNode> sql_stmt stmt
%type <pNode> insert_stmt opt_col_names value_list 

%type <pNode> column_list row_value
%type <pNode> update_stmt update_asgn_list 
%type <pNode> delete_stmt
%type <pNode> get_stmt merge_stmt values_stmt
%type <pNode> show_tables_stmt desc_table_stmt workload_stmt
%type <pNode> table_or_query opt_alias

%start sql_stmt
%%

sql_stmt: stmt ';'
	{
		pResult->m_pResult = $$ = $1;
		YYACCEPT;
	}
	| EXPLAIN stmt ';'
	{
		$$ = pResult->newParentNode( "ExplainStmt", @$.first_column, @$.last_column, { $2 });
		$$->m_fnBuildPlan = buildPlanForExplain;
		pResult->m_pResult = $$;
		YYACCEPT;
	}
	;

merge_stmt: '(' get_stmt ')' UNION ALL '(' get_stmt ')'
	{
		$$ = pResult->newParentNode( "UnionAll", @$.first_column, @$.last_column, { $2, $7 });
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
	| PARAM {$$ = $1;}
	| NAME '.' NAME {$$ = pResult->newExprNode( Operation::MEMBER, @$.first_column, @$.last_column,{ $1, $3});}
	| ERROR {$$ = 0;YYERROR;}
	;

expr: expr '+' expr {$$ = pResult->newExprNode( Operation::ADD, @$.first_column, @$.last_column, {$1, $3});}
	| expr '-' expr {$$ = pResult->newExprNode( Operation::SUB, @$.first_column, @$.last_column, { $1, $3});}
	| expr '*' expr {$$ = pResult->newExprNode( Operation::MUL, @$.first_column, @$.last_column, { $1, $3});}
	| expr '/' expr {$$ = pResult->newExprNode( Operation::DIV, @$.first_column, @$.last_column, { $1, $3});}
	| expr '%' expr {$$ = pResult->newExprNode( Operation::MOD, @$.first_column, @$.last_column, { $1, $3});}
	| expr MOD expr {$$ = pResult->newExprNode( Operation::MOD, @$.first_column, @$.last_column, { $1, $3});}
	| '-' expr %prec UMINUS {
		if($2->m_type == NodeType::INT)
		{
			$2 = pResult->newSimpleNode(NodeType::INT,@$.first_column, @$.last_column);
			$2->m_iValue = - $2->m_iValue;
			$$ = $2;
		}
		else
		{
			$$ = pResult->newExprNode( Operation::MINUS,@$.first_column, @$.last_column, { $2 });
		}
	}
	| '+' expr %prec UMINUS {
		$$ = $2;
	}
	| expr COMP_LE expr {$$ = pResult->newExprNode( Operation::COMP_LE, @$.first_column, @$.last_column, {$1, $3});}
	| expr COMP_LT expr {$$ = pResult->newExprNode( Operation::COMP_LT, @$.first_column, @$.last_column, { $1, $3});}
	| expr COMP_EQ expr {$$ = pResult->newExprNode( Operation::COMP_EQ, @$.first_column, @$.last_column, { $1, $3});}
	| expr COMP_GE expr {$$ = pResult->newExprNode( Operation::COMP_GE, @$.first_column, @$.last_column, { $1, $3});}
	| expr COMP_GT expr {$$ = pResult->newExprNode( Operation::COMP_GT, @$.first_column, @$.last_column, { $1, $3});}
	| expr COMP_NE expr {$$ = pResult->newExprNode( Operation::COMP_NE, @$.first_column, @$.last_column, { $1, $3});}
	| expr LIKE STRING {
		auto len =  $3->m_sValue.length();
		if($3->m_sValue[0] != '%' || $3->m_sValue[len - 1] != '%')
		{
			yyerror(&@3,pResult, nullptr, ConcateToString("missing %% for like ", $3->m_sValue));
			YYERROR;
		}
		$3->m_sValue =  $3->m_sValue.substr(1, len -2);
		$$ = pResult->newExprNode( Operation::LIKE, @$.first_column, @$.last_column, { $1, $3});
	}
	| expr ANDOP expr {$$ = pResult->newExprNode( Operation::AND, @$.first_column, @$.last_column, { $1, $3});}
	| expr OR expr {$$ = pResult->newExprNode( Operation::OR, @$.first_column, @$.last_column, { $1, $3});}
	| '(' expr ')' { $$ = $2;}
	;


expr: expr IS NULLX {
		$$ = pResult->newExprNode(Operation::COMP_EQ, @$.first_column, @$.last_column, { $1, $3}); 
	}
	| expr IS NOT NULLX {
		$$ = pResult->newExprNode( Operation::COMP_NE, @$.first_column, @$.last_column, { $1, $4}); 
	}
	;

expr: expr IN '(' val_list ')' {
		$4 = pResult->merge($4,"ValueList", "ValueList");
		$$ = pResult->newExprNode( Operation::IN, @$.first_column, @$.last_column, { $1, $4});
		}
	| expr NOT IN '(' val_list ')' { 
		$5 = pResult->merge($5,"ValueList", "ValueList");
		$$ = pResult->newExprNode( Operation::NOT_IN, @$.first_column, @$.last_column, { $1, $5});
	}
	;
	
expr: NAME '(' expr ')' {
        $$ = pResult->newFuncNode($1->m_sValue, @$.first_column, @$.last_column, { $3 });
}       
;

val_list: expr {$$ = $1;}
	| expr ',' val_list { $$ = pResult->newParentNode( "ValueList",@$.first_column, @$.last_column,  {$1, $3});}
	;

delete_stmt: DELETE FROM table_factor opt_where
	{
		ParseNode* pTable = $3;
		auto builder = getPlanBuilder(pResult, pTable);
		if(builder.m_pfnDelete == nullptr)
		{
		  yyerror(&@3,pResult,nullptr, "Delete is not supported for current database");
		  YYERROR;
		}
		$$ = pResult->newParentNode( "DeleteStmt", @$.first_column, @$.last_column, {pTable, $4 });
		$$->m_fnBuildPlan = builder.m_pfnDelete;
	}

update_stmt: UPDATE table_factor SET update_asgn_list opt_where
	{
		$4 = pResult->merge($4,"AssignValueList", "AssignValueList");
		yyerror(&@1,pResult,nullptr, "Update is not supported for current database");
		YYERROR;
	}
	;

update_asgn_list:NAME COMP_EQ expr 
	{
		$$ = pResult->newParentNode( "AssignValue",@$.first_column, @$.last_column, {$1, $3}); 
	}
	| update_asgn_list ',' NAME COMP_EQ expr
	{
		ParseNode* pNode = pResult->newParentNode( "AssignValue",@$.first_column, @$.last_column,  {$3, $5 }); 
		$$ = pResult->newParentNode( "AssignValueList",@$.first_column, @$.last_column,  {$1, pNode });

	}
	;
values_stmt:VALUES value_list
	{
		$2 = pResult->merge($2,"ValueList","ValueList");
		$$ = $2;
		$$->m_fnBuildPlan = buildPlanForConst;
	}
	;

insert_stmt: INSERT INTO table_factor opt_col_names select_stmt
	{
	  ParseNode* pTable = $3;
		auto builder = getPlanBuilder(pResult, pTable);
		if(builder.m_pfnInsert == nullptr)
		{
			yyerror(&@3,pResult,nullptr, "Insert is not supported for current database");
			YYERROR;
		}
		$$ = pResult->newParentNode( "InsertStmt",@$.first_column, @$.last_column,  { pTable,$4,$5 });
		$$->m_fnBuildPlan = builder.m_pfnInsert;
	}
	| INSERT INTO table_factor opt_col_names values_stmt
	{
	  ParseNode* pTable = $3;
		auto builder = getPlanBuilder(pResult, pTable);
		if(builder.m_pfnInsert == nullptr)
		{
			yyerror(&@3,pResult,nullptr, "Insert is not supported for current database");
			YYERROR;
		}
		$$ = pResult->newParentNode( "InsertStmt",  @$.first_column, @$.last_column, { pTable,$4,$5 });
		$$->m_fnBuildPlan = builder.m_pfnInsert;
	}
	;

show_tables_stmt:SHOW TABLES
	{
		$$ = pResult->newInfoNode(  Operation::SHOW_TABLES,  @$.first_column, @$.last_column);
		$$->m_fnBuildPlan = buildPlanForShowTables;
	}
	;
	
desc_table_stmt:DESC table_factor
	{
		$$ = pResult->newParentNode( "DescStmt", @$.first_column, @$.last_column, {$2 });
		$$->m_fnBuildPlan = buildPlanForDesc;
	}
	; 

workload_stmt:WORKLOAD
	{
		$$ = pResult->newInfoNode(  Operation::WORKLOAD,  @$.first_column, @$.last_column);
		$$->m_fnBuildPlan = buildPlanForWorkload;	
	}
 	;

	
opt_col_names: /* empty */{$$ = nullptr;}
	| '(' column_list ')' {
		$2 = pResult->merge($2,"ColumnList", "ColumnList");
		$$ = $2;
	}
	;

value_list: '(' row_value ')' { 
		$2 = pResult->merge($2,"ExprList", "ExprList");
		$$ = $2;
	}
	| value_list ',' '(' row_value ')' {
		$4 = pResult->merge($4,"ExprList", "ExprList");
		$$ = pResult->newParentNode( "ValueList",@$.first_column, @$.last_column,  { $1, $4 });
	}

row_value: expr {$$ = $1;}
	| row_value ',' expr { 
	$$ = pResult->newParentNode( "ExprList", @$.first_column, @$.last_column,{ $1, $3 });}
	;

column_list: NAME { $$ = $1;}
	| column_list ',' NAME {
		$$ = pResult->newParentNode( "ColumnList", @$.first_column, @$.last_column,{ $1, $3 });
	}
	;

table_or_query: table_factor {$$ = $1;}
	|  '(' get_stmt ')' {$$ = $2;}
	;

opt_alias: {$$ = 0;}
	|  AS NAME {$$ = $2;}
	;


select_stmt: SELECT select_expr_list FROM table_or_query opt_alias 
			opt_where opt_groupby opt_having opt_orderby opt_limit
	{
		$2 = pResult->merge($2, "SelectExprList","ExprList");
		ParseNode* pProject = $2;
		ParseNode* pTable = $4;
		ParseNode* pAlias = $5;
		ParseNode* pPredicate = $6;

		pProject->m_fnBuildPlan = buildPlanForProjection;
		if(pAlias != nullptr)
		{
			yyerror(&@5,pResult,nullptr, "table alias name is not supported");
			YYERROR;
		}
		if(pTable->m_type != NodeType::NAME && pTable->m_type != NodeType::OP)
		{
			//this is a select statement with subquery
			// children order is important, it is the BuildPlan order
			$$ = pResult->newParentNode( "SubQueryStmt", @$.first_column, @$.last_column, { pTable, pPredicate, $7, $8, $9, $10, pProject});
			$$->m_fnBuildPlan = buildPlanDefault;
		}
		else
		{
			auto builder = getPlanBuilder(pResult, pTable);
		    if(builder.m_pfnSelect == nullptr)
		    {
		      yyerror(&@3,pResult,nullptr, "Select is not supported for current database");
		      YYERROR;
		    }

			$$ = pResult->newParentNode( "SelectStmt", @$.first_column, @$.last_column, { pProject, pTable, pPredicate, $7, $8, $9, $10 });
			$$->m_fnBuildPlan = builder.m_pfnSelect;
		}
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
	       $$ = pResult->newParentNode( "Limit",@$.first_column, @$.last_column, { $2, $4}); 
		$$->m_fnBuildPlan = buildPlanForLimit;
	}
	| LIMIT INTNUM ',' INTNUM 
	{
	       $$ = pResult->newParentNode( "Limit",@$.first_column, @$.last_column,{ $4, $2 }); 
		$$->m_fnBuildPlan = buildPlanForLimit;
	}

opt_groupby:{$$ = 0;}
	| GROUP BY column_list {
		$3 = pResult->merge($3,"GroupBy",  "ColumnList");
		$$ = $3;
		$$->m_fnBuildPlan = buildPlanForGroupBy;
	}
	;

sort_list: expr opt_asc_desc {
			$$ = pResult->newParentNode( "SortItem", @$.first_column, @$.last_column,{ $1, $2 }); 
		}
	| sort_list ',' expr opt_asc_desc { 
			auto pChild =  pResult->newParentNode( "SortItem",@$.first_column, @$.last_column, { $3, $4 });
			$$ = pResult->newParentNode( "SortList",@$.first_column, @$.last_column, { $1,pChild });
		}
	;

opt_asc_desc:{$$ = pResult->newInfoNode( Operation::ASC,  @$.first_column, @$.last_column);}
	| ASC {$$ = pResult->newInfoNode(  Operation::ASC,  @$.first_column, @$.last_column);}
	| DESC {$$ = pResult->newInfoNode(  Operation::DESC,  @$.first_column, @$.last_column);}
	;

opt_having:{$$ = 0;}
	| HAVING expr {
		$$ = $2;
		$$->m_fnBuildPlan = buildPlanForFilter;
	}

	;
	
opt_orderby:{$$ = 0;}
	| ORDER BY sort_list {
		$3 = pResult->merge($3,"OrderBy", "SortList");
		$$ = $3;
		$$->m_fnBuildPlan = buildPlanForOrderBy;
	}
	;

projection: expr {
		$$ = $1; 
	} | expr AS NAME { 
		$$ = pResult->newExprNode( Operation::AS, @$.first_column, @$.last_column, { $1, $3 }); 
	}

select_expr_list: projection { 
		$$ = $1;
	}
	| select_expr_list ',' projection {
		$$ = pResult->newParentNode( "ExprList", @$.first_column, @$.last_column, { $1, $3 });
	}
	| '*' {
		$$ = pResult->newInfoNode( Operation::ALL_COLUMNS,  @$.first_column, @$.last_column);
	}
	;

table_factor: NAME { 
		$$ = $1;
	}
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
	return yylex_init_extra(p, &(p->m_scanInfo));
}

int parseTerminate(ParseResult* p)
{
	return yylex_destroy(p->m_scanInfo);
}

void parseSql(ParseResult* p, const std::string_view sql)
{
	p->initParse(sql);
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
