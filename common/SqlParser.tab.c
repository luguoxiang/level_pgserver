/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 4 "SqlParser.y"

#include "BuildPlan.h"
#include <stdint.h>


/* Line 268 of yacc.c  */
#line 77 "SqlParser.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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
typedef union YYSTYPE
{

/* Line 301 of yacc.c  */
#line 9 "SqlParser.y"

	struct _ParseNode* pNode;



/* Line 301 of yacc.c  */
#line 215 "SqlParser.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */

/* Line 343 of yacc.c  */
#line 13 "SqlParser.y"

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



/* Line 343 of yacc.c  */
#line 314 "SqlParser.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  51
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   425

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  111
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  36
/* YYNRULES -- Number of rules.  */
#define YYNRULES  101
/* YYNRULES -- Number of states.  */
#define YYNSTATES  210

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   351

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    21,     2,     2,     2,    35,    23,     2,
     107,   108,    33,    31,   110,    32,   109,    34,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   106,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    37,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    30,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    22,    24,    25,    26,
      27,    28,    29,    36,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,    10,    19,    21,    23,    25,    27,
      29,    31,    33,    35,    37,    39,    41,    43,    45,    47,
      49,    51,    53,    55,    59,    61,    65,    69,    73,    77,
      81,    85,    88,    91,    95,    99,   103,   107,   111,   115,
     119,   123,   127,   131,   135,   140,   146,   153,   158,   160,
     164,   169,   175,   179,   185,   188,   194,   200,   203,   206,
     208,   221,   222,   226,   230,   236,   238,   242,   244,   248,
     250,   254,   255,   258,   259,   261,   273,   281,   283,   286,
     287,   290,   291,   296,   301,   302,   306,   309,   314,   315,
     317,   319,   320,   323,   324,   328,   330,   334,   336,   340,
     342,   344
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     112,     0,    -1,   115,   106,    -1,    67,   115,   106,    -1,
     107,   114,   108,    84,    49,   107,   114,   108,    -1,   134,
      -1,   113,    -1,   121,    -1,   125,    -1,   123,    -1,   124,
      -1,   114,    -1,   122,    -1,   126,    -1,   119,    -1,   118,
      -1,     3,    -1,     4,    -1,     5,    -1,     7,    -1,     6,
      -1,    10,    -1,     9,    -1,     3,   109,     3,    -1,    47,
      -1,   116,    31,   116,    -1,   116,    32,   116,    -1,   116,
      33,   116,    -1,   116,    34,   116,    -1,   116,    35,   116,
      -1,   116,    36,   116,    -1,    32,   116,    -1,    31,   116,
      -1,   116,    29,   116,    -1,   116,    28,   116,    -1,   116,
      27,   116,    -1,   116,    25,   116,    -1,   116,    26,   116,
      -1,   116,    24,   116,    -1,   116,    18,     4,    -1,   116,
      16,   116,    -1,   116,    14,   116,    -1,   107,   116,   108,
      -1,   116,    19,    11,    -1,   116,    19,    22,    11,    -1,
     116,    20,   107,   117,   108,    -1,   116,    22,    20,   107,
     117,   108,    -1,     3,   107,   116,   108,    -1,   116,    -1,
     116,   110,   117,    -1,    61,    68,   146,   137,    -1,    87,
     146,    88,   120,   137,    -1,     3,    27,   116,    -1,   120,
     110,     3,    27,   116,    -1,    89,   128,    -1,    71,    73,
     146,   127,   134,    -1,    71,    73,   146,   127,   121,    -1,
      43,    44,    -1,    62,   146,    -1,    46,    -1,    75,    40,
      42,     4,    73,    82,   146,   127,    41,    45,    80,     4,
      -1,    -1,   107,   130,   108,    -1,   107,   129,   108,    -1,
     128,   110,   107,   129,   108,    -1,   116,    -1,   129,   110,
     116,    -1,     3,    -1,   130,   110,     3,    -1,   146,    -1,
     107,   114,   108,    -1,    -1,    52,     3,    -1,    -1,   136,
      -1,    78,   145,    68,   131,   132,   137,   139,   142,   143,
     138,   133,    -1,   104,   103,   146,   105,   107,   130,   108,
      -1,   135,    -1,   136,   135,    -1,    -1,    79,   116,    -1,
      -1,    74,     6,    91,     6,    -1,    74,     6,   110,     6,
      -1,    -1,    69,    80,   130,    -1,   116,   141,    -1,   140,
     110,   116,   141,    -1,    -1,    53,    -1,    62,    -1,    -1,
      70,   116,    -1,    -1,    77,    80,   140,    -1,   116,    -1,
     116,    52,     3,    -1,   144,    -1,   145,   110,   144,    -1,
      33,    -1,     3,    -1,     3,   109,     3,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   205,   205,   210,   219,   225,   226,   227,   228,   229,
     230,   233,   234,   235,   236,   237,   240,   241,   242,   243,
     244,   245,   246,   247,   248,   251,   252,   253,   254,   255,
     256,   257,   272,   275,   276,   277,   278,   279,   280,   281,
     291,   292,   293,   297,   300,   305,   309,   315,   320,   321,
     324,   337,   345,   349,   356,   363,   375,   389,   396,   403,
     410,   428,   429,   432,   435,   440,   441,   445,   446,   451,
     452,   455,   456,   459,   460,   462,   521,   528,   532,   537,
     538,   544,   545,   550,   556,   557,   563,   566,   572,   573,
     574,   577,   578,   585,   586,   592,   594,   598,   601,   604,
     609,   612
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NAME", "STRING", "BINARY", "INTNUM",
  "DATE_VALUE", "HINT_VALUE", "BOOL", "APPROXNUM", "NULLX", "PARAM",
  "ASSIGN", "OR", "XOR", "ANDOP", "REGEXP", "LIKE", "IS", "IN", "'!'",
  "NOT", "'&'", "COMP_NE", "COMP_GE", "COMP_GT", "COMP_EQ", "COMP_LT",
  "COMP_LE", "'|'", "'+'", "'-'", "'*'", "'/'", "'%'", "MOD", "'^'",
  "UMINUS", "UPLUS", "DATA", "FIELDS", "INFILE", "SHOW", "TABLES",
  "TERMINATED", "WORKLOAD", "ERROR", "ADD", "ALL", "AND", "ANY", "AS",
  "ASC", "BETWEEN", "CHAR", "CREATE", "DATABASE", "DATE", "DATETIME",
  "DEFAULT", "DELETE", "DESC", "DISTINCT", "FLOAT", "DOUBLE", "DROP",
  "EXPLAIN", "FROM", "GROUP", "HAVING", "INSERT", "INTEGER", "INTO",
  "LIMIT", "LOAD", "ON", "ORDER", "SELECT", "WHERE", "BY", "SMALLINT",
  "TABLE", "TO", "UNION", "UNIQUE", "UNSIGNED", "UPDATE", "SET", "VALUES",
  "VARCHAR", "OFFSET", "NOT_IN", "NOT_LIKE", "ALL_ROW", "ALL_COLUMN",
  "PRIMARY_KEY", "NOT_NULL", "KEY", "PRIMARY", "IF", "EXISTS", "IF_EXISTS",
  "JOIN", "LEFT", "USING", "';'", "'('", "')'", "'.'", "','", "$accept",
  "sql_stmt", "merge_stmt", "get_stmt", "stmt", "expr", "val_list",
  "delete_stmt", "update_stmt", "update_asgn_list", "values_stmt",
  "insert_stmt", "show_tables_stmt", "desc_table_stmt", "workload_stmt",
  "load_stmt", "opt_col_names", "value_list", "row_value", "column_list",
  "table_or_query", "opt_alias", "opt_join", "select_stmt", "join_clause",
  "join_list", "opt_where", "opt_limit", "opt_groupby", "sort_list",
  "opt_asc_desc", "opt_having", "opt_orderby", "projection",
  "select_expr_list", "table_factor", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,    33,   276,    38,   277,   278,   279,   280,   281,   282,
     124,    43,    45,    42,    47,    37,   283,    94,   284,   285,
     286,   287,   288,   289,   290,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   350,   351,    59,    40,    41,    46,
      44
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   111,   112,   112,   113,   114,   114,   114,   114,   114,
     114,   115,   115,   115,   115,   115,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   117,   117,
     118,   119,   120,   120,   121,   122,   122,   123,   124,   125,
     126,   127,   127,   128,   128,   129,   129,   130,   130,   131,
     131,   132,   132,   133,   133,   134,   135,   136,   136,   137,
     137,   138,   138,   138,   139,   139,   140,   140,   141,   141,
     141,   142,   142,   143,   143,   144,   144,   145,   145,   145,
     146,   146
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     3,     8,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     3,     3,     3,     3,     3,
       3,     2,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     4,     5,     6,     4,     1,     3,
       4,     5,     3,     5,     2,     5,     5,     2,     2,     1,
      12,     0,     3,     3,     5,     1,     3,     1,     3,     1,
       3,     0,     2,     0,     1,    11,     7,     1,     2,     0,
       2,     0,     4,     4,     0,     3,     2,     4,     0,     1,
       1,     0,     2,     0,     3,     1,     3,     1,     3,     1,
       1,     3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,    59,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     6,    11,     0,    15,    14,     7,    12,
       9,    10,     8,    13,     5,    57,     0,   100,    58,     0,
       0,     0,    16,    17,    18,    20,    19,    22,    21,     0,
       0,    99,    24,     0,    95,    97,     0,     0,     0,    54,
       0,     1,     2,    79,     0,     3,    61,     0,     0,     0,
      32,    31,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    65,     0,     0,     0,     0,
      50,   101,     0,     0,     0,     0,    23,    42,    41,    40,
      39,    43,     0,     0,     0,    38,    36,    37,    35,    34,
      33,    25,    26,    27,    28,    29,    30,    96,     0,    71,
      69,    98,     0,    79,    63,     0,     0,     0,    80,    67,
       0,    56,    55,     0,    47,    44,    48,     0,     0,     0,
       0,    79,     0,     0,    51,    66,     0,     0,    62,     0,
       0,     0,    45,     0,    70,    72,    84,    52,     0,    64,
       0,    68,    61,    49,    46,     0,    91,     0,     0,     0,
       0,     0,    93,    53,     4,     0,    85,    92,     0,    81,
       0,     0,     0,    73,     0,    88,    94,     0,     0,    75,
      77,    74,    60,    89,    90,    86,     0,     0,     0,     0,
      78,    88,    82,    83,     0,    87,     0,     0,     0,    76
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    12,    13,    14,    15,   136,   137,    16,    17,   123,
      18,    19,    20,    21,    22,    23,    93,    49,    86,   130,
     119,   141,   189,    24,   190,   191,    90,   183,   166,   186,
     195,   172,   179,    45,    46,    28
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -161
static const yytype_int16 yypact[] =
{
     195,   -29,  -161,   -49,    33,   203,   -30,    24,    -1,    33,
     -42,     9,    86,  -161,  -161,    -7,  -161,  -161,  -161,  -161,
    -161,  -161,  -161,  -161,  -161,  -161,    33,   -13,  -161,    -6,
      33,    55,   -89,  -161,  -161,  -161,  -161,  -161,  -161,    19,
      19,  -161,  -161,    19,   320,  -161,   -56,    13,    19,    -8,
      -5,  -161,  -161,    25,   102,  -161,     0,   105,    19,   107,
    -161,  -161,   178,    19,    19,   108,    27,     6,    94,    19,
      19,    19,    19,    19,    19,    19,    19,    19,    19,    19,
      19,   112,     4,    19,   114,   349,   -73,    11,    35,    19,
    -161,  -161,   117,   -22,    48,   201,  -161,  -161,   370,   389,
    -161,  -161,   111,    19,    16,    26,    26,    26,    26,    26,
      26,    56,    56,  -161,  -161,  -161,  -161,  -161,     9,    72,
    -161,  -161,    98,   -68,  -161,    19,    19,    78,   349,  -161,
     -69,  -161,  -161,    46,  -161,  -161,   135,    21,    19,    28,
     129,    25,    19,   130,  -161,   349,   -38,    30,  -161,   131,
      33,    19,  -161,    31,  -161,  -161,    66,   349,   113,  -161,
       9,  -161,     0,  -161,  -161,    58,    73,    19,    34,   103,
     117,    19,    68,   349,  -161,   101,    38,   349,    76,    84,
      85,    19,   146,    69,   170,   297,    65,   -77,    74,  -161,
    -161,    69,  -161,  -161,  -161,  -161,    19,   172,   173,    33,
    -161,   297,  -161,  -161,    75,  -161,    77,   117,   -15,  -161
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -161,  -161,  -161,   -10,   176,     5,  -111,  -161,  -161,  -161,
      89,  -161,  -161,  -161,  -161,  -161,    23,  -161,    57,  -160,
    -161,  -161,  -161,    95,    -4,  -161,  -107,  -161,  -161,  -161,
     -12,  -161,  -161,   110,  -161,    -9
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      47,    50,    32,    33,    34,    35,    36,    27,    37,    38,
     176,    89,    82,    44,   197,    25,   144,    53,    58,    26,
      59,    56,    32,    33,    34,    35,    36,   153,    37,    38,
      39,    40,    41,   198,   156,   124,    27,   125,   101,   148,
     163,   149,   143,    30,    60,    61,    42,   208,    62,   102,
      39,    40,     1,    85,    83,     2,     8,    75,    76,    77,
      78,    79,    80,    95,    31,    48,    42,    10,    98,    99,
     159,     4,   125,   120,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,    51,     8,    44,    77,
      78,    79,    80,   209,   128,   149,    54,    57,    10,    52,
      55,    84,    87,    88,    89,    91,    43,    92,   139,    94,
      96,   118,   100,   103,   104,   117,    11,   122,   126,   127,
     129,   133,   135,   138,   140,   142,    43,   147,   150,   152,
     145,    85,   155,   158,   161,   165,   154,   160,   170,   164,
     167,   162,   174,   171,   175,   178,   180,   157,   149,    63,
     168,    64,   187,    65,    66,    67,   181,    68,   182,    69,
      70,    71,    72,    73,    74,   184,    75,    76,    77,    78,
      79,    80,   173,   188,   192,   196,   177,   199,   202,   203,
     206,    29,   131,   146,   207,   169,   185,   200,   132,   205,
     204,     0,    63,   121,    64,     0,    65,    66,    67,     0,
      68,   201,    69,    70,    71,    72,    73,    74,     0,    75,
      76,    77,    78,    79,    80,    63,     0,    64,     0,    65,
      66,    67,     0,    68,     0,    69,    70,    71,    72,    73,
      74,     0,    75,    76,    77,    78,    79,    80,     1,     0,
       0,     2,     0,     0,     0,   151,     1,     0,     0,     2,
       0,     0,     0,     0,     0,     0,     3,     4,     0,     0,
       0,     0,     5,     0,     3,     4,     6,     0,     0,     0,
       7,     0,     0,     8,     6,     0,     0,     0,     7,     0,
       0,     8,     9,     0,    10,     0,    97,     0,     0,     0,
       9,     0,    10,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    11,     0,     0,     0,     0,     0,     0,   134,
      11,    63,     0,    64,     0,    65,    66,    67,     0,    68,
       0,    69,    70,    71,    72,    73,    74,     0,    75,    76,
      77,    78,    79,    80,    63,     0,    64,     0,    65,    66,
      67,     0,    68,     0,    69,    70,    71,    72,    73,    74,
     193,    75,    76,    77,    78,    79,    80,     0,     0,   194,
       0,     0,     0,    63,     0,    64,     0,    65,    66,    67,
       0,    68,    81,    69,    70,    71,    72,    73,    74,     0,
      75,    76,    77,    78,    79,    80,    64,     0,    65,    66,
      67,     0,    68,     0,    69,    70,    71,    72,    73,    74,
       0,    75,    76,    77,    78,    79,    80,    65,    66,    67,
       0,    68,     0,    69,    70,    71,    72,    73,    74,     0,
      75,    76,    77,    78,    79,    80
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-161))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       9,    11,     3,     4,     5,     6,     7,     3,     9,    10,
     170,    79,    68,     8,    91,    44,   123,    26,   107,    68,
     109,    30,     3,     4,     5,     6,     7,   138,     9,    10,
      31,    32,    33,   110,   141,   108,     3,   110,    11,   108,
     151,   110,   110,    73,    39,    40,    47,   207,    43,    22,
      31,    32,    43,    48,   110,    46,    78,    31,    32,    33,
      34,    35,    36,    58,    40,   107,    47,    89,    63,    64,
     108,    62,   110,    82,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,     0,    78,    83,    33,
      34,    35,    36,   108,    89,   110,   109,    42,    89,   106,
     106,    88,   110,   108,    79,     3,   107,   107,   118,     4,
       3,   107,     4,   107,    20,     3,   107,     3,   107,    84,
       3,    73,    11,   107,    52,    27,   107,    49,    82,   108,
     125,   126,     3,     3,     3,    69,   108,   107,    80,   108,
      27,   150,   108,    70,    41,    77,    45,   142,   110,    14,
     160,    16,     6,    18,    19,    20,    80,    22,    74,    24,
      25,    26,    27,    28,    29,    80,    31,    32,    33,    34,
      35,    36,   167,   104,     4,   110,   171,   103,     6,     6,
     105,     5,    93,   126,   107,   162,   181,   191,    93,   201,
     199,    -1,    14,    83,    16,    -1,    18,    19,    20,    -1,
      22,   196,    24,    25,    26,    27,    28,    29,    -1,    31,
      32,    33,    34,    35,    36,    14,    -1,    16,    -1,    18,
      19,    20,    -1,    22,    -1,    24,    25,    26,    27,    28,
      29,    -1,    31,    32,    33,    34,    35,    36,    43,    -1,
      -1,    46,    -1,    -1,    -1,   110,    43,    -1,    -1,    46,
      -1,    -1,    -1,    -1,    -1,    -1,    61,    62,    -1,    -1,
      -1,    -1,    67,    -1,    61,    62,    71,    -1,    -1,    -1,
      75,    -1,    -1,    78,    71,    -1,    -1,    -1,    75,    -1,
      -1,    78,    87,    -1,    89,    -1,   108,    -1,    -1,    -1,
      87,    -1,    89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107,    -1,    -1,    -1,    -1,    -1,    -1,   108,
     107,    14,    -1,    16,    -1,    18,    19,    20,    -1,    22,
      -1,    24,    25,    26,    27,    28,    29,    -1,    31,    32,
      33,    34,    35,    36,    14,    -1,    16,    -1,    18,    19,
      20,    -1,    22,    -1,    24,    25,    26,    27,    28,    29,
      53,    31,    32,    33,    34,    35,    36,    -1,    -1,    62,
      -1,    -1,    -1,    14,    -1,    16,    -1,    18,    19,    20,
      -1,    22,    52,    24,    25,    26,    27,    28,    29,    -1,
      31,    32,    33,    34,    35,    36,    16,    -1,    18,    19,
      20,    -1,    22,    -1,    24,    25,    26,    27,    28,    29,
      -1,    31,    32,    33,    34,    35,    36,    18,    19,    20,
      -1,    22,    -1,    24,    25,    26,    27,    28,    29,    -1,
      31,    32,    33,    34,    35,    36
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    43,    46,    61,    62,    67,    71,    75,    78,    87,
      89,   107,   112,   113,   114,   115,   118,   119,   121,   122,
     123,   124,   125,   126,   134,    44,    68,     3,   146,   115,
      73,    40,     3,     4,     5,     6,     7,     9,    10,    31,
      32,    33,    47,   107,   116,   144,   145,   146,   107,   128,
     114,     0,   106,   146,   109,   106,   146,    42,   107,   109,
     116,   116,   116,    14,    16,    18,    19,    20,    22,    24,
      25,    26,    27,    28,    29,    31,    32,    33,    34,    35,
      36,    52,    68,   110,    88,   116,   129,   110,   108,    79,
     137,     3,   107,   127,     4,   116,     3,   108,   116,   116,
       4,    11,    22,   107,    20,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,     3,   107,   131,
     146,   144,     3,   120,   108,   110,   107,    84,   116,     3,
     130,   121,   134,    73,   108,    11,   116,   117,   107,   114,
      52,   132,    27,   110,   137,   116,   129,    49,   108,   110,
      82,   110,   108,   117,   108,     3,   137,   116,     3,   108,
     107,     3,   146,   117,   108,    69,   139,    27,   114,   127,
      80,    70,   142,   116,   108,    41,   130,   116,    77,   143,
      45,    80,    74,   138,    80,   116,   140,     6,   104,   133,
     135,   136,     4,    53,    62,   141,   110,    91,   110,   103,
     135,   116,     6,     6,   146,   141,   105,   107,   130,   108
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (&yylloc, pResult, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location, pResult); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, ParseResult* pResult)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, pResult)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    ParseResult* pResult;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (pResult);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, ParseResult* pResult)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, pResult)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    ParseResult* pResult;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, pResult);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, ParseResult* pResult)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, pResult)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    ParseResult* pResult;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , pResult);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, pResult); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, ParseResult* pResult)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, pResult)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    ParseResult* pResult;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (pResult);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (ParseResult* pResult);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (ParseResult* pResult)
#else
int
yyparse (pResult)
    ParseResult* pResult;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1821 of yacc.c  */
#line 206 "SqlParser.y"
    {
		pResult->m_pResult = (yyval.pNode) = (yyvsp[(1) - (2)].pNode);
		YYACCEPT;
	}
    break;

  case 3:

/* Line 1821 of yacc.c  */
#line 211 "SqlParser.y"
    {
		(yyval.pNode) = newParentNode(pResult, "ExplainStmt", 1, (yyvsp[(2) - (3)].pNode));
		(yyval.pNode)->m_fnBuildPlan = buildPlanForExplain;
		pResult->m_pResult = (yyval.pNode);
		YYACCEPT;
	}
    break;

  case 4:

/* Line 1821 of yacc.c  */
#line 220 "SqlParser.y"
    {
		(yyval.pNode) = newParentNode(pResult, "UnionAll", 2, (yyvsp[(2) - (8)].pNode), (yyvsp[(7) - (8)].pNode));
		(yyval.pNode)->m_fnBuildPlan = buildPlanForUnionAll;
	}
    break;

  case 5:

/* Line 1821 of yacc.c  */
#line 225 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 6:

/* Line 1821 of yacc.c  */
#line 226 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 7:

/* Line 1821 of yacc.c  */
#line 227 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 8:

/* Line 1821 of yacc.c  */
#line 228 "SqlParser.y"
    { (yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 9:

/* Line 1821 of yacc.c  */
#line 229 "SqlParser.y"
    { (yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 10:

/* Line 1821 of yacc.c  */
#line 230 "SqlParser.y"
    { (yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 11:

/* Line 1821 of yacc.c  */
#line 233 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 12:

/* Line 1821 of yacc.c  */
#line 234 "SqlParser.y"
    { (yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 13:

/* Line 1821 of yacc.c  */
#line 235 "SqlParser.y"
    { (yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 14:

/* Line 1821 of yacc.c  */
#line 236 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 15:

/* Line 1821 of yacc.c  */
#line 237 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 16:

/* Line 1821 of yacc.c  */
#line 240 "SqlParser.y"
    { (yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 17:

/* Line 1821 of yacc.c  */
#line 241 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 18:

/* Line 1821 of yacc.c  */
#line 242 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 19:

/* Line 1821 of yacc.c  */
#line 243 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 20:

/* Line 1821 of yacc.c  */
#line 244 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 21:

/* Line 1821 of yacc.c  */
#line 245 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 22:

/* Line 1821 of yacc.c  */
#line 246 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 23:

/* Line 1821 of yacc.c  */
#line 247 "SqlParser.y"
    {(yyval.pNode) = newExprNode(pResult, '.', (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;

  case 24:

/* Line 1821 of yacc.c  */
#line 248 "SqlParser.y"
    {(yyval.pNode) = 0;YYERROR;}
    break;

  case 25:

/* Line 1821 of yacc.c  */
#line 251 "SqlParser.y"
    {(yyval.pNode) = newExprNode(pResult, '+', (yyloc).first_column, (yyloc).last_column,  2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;

  case 26:

/* Line 1821 of yacc.c  */
#line 252 "SqlParser.y"
    {(yyval.pNode) = newExprNode(pResult, '-', (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;

  case 27:

/* Line 1821 of yacc.c  */
#line 253 "SqlParser.y"
    {(yyval.pNode) = newExprNode(pResult, '*', (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;

  case 28:

/* Line 1821 of yacc.c  */
#line 254 "SqlParser.y"
    {(yyval.pNode) = newExprNode(pResult, '/', (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;

  case 29:

/* Line 1821 of yacc.c  */
#line 255 "SqlParser.y"
    {(yyval.pNode) = newExprNode(pResult, '%', (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;

  case 30:

/* Line 1821 of yacc.c  */
#line 256 "SqlParser.y"
    {(yyval.pNode) = newExprNode(pResult, '%', (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;

  case 31:

/* Line 1821 of yacc.c  */
#line 257 "SqlParser.y"
    {
		if((yyvsp[(2) - (2)].pNode)->m_iType == INT_NODE)
		{
			(yyvsp[(2) - (2)].pNode)->m_iValue = - (yyvsp[(2) - (2)].pNode)->m_iValue;
			char szBuf[20];
			snprintf(szBuf,20, "%ld", (yyvsp[(2) - (2)].pNode)->m_iValue);
			(yyvsp[(2) - (2)].pNode)->m_pszValue = my_strdup(pResult, szBuf);
			(yyvsp[(2) - (2)].pNode)->m_pszExpr = (yyvsp[(2) - (2)].pNode)->m_pszValue;
			(yyval.pNode) = (yyvsp[(2) - (2)].pNode);
		}
		else
		{
			(yyval.pNode) = newExprNode(pResult, '-',(yyloc).first_column, (yyloc).last_column, 1, (yyvsp[(2) - (2)].pNode));
		}
	}
    break;

  case 32:

/* Line 1821 of yacc.c  */
#line 272 "SqlParser.y"
    {
		(yyval.pNode) = (yyvsp[(2) - (2)].pNode);
	}
    break;

  case 33:

/* Line 1821 of yacc.c  */
#line 275 "SqlParser.y"
    {(yyval.pNode) = newExprNode(pResult, COMP_LE, (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;

  case 34:

/* Line 1821 of yacc.c  */
#line 276 "SqlParser.y"
    {(yyval.pNode) = newExprNode(pResult, COMP_LT, (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;

  case 35:

/* Line 1821 of yacc.c  */
#line 277 "SqlParser.y"
    {(yyval.pNode) = newExprNode(pResult, COMP_EQ, (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;

  case 36:

/* Line 1821 of yacc.c  */
#line 278 "SqlParser.y"
    {(yyval.pNode) = newExprNode(pResult, COMP_GE, (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;

  case 37:

/* Line 1821 of yacc.c  */
#line 279 "SqlParser.y"
    {(yyval.pNode) = newExprNode(pResult, COMP_GT, (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;

  case 38:

/* Line 1821 of yacc.c  */
#line 280 "SqlParser.y"
    {(yyval.pNode) = newExprNode(pResult, COMP_NE, (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;

  case 39:

/* Line 1821 of yacc.c  */
#line 281 "SqlParser.y"
    {
		if((yyvsp[(3) - (3)].pNode)->m_pszValue[0] != '%' || (yyvsp[(3) - (3)].pNode)->m_pszValue[(yyvsp[(3) - (3)].pNode)->m_iValue - 1] != '%')
		{
			yyerror(&(yylsp[(3) - (3)]),pResult, "missing %% for like '%s'", (yyvsp[(3) - (3)].pNode)->m_pszValue);
			YYERROR;
		}
		(yyvsp[(3) - (3)].pNode)->m_iValue = (yyvsp[(3) - (3)].pNode)->m_iValue - 2;
		strncpy((char*)(yyvsp[(3) - (3)].pNode)->m_pszValue, (yyvsp[(3) - (3)].pNode)->m_pszValue + 1, (yyvsp[(3) - (3)].pNode)->m_iValue);
		(yyval.pNode) = newExprNode(pResult, LIKE, (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));
	}
    break;

  case 40:

/* Line 1821 of yacc.c  */
#line 291 "SqlParser.y"
    {(yyval.pNode) = newExprNode(pResult, ANDOP, (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;

  case 41:

/* Line 1821 of yacc.c  */
#line 292 "SqlParser.y"
    {(yyval.pNode) = newExprNode(pResult, OR, (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;

  case 42:

/* Line 1821 of yacc.c  */
#line 293 "SqlParser.y"
    { (yyval.pNode) = (yyvsp[(2) - (3)].pNode);}
    break;

  case 43:

/* Line 1821 of yacc.c  */
#line 297 "SqlParser.y"
    {
		(yyval.pNode) = newExprNode(pResult,COMP_EQ, (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode)); 
	}
    break;

  case 44:

/* Line 1821 of yacc.c  */
#line 300 "SqlParser.y"
    {
		(yyval.pNode) = newExprNode(pResult, COMP_NE, (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (4)].pNode), (yyvsp[(4) - (4)].pNode)); 
	}
    break;

  case 45:

/* Line 1821 of yacc.c  */
#line 305 "SqlParser.y"
    {
		(yyval.pNode) = newExprNode(pResult, IN, (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (5)].pNode), 
			mergeTree(pResult, "ValueList", (yyvsp[(4) - (5)].pNode), "ValueList"));
		}
    break;

  case 46:

/* Line 1821 of yacc.c  */
#line 309 "SqlParser.y"
    { 
		(yyval.pNode) = newExprNode(pResult, NOT_IN, (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (6)].pNode), 
			mergeTree(pResult, "ValueList", (yyvsp[(5) - (6)].pNode), "ValueList"));
	}
    break;

  case 47:

/* Line 1821 of yacc.c  */
#line 315 "SqlParser.y"
    {
        (yyval.pNode) = newFuncNode(pResult, (yyvsp[(1) - (4)].pNode)->m_pszValue, (yyloc).first_column, (yyloc).last_column, 1, (yyvsp[(3) - (4)].pNode));
}
    break;

  case 48:

/* Line 1821 of yacc.c  */
#line 320 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 49:

/* Line 1821 of yacc.c  */
#line 321 "SqlParser.y"
    { (yyval.pNode) = newParentNode(pResult, "ValueList", 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;

  case 50:

/* Line 1821 of yacc.c  */
#line 325 "SqlParser.y"
    {
		ParseNode* pTable = (yyvsp[(3) - (4)].pNode);
		struct DbPlanBuilder* pBuilder = getPlanBuilder(pResult, &pTable);
		if(pBuilder == NULL || pBuilder->m_pfnDelete == NULL)
		{
		  yyerror(&(yylsp[(3) - (4)]),pResult, "Delete is not supported for current database");
		  YYERROR;
		}
		(yyval.pNode) = newParentNode(pResult, "DeleteStmt", 2, pTable, (yyvsp[(4) - (4)].pNode));
		(yyval.pNode)->m_fnBuildPlan = pBuilder->m_pfnDelete;
	}
    break;

  case 51:

/* Line 1821 of yacc.c  */
#line 338 "SqlParser.y"
    {
		(yyvsp[(4) - (5)].pNode) = mergeTree(pResult, "AssignValueList", (yyvsp[(4) - (5)].pNode), "AssignValueList");
		yyerror(&(yylsp[(1) - (5)]),pResult, "Update is not supported for current database");
		YYERROR;
	}
    break;

  case 52:

/* Line 1821 of yacc.c  */
#line 346 "SqlParser.y"
    {
		(yyval.pNode) = newParentNode(pResult, "AssignValue", 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode)); 
	}
    break;

  case 53:

/* Line 1821 of yacc.c  */
#line 350 "SqlParser.y"
    {
		ParseNode* pNode = newParentNode(pResult, "AssignValue", 2, (yyvsp[(3) - (5)].pNode), (yyvsp[(5) - (5)].pNode)); 
		(yyval.pNode) = newParentNode(pResult, "AssignValueList", 2, (yyvsp[(1) - (5)].pNode), pNode);

	}
    break;

  case 54:

/* Line 1821 of yacc.c  */
#line 357 "SqlParser.y"
    {
		(yyval.pNode) = mergeTree(pResult, "ValueList", (yyvsp[(2) - (2)].pNode), "ValueList");
		(yyval.pNode)->m_fnBuildPlan = buildPlanForConst;
	}
    break;

  case 55:

/* Line 1821 of yacc.c  */
#line 364 "SqlParser.y"
    {
	  ParseNode* pTable = (yyvsp[(3) - (5)].pNode);
		struct DbPlanBuilder* pBuilder = getPlanBuilder(pResult, &pTable);
		if(pBuilder == NULL || pBuilder->m_pfnInsert == NULL)
		{
			yyerror(&(yylsp[(3) - (5)]),pResult, "Insert is not supported for current database");
			YYERROR;
		}
		(yyval.pNode) = newParentNode(pResult, "InsertStmt", 3, pTable,(yyvsp[(4) - (5)].pNode),(yyvsp[(5) - (5)].pNode));
		(yyval.pNode)->m_fnBuildPlan = pBuilder->m_pfnInsert;
	}
    break;

  case 56:

/* Line 1821 of yacc.c  */
#line 376 "SqlParser.y"
    {
	  ParseNode* pTable = (yyvsp[(3) - (5)].pNode);
		struct DbPlanBuilder* pBuilder = getPlanBuilder(pResult, &pTable);
		if(pBuilder == NULL || pBuilder->m_pfnInsert == NULL)
		{
			yyerror(&(yylsp[(3) - (5)]),pResult, "Insert is not supported for current database");
			YYERROR;
		}
		(yyval.pNode) = newParentNode(pResult, "InsertStmt", 3, pTable,(yyvsp[(4) - (5)].pNode),(yyvsp[(5) - (5)].pNode));
		(yyval.pNode)->m_fnBuildPlan = pBuilder->m_pfnInsert;
	}
    break;

  case 57:

/* Line 1821 of yacc.c  */
#line 390 "SqlParser.y"
    {
		(yyval.pNode) = newIntNode(pResult, INFO_NODE, SHOW, 0);
		(yyval.pNode)->m_fnBuildPlan = buildPlanForShowTables;
	}
    break;

  case 58:

/* Line 1821 of yacc.c  */
#line 397 "SqlParser.y"
    {
		(yyval.pNode) = newParentNode(pResult, "DescStmt", 1, (yyvsp[(2) - (2)].pNode));
		(yyval.pNode)->m_fnBuildPlan = buildPlanForDesc;
	}
    break;

  case 59:

/* Line 1821 of yacc.c  */
#line 404 "SqlParser.y"
    {
		(yyval.pNode) = newIntNode(pResult, INFO_NODE, WORKLOAD, 0);
		(yyval.pNode)->m_fnBuildPlan = buildPlanForWorkload;	
	}
    break;

  case 60:

/* Line 1821 of yacc.c  */
#line 411 "SqlParser.y"
    {
		ParseNode* pTable = (yyvsp[(7) - (12)].pNode);
		struct DbPlanBuilder* pBuilder = getPlanBuilder(pResult, &pTable);
		if(pBuilder == NULL || pBuilder->m_pfnInsert == NULL)
		{
			yyerror(&(yylsp[(3) - (12)]),pResult, "Insert is not supported for current database");
			YYERROR;
		}

		ParseNode* pFileNode = newParentNode(pResult, "FileNode", 4, (yyvsp[(4) - (12)].pNode), pTable, (yyvsp[(8) - (12)].pNode), (yyvsp[(12) - (12)].pNode));
		pFileNode->m_fnBuildPlan = buildPlanForReadFile;	

		(yyval.pNode) = newParentNode(pResult, "Loadtmt", 3, pTable, (yyvsp[(8) - (12)].pNode), pFileNode);
		(yyval.pNode)->m_fnBuildPlan = pBuilder->m_pfnInsert;
	}
    break;

  case 61:

/* Line 1821 of yacc.c  */
#line 428 "SqlParser.y"
    {(yyval.pNode) = 0;}
    break;

  case 62:

/* Line 1821 of yacc.c  */
#line 429 "SqlParser.y"
    { (yyval.pNode) = mergeTree(pResult, "ColumnList", (yyvsp[(2) - (3)].pNode), "ColumnList");}
    break;

  case 63:

/* Line 1821 of yacc.c  */
#line 432 "SqlParser.y"
    { 
		(yyval.pNode) = mergeTree(pResult, "ExprList",(yyvsp[(2) - (3)].pNode),"ExprList");
	}
    break;

  case 64:

/* Line 1821 of yacc.c  */
#line 435 "SqlParser.y"
    {
		(yyvsp[(4) - (5)].pNode) = mergeTree(pResult, "ExprList", (yyvsp[(4) - (5)].pNode), "ExprList");
		(yyval.pNode) = newParentNode(pResult, "ValueList", 2, (yyvsp[(1) - (5)].pNode), (yyvsp[(4) - (5)].pNode));
	}
    break;

  case 65:

/* Line 1821 of yacc.c  */
#line 440 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 66:

/* Line 1821 of yacc.c  */
#line 441 "SqlParser.y"
    { 
	(yyval.pNode) = newParentNode(pResult, "ExprList", 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;

  case 67:

/* Line 1821 of yacc.c  */
#line 445 "SqlParser.y"
    { (yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 68:

/* Line 1821 of yacc.c  */
#line 446 "SqlParser.y"
    {
		(yyval.pNode) = newParentNode(pResult, "ColumnList", 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));
	}
    break;

  case 69:

/* Line 1821 of yacc.c  */
#line 451 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 70:

/* Line 1821 of yacc.c  */
#line 452 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(2) - (3)].pNode);}
    break;

  case 71:

/* Line 1821 of yacc.c  */
#line 455 "SqlParser.y"
    {(yyval.pNode) = 0;}
    break;

  case 72:

/* Line 1821 of yacc.c  */
#line 456 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(2) - (2)].pNode);}
    break;

  case 73:

/* Line 1821 of yacc.c  */
#line 459 "SqlParser.y"
    {(yyval.pNode) = 0;}
    break;

  case 74:

/* Line 1821 of yacc.c  */
#line 460 "SqlParser.y"
    {(yyval.pNode) = (yyvsp[(1) - (1)].pNode);}
    break;

  case 75:

/* Line 1821 of yacc.c  */
#line 464 "SqlParser.y"
    {
		ParseNode* pProject = mergeTree(pResult, "SelectExprList", (yyvsp[(2) - (11)].pNode), "ExprList");
		ParseNode* pTable = (yyvsp[(4) - (11)].pNode);
		ParseNode* pAlias = (yyvsp[(5) - (11)].pNode);
		ParseNode* pPredicate = (yyvsp[(6) - (11)].pNode);
		ParseNode* pJoin = (yyvsp[(11) - (11)].pNode);

		int hasSubquery = (pTable->m_iType != NAME_NODE && pTable->m_iType != OP_NODE);
		if(pJoin != 0)
		{
			//This is a left join statement
			ParseNode* pJoinList = mergeTree(pResult, "JoinList", pJoin, "JoinList");
			if(pAlias == NULL)
			{
				yyerror(&(yylsp[(5) - (11)]),pResult, "table in left join statement  must have a alias name");
				YYERROR;
			}
			if(!hasSubquery)
			{
				yyerror(&(yylsp[(4) - (11)]),pResult, "left join target must be subquery");
				YYERROR;
			}

			(yyval.pNode) = newParentNode(pResult, "LeftJoinStmt", 4, pProject, pTable, pAlias, pJoinList);
			(yyval.pNode)->m_fnBuildPlan = buildPlanForLeftJoin;
		}	
		else
		{
			pProject->m_fnBuildPlan = buildPlanForProjection;
			if(pAlias != NULL)
			{
				yyerror(&(yylsp[(5) - (11)]),pResult, "table alias name in non-join statement  is not supported");
				YYERROR;
			}
			if(hasSubquery)
			{
				//this is a select statement with subquery
				// children order is important, it is the BuildPlan order
				(yyval.pNode) = newParentNode(pResult, "SubQueryStmt", 7, pTable, pPredicate, (yyvsp[(7) - (11)].pNode), (yyvsp[(8) - (11)].pNode), (yyvsp[(9) - (11)].pNode), (yyvsp[(10) - (11)].pNode), pProject);
				(yyval.pNode)->m_fnBuildPlan = buildPlanDefault;
			}
			else
			{
				struct DbPlanBuilder* pBuilder = getPlanBuilder(pResult, &pTable);
		    if(pBuilder == NULL || pBuilder->m_pfnSelect == NULL)
		    {
		      yyerror(&(yylsp[(3) - (11)]),pResult, "Select is not supported for current database");
		      YYERROR;
		    }

				(yyval.pNode) = newParentNode(pResult, "SelectStmt", 7, pProject, pTable, pPredicate, (yyvsp[(7) - (11)].pNode), (yyvsp[(8) - (11)].pNode), (yyvsp[(9) - (11)].pNode), (yyvsp[(10) - (11)].pNode));
				(yyval.pNode)->m_fnBuildPlan = pBuilder->m_pfnSelect;
			}
		}
	}
    break;

  case 76:

/* Line 1821 of yacc.c  */
#line 522 "SqlParser.y"
    {
		(yyvsp[(6) - (7)].pNode) = mergeTree(pResult, "Using", (yyvsp[(6) - (7)].pNode), "ColumnList");
    (yyval.pNode) = newParentNode(pResult, "LeftJoin", 2, (yyvsp[(3) - (7)].pNode), (yyvsp[(6) - (7)].pNode));
	}
    break;

  case 77:

/* Line 1821 of yacc.c  */
#line 529 "SqlParser.y"
    {
		(yyval.pNode) = newParentNode(pResult, "JoinList", 1, (yyvsp[(1) - (1)].pNode));
	}
    break;

  case 78:

/* Line 1821 of yacc.c  */
#line 533 "SqlParser.y"
    {
		(yyval.pNode) = newParentNode(pResult, "JoinList", 2, (yyvsp[(1) - (2)].pNode), (yyvsp[(2) - (2)].pNode));
	}
    break;

  case 79:

/* Line 1821 of yacc.c  */
#line 537 "SqlParser.y"
    {(yyval.pNode) = 0;}
    break;

  case 80:

/* Line 1821 of yacc.c  */
#line 538 "SqlParser.y"
    {
		(yyval.pNode) = (yyvsp[(2) - (2)].pNode);
		(yyval.pNode)->m_fnBuildPlan = buildPlanForFilter;
	}
    break;

  case 81:

/* Line 1821 of yacc.c  */
#line 544 "SqlParser.y"
    {(yyval.pNode) = 0;}
    break;

  case 82:

/* Line 1821 of yacc.c  */
#line 546 "SqlParser.y"
    {
	       (yyval.pNode) = newParentNode(pResult, "Limit", 2, (yyvsp[(2) - (4)].pNode), (yyvsp[(4) - (4)].pNode)); 
		(yyval.pNode)->m_fnBuildPlan = buildPlanForLimit;
	}
    break;

  case 83:

/* Line 1821 of yacc.c  */
#line 551 "SqlParser.y"
    {
	       (yyval.pNode) = newParentNode(pResult, "Limit", 2, (yyvsp[(4) - (4)].pNode), (yyvsp[(2) - (4)].pNode)); 
		(yyval.pNode)->m_fnBuildPlan = buildPlanForLimit;
	}
    break;

  case 84:

/* Line 1821 of yacc.c  */
#line 556 "SqlParser.y"
    {(yyval.pNode) = 0;}
    break;

  case 85:

/* Line 1821 of yacc.c  */
#line 557 "SqlParser.y"
    {
		(yyval.pNode) = mergeTree(pResult, "GroupBy", (yyvsp[(3) - (3)].pNode), "ColumnList");
		(yyval.pNode)->m_fnBuildPlan = buildPlanForGroupBy;
	}
    break;

  case 86:

/* Line 1821 of yacc.c  */
#line 563 "SqlParser.y"
    {
			(yyval.pNode) = newParentNode(pResult, "SortItem", 2, (yyvsp[(1) - (2)].pNode), (yyvsp[(2) - (2)].pNode)); 
		}
    break;

  case 87:

/* Line 1821 of yacc.c  */
#line 566 "SqlParser.y"
    { 
			(yyval.pNode) = newParentNode(pResult, "SortList", 2, (yyvsp[(1) - (4)].pNode),
				newParentNode(pResult, "SortItem", 2, (yyvsp[(3) - (4)].pNode), (yyvsp[(4) - (4)].pNode)));
		}
    break;

  case 88:

/* Line 1821 of yacc.c  */
#line 572 "SqlParser.y"
    {(yyval.pNode) = newIntNode(pResult, INFO_NODE, ASC, 0);}
    break;

  case 89:

/* Line 1821 of yacc.c  */
#line 573 "SqlParser.y"
    {(yyval.pNode) = newIntNode(pResult, INFO_NODE, ASC, 0);}
    break;

  case 90:

/* Line 1821 of yacc.c  */
#line 574 "SqlParser.y"
    {(yyval.pNode) = newIntNode(pResult, INFO_NODE, DESC, 0);}
    break;

  case 91:

/* Line 1821 of yacc.c  */
#line 577 "SqlParser.y"
    {(yyval.pNode) = 0;}
    break;

  case 92:

/* Line 1821 of yacc.c  */
#line 578 "SqlParser.y"
    {
		(yyval.pNode) = (yyvsp[(2) - (2)].pNode);
		(yyval.pNode)->m_fnBuildPlan = buildPlanForFilter;
	}
    break;

  case 93:

/* Line 1821 of yacc.c  */
#line 585 "SqlParser.y"
    {(yyval.pNode) = 0;}
    break;

  case 94:

/* Line 1821 of yacc.c  */
#line 586 "SqlParser.y"
    {
		(yyval.pNode) = mergeTree(pResult, "OrderBy", (yyvsp[(3) - (3)].pNode), "SortList");
		(yyval.pNode)->m_fnBuildPlan = buildPlanForOrderBy;
	}
    break;

  case 95:

/* Line 1821 of yacc.c  */
#line 592 "SqlParser.y"
    {
		(yyval.pNode) = (yyvsp[(1) - (1)].pNode); 
	}
    break;

  case 96:

/* Line 1821 of yacc.c  */
#line 594 "SqlParser.y"
    { 
		(yyval.pNode) = newIntNode(pResult, OP_NODE, AS, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode)); 
	}
    break;

  case 97:

/* Line 1821 of yacc.c  */
#line 598 "SqlParser.y"
    { 
		(yyval.pNode) = (yyvsp[(1) - (1)].pNode);
	}
    break;

  case 98:

/* Line 1821 of yacc.c  */
#line 601 "SqlParser.y"
    {
		(yyval.pNode) = newParentNode(pResult, "ExprList", 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));
	}
    break;

  case 99:

/* Line 1821 of yacc.c  */
#line 604 "SqlParser.y"
    {
		(yyval.pNode) = newIntNode(pResult, INFO_NODE, ALL_COLUMN, 0);
	}
    break;

  case 100:

/* Line 1821 of yacc.c  */
#line 609 "SqlParser.y"
    { 
		(yyval.pNode) = (yyvsp[(1) - (1)].pNode);
	}
    break;

  case 101:

/* Line 1821 of yacc.c  */
#line 612 "SqlParser.y"
    {(yyval.pNode) = newExprNode(pResult, '.', (yyloc).first_column, (yyloc).last_column, 2, (yyvsp[(1) - (3)].pNode), (yyvsp[(3) - (3)].pNode));}
    break;



/* Line 1821 of yacc.c  */
#line 2758 "SqlParser.tab.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, pResult, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, pResult, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc, pResult);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, pResult);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, pResult, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, pResult);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, pResult);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 615 "SqlParser.y"


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

