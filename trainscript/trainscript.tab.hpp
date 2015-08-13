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

#ifndef YY_YY_TRAINSCRIPT_TRAINSCRIPT_TAB_HPP_INCLUDED
# define YY_YY_TRAINSCRIPT_TRAINSCRIPT_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TAB = 258,
    TYPENAME = 259,
    SEMICOLON = 260,
    COLON = 261,
    COMMA = 262,
    PIPE = 263,
    PLUS = 264,
    MINUS = 265,
    MULTIPLY = 266,
    DIVIDE = 267,
    MODULO = 268,
    LBRACKET = 269,
    RBRACKET = 270,
    RARROW = 271,
    LARROW = 272,
    OP_LT = 273,
    OP_LE = 274,
    OP_GT = 275,
    OP_GE = 276,
    OP_EQ = 277,
    OP_NEQ = 278,
    REAL = 279,
    INT = 280,
    IDENTIFIER = 281,
    KW_PUB = 282,
    KW_PRI = 283,
    KW_VAR = 284,
    KW_PTR = 285,
    KW_VOID = 286,
    KW_INT = 287,
    KW_REAL = 288,
    KW_TEXT = 289,
    KW_BOOL = 290,
    KW_BEGIN = 291,
    KW_END = 292,
    KW_IF = 293,
    KW_THEN = 294,
    KW_ELSE = 295,
    KW_ELSEIF = 296,
    KW_REPEAT = 297,
    KW_FROM = 298,
    KW_TO = 299,
    KW_UNTIL = 300,
    KW_WHILE = 301,
    KW_DO = 302
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 44 "trainscript/trainscript.y" /* yacc.c:1909  */

    float fval;
	int ival;
	char *text;
	int indentation;
	trainscript::Type type;
	VariableDeclaration varDecl;
	MethodDeclaration method;
	MethodBody *body;
	MethodHeader methodHeader;
	trainscript::Instruction *instruction;
	LocalVariable *local;
	ExpressionList *expressions;

#line 117 "trainscript/trainscript.tab.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (ParserData * context);

#endif /* !YY_YY_TRAINSCRIPT_TRAINSCRIPT_TAB_HPP_INCLUDED  */
