/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
 
 Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
 
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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
/* Put the tokens into the symbol table, so that GDB and other debuggers
 know about them.  */
enum yytokentype {
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
	REGEXP = 272,
	LIKE = 273,
	IS = 274,
	IN = 275,
	NOT = 276,
	COMP_NE = 277,
	COMP_GE = 278,
	COMP_GT = 279,
	COMP_EQ = 280,
	COMP_LT = 281,
	COMP_LE = 282,
	MOD = 283,
	UMINUS = 284,
	UPLUS = 285,
	DATA = 286,
	FIELDS = 287,
	INFILE = 288,
	SHOW = 289,
	TABLES = 290,
	TERMINATED = 291,
	WORKLOAD = 292,
	ERROR = 293,
	ADD = 294,
	ALL = 295,
	AND = 296,
	ANY = 297,
	AS = 298,
	ASC = 299,
	BETWEEN = 300,
	CHAR = 301,
	CREATE = 302,
	DATABASE = 303,
	DATE = 304,
	DATETIME = 305,
	DEFAULT = 306,
	DELETE = 307,
	DESC = 308,
	DISTINCT = 309,
	FLOAT = 310,
	DOUBLE = 311,
	DROP = 312,
	EXPLAIN = 313,
	FROM = 314,
	GROUP = 315,
	HAVING = 316,
	INSERT = 317,
	INTEGER = 318,
	INTO = 319,
	LIMIT = 320,
	LOAD = 321,
	ON = 322,
	ORDER = 323,
	SELECT = 324,
	WHERE = 325,
	BY = 326,
	SMALLINT = 327,
	TABLE = 328,
	TO = 329,
	UNION = 330,
	UNIQUE = 331,
	UNSIGNED = 332,
	UPDATE = 333,
	SET = 334,
	VALUES = 335,
	VARCHAR = 336,
	OFFSET = 337,
	NOT_IN = 338,
	NOT_LIKE = 339,
	ALL_ROW = 340,
	ALL_COLUMN = 341,
	PRIMARY_KEY = 342,
	NOT_NULL = 343,
	KEY = 344,
	PRIMARY = 345,
	IF = 346,
	EXISTS = 347,
	IF_EXISTS = 348,
	JOIN = 349,
	LEFT = 350,
	USING = 351
};
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE {

	/* Line 2132 of yacc.c  */
#line 9 "SqlParser.y"

	struct _ParseNode* pNode;

	/* Line 2132 of yacc.c  */
#line 152 "SqlParser.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE {
	int first_line;
	int first_column;
	int last_line;
	int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

