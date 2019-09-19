%define api.pure
%parse-param {ParseResult* pResult}
%locations
%param { yyscan_t scanner }
%code requires {
typedef void* yyscan_t;
#include "ParseResult.h"
#include "MetaConfig.h"
#include <stdint.h>
#include <absl/strings/str_cat.h>
}

%union{
	ParseNode* pNode;
}

%{
#include <array>

#include "SqlParser.lex.h"

#define YYLEX_PARAM pResult->m_scanInfo

extern void yyerror(YYLTYPE* yylloc, ParseResult* p, yyscan_t scanner,  const std::string& msg);

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
%type <pNode> delete_stmt
%type <pNode> get_stmt values_stmt
%type <pNode> show_tables_stmt desc_table_stmt 
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
		$$ = pResult->newPlanNode( "ExplainStmt", Operation::EXPLAIN, @$.first_column, @$.last_column, { $2 });
		pResult->m_pResult = $$;
		YYACCEPT;
	}
	;

get_stmt: select_stmt {$$ = $1;}
	| values_stmt {$$ = $1;}
	| show_tables_stmt { $$ = $1;}
	| desc_table_stmt { $$ = $1;}
	;

stmt: get_stmt {$$ = $1;}
	| insert_stmt { $$ = $1;}
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
			$$ = pResult->newSimpleNode(NodeType::INT,@$.first_column, @$.last_column);
			$$->setString($$->m_sExpr);
		}
		else if($2->m_type == NodeType::FLOAT) 
		{
			$$ = pResult->newSimpleNode( NodeType::FLOAT, @$.first_column, @$.last_column);
			$$->setString($$->m_sExpr);
		}
		else
		{
			yyerror(&@2,pResult, nullptr, "minus is not supporteed");
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
		auto sValue =  $3->getString();
		auto len =  sValue.length();
		if(sValue[0] != '%' || sValue[len - 1] != '%')
		{
			yyerror(&@3,pResult, nullptr, absl::StrCat("missing ",sValue," for like "));
			YYERROR;
		}
		$3->setString(sValue.substr(1, len -2));
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
		$$ = pResult->newExprNode( Operation::IN, @$.first_column, @$.last_column, { $1, $4});
		}
	| expr NOT IN '(' val_list ')' { 
		$$ = pResult->newExprNode( Operation::NOT_IN, @$.first_column, @$.last_column, { $1, $5});
	}
	;
	
expr: NAME '(' expr ')' {
        $$ = pResult->newFuncNode($1->getString(), @$.first_column, @$.last_column, { $3 });
}       
;

val_list: expr {
	$$ = pResult->newListNode( "ValueList",@$.first_column, @$.last_column,  { $1 } );
	}
	| expr ',' val_list { 
	$$ = pResult->newListNode( "ValueList",@$.first_column, @$.last_column,  { $1, $3 } );
	}
	;

delete_stmt: DELETE FROM table_factor opt_where
	{
		ParseNode* pTable = $3;
		$$ = pResult->newPlanNode( "DeleteStmt", Operation::DELETE, @$.first_column, @$.last_column, {pTable, $4 });
	}


values_stmt:VALUES value_list
	{
		$$ = pResult->newPlanNode( "Values", Operation::VALUES, @$.first_column, @$.last_column, { $2 });
	}
	;

insert_stmt: INSERT INTO table_factor opt_col_names select_stmt
	{
	  	ParseNode* pTable = $3;
		$$ = pResult->newPlanNode( "InsertStmt", Operation::INSERT, @$.first_column, @$.last_column,  { pTable,$4,$5 });
	}
	| INSERT INTO table_factor opt_col_names values_stmt
	{
	  	ParseNode* pTable = $3;
		$$ = pResult->newPlanNode( "InsertStmt", Operation::INSERT, @$.first_column, @$.last_column, { pTable,$4,$5 });

	}
	;

show_tables_stmt:SHOW TABLES
	{
		$$ = pResult->newPlanNode("ShowTable",  Operation::SHOW_TABLES,  @$.first_column, @$.last_column, {});
	}
	;
	
desc_table_stmt:DESC table_factor
	{
		$$ = pResult->newPlanNode( "DescStmt", Operation::DESC_TABLE,  @$.first_column, @$.last_column, {$2 });
	}
	; 



	
opt_col_names: /* empty */{$$ = nullptr;}
	| '(' column_list ')' {
		$$ = $2;
	}
	;

value_list: '(' row_value ')' { 
		$$ = pResult->newListNode( "ValueList",@$.first_column, @$.last_column,  { $2 });
	}
	| value_list ',' '(' row_value ')' {
		$$ = pResult->newListNode( "ValueList",@$.first_column, @$.last_column,  { $1, $4 });
	}

row_value: expr {
		$$ = pResult->newListNode( "ExprList", @$.first_column, @$.last_column, { $1 });
	}
	| row_value ',' expr { 
		$$ = pResult->newListNode( "ExprList", @$.first_column, @$.last_column, { $1, $3 });
	}
	;

column_list: NAME { 
		$$ = pResult->newListNode( "ColumnList", @$.first_column, @$.last_column, { $1 });
	}
	| column_list ',' NAME {
		$$ = pResult->newListNode( "ColumnList", @$.first_column, @$.last_column, { $1, $3 });
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
		ParseNode* pProject = $2;
		ParseNode* pTable = $4;
		ParseNode* pAlias = $5;
		ParseNode* pPredicate = $6;

		if(pAlias != nullptr)
		{
			yyerror(&@5,pResult,nullptr, "table alias name is not supported");
			YYERROR;
		}
		$$ = pResult->newPlanNode( "SelectStmt", Operation::SELECT, @$.first_column, @$.last_column, { pProject, pTable, pPredicate, $7, $8, $9, $10 });
}
	;


opt_where:{$$ = 0;}
	| WHERE expr {
		$$ = $2;
	}
	;

opt_limit:{$$ = 0;}
	| LIMIT INTNUM OFFSET INTNUM 
	{
	       $$ = pResult->newParentNode( "Limit",@$.first_column, @$.last_column, { $2, $4}); 
	}
	| LIMIT INTNUM ',' INTNUM 
	{
	       $$ = pResult->newParentNode( "Limit",@$.first_column, @$.last_column,{ $4, $2 }); 
	}

opt_groupby:{$$ = 0;}
	| GROUP BY column_list {
		$$ = $3;
	}
	;

sort_list: expr opt_asc_desc {
			auto pChild = pResult->newParentNode( "SortItem", @$.first_column, @$.last_column,{ $1, $2 }); 
			$$ = pResult->newListNode( "SortList",@$.first_column, @$.last_column, { pChild });
		}
	| sort_list ',' expr opt_asc_desc { 
			auto pChild =  pResult->newParentNode( "SortItem",@$.first_column, @$.last_column, { $3, $4 });
			$$ = pResult->newListNode( "SortList",@$.first_column, @$.last_column, { $1,pChild });
		}
	;

opt_asc_desc:{$$ = pResult->newInfoNode( Operation::ASC,  @$.first_column, @$.last_column);}
	| ASC {$$ = pResult->newInfoNode(  Operation::ASC,  @$.first_column, @$.last_column);}
	| DESC {$$ = pResult->newInfoNode(  Operation::DESC,  @$.first_column, @$.last_column);}
	;

opt_having:{$$ = 0;}
	| HAVING expr {
		$$ = $2;
	}

	;
	
opt_orderby:{$$ = 0;}
	| ORDER BY sort_list {
		$$ = $3;
	}
	;

projection: expr {
		$$ = $1; 
	} | expr AS NAME { 
		$$ = pResult->newExprNode( Operation::AS, @$.first_column, @$.last_column, { $1, $3 }); 
	}

select_expr_list: projection { 
		$$ = pResult->newListNode( "ExprList", @$.first_column, @$.last_column, { $1 });
	}
	| select_expr_list ',' projection {
		$$ = pResult->newListNode( "ExprList", @$.first_column, @$.last_column, { $1, $3 });
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

	YY_BUFFER_STATE bp;

	bp = yy_scan_string(p->m_sSql.data(), p->m_scanInfo);
	yy_switch_to_buffer(bp, p->m_scanInfo);
	yyparse(p, p->m_scanInfo);
	yy_delete_buffer(bp, p->m_scanInfo);
}
