/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_HOME_GUOXIANG_GOPATH_SRC_GITHUB_COM_LUGUOXIANG_LEVEL_PGSERVER_COMMON_SQLPARSER_TAB_H_INCLUDED
# define YY_YY_HOME_GUOXIANG_GOPATH_SRC_GITHUB_COM_LUGUOXIANG_LEVEL_PGSERVER_COMMON_SQLPARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 5 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1909  */

typedef void* yyscan_t;
#include "ParseResult.h"
#include "MetaConfig.h"
#include <stdint.h>
#include <absl/strings/str_cat.h>

#line 52 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    NAME = 258,
    STRING = 259,
    BINARY = 260,
    INTNUM = 261,
    DATE_VALUE = 262,
    HINT_VALUE = 263,
    BOOL = 264,
    APPROXNUM = 265,
    NULLX = 266,
    PARAM = 267,
    ASSIGN = 268,
    OR = 269,
    XOR = 270,
    ANDOP = 271,
    IN = 272,
    IS = 273,
    LIKE = 274,
    REGEXP = 275,
    NOT = 276,
    COMP_LE = 277,
    COMP_LT = 278,
    COMP_EQ = 279,
    COMP_GT = 280,
    COMP_GE = 281,
    COMP_NE = 282,
    MOD = 283,
    UMINUS = 284,
    UPLUS = 285,
    DATA = 286,
    FIELDS = 287,
    INFILE = 288,
    SHOW = 289,
    TABLES = 290,
    TERMINATED = 291,
    ERROR = 292,
    ADD = 293,
    ALL = 294,
    AND = 295,
    ANY = 296,
    AS = 297,
    ASC = 298,
    BETWEEN = 299,
    CHAR = 300,
    CREATE = 301,
    DATABASE = 302,
    DATE = 303,
    DATETIME = 304,
    DEFAULT = 305,
    DELETE = 306,
    DESC = 307,
    DISTINCT = 308,
    FLOAT = 309,
    DOUBLE = 310,
    DROP = 311,
    EXPLAIN = 312,
    FROM = 313,
    GROUP = 314,
    HAVING = 315,
    INSERT = 316,
    INTEGER = 317,
    INTO = 318,
    LIMIT = 319,
    LOAD = 320,
    ON = 321,
    ORDER = 322,
    SELECT = 323,
    WHERE = 324,
    BY = 325,
    SMALLINT = 326,
    TABLE = 327,
    TO = 328,
    UNION = 329,
    UNIQUE = 330,
    UNSIGNED = 331,
    UPDATE = 332,
    SET = 333,
    VALUES = 334,
    VARCHAR = 335,
    OFFSET = 336,
    NOT_IN = 337,
    NOT_LIKE = 338,
    ALL_ROW = 339,
    ALL_COLUMN = 340,
    PRIMARY_KEY = 341,
    NOT_NULL = 342,
    KEY = 343,
    PRIMARY = 344,
    IF = 345,
    EXISTS = 346,
    IF_EXISTS = 347,
    JOIN = 348,
    LEFT = 349,
    USING = 350
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 13 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1909  */

	ParseNode* pNode;

#line 164 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int yyparse (ParseResult* pResult, yyscan_t scanner);

#endif /* !YY_YY_HOME_GUOXIANG_GOPATH_SRC_GITHUB_COM_LUGUOXIANG_LEVEL_PGSERVER_COMMON_SQLPARSER_TAB_H_INCLUDED  */
