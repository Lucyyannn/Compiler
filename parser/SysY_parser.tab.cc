/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser/SysY_parser.y"

#include <fstream>
#include "SysY_tree.h"
#include "type.h"
Program ast_root;

void yyerror(char *s, ...);
int yylex();
int error_num = 0;
extern int line_number;
extern std::ofstream fout;
extern IdTable id_table;

#line 85 "SysY_parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "SysY_parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_STR_CONST = 3,                  /* STR_CONST  */
  YYSYMBOL_IDENT = 4,                      /* IDENT  */
  YYSYMBOL_FLOAT_CONST = 5,                /* FLOAT_CONST  */
  YYSYMBOL_INT_CONST = 6,                  /* INT_CONST  */
  YYSYMBOL_LEQ = 7,                        /* LEQ  */
  YYSYMBOL_GEQ = 8,                        /* GEQ  */
  YYSYMBOL_EQ = 9,                         /* EQ  */
  YYSYMBOL_NE = 10,                        /* NE  */
  YYSYMBOL_AND = 11,                       /* AND  */
  YYSYMBOL_OR = 12,                        /* OR  */
  YYSYMBOL_CONST = 13,                     /* CONST  */
  YYSYMBOL_IF = 14,                        /* IF  */
  YYSYMBOL_ELSE = 15,                      /* ELSE  */
  YYSYMBOL_WHILE = 16,                     /* WHILE  */
  YYSYMBOL_NONE_TYPE = 17,                 /* NONE_TYPE  */
  YYSYMBOL_INT = 18,                       /* INT  */
  YYSYMBOL_FLOAT = 19,                     /* FLOAT  */
  YYSYMBOL_FOR = 20,                       /* FOR  */
  YYSYMBOL_RETURN = 21,                    /* RETURN  */
  YYSYMBOL_BREAK = 22,                     /* BREAK  */
  YYSYMBOL_CONTINUE = 23,                  /* CONTINUE  */
  YYSYMBOL_ERROR = 24,                     /* ERROR  */
  YYSYMBOL_THEN = 25,                      /* THEN  */
  YYSYMBOL_26_ = 26,                       /* ';'  */
  YYSYMBOL_27_ = 27,                       /* ','  */
  YYSYMBOL_28_ = 28,                       /* '['  */
  YYSYMBOL_29_ = 29,                       /* ']'  */
  YYSYMBOL_30_ = 30,                       /* '('  */
  YYSYMBOL_31_ = 31,                       /* ')'  */
  YYSYMBOL_32_ = 32,                       /* '='  */
  YYSYMBOL_33_ = 33,                       /* '{'  */
  YYSYMBOL_34_ = 34,                       /* '}'  */
  YYSYMBOL_35_ = 35,                       /* '*'  */
  YYSYMBOL_36_ = 36,                       /* '/'  */
  YYSYMBOL_37_ = 37,                       /* '%'  */
  YYSYMBOL_38_ = 38,                       /* '+'  */
  YYSYMBOL_39_ = 39,                       /* '-'  */
  YYSYMBOL_40_ = 40,                       /* '<'  */
  YYSYMBOL_41_ = 41,                       /* '>'  */
  YYSYMBOL_42_ = 42,                       /* '!'  */
  YYSYMBOL_YYACCEPT = 43,                  /* $accept  */
  YYSYMBOL_Program = 44,                   /* Program  */
  YYSYMBOL_Comp_list = 45,                 /* Comp_list  */
  YYSYMBOL_CompUnit = 46,                  /* CompUnit  */
  YYSYMBOL_Decl = 47,                      /* Decl  */
  YYSYMBOL_VarDecl = 48,                   /* VarDecl  */
  YYSYMBOL_VarDef_list = 49,               /* VarDef_list  */
  YYSYMBOL_ConstDecl = 50,                 /* ConstDecl  */
  YYSYMBOL_ConstDef_list = 51,             /* ConstDef_list  */
  YYSYMBOL_ArrayDim = 52,                  /* ArrayDim  */
  YYSYMBOL_ArrayDim_list = 53,             /* ArrayDim_list  */
  YYSYMBOL_FuncDef = 54,                   /* FuncDef  */
  YYSYMBOL_VarDef = 55,                    /* VarDef  */
  YYSYMBOL_ConstDef = 56,                  /* ConstDef  */
  YYSYMBOL_ConstInitVal_list = 57,         /* ConstInitVal_list  */
  YYSYMBOL_ConstInitVal = 58,              /* ConstInitVal  */
  YYSYMBOL_VarInitVal_list = 59,           /* VarInitVal_list  */
  YYSYMBOL_VarInitVal = 60,                /* VarInitVal  */
  YYSYMBOL_FuncFParams = 61,               /* FuncFParams  */
  YYSYMBOL_FuncFParam = 62,                /* FuncFParam  */
  YYSYMBOL_Block = 63,                     /* Block  */
  YYSYMBOL_BlockItem_list = 64,            /* BlockItem_list  */
  YYSYMBOL_BlockItem = 65,                 /* BlockItem  */
  YYSYMBOL_Stmt = 66,                      /* Stmt  */
  YYSYMBOL_Exp = 67,                       /* Exp  */
  YYSYMBOL_ConstExp = 68,                  /* ConstExp  */
  YYSYMBOL_Exp_list = 69,                  /* Exp_list  */
  YYSYMBOL_MulExp = 70,                    /* MulExp  */
  YYSYMBOL_AddExp = 71,                    /* AddExp  */
  YYSYMBOL_Cond = 72,                      /* Cond  */
  YYSYMBOL_RelExp = 73,                    /* RelExp  */
  YYSYMBOL_EqExp = 74,                     /* EqExp  */
  YYSYMBOL_LAndExp = 75,                   /* LAndExp  */
  YYSYMBOL_LOrExp = 76,                    /* LOrExp  */
  YYSYMBOL_PrimaryExp = 77,                /* PrimaryExp  */
  YYSYMBOL_IntConst = 78,                  /* IntConst  */
  YYSYMBOL_FloatConst = 79,                /* FloatConst  */
  YYSYMBOL_Lval = 80,                      /* Lval  */
  YYSYMBOL_UnaryExp = 81,                  /* UnaryExp  */
  YYSYMBOL_FuncRParams = 82                /* FuncRParams  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
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
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  20
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   307

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  43
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  105
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  195

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   280


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    42,     2,     2,     2,    37,     2,     2,
      30,    31,    35,    38,    27,    39,     2,    36,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    26,
      40,    32,    41,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    28,     2,    29,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    33,     2,    34,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    68,    68,    76,    77,    80,    81,    85,    86,    90,
      91,    94,    95,    99,   100,   103,   104,   109,   112,   113,
     117,   122,   127,   132,   137,   142,   150,   153,   157,   161,
     168,   171,   178,   179,   182,   185,   188,   193,   194,   197,
     200,   203,   210,   211,   214,   218,   222,   228,   234,   239,
     249,   250,   254,   255,   259,   260,   264,   268,   272,   276,
     280,   284,   288,   292,   296,   300,   304,   312,   318,   321,
     322,   325,   326,   327,   328,   331,   332,   333,   337,   342,
     343,   344,   345,   346,   349,   350,   351,   354,   355,   358,
     359,   365,   368,   371,   374,   380,   383,   386,   390,   397,
     400,   404,   428,   429,   430,   433
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "STR_CONST", "IDENT",
  "FLOAT_CONST", "INT_CONST", "LEQ", "GEQ", "EQ", "NE", "AND", "OR",
  "CONST", "IF", "ELSE", "WHILE", "NONE_TYPE", "INT", "FLOAT", "FOR",
  "RETURN", "BREAK", "CONTINUE", "ERROR", "THEN", "';'", "','", "'['",
  "']'", "'('", "')'", "'='", "'{'", "'}'", "'*'", "'/'", "'%'", "'+'",
  "'-'", "'<'", "'>'", "'!'", "$accept", "Program", "Comp_list",
  "CompUnit", "Decl", "VarDecl", "VarDef_list", "ConstDecl",
  "ConstDef_list", "ArrayDim", "ArrayDim_list", "FuncDef", "VarDef",
  "ConstDef", "ConstInitVal_list", "ConstInitVal", "VarInitVal_list",
  "VarInitVal", "FuncFParams", "FuncFParam", "Block", "BlockItem_list",
  "BlockItem", "Stmt", "Exp", "ConstExp", "Exp_list", "MulExp", "AddExp",
  "Cond", "RelExp", "EqExp", "LAndExp", "LOrExp", "PrimaryExp", "IntConst",
  "FloatConst", "Lval", "UnaryExp", "FuncRParams", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-159)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     105,    54,     1,    29,    43,    62,   105,  -159,  -159,  -159,
    -159,  -159,    77,    77,    34,     0,   116,  -159,   106,   118,
    -159,  -159,    35,   131,  -159,   133,     3,   265,     6,   211,
    -159,    60,  -159,   100,    23,  -159,   225,    61,  -159,    77,
    -159,   103,   109,    88,    68,  -159,    20,  -159,  -159,   265,
     265,   265,   265,   111,    92,   123,  -159,  -159,  -159,  -159,
    -159,    88,    83,   194,  -159,  -159,   123,   211,  -159,   107,
    -159,    88,   110,   204,  -159,  -159,   225,  -159,   137,   139,
      64,  -159,   156,    88,    70,   142,   148,  -159,  -159,  -159,
    -159,   265,   265,   265,   265,   265,  -159,    88,  -159,     4,
    -159,  -159,  -159,    88,  -159,     9,  -159,  -159,   164,   165,
     166,   171,   100,   100,   255,   179,   180,  -159,  -159,  -159,
    -159,   150,  -159,  -159,   186,   189,  -159,  -159,  -159,  -159,
     191,   182,  -159,  -159,  -159,  -159,    92,    92,  -159,   211,
    -159,  -159,   225,  -159,   142,   142,   265,   265,  -159,   196,
    -159,  -159,  -159,  -159,  -159,   265,   265,  -159,  -159,  -159,
     142,   142,   123,   195,    12,   168,   214,   223,   208,  -159,
     219,  -159,   181,   265,   265,   265,   265,   265,   265,   265,
     265,   181,  -159,   232,   123,   123,   123,   123,    12,    12,
     168,   214,  -159,   181,  -159
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     2,     3,     5,     7,
       8,     6,     0,     0,     0,    27,     0,    11,    27,     0,
       1,     4,     0,     0,    15,     0,     0,     0,     0,     0,
      18,    28,     9,     0,     0,    10,     0,     0,    13,     0,
      14,     0,     0,     0,     0,    42,    97,    96,    95,     0,
       0,     0,     0,     0,    75,    68,    99,    91,    92,    93,
      71,     0,     0,     0,    26,    39,    67,     0,    19,    27,
      12,     0,     0,     0,    30,    34,     0,    16,    44,    45,
       0,    25,     0,     0,     0,    98,     0,   102,   103,   104,
      17,     0,     0,     0,     0,     0,    21,     0,    41,     0,
      37,    29,    23,     0,    36,     0,    32,    31,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    58,    51,    54,
      59,     0,    52,    55,     0,    93,    43,    24,   101,    69,
     105,     0,    94,    72,    73,    74,    76,    77,    20,     0,
      40,    22,     0,    35,    46,    47,     0,     0,    66,     0,
      63,    64,    50,    53,    57,     0,     0,   100,    38,    33,
      48,    49,    79,     0,    84,    87,    89,    78,     0,    65,
       0,    70,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    56,    60,    82,    83,    80,    81,    85,    86,
      88,    90,    62,     0,    61
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -159,  -159,  -159,   234,   -62,  -159,    -2,  -159,   235,   -30,
     -19,  -159,   218,   213,  -159,   -63,  -159,   -55,    32,   172,
     -32,  -159,   135,  -158,   -23,   230,  -159,    87,   -27,   115,
      13,    86,    93,  -159,  -159,  -159,  -159,   -76,   -35,  -159
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     5,     6,     7,     8,     9,    16,    10,    23,    30,
      31,    11,    17,    24,   105,    74,    99,    64,    44,    45,
     120,   121,   122,   123,   124,    75,   130,    54,    66,   163,
     164,   165,   166,   167,    56,    57,    58,    59,    60,   131
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      55,    68,    19,    37,   125,    14,    65,    68,   100,    55,
     106,    81,   101,   107,   183,    87,    88,    89,   119,   173,
     174,    41,    42,   192,    41,    42,    86,    85,    27,    96,
      28,   139,    29,    15,    43,   194,   142,    61,   140,   102,
      65,    41,    42,   143,    65,   125,    55,    18,    27,    55,
      84,   127,   175,   176,    71,    68,   133,   134,   135,   119,
      62,   129,    20,    27,    26,   138,    72,    36,    46,    47,
      48,   141,    12,    13,    46,    47,    48,     1,   110,   159,
     111,    22,   112,   113,   158,   114,   115,   116,    27,    27,
     117,   149,    67,    76,    49,    82,   125,    80,   118,    83,
      49,   128,    50,    51,    69,   125,    52,    78,    50,    51,
      82,    19,    52,    79,    97,    55,    65,   125,     1,   162,
     162,    80,     2,     3,     4,   160,   161,    91,    92,    93,
      68,    68,   170,   171,    27,    27,    34,    82,    29,    29,
      90,   103,    32,    33,    35,    33,   184,   185,   186,   187,
     162,   162,   162,   162,    46,    47,    48,    38,    39,    40,
      39,    94,    95,     1,   110,   108,   111,   109,   112,   113,
      27,   114,   115,   116,    41,    42,   117,   177,   178,   132,
      49,   136,   137,    80,   152,    46,    47,    48,    50,    51,
     188,   189,    52,   144,   145,   110,   146,   111,    46,    47,
      48,   147,   114,   115,   116,   150,   151,   117,    46,    47,
      48,    49,   154,   157,    80,    46,    47,    48,   156,    50,
      51,   155,   169,    52,    49,   179,   172,    63,    98,    46,
      47,    48,    50,    51,    49,   180,    52,    73,   104,   181,
      21,    49,    50,    51,    63,   182,    52,   193,    25,    50,
      51,    70,    77,    52,   126,    49,   153,    53,    73,    46,
      47,    48,   168,    50,    51,   190,     0,    52,     0,    46,
      47,    48,     0,   191,     0,     0,     0,     0,     0,     0,
       0,   148,     0,     0,     0,    49,     0,     0,     0,     0,
       0,     0,     0,    50,    51,    49,     0,    52,     0,     0,
       0,     0,     0,    50,    51,     0,     0,    52
};

static const yytype_int16 yycheck[] =
{
      27,    31,     4,    22,    80,     4,    29,    37,    63,    36,
      73,    43,    67,    76,   172,    50,    51,    52,    80,     7,
       8,    18,    19,   181,    18,    19,    49,    46,    28,    61,
      30,    27,    32,     4,    31,   193,    27,    31,    34,    71,
      63,    18,    19,    34,    67,   121,    73,     4,    28,    76,
      30,    83,    40,    41,    31,    85,    91,    92,    93,   121,
      28,    84,     0,    28,    30,    97,    34,    32,     4,     5,
       6,   103,    18,    19,     4,     5,     6,    13,    14,   142,
      16,     4,    18,    19,   139,    21,    22,    23,    28,    28,
      26,   114,    32,    32,    30,    27,   172,    33,    34,    31,
      30,    31,    38,    39,     4,   181,    42,     4,    38,    39,
      27,   113,    42,     4,    31,   142,   139,   193,    13,   146,
     147,    33,    17,    18,    19,   144,   145,    35,    36,    37,
     160,   161,   155,   156,    28,    28,    30,    27,    32,    32,
      29,    31,    26,    27,    26,    27,   173,   174,   175,   176,
     177,   178,   179,   180,     4,     5,     6,    26,    27,    26,
      27,    38,    39,    13,    14,    28,    16,    28,    18,    19,
      28,    21,    22,    23,    18,    19,    26,     9,    10,    31,
      30,    94,    95,    33,    34,     4,     5,     6,    38,    39,
     177,   178,    42,    29,    29,    14,    30,    16,     4,     5,
       6,    30,    21,    22,    23,    26,    26,    26,     4,     5,
       6,    30,    26,    31,    33,     4,     5,     6,    27,    38,
      39,    32,    26,    42,    30,    11,    31,    33,    34,     4,
       5,     6,    38,    39,    30,    12,    42,    33,    34,    31,
       6,    30,    38,    39,    33,    26,    42,    15,    13,    38,
      39,    33,    39,    42,    82,    30,   121,    27,    33,     4,
       5,     6,   147,    38,    39,   179,    -1,    42,    -1,     4,
       5,     6,    -1,   180,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    26,    -1,    -1,    -1,    30,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    38,    39,    30,    -1,    42,    -1,    -1,
      -1,    -1,    -1,    38,    39,    -1,    -1,    42
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    13,    17,    18,    19,    44,    45,    46,    47,    48,
      50,    54,    18,    19,     4,     4,    49,    55,     4,    49,
       0,    46,     4,    51,    56,    51,    30,    28,    30,    32,
      52,    53,    26,    27,    30,    26,    32,    53,    26,    27,
      26,    18,    19,    31,    61,    62,     4,     5,     6,    30,
      38,    39,    42,    68,    70,    71,    77,    78,    79,    80,
      81,    31,    61,    33,    60,    67,    71,    32,    52,     4,
      55,    31,    61,    33,    58,    68,    32,    56,     4,     4,
      33,    63,    27,    31,    30,    53,    67,    81,    81,    81,
      29,    35,    36,    37,    38,    39,    63,    31,    34,    59,
      60,    60,    63,    31,    34,    57,    58,    58,    28,    28,
      14,    16,    18,    19,    21,    22,    23,    26,    34,    47,
      63,    64,    65,    66,    67,    80,    62,    63,    31,    67,
      69,    82,    31,    81,    81,    81,    70,    70,    63,    27,
      34,    63,    27,    34,    29,    29,    30,    30,    26,    67,
      26,    26,    34,    65,    26,    32,    27,    31,    60,    58,
      53,    53,    71,    72,    73,    74,    75,    76,    72,    26,
      67,    67,    31,     7,     8,    40,    41,     9,    10,    11,
      12,    31,    26,    66,    71,    71,    71,    71,    73,    73,
      74,    75,    66,    15,    66
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    43,    44,    45,    45,    46,    46,    47,    47,    48,
      48,    49,    49,    50,    50,    51,    51,    52,    53,    53,
      54,    54,    54,    54,    54,    54,    55,    55,    55,    55,
      56,    56,    57,    57,    58,    58,    58,    59,    59,    60,
      60,    60,    61,    61,    62,    62,    62,    62,    62,    62,
      63,    63,    64,    64,    65,    65,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    67,    68,    69,
      69,    70,    70,    70,    70,    71,    71,    71,    72,    73,
      73,    73,    73,    73,    74,    74,    74,    75,    75,    76,
      76,    77,    77,    77,    77,    78,    79,    80,    80,    81,
      81,    81,    81,    81,    81,    82
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     1,     1,     3,
       3,     1,     3,     4,     4,     1,     3,     3,     1,     2,
       6,     5,     6,     5,     6,     5,     3,     1,     2,     4,
       3,     4,     1,     3,     1,     3,     2,     1,     3,     1,
       3,     2,     1,     3,     2,     2,     4,     4,     5,     5,
       3,     2,     1,     2,     1,     1,     4,     2,     1,     1,
       5,     7,     5,     2,     2,     3,     2,     1,     1,     1,
       3,     1,     3,     3,     3,     1,     3,     3,     1,     1,
       3,     3,     3,     3,     1,     3,     3,     1,     3,     1,
       3,     1,     1,     1,     3,     1,     1,     1,     2,     1,
       4,     3,     2,     2,     2,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
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
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

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


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
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

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
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

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* Program: Comp_list  */
#line 69 "parser/SysY_parser.y"
{
    (yyloc) = (yylsp[0]);      //继承位置信息
    ast_root = new __Program((yyvsp[0].comps));
    ast_root->SetLineNumber(line_number);
}
#line 1427 "SysY_parser.tab.c"
    break;

  case 3: /* Comp_list: CompUnit  */
#line 76 "parser/SysY_parser.y"
          {(yyval.comps) = new std::vector<CompUnit>; ((yyval.comps))->push_back((yyvsp[0].comp_unit));}
#line 1433 "SysY_parser.tab.c"
    break;

  case 4: /* Comp_list: Comp_list CompUnit  */
#line 77 "parser/SysY_parser.y"
                    {((yyvsp[-1].comps))->push_back((yyvsp[0].comp_unit)); (yyval.comps) = (yyvsp[-1].comps);}
#line 1439 "SysY_parser.tab.c"
    break;

  case 5: /* CompUnit: Decl  */
#line 80 "parser/SysY_parser.y"
      {(yyval.comp_unit) = new CompUnit_Decl((yyvsp[0].decl)); (yyval.comp_unit)->SetLineNumber(line_number);}
#line 1445 "SysY_parser.tab.c"
    break;

  case 6: /* CompUnit: FuncDef  */
#line 81 "parser/SysY_parser.y"
         {(yyval.comp_unit) = new CompUnit_FuncDef((yyvsp[0].func_def)); (yyval.comp_unit)->SetLineNumber(line_number);}
#line 1451 "SysY_parser.tab.c"
    break;

  case 7: /* Decl: VarDecl  */
#line 85 "parser/SysY_parser.y"
        {(yyval.decl) = (yyvsp[0].decl); (yyval.decl)->SetLineNumber(line_number);}
#line 1457 "SysY_parser.tab.c"
    break;

  case 8: /* Decl: ConstDecl  */
#line 86 "parser/SysY_parser.y"
          {(yyval.decl) = (yyvsp[0].decl); (yyval.decl)->SetLineNumber(line_number);}
#line 1463 "SysY_parser.tab.c"
    break;

  case 9: /* VarDecl: INT VarDef_list ';'  */
#line 90 "parser/SysY_parser.y"
                    {(yyval.decl) = new VarDecl(Type::INT,(yyvsp[-1].defs)); (yyval.decl)->SetLineNumber(line_number);}
#line 1469 "SysY_parser.tab.c"
    break;

  case 10: /* VarDecl: FLOAT VarDef_list ';'  */
#line 91 "parser/SysY_parser.y"
                       {(yyval.decl) = new VarDecl(Type::FLOAT,(yyvsp[-1].defs));(yyval.decl)->SetLineNumber(line_number);}
#line 1475 "SysY_parser.tab.c"
    break;

  case 11: /* VarDef_list: VarDef  */
#line 94 "parser/SysY_parser.y"
         {(yyval.defs) = new std::vector<Def>;((yyval.defs))->push_back((yyvsp[0].def));}
#line 1481 "SysY_parser.tab.c"
    break;

  case 12: /* VarDef_list: VarDef_list ',' VarDef  */
#line 95 "parser/SysY_parser.y"
                        {((yyvsp[-2].defs))->push_back((yyvsp[0].def));(yyval.defs) = (yyvsp[-2].defs);}
#line 1487 "SysY_parser.tab.c"
    break;

  case 13: /* ConstDecl: CONST INT ConstDef_list ';'  */
#line 99 "parser/SysY_parser.y"
                            {(yyval.decl) = new ConstDecl(Type::INT,(yyvsp[-1].defs)); (yyval.decl)->SetLineNumber(line_number);}
#line 1493 "SysY_parser.tab.c"
    break;

  case 14: /* ConstDecl: CONST FLOAT ConstDef_list ';'  */
#line 100 "parser/SysY_parser.y"
                              {(yyval.decl) = new ConstDecl(Type::FLOAT,(yyvsp[-1].defs));(yyval.decl)->SetLineNumber(line_number);}
#line 1499 "SysY_parser.tab.c"
    break;

  case 15: /* ConstDef_list: ConstDef  */
#line 103 "parser/SysY_parser.y"
          {(yyval.defs) = new std::vector<Def>; ((yyval.defs))->push_back((yyvsp[0].def));}
#line 1505 "SysY_parser.tab.c"
    break;

  case 16: /* ConstDef_list: ConstDef_list ',' ConstDef  */
#line 104 "parser/SysY_parser.y"
                            {((yyvsp[-2].defs))->push_back((yyvsp[0].def));(yyval.defs) = (yyvsp[-2].defs);}
#line 1511 "SysY_parser.tab.c"
    break;

  case 17: /* ArrayDim: '[' ConstExp ']'  */
#line 109 "parser/SysY_parser.y"
                   {(yyval.expression)=(yyvsp[-1].expression);(yyval.expression)->SetLineNumber(line_number);}
#line 1517 "SysY_parser.tab.c"
    break;

  case 18: /* ArrayDim_list: ArrayDim  */
#line 112 "parser/SysY_parser.y"
         {(yyval.expressions) = new std::vector<Expression>;(yyval.expressions)->push_back((yyvsp[0].expression)); }
#line 1523 "SysY_parser.tab.c"
    break;

  case 19: /* ArrayDim_list: ArrayDim_list ArrayDim  */
#line 113 "parser/SysY_parser.y"
                       {(yyval.expressions) = (yyvsp[-1].expressions); (yyval.expressions)->push_back((yyvsp[0].expression));}
#line 1529 "SysY_parser.tab.c"
    break;

  case 20: /* FuncDef: INT IDENT '(' FuncFParams ')' Block  */
#line 118 "parser/SysY_parser.y"
{
    (yyval.func_def) = new __FuncDef(Type::INT,(yyvsp[-4].symbol_token),(yyvsp[-2].formals),(yyvsp[0].block));
    (yyval.func_def)->SetLineNumber(line_number);
}
#line 1538 "SysY_parser.tab.c"
    break;

  case 21: /* FuncDef: INT IDENT '(' ')' Block  */
#line 123 "parser/SysY_parser.y"
{
    (yyval.func_def) = new __FuncDef(Type::INT,(yyvsp[-3].symbol_token),new std::vector<FuncFParam>(),(yyvsp[0].block)); 
    (yyval.func_def)->SetLineNumber(line_number);
}
#line 1547 "SysY_parser.tab.c"
    break;

  case 22: /* FuncDef: FLOAT IDENT '(' FuncFParams ')' Block  */
#line 128 "parser/SysY_parser.y"
{
    (yyval.func_def) = new __FuncDef(Type::FLOAT,(yyvsp[-4].symbol_token),(yyvsp[-2].formals),(yyvsp[0].block));
    (yyval.func_def)->SetLineNumber(line_number);
}
#line 1556 "SysY_parser.tab.c"
    break;

  case 23: /* FuncDef: FLOAT IDENT '(' ')' Block  */
#line 133 "parser/SysY_parser.y"
{
    (yyval.func_def) = new __FuncDef(Type::FLOAT,(yyvsp[-3].symbol_token),new std::vector<FuncFParam>(),(yyvsp[0].block)); 
    (yyval.func_def)->SetLineNumber(line_number);
}
#line 1565 "SysY_parser.tab.c"
    break;

  case 24: /* FuncDef: NONE_TYPE IDENT '(' FuncFParams ')' Block  */
#line 138 "parser/SysY_parser.y"
{
    (yyval.func_def) = new __FuncDef(Type::VOID,(yyvsp[-4].symbol_token),(yyvsp[-2].formals),(yyvsp[0].block));
    (yyval.func_def)->SetLineNumber(line_number);
}
#line 1574 "SysY_parser.tab.c"
    break;

  case 25: /* FuncDef: NONE_TYPE IDENT '(' ')' Block  */
#line 143 "parser/SysY_parser.y"
{
    (yyval.func_def) = new __FuncDef(Type::VOID,(yyvsp[-3].symbol_token),new std::vector<FuncFParam>(),(yyvsp[0].block)); 
    (yyval.func_def)->SetLineNumber(line_number);
}
#line 1583 "SysY_parser.tab.c"
    break;

  case 26: /* VarDef: IDENT '=' VarInitVal  */
#line 151 "parser/SysY_parser.y"
{   (yyval.def) = new VarDef((yyvsp[-2].symbol_token),nullptr,(yyvsp[0].initval));
    (yyval.def)->SetLineNumber(line_number);}
#line 1590 "SysY_parser.tab.c"
    break;

  case 27: /* VarDef: IDENT  */
#line 154 "parser/SysY_parser.y"
{   (yyval.def) = new VarDef_no_init((yyvsp[0].symbol_token),nullptr); 
    (yyval.def)->SetLineNumber(line_number);}
#line 1597 "SysY_parser.tab.c"
    break;

  case 28: /* VarDef: IDENT ArrayDim_list  */
#line 157 "parser/SysY_parser.y"
                     {
    (yyval.def) = new VarDef_no_init((yyvsp[-1].symbol_token), (yyvsp[0].expressions)); 
    (yyval.def)->SetLineNumber(line_number);
}
#line 1606 "SysY_parser.tab.c"
    break;

  case 29: /* VarDef: IDENT ArrayDim_list '=' VarInitVal  */
#line 161 "parser/SysY_parser.y"
                                    {
    (yyval.def) = new VarDef((yyvsp[-3].symbol_token), (yyvsp[-2].expressions), (yyvsp[0].initval)); 
    (yyval.def)->SetLineNumber(line_number);
}
#line 1615 "SysY_parser.tab.c"
    break;

  case 30: /* ConstDef: IDENT '=' ConstInitVal  */
#line 169 "parser/SysY_parser.y"
{   (yyval.def) = new ConstDef((yyvsp[-2].symbol_token),nullptr,(yyvsp[0].initval)); 
    (yyval.def)->SetLineNumber(line_number);}
#line 1622 "SysY_parser.tab.c"
    break;

  case 31: /* ConstDef: IDENT ArrayDim_list '=' ConstInitVal  */
#line 171 "parser/SysY_parser.y"
                                      {
    (yyval.def) = new ConstDef((yyvsp[-3].symbol_token), (yyvsp[-2].expressions), (yyvsp[0].initval)); 
    (yyval.def)->SetLineNumber(line_number);
}
#line 1631 "SysY_parser.tab.c"
    break;

  case 32: /* ConstInitVal_list: ConstInitVal  */
#line 178 "parser/SysY_parser.y"
             {(yyval.initvals) = new std::vector<InitVal>;((yyval.initvals))->push_back((yyvsp[0].initval));}
#line 1637 "SysY_parser.tab.c"
    break;

  case 33: /* ConstInitVal_list: ConstInitVal_list ',' ConstInitVal  */
#line 179 "parser/SysY_parser.y"
                                   {((yyvsp[-2].initvals))->push_back((yyvsp[0].initval)); (yyval.initvals) = (yyvsp[-2].initvals);}
#line 1643 "SysY_parser.tab.c"
    break;

  case 34: /* ConstInitVal: ConstExp  */
#line 182 "parser/SysY_parser.y"
         { //单值初始化
    (yyval.initval) = new ConstInitVal_exp((yyvsp[0].expression)); 
    (yyval.initval)->SetLineNumber(line_number);}
#line 1651 "SysY_parser.tab.c"
    break;

  case 35: /* ConstInitVal: '{' ConstInitVal_list '}'  */
#line 185 "parser/SysY_parser.y"
                          { //初始化列表
    (yyval.initval) = new ConstInitVal((yyvsp[-1].initvals));
     (yyval.initval)->SetLineNumber(line_number);}
#line 1659 "SysY_parser.tab.c"
    break;

  case 36: /* ConstInitVal: '{' '}'  */
#line 188 "parser/SysY_parser.y"
        {//初始化为空
    (yyval.initval) = new ConstInitVal(new std::vector<InitVal>()); 
    (yyval.initval)->SetLineNumber(line_number);}
#line 1667 "SysY_parser.tab.c"
    break;

  case 37: /* VarInitVal_list: VarInitVal  */
#line 193 "parser/SysY_parser.y"
           {(yyval.initvals) = new std::vector<InitVal>;((yyval.initvals))->push_back((yyvsp[0].initval));}
#line 1673 "SysY_parser.tab.c"
    break;

  case 38: /* VarInitVal_list: VarInitVal_list ',' VarInitVal  */
#line 194 "parser/SysY_parser.y"
                               {((yyvsp[-2].initvals))->push_back((yyvsp[0].initval));(yyval.initvals) = (yyvsp[-2].initvals);}
#line 1679 "SysY_parser.tab.c"
    break;

  case 39: /* VarInitVal: Exp  */
#line 197 "parser/SysY_parser.y"
    { //单值初始化(支持 int b=a ；故不为ConstExp)
    (yyval.initval) = new VarInitVal_exp((yyvsp[0].expression)); 
    (yyval.initval)->SetLineNumber(line_number);}
#line 1687 "SysY_parser.tab.c"
    break;

  case 40: /* VarInitVal: '{' VarInitVal_list '}'  */
#line 200 "parser/SysY_parser.y"
                        { //初始化列表
    (yyval.initval) = new VarInitVal((yyvsp[-1].initvals));
     (yyval.initval)->SetLineNumber(line_number);}
#line 1695 "SysY_parser.tab.c"
    break;

  case 41: /* VarInitVal: '{' '}'  */
#line 203 "parser/SysY_parser.y"
        {
    (yyval.initval) = new VarInitVal(new std::vector<InitVal>()); 
    (yyval.initval)->SetLineNumber(line_number);}
#line 1703 "SysY_parser.tab.c"
    break;

  case 42: /* FuncFParams: FuncFParam  */
#line 210 "parser/SysY_parser.y"
           {(yyval.formals) = new std::vector<FuncFParam>;((yyval.formals))->push_back((yyvsp[0].formal));}
#line 1709 "SysY_parser.tab.c"
    break;

  case 43: /* FuncFParams: FuncFParams ',' FuncFParam  */
#line 211 "parser/SysY_parser.y"
                           {((yyvsp[-2].formals))->push_back((yyvsp[0].formal));(yyval.formals) = (yyvsp[-2].formals);}
#line 1715 "SysY_parser.tab.c"
    break;

  case 44: /* FuncFParam: INT IDENT  */
#line 214 "parser/SysY_parser.y"
          {
    (yyval.formal) = new __FuncFParam(Type::INT,(yyvsp[0].symbol_token),nullptr);
    (yyval.formal)->SetLineNumber(line_number);
}
#line 1724 "SysY_parser.tab.c"
    break;

  case 45: /* FuncFParam: FLOAT IDENT  */
#line 218 "parser/SysY_parser.y"
            {
    (yyval.formal) = new __FuncFParam(Type::FLOAT,(yyvsp[0].symbol_token),nullptr);
    (yyval.formal)->SetLineNumber(line_number);
}
#line 1733 "SysY_parser.tab.c"
    break;

  case 46: /* FuncFParam: INT IDENT '[' ']'  */
#line 222 "parser/SysY_parser.y"
                    {
    std::vector<Expression>* temp = new std::vector<Expression>;
    temp->push_back(nullptr);
    (yyval.formal) = new __FuncFParam(Type::INT,(yyvsp[-2].symbol_token),temp);
    (yyval.formal)->SetLineNumber(line_number);
}
#line 1744 "SysY_parser.tab.c"
    break;

  case 47: /* FuncFParam: FLOAT IDENT '[' ']'  */
#line 228 "parser/SysY_parser.y"
                      {
    std::vector<Expression>* temp = new std::vector<Expression>;
    temp->push_back(nullptr);
    (yyval.formal) = new __FuncFParam(Type::FLOAT,(yyvsp[-2].symbol_token),temp);
    (yyval.formal)->SetLineNumber(line_number);
}
#line 1755 "SysY_parser.tab.c"
    break;

  case 48: /* FuncFParam: INT IDENT '[' ']' ArrayDim_list  */
#line 234 "parser/SysY_parser.y"
                                {
    (yyvsp[0].expressions)->insert((yyvsp[0].expressions)->begin(),nullptr);
    (yyval.formal) = new __FuncFParam(Type::INT,(yyvsp[-3].symbol_token),(yyvsp[0].expressions));
    (yyval.formal)->SetLineNumber(line_number);
}
#line 1765 "SysY_parser.tab.c"
    break;

  case 49: /* FuncFParam: FLOAT IDENT '[' ']' ArrayDim_list  */
#line 239 "parser/SysY_parser.y"
                                  {
    (yyvsp[0].expressions)->insert((yyvsp[0].expressions)->begin(),nullptr);
    (yyval.formal) = new __FuncFParam(Type::FLOAT,(yyvsp[-3].symbol_token),(yyvsp[0].expressions));
    (yyval.formal)->SetLineNumber(line_number);
}
#line 1775 "SysY_parser.tab.c"
    break;

  case 50: /* Block: '{' BlockItem_list '}'  */
#line 249 "parser/SysY_parser.y"
                       {(yyval.block) = new __Block((yyvsp[-1].block_items));(yyval.block)->SetLineNumber(line_number);}
#line 1781 "SysY_parser.tab.c"
    break;

  case 51: /* Block: '{' '}'  */
#line 250 "parser/SysY_parser.y"
        {(yyval.block) = new __Block(new std::vector<BlockItem>);(yyval.block)->SetLineNumber(line_number);}
#line 1787 "SysY_parser.tab.c"
    break;

  case 52: /* BlockItem_list: BlockItem  */
#line 254 "parser/SysY_parser.y"
          {(yyval.block_items) = new std::vector<BlockItem>;((yyval.block_items))->push_back((yyvsp[0].block_item));}
#line 1793 "SysY_parser.tab.c"
    break;

  case 53: /* BlockItem_list: BlockItem_list BlockItem  */
#line 255 "parser/SysY_parser.y"
                          {((yyvsp[-1].block_items))->push_back((yyvsp[0].block_item));(yyval.block_items) = (yyvsp[-1].block_items);}
#line 1799 "SysY_parser.tab.c"
    break;

  case 54: /* BlockItem: Decl  */
#line 259 "parser/SysY_parser.y"
     {(yyval.block_item) = new BlockItem_Decl((yyvsp[0].decl));(yyval.block_item)->SetLineNumber(line_number);}
#line 1805 "SysY_parser.tab.c"
    break;

  case 55: /* BlockItem: Stmt  */
#line 260 "parser/SysY_parser.y"
     {(yyval.block_item) = new BlockItem_Stmt((yyvsp[0].stmt));(yyval.block_item)->SetLineNumber(line_number);}
#line 1811 "SysY_parser.tab.c"
    break;

  case 56: /* Stmt: Lval '=' Exp ';'  */
#line 264 "parser/SysY_parser.y"
                 {//赋值
    (yyval.stmt) = new assign_stmt((yyvsp[-3].expression),(yyvsp[-1].expression));
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1820 "SysY_parser.tab.c"
    break;

  case 57: /* Stmt: Exp ';'  */
#line 268 "parser/SysY_parser.y"
        {
    (yyval.stmt) = new expr_stmt((yyvsp[-1].expression));
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1829 "SysY_parser.tab.c"
    break;

  case 58: /* Stmt: ';'  */
#line 272 "parser/SysY_parser.y"
    {
    (yyval.stmt) = new null_stmt();
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1838 "SysY_parser.tab.c"
    break;

  case 59: /* Stmt: Block  */
#line 276 "parser/SysY_parser.y"
      {
    (yyval.stmt) = new block_stmt((yyvsp[0].block));
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1847 "SysY_parser.tab.c"
    break;

  case 60: /* Stmt: IF '(' Cond ')' Stmt  */
#line 280 "parser/SysY_parser.y"
                                {
    (yyval.stmt) = new if_stmt((yyvsp[-2].expression),(yyvsp[0].stmt));
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1856 "SysY_parser.tab.c"
    break;

  case 61: /* Stmt: IF '(' Cond ')' Stmt ELSE Stmt  */
#line 284 "parser/SysY_parser.y"
                               {
    (yyval.stmt) = new ifelse_stmt((yyvsp[-4].expression),(yyvsp[-2].stmt),(yyvsp[0].stmt));
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1865 "SysY_parser.tab.c"
    break;

  case 62: /* Stmt: WHILE '(' Cond ')' Stmt  */
#line 288 "parser/SysY_parser.y"
                        {
    (yyval.stmt) = new while_stmt((yyvsp[-2].expression),(yyvsp[0].stmt));
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1874 "SysY_parser.tab.c"
    break;

  case 63: /* Stmt: BREAK ';'  */
#line 292 "parser/SysY_parser.y"
          {
    (yyval.stmt) = new break_stmt();
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1883 "SysY_parser.tab.c"
    break;

  case 64: /* Stmt: CONTINUE ';'  */
#line 296 "parser/SysY_parser.y"
             {
    (yyval.stmt) = new continue_stmt();
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1892 "SysY_parser.tab.c"
    break;

  case 65: /* Stmt: RETURN Exp ';'  */
#line 300 "parser/SysY_parser.y"
               {
    (yyval.stmt) = new return_stmt((yyvsp[-1].expression));
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1901 "SysY_parser.tab.c"
    break;

  case 66: /* Stmt: RETURN ';'  */
#line 304 "parser/SysY_parser.y"
           {
    (yyval.stmt) = new return_stmt_void();
    (yyval.stmt)->SetLineNumber(line_number);
}
#line 1910 "SysY_parser.tab.c"
    break;

  case 67: /* Exp: AddExp  */
#line 312 "parser/SysY_parser.y"
       { //"+""-"类 它包含了更高级别的运算
    (yyval.expression) = (yyvsp[0].expression); 
    (yyval.expression)->SetLineNumber(line_number);
}
#line 1919 "SysY_parser.tab.c"
    break;

  case 68: /* ConstExp: AddExp  */
#line 318 "parser/SysY_parser.y"
       {(yyval.expression) = (yyvsp[0].expression);(yyval.expression)->SetLineNumber(line_number);}
#line 1925 "SysY_parser.tab.c"
    break;

  case 69: /* Exp_list: Exp  */
#line 321 "parser/SysY_parser.y"
    {(yyval.expressions) = new std::vector<Expression>;((yyval.expressions))->push_back((yyvsp[0].expression));}
#line 1931 "SysY_parser.tab.c"
    break;

  case 70: /* Exp_list: Exp_list ',' Exp  */
#line 322 "parser/SysY_parser.y"
                 {((yyvsp[-2].expressions))->push_back((yyvsp[0].expression));(yyval.expressions) = (yyvsp[-2].expressions);}
#line 1937 "SysY_parser.tab.c"
    break;

  case 71: /* MulExp: UnaryExp  */
#line 325 "parser/SysY_parser.y"
         {(yyval.expression) = (yyvsp[0].expression);(yyval.expression)->SetLineNumber(line_number);}
#line 1943 "SysY_parser.tab.c"
    break;

  case 72: /* MulExp: MulExp '*' UnaryExp  */
#line 326 "parser/SysY_parser.y"
                    {(yyval.expression) = new MulExp_mul((yyvsp[-2].expression),(yyvsp[0].expression)); (yyval.expression)->SetLineNumber(line_number);}
#line 1949 "SysY_parser.tab.c"
    break;

  case 73: /* MulExp: MulExp '/' UnaryExp  */
#line 327 "parser/SysY_parser.y"
                    {(yyval.expression) = new MulExp_div((yyvsp[-2].expression),(yyvsp[0].expression)); (yyval.expression)->SetLineNumber(line_number);}
#line 1955 "SysY_parser.tab.c"
    break;

  case 74: /* MulExp: MulExp '%' UnaryExp  */
#line 328 "parser/SysY_parser.y"
                    {(yyval.expression) = new MulExp_mod((yyvsp[-2].expression),(yyvsp[0].expression)); (yyval.expression)->SetLineNumber(line_number);}
#line 1961 "SysY_parser.tab.c"
    break;

  case 75: /* AddExp: MulExp  */
#line 331 "parser/SysY_parser.y"
       {(yyval.expression) = (yyvsp[0].expression);(yyval.expression)->SetLineNumber(line_number);}
#line 1967 "SysY_parser.tab.c"
    break;

  case 76: /* AddExp: AddExp '+' MulExp  */
#line 332 "parser/SysY_parser.y"
                  {(yyval.expression) = new AddExp_plus((yyvsp[-2].expression),(yyvsp[0].expression)); (yyval.expression)->SetLineNumber(line_number);}
#line 1973 "SysY_parser.tab.c"
    break;

  case 77: /* AddExp: AddExp '-' MulExp  */
#line 333 "parser/SysY_parser.y"
                  {(yyval.expression) = new AddExp_sub((yyvsp[-2].expression),(yyvsp[0].expression)); (yyval.expression)->SetLineNumber(line_number);}
#line 1979 "SysY_parser.tab.c"
    break;

  case 78: /* Cond: LOrExp  */
#line 337 "parser/SysY_parser.y"
       {//"||"  它包含了更高级别的逻辑
    (yyval.expression) = (yyvsp[0].expression); 
    (yyval.expression)->SetLineNumber(line_number);
}
#line 1988 "SysY_parser.tab.c"
    break;

  case 79: /* RelExp: AddExp  */
#line 342 "parser/SysY_parser.y"
       {(yyval.expression) = (yyvsp[0].expression);(yyval.expression)->SetLineNumber(line_number);}
#line 1994 "SysY_parser.tab.c"
    break;

  case 80: /* RelExp: RelExp '<' AddExp  */
#line 343 "parser/SysY_parser.y"
                  {(yyval.expression) = new RelExp_lt((yyvsp[-2].expression),(yyvsp[0].expression)); (yyval.expression)->SetLineNumber(line_number);}
#line 2000 "SysY_parser.tab.c"
    break;

  case 81: /* RelExp: RelExp '>' AddExp  */
#line 344 "parser/SysY_parser.y"
                  {(yyval.expression) = new RelExp_gt((yyvsp[-2].expression),(yyvsp[0].expression)); (yyval.expression)->SetLineNumber(line_number);}
#line 2006 "SysY_parser.tab.c"
    break;

  case 82: /* RelExp: RelExp LEQ AddExp  */
#line 345 "parser/SysY_parser.y"
                  {(yyval.expression) = new RelExp_leq((yyvsp[-2].expression),(yyvsp[0].expression)); (yyval.expression)->SetLineNumber(line_number);}
#line 2012 "SysY_parser.tab.c"
    break;

  case 83: /* RelExp: RelExp GEQ AddExp  */
#line 346 "parser/SysY_parser.y"
                  {(yyval.expression) = new RelExp_geq((yyvsp[-2].expression),(yyvsp[0].expression)); (yyval.expression)->SetLineNumber(line_number);}
#line 2018 "SysY_parser.tab.c"
    break;

  case 84: /* EqExp: RelExp  */
#line 349 "parser/SysY_parser.y"
       {(yyval.expression) = (yyvsp[0].expression);(yyval.expression)->SetLineNumber(line_number);}
#line 2024 "SysY_parser.tab.c"
    break;

  case 85: /* EqExp: EqExp EQ RelExp  */
#line 350 "parser/SysY_parser.y"
                {(yyval.expression) = new EqExp_eq((yyvsp[-2].expression),(yyvsp[0].expression)); (yyval.expression)->SetLineNumber(line_number);}
#line 2030 "SysY_parser.tab.c"
    break;

  case 86: /* EqExp: EqExp NE RelExp  */
#line 351 "parser/SysY_parser.y"
                {(yyval.expression) = new EqExp_neq((yyvsp[-2].expression),(yyvsp[0].expression)); (yyval.expression)->SetLineNumber(line_number);}
#line 2036 "SysY_parser.tab.c"
    break;

  case 87: /* LAndExp: EqExp  */
#line 354 "parser/SysY_parser.y"
      {(yyval.expression) = (yyvsp[0].expression);(yyval.expression)->SetLineNumber(line_number);}
#line 2042 "SysY_parser.tab.c"
    break;

  case 88: /* LAndExp: LAndExp AND EqExp  */
#line 355 "parser/SysY_parser.y"
                  {(yyval.expression) = new LAndExp_and((yyvsp[-2].expression),(yyvsp[0].expression)); (yyval.expression)->SetLineNumber(line_number);}
#line 2048 "SysY_parser.tab.c"
    break;

  case 89: /* LOrExp: LAndExp  */
#line 358 "parser/SysY_parser.y"
        {(yyval.expression) = (yyvsp[0].expression);(yyval.expression)->SetLineNumber(line_number);}
#line 2054 "SysY_parser.tab.c"
    break;

  case 90: /* LOrExp: LOrExp OR LAndExp  */
#line 359 "parser/SysY_parser.y"
                  {(yyval.expression) = new LOrExp_or((yyvsp[-2].expression),(yyvsp[0].expression)); (yyval.expression)->SetLineNumber(line_number);}
#line 2060 "SysY_parser.tab.c"
    break;

  case 91: /* PrimaryExp: IntConst  */
#line 365 "parser/SysY_parser.y"
         { //INT/FLOAT数值、左值表达式、（)的语句
    (yyval.expression) = (yyvsp[0].expression); 
    (yyval.expression)->SetLineNumber(line_number);}
#line 2068 "SysY_parser.tab.c"
    break;

  case 92: /* PrimaryExp: FloatConst  */
#line 368 "parser/SysY_parser.y"
           {
    (yyval.expression) = (yyvsp[0].expression); 
    (yyval.expression)->SetLineNumber(line_number);}
#line 2076 "SysY_parser.tab.c"
    break;

  case 93: /* PrimaryExp: Lval  */
#line 371 "parser/SysY_parser.y"
     {
    (yyval.expression) = (yyvsp[0].expression); 
    (yyval.expression)->SetLineNumber(line_number);}
#line 2084 "SysY_parser.tab.c"
    break;

  case 94: /* PrimaryExp: '(' Exp ')'  */
#line 374 "parser/SysY_parser.y"
            {
    (yyval.expression) = new PrimaryExp_branch((yyvsp[-1].expression));
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2093 "SysY_parser.tab.c"
    break;

  case 95: /* IntConst: INT_CONST  */
#line 380 "parser/SysY_parser.y"
          {(yyval.expression) = new IntConst((yyvsp[0].int_token));(yyval.expression)->SetLineNumber(line_number);}
#line 2099 "SysY_parser.tab.c"
    break;

  case 96: /* FloatConst: FLOAT_CONST  */
#line 383 "parser/SysY_parser.y"
            {(yyval.expression) = new FloatConst((yyvsp[0].float_token));(yyval.expression)->SetLineNumber(line_number);}
#line 2105 "SysY_parser.tab.c"
    break;

  case 97: /* Lval: IDENT  */
#line 386 "parser/SysY_parser.y"
      {//普通变量
    (yyval.expression) = new Lval((yyvsp[0].symbol_token),nullptr);
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2114 "SysY_parser.tab.c"
    break;

  case 98: /* Lval: IDENT ArrayDim_list  */
#line 390 "parser/SysY_parser.y"
                    { //数组
    (yyval.expression) = new Lval((yyvsp[-1].symbol_token),(yyvsp[0].expressions));
    (yyval.expression)->SetLineNumber(line_number);}
#line 2122 "SysY_parser.tab.c"
    break;

  case 99: /* UnaryExp: PrimaryExp  */
#line 397 "parser/SysY_parser.y"
           {
    (yyval.expression) = (yyvsp[0].expression);
}
#line 2130 "SysY_parser.tab.c"
    break;

  case 100: /* UnaryExp: IDENT '(' FuncRParams ')'  */
#line 400 "parser/SysY_parser.y"
                          {
    (yyval.expression) = new Func_call((yyvsp[-3].symbol_token),(yyvsp[-1].expression));
    (yyval.expression)->SetLineNumber(line_number);
}
#line 2139 "SysY_parser.tab.c"
    break;

  case 101: /* UnaryExp: IDENT '(' ')'  */
#line 404 "parser/SysY_parser.y"
              {
    // 在sylib.h这个文件中,starttime()是一个宏定义
    // #define starttime() _sysy_starttime(__LINE__)
    // 我们在语法分析中将其替换为_sysy_starttime(line_number)
    // stoptime同理
    if((yyvsp[-2].symbol_token)->get_string() == "starttime"){
        auto params = new std::vector<Expression>;
        params->push_back(new IntConst(line_number));
        Expression temp = new FuncRParams(params);
        (yyval.expression) = new Func_call(id_table.add_id("_sysy_starttime"),temp);
        (yyval.expression)->SetLineNumber(line_number);
    }
    else if((yyvsp[-2].symbol_token)->get_string() == "stoptime"){
        auto params = new std::vector<Expression>;
        params->push_back(new IntConst(line_number));
        Expression temp = new FuncRParams(params);
        (yyval.expression) = new Func_call(id_table.add_id("_sysy_stoptime"),temp);
        (yyval.expression)->SetLineNumber(line_number);
    }
    else{
        (yyval.expression) = new Func_call((yyvsp[-2].symbol_token),nullptr);
        (yyval.expression)->SetLineNumber(line_number);
    }
}
#line 2168 "SysY_parser.tab.c"
    break;

  case 102: /* UnaryExp: '+' UnaryExp  */
#line 428 "parser/SysY_parser.y"
             { (yyval.expression) = new UnaryExp_plus((yyvsp[0].expression));(yyval.expression)->SetLineNumber(line_number);}
#line 2174 "SysY_parser.tab.c"
    break;

  case 103: /* UnaryExp: '-' UnaryExp  */
#line 429 "parser/SysY_parser.y"
             {(yyval.expression) = new UnaryExp_neg((yyvsp[0].expression));(yyval.expression)->SetLineNumber(line_number);}
#line 2180 "SysY_parser.tab.c"
    break;

  case 104: /* UnaryExp: '!' UnaryExp  */
#line 430 "parser/SysY_parser.y"
             {(yyval.expression) = new UnaryExp_not((yyvsp[0].expression));(yyval.expression)->SetLineNumber(line_number);}
#line 2186 "SysY_parser.tab.c"
    break;

  case 105: /* FuncRParams: Exp_list  */
#line 433 "parser/SysY_parser.y"
         {(yyval.expression) = new FuncRParams((yyvsp[0].expressions));(yyval.expression)->SetLineNumber(line_number);}
#line 2192 "SysY_parser.tab.c"
    break;


#line 2196 "SysY_parser.tab.c"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
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
                      yytoken, &yylval, &yylloc);
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

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

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 435 "parser/SysY_parser.y"
 

void yyerror(char* s, ...)
{
    ++error_num;
    fout<<"parser error in line "<<line_number<<"\n";
}
