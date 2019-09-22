/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

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
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */

#line 67 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "SqlParser.tab.h".  */
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
#line 5 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:355  */

typedef void* yyscan_t;
#include "ParseResult.h"
#include "MetaConfig.h"
#include <stdint.h>
#include <absl/strings/str_cat.h>

#line 105 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:355  */

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
#line 13 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:355  */

	ParseNode* pNode;

#line 217 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:355  */
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

/* Copy the second part of user declarations.  */
#line 17 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:358  */

#include <array>

#include "SqlParser.lex.h"

#define YYLEX_PARAM pResult->m_scanInfo

extern void yyerror(YYLTYPE* yylloc, ParseResult* p, yyscan_t scanner,  const std::string& msg);


#line 257 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
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
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  41
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   327

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  110
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  28
/* YYNRULES -- Number of rules.  */
#define YYNRULES  86
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  161

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   350

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    22,     2,     2,     2,    35,    23,     2,
     107,   108,    33,    31,   109,    32,   106,    34,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   105,
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
      15,    16,    17,    18,    19,    20,    21,    24,    25,    26,
      27,    28,    29,    36,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   144,   144,   149,   157,   158,   159,   160,   163,   164,
     165,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   180,   181,   182,   183,   184,   185,   186,   202,   205,
     206,   207,   208,   209,   210,   211,   222,   223,   224,   228,
     231,   236,   239,   244,   249,   252,   257,   264,   270,   275,
     283,   289,   298,   299,   304,   307,   311,   314,   319,   322,
     327,   328,   331,   332,   336,   354,   355,   360,   361,   365,
     370,   371,   376,   380,   386,   387,   388,   391,   392,   398,
     399,   404,   406,   410,   413,   416,   421
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NAME", "STRING", "BINARY", "INTNUM",
  "DATE_VALUE", "HINT_VALUE", "BOOL", "APPROXNUM", "NULLX", "PARAM",
  "ASSIGN", "OR", "XOR", "ANDOP", "IN", "IS", "LIKE", "REGEXP", "NOT",
  "'!'", "'&'", "COMP_LE", "COMP_LT", "COMP_EQ", "COMP_GT", "COMP_GE",
  "COMP_NE", "'|'", "'+'", "'-'", "'*'", "'/'", "'%'", "MOD", "'^'",
  "UMINUS", "UPLUS", "DATA", "FIELDS", "INFILE", "SHOW", "TABLES",
  "TERMINATED", "ERROR", "ADD", "ALL", "AND", "ANY", "AS", "ASC",
  "BETWEEN", "CHAR", "CREATE", "DATABASE", "DATE", "DATETIME", "DEFAULT",
  "DELETE", "DESC", "DISTINCT", "FLOAT", "DOUBLE", "DROP", "EXPLAIN",
  "FROM", "GROUP", "HAVING", "INSERT", "INTEGER", "INTO", "LIMIT", "LOAD",
  "ON", "ORDER", "SELECT", "WHERE", "BY", "SMALLINT", "TABLE", "TO",
  "UNION", "UNIQUE", "UNSIGNED", "UPDATE", "SET", "VALUES", "VARCHAR",
  "OFFSET", "NOT_IN", "NOT_LIKE", "ALL_ROW", "ALL_COLUMN", "PRIMARY_KEY",
  "NOT_NULL", "KEY", "PRIMARY", "IF", "EXISTS", "IF_EXISTS", "JOIN",
  "LEFT", "USING", "';'", "'.'", "'('", "')'", "','", "$accept",
  "sql_stmt", "get_stmt", "stmt", "expr", "val_list", "delete_stmt",
  "values_stmt", "insert_stmt", "show_tables_stmt", "desc_table_stmt",
  "opt_col_names", "value_list", "row_value", "column_list",
  "table_or_query", "opt_alias", "select_stmt", "opt_where", "opt_limit",
  "opt_groupby", "sort_list", "opt_asc_desc", "opt_having", "opt_orderby",
  "projection", "select_expr_list", "table_factor", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,    33,    38,   277,   278,   279,   280,   281,   282,
     124,    43,    45,    42,    47,    37,   283,    94,   284,   285,
     286,   287,   288,   289,   290,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   350,    59,    46,    40,    41,    44
};
# endif

#define YYPACT_NINF -77

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-77)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -38,   -33,   -63,    14,   -40,   -48,     3,   -76,    40,   -77,
     -58,   -77,   -77,   -77,   -77,   -77,   -77,   -77,    14,   -77,
     -77,   -11,    14,   -64,   -77,   -77,   -77,   -77,   -77,   -77,
     -77,    67,    67,   -77,   -77,    67,   222,   -77,   -65,    67,
     -14,   -77,   -77,    18,   -77,   -10,    97,    67,   -77,   -77,
     126,    67,    67,     5,    57,    99,   103,    67,    67,    67,
      67,    67,    67,    67,    67,    67,    67,    67,    67,   124,
      -2,    67,   250,   -45,    21,    67,   -77,   127,   -61,   -77,
     151,   -77,   271,   291,    67,   -77,   118,   -77,    24,    49,
      49,    49,    49,    49,    49,    53,    53,   -77,   -77,   -77,
     -77,   -77,   145,    81,   -77,   -77,   -77,    67,    67,   250,
     -77,   -18,   -77,   -77,   -77,    90,    26,   -77,    67,    27,
     133,    18,   250,   -16,   -77,   134,    67,   -77,    30,   -77,
     -77,    73,   -77,   -77,   -77,   -77,    70,    77,   127,    67,
      80,    54,   250,    85,    93,    67,   165,   -77,   176,    64,
     -71,   -77,   -77,   -77,    67,   175,   183,   176,   -77,   -77,
     -77
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     8,
       0,    10,     5,     9,     6,     7,     4,    50,     0,    86,
      51,     0,     0,    11,    12,    13,    15,    14,    17,    16,
      18,     0,     0,    85,    20,     0,    81,    83,     0,     0,
      47,     1,     2,    65,     3,    52,     0,     0,    28,    27,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    56,     0,     0,     0,    46,     0,     0,    19,
       0,    38,    37,    36,     0,    39,     0,    35,     0,    29,
      30,    31,    33,    32,    34,    21,    22,    23,    24,    25,
      26,    82,     0,    62,    60,    84,    54,     0,     0,    66,
      58,     0,    49,    48,    43,    44,     0,    40,     0,     0,
       0,    65,    57,     0,    53,     0,     0,    41,     0,    61,
      63,    70,    55,    59,    45,    42,     0,    77,     0,     0,
      79,    71,    78,     0,    67,     0,     0,    64,    74,    80,
       0,    75,    76,    72,     0,     0,     0,    74,    68,    69,
      73
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -77,   -77,    89,   192,    -6,   -51,   -77,   120,   -77,   -77,
     -77,   -77,   -77,   105,    76,   -77,   -77,   137,    95,   -77,
     -77,   -77,    60,   -77,   -77,   147,   -77,    -4
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     8,     9,    10,   115,   116,    11,    12,    13,    14,
      15,    78,    40,    73,   111,   103,   121,    16,    76,   147,
     137,   149,   153,   140,   144,    37,    38,    20
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      36,    19,    70,     1,    18,     1,    23,    24,    25,    26,
      27,    17,    28,    29,    43,    30,     6,    19,    45,   155,
       2,     3,     2,     3,    22,    48,    49,     7,     4,    50,
       5,    39,     5,    72,    31,    32,    33,     6,   156,     6,
      41,    80,    46,    47,    71,    82,    83,    42,     7,    34,
       7,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   106,   107,    36,   104,   128,    85,   109,
      23,    24,    25,    26,    27,   134,    28,    29,    86,    30,
      63,    64,    65,    66,    67,    68,    65,    66,    67,    68,
     124,   125,   132,   107,    44,    74,    75,    77,    31,    32,
      79,   122,    72,    87,    51,   102,    52,    53,    54,    55,
      35,    56,    84,    34,    57,    58,    59,    60,    61,    62,
      88,    63,    64,    65,    66,    67,    68,   101,   108,   117,
     110,   118,   120,   142,   127,   129,   130,   133,   135,   148,
      51,   136,    52,    53,    54,    55,   139,    56,   157,   138,
      57,    58,    59,    60,    61,    62,   143,    63,    64,    65,
      66,    67,    68,   125,   145,    51,   146,    52,    53,    54,
      55,   150,    56,   154,    35,    57,    58,    59,    60,    61,
      62,   158,    63,    64,    65,    66,    67,    68,     1,   159,
      51,   119,    52,    53,    54,    55,    21,    56,   112,   126,
      57,    58,    59,    60,    61,    62,     3,    63,    64,    65,
      66,    67,    68,   123,   141,   113,   131,   160,   105,     0,
       0,     0,     6,     0,     0,     0,     0,     0,   151,     0,
       0,     0,     0,     7,    81,     0,    51,   152,    52,    53,
      54,    55,     0,    56,     0,     0,    57,    58,    59,    60,
      61,    62,     0,    63,    64,    65,    66,    67,    68,   114,
       0,     0,     0,     0,    51,     0,    52,    53,    54,    55,
       0,    56,     0,    69,    57,    58,    59,    60,    61,    62,
       0,    63,    64,    65,    66,    67,    68,    52,    53,    54,
      55,     0,    56,     0,     0,    57,    58,    59,    60,    61,
      62,     0,    63,    64,    65,    66,    67,    68,    53,    54,
      55,     0,    56,     0,     0,    57,    58,    59,    60,    61,
      62,     0,    63,    64,    65,    66,    67,    68
};

static const yytype_int16 yycheck[] =
{
       6,     3,    67,    43,    67,    43,     3,     4,     5,     6,
       7,    44,     9,    10,    18,    12,    77,     3,    22,    90,
      60,    61,    60,    61,    72,    31,    32,    88,    66,    35,
      70,   107,    70,    39,    31,    32,    33,    77,   109,    77,
       0,    47,   106,   107,   109,    51,    52,   105,    88,    46,
      88,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,   108,   109,    71,    70,   118,    11,    75,
       3,     4,     5,     6,     7,   126,     9,    10,    21,    12,
      31,    32,    33,    34,    35,    36,    33,    34,    35,    36,
     108,   109,   108,   109,   105,   109,    78,   107,    31,    32,
       3,   107,   108,     4,    14,   107,    16,    17,    18,    19,
     107,    21,   107,    46,    24,    25,    26,    27,    28,    29,
      17,    31,    32,    33,    34,    35,    36,     3,   107,    11,
       3,   107,    51,   139,   108,   108,     3,     3,   108,   145,
      14,    68,    16,    17,    18,    19,    69,    21,   154,    79,
      24,    25,    26,    27,    28,    29,    76,    31,    32,    33,
      34,    35,    36,   109,    79,    14,    73,    16,    17,    18,
      19,     6,    21,   109,   107,    24,    25,    26,    27,    28,
      29,     6,    31,    32,    33,    34,    35,    36,    43,     6,
      14,   102,    16,    17,    18,    19,     4,    21,    78,   109,
      24,    25,    26,    27,    28,    29,    61,    31,    32,    33,
      34,    35,    36,   108,   138,    78,   121,   157,    71,    -1,
      -1,    -1,    77,    -1,    -1,    -1,    -1,    -1,    52,    -1,
      -1,    -1,    -1,    88,   108,    -1,    14,    61,    16,    17,
      18,    19,    -1,    21,    -1,    -1,    24,    25,    26,    27,
      28,    29,    -1,    31,    32,    33,    34,    35,    36,   108,
      -1,    -1,    -1,    -1,    14,    -1,    16,    17,    18,    19,
      -1,    21,    -1,    51,    24,    25,    26,    27,    28,    29,
      -1,    31,    32,    33,    34,    35,    36,    16,    17,    18,
      19,    -1,    21,    -1,    -1,    24,    25,    26,    27,    28,
      29,    -1,    31,    32,    33,    34,    35,    36,    17,    18,
      19,    -1,    21,    -1,    -1,    24,    25,    26,    27,    28,
      29,    -1,    31,    32,    33,    34,    35,    36
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    43,    60,    61,    66,    70,    77,    88,   111,   112,
     113,   116,   117,   118,   119,   120,   127,    44,    67,     3,
     137,   113,    72,     3,     4,     5,     6,     7,     9,    10,
      12,    31,    32,    33,    46,   107,   114,   135,   136,   107,
     122,     0,   105,   137,   105,   137,   106,   107,   114,   114,
     114,    14,    16,    17,    18,    19,    21,    24,    25,    26,
      27,    28,    29,    31,    32,    33,    34,    35,    36,    51,
      67,   109,   114,   123,   109,    78,   128,   107,   121,     3,
     114,   108,   114,   114,   107,    11,    21,     4,    17,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,     3,   107,   125,   137,   135,   108,   109,   107,   114,
       3,   124,   117,   127,   108,   114,   115,    11,   107,   112,
      51,   126,   114,   123,   108,   109,   109,   108,   115,   108,
       3,   128,   108,     3,   115,   108,    68,   130,    79,    69,
     133,   124,   114,    76,   134,    79,    73,   129,   114,   131,
       6,    52,    61,   132,   109,    90,   109,   114,     6,     6,
     132
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   110,   111,   111,   112,   112,   112,   112,   113,   113,
     113,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   115,   115,   116,   117,   118,   118,
     119,   120,   121,   121,   122,   122,   123,   123,   124,   124,
     125,   125,   126,   126,   127,   128,   128,   129,   129,   129,
     130,   130,   131,   131,   132,   132,   132,   133,   133,   134,
     134,   135,   135,   136,   136,   136,   137
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     3,     3,     3,     3,     3,     3,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       4,     5,     6,     4,     1,     3,     4,     2,     5,     5,
       2,     2,     0,     3,     3,     5,     1,     3,     1,     3,
       1,     3,     0,     2,    10,     0,     2,     0,     4,     4,
       0,     3,     2,     4,     0,     1,     1,     0,     2,     0,
       3,     1,     3,     1,     3,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (&yylloc, pResult, scanner, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location, pResult, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, ParseResult* pResult, yyscan_t scanner)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  YYUSE (pResult);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, ParseResult* pResult, yyscan_t scanner)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, pResult, scanner);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, ParseResult* pResult, yyscan_t scanner)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       , pResult, scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, pResult, scanner); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
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
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
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

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

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

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, ParseResult* pResult, yyscan_t scanner)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (pResult);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (ParseResult* pResult, yyscan_t scanner)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
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
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
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
      yychar = yylex (&yylval, &yylloc, scanner);
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
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
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
     '$$ = $1'.

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
#line 145 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		pResult->m_pResult = (yyval.pNode) = (yyvsp[-1].pNode);
		YYACCEPT;
	}
#line 1609 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 3:
#line 150 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = pResult->newPlanNode( "ExplainStmt", Operation::EXPLAIN, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-1].pNode) });
		pResult->m_pResult = (yyval.pNode);
		YYACCEPT;
	}
#line 1619 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 4:
#line 157 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = (yyvsp[0].pNode);}
#line 1625 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 5:
#line 158 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = (yyvsp[0].pNode);}
#line 1631 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 6:
#line 159 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    { (yyval.pNode) = (yyvsp[0].pNode);}
#line 1637 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 160 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    { (yyval.pNode) = (yyvsp[0].pNode);}
#line 1643 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 163 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = (yyvsp[0].pNode);}
#line 1649 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 9:
#line 164 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    { (yyval.pNode) = (yyvsp[0].pNode);}
#line 1655 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 10:
#line 165 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = (yyvsp[0].pNode);}
#line 1661 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 11:
#line 168 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    { (yyval.pNode) = (yyvsp[0].pNode);}
#line 1667 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 12:
#line 169 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = (yyvsp[0].pNode);}
#line 1673 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 170 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = (yyvsp[0].pNode);}
#line 1679 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 171 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = (yyvsp[0].pNode);}
#line 1685 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 172 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = (yyvsp[0].pNode);}
#line 1691 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 173 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = (yyvsp[0].pNode);}
#line 1697 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 174 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = (yyvsp[0].pNode);}
#line 1703 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 18:
#line 175 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = (yyvsp[0].pNode);}
#line 1709 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 19:
#line 176 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newExprNode( Operation::MEMBER, (yyloc).first_column, (yyloc).last_column,{ (yyvsp[-2].pNode), (yyvsp[0].pNode)});}
#line 1715 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 20:
#line 177 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = 0;YYERROR;}
#line 1721 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 180 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newExprNode( Operation::ADD, (yyloc).first_column, (yyloc).last_column, {(yyvsp[-2].pNode), (yyvsp[0].pNode)});}
#line 1727 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 22:
#line 181 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newExprNode( Operation::SUB, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode)});}
#line 1733 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 182 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newExprNode( Operation::MUL, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode)});}
#line 1739 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 183 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newExprNode( Operation::DIV, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode)});}
#line 1745 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 184 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newExprNode( Operation::MOD, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode)});}
#line 1751 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 185 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newExprNode( Operation::MOD, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode)});}
#line 1757 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 186 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		if((yyvsp[0].pNode)->m_type == NodeType::INT)
		{
			(yyval.pNode) = pResult->newSimpleNode(NodeType::INT,(yyloc).first_column, (yyloc).last_column);
			(yyval.pNode)->setString((yyval.pNode)->m_sExpr);
		}
		else if((yyvsp[0].pNode)->m_type == NodeType::FLOAT) 
		{
			(yyval.pNode) = pResult->newSimpleNode( NodeType::FLOAT, (yyloc).first_column, (yyloc).last_column);
			(yyval.pNode)->setString((yyval.pNode)->m_sExpr);
		}
		else
		{
			yyerror(&(yylsp[0]),pResult, nullptr, "minus is not supporteed");
		}
	}
#line 1778 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 28:
#line 202 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = (yyvsp[0].pNode);
	}
#line 1786 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 29:
#line 205 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newExprNode( Operation::COMP_LE, (yyloc).first_column, (yyloc).last_column, {(yyvsp[-2].pNode), (yyvsp[0].pNode)});}
#line 1792 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 30:
#line 206 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newExprNode( Operation::COMP_LT, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode)});}
#line 1798 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 31:
#line 207 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newExprNode( Operation::COMP_EQ, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode)});}
#line 1804 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 208 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newExprNode( Operation::COMP_GE, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode)});}
#line 1810 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 209 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newExprNode( Operation::COMP_GT, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode)});}
#line 1816 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 210 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newExprNode( Operation::COMP_NE, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode)});}
#line 1822 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 35:
#line 211 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		auto sValue =  (yyvsp[0].pNode)->getString();
		auto len =  sValue.length();
		if(sValue[0] != '%' || sValue[len - 1] != '%')
		{
			yyerror(&(yylsp[0]),pResult, nullptr, absl::StrCat("missing ",sValue," for like "));
			YYERROR;
		}
		(yyvsp[0].pNode)->setString(sValue.substr(1, len -2));
		(yyval.pNode) = pResult->newExprNode( Operation::LIKE, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode)});
	}
#line 1838 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 36:
#line 222 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newExprNode( Operation::AND, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode)});}
#line 1844 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 37:
#line 223 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newExprNode( Operation::OR, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode)});}
#line 1850 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 38:
#line 224 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    { (yyval.pNode) = (yyvsp[-1].pNode);}
#line 1856 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 228 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = pResult->newExprNode(Operation::COMP_EQ, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode)}); 
	}
#line 1864 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 40:
#line 231 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = pResult->newExprNode( Operation::COMP_NE, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-3].pNode), (yyvsp[0].pNode)}); 
	}
#line 1872 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 236 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = pResult->newExprNode( Operation::IN, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-4].pNode), (yyvsp[-1].pNode)});
		}
#line 1880 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 42:
#line 239 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    { 
		(yyval.pNode) = pResult->newExprNode( Operation::NOT_IN, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-5].pNode), (yyvsp[-1].pNode)});
	}
#line 1888 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 43:
#line 244 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
        (yyval.pNode) = pResult->newFuncNode((yyvsp[-3].pNode)->getString(), (yyloc).first_column, (yyloc).last_column, { (yyvsp[-1].pNode) });
}
#line 1896 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 44:
#line 249 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
	(yyval.pNode) = pResult->newListNode( "ValueList",(yyloc).first_column, (yyloc).last_column,  { (yyvsp[0].pNode) } );
	}
#line 1904 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 45:
#line 252 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    { 
	(yyval.pNode) = pResult->newListNode( "ValueList",(yyloc).first_column, (yyloc).last_column,  { (yyvsp[-2].pNode), (yyvsp[0].pNode) } );
	}
#line 1912 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 46:
#line 258 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		ParseNode* pTable = (yyvsp[-1].pNode);
		(yyval.pNode) = pResult->newPlanNode( "DeleteStmt", Operation::DELETE, (yyloc).first_column, (yyloc).last_column, {pTable, (yyvsp[0].pNode) });
	}
#line 1921 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 265 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = pResult->newPlanNode( "Values", Operation::VALUES, (yyloc).first_column, (yyloc).last_column, { (yyvsp[0].pNode) });
	}
#line 1929 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 48:
#line 271 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
	  	ParseNode* pTable = (yyvsp[-2].pNode);
		(yyval.pNode) = pResult->newPlanNode( "InsertStmt", Operation::INSERT, (yyloc).first_column, (yyloc).last_column,  { pTable,(yyvsp[-1].pNode),(yyvsp[0].pNode) });
	}
#line 1938 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 49:
#line 276 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
	  	ParseNode* pTable = (yyvsp[-2].pNode);
		(yyval.pNode) = pResult->newPlanNode( "InsertStmt", Operation::INSERT, (yyloc).first_column, (yyloc).last_column, { pTable,(yyvsp[-1].pNode),(yyvsp[0].pNode) });

	}
#line 1948 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 50:
#line 284 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = pResult->newPlanNode("ShowTable",  Operation::SHOW_TABLES,  (yyloc).first_column, (yyloc).last_column, {});
	}
#line 1956 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 51:
#line 290 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = pResult->newPlanNode( "DescStmt", Operation::DESC_TABLE,  (yyloc).first_column, (yyloc).last_column, {(yyvsp[0].pNode) });
	}
#line 1964 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 298 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = nullptr;}
#line 1970 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 299 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = (yyvsp[-1].pNode);
	}
#line 1978 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 54:
#line 304 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    { 
		(yyval.pNode) = pResult->newListNode( "ValueList",(yyloc).first_column, (yyloc).last_column,  { (yyvsp[-1].pNode) });
	}
#line 1986 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 55:
#line 307 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = pResult->newListNode( "ValueList",(yyloc).first_column, (yyloc).last_column,  { (yyvsp[-4].pNode), (yyvsp[-1].pNode) });
	}
#line 1994 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 56:
#line 311 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = pResult->newListNode( "ExprList", (yyloc).first_column, (yyloc).last_column, { (yyvsp[0].pNode) });
	}
#line 2002 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 314 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    { 
		(yyval.pNode) = pResult->newListNode( "ExprList", (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode) });
	}
#line 2010 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 58:
#line 319 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    { 
		(yyval.pNode) = pResult->newListNode( "ColumnList", (yyloc).first_column, (yyloc).last_column, { (yyvsp[0].pNode) });
	}
#line 2018 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 59:
#line 322 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = pResult->newListNode( "ColumnList", (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode) });
	}
#line 2026 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 60:
#line 327 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = (yyvsp[0].pNode);}
#line 2032 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 61:
#line 328 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = (yyvsp[-1].pNode);}
#line 2038 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 62:
#line 331 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = 0;}
#line 2044 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 63:
#line 332 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = (yyvsp[0].pNode);}
#line 2050 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 338 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		ParseNode* pProject = (yyvsp[-8].pNode);
		ParseNode* pTable = (yyvsp[-6].pNode);
		ParseNode* pAlias = (yyvsp[-5].pNode);
		ParseNode* pPredicate = (yyvsp[-4].pNode);

		if(pAlias != nullptr)
		{
			yyerror(&(yylsp[-5]),pResult,nullptr, "table alias name is not supported");
			YYERROR;
		}
		(yyval.pNode) = pResult->newPlanNode( "SelectStmt", Operation::SELECT, (yyloc).first_column, (yyloc).last_column, { pProject, pTable, pPredicate, (yyvsp[-3].pNode), (yyvsp[-2].pNode), (yyvsp[-1].pNode), (yyvsp[0].pNode) });
}
#line 2068 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 354 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = 0;}
#line 2074 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 355 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = (yyvsp[0].pNode);
	}
#line 2082 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 67:
#line 360 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = 0;}
#line 2088 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 68:
#line 362 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
	       (yyval.pNode) = pResult->newParentNode( "Limit",(yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode)}); 
	}
#line 2096 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 366 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
	       (yyval.pNode) = pResult->newParentNode( "Limit",(yyloc).first_column, (yyloc).last_column,{ (yyvsp[0].pNode), (yyvsp[-2].pNode) }); 
	}
#line 2104 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 370 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = 0;}
#line 2110 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 371 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = (yyvsp[0].pNode);
	}
#line 2118 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 376 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
			auto pChild = pResult->newParentNode( "SortItem", (yyloc).first_column, (yyloc).last_column,{ (yyvsp[-1].pNode), (yyvsp[0].pNode) }); 
			(yyval.pNode) = pResult->newListNode( "SortList",(yyloc).first_column, (yyloc).last_column, { pChild });
		}
#line 2127 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 380 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    { 
			auto pChild =  pResult->newParentNode( "SortItem",(yyloc).first_column, (yyloc).last_column, { (yyvsp[-1].pNode), (yyvsp[0].pNode) });
			(yyval.pNode) = pResult->newListNode( "SortList",(yyloc).first_column, (yyloc).last_column, { (yyvsp[-3].pNode),pChild });
		}
#line 2136 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 386 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newInfoNode( Operation::ASC,  (yyloc).first_column, (yyloc).last_column);}
#line 2142 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 387 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newInfoNode(  Operation::ASC,  (yyloc).first_column, (yyloc).last_column);}
#line 2148 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 388 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = pResult->newInfoNode(  Operation::DESC,  (yyloc).first_column, (yyloc).last_column);}
#line 2154 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 77:
#line 391 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = 0;}
#line 2160 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 392 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = (yyvsp[0].pNode);
	}
#line 2168 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 79:
#line 398 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {(yyval.pNode) = 0;}
#line 2174 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 399 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = (yyvsp[0].pNode);
	}
#line 2182 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 404 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = (yyvsp[0].pNode); 
	}
#line 2190 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 82:
#line 406 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    { 
		(yyval.pNode) = pResult->newExprNode( Operation::AS, (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode) }); 
	}
#line 2198 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 83:
#line 410 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    { 
		(yyval.pNode) = pResult->newListNode( "ExprList", (yyloc).first_column, (yyloc).last_column, { (yyvsp[0].pNode) });
	}
#line 2206 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 413 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = pResult->newListNode( "ExprList", (yyloc).first_column, (yyloc).last_column, { (yyvsp[-2].pNode), (yyvsp[0].pNode) });
	}
#line 2214 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 85:
#line 416 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    {
		(yyval.pNode) = pResult->newInfoNode( Operation::ALL_COLUMNS,  (yyloc).first_column, (yyloc).last_column);
	}
#line 2222 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 421 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1646  */
    { 
		(yyval.pNode) = (yyvsp[0].pNode);
	}
#line 2230 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
    break;


#line 2234 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.tab.cpp" /* yacc.c:1646  */
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

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, pResult, scanner, YY_("syntax error"));
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
        yyerror (&yylloc, pResult, scanner, yymsgp);
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
                      yytoken, &yylval, &yylloc, pResult, scanner);
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
  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

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
                  yystos[yystate], yyvsp, yylsp, pResult, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, pResult, scanner, YY_("memory exhausted"));
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
                  yytoken, &yylval, &yylloc, pResult, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp, pResult, scanner);
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
  return yyresult;
}
#line 426 "/home/guoxiang/gopath/src/github.com/luguoxiang/level_pgserver/common/SqlParser.y" /* yacc.c:1906  */


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
