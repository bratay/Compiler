%skeleton "lalr1.cc"
%require "3.0"
%debug
%defines
%define api.namespace {holyc}
 /*
 If your bison install has trouble with the 
 line %define api.parser.class {Parser} try
 using the older %define parser_class_name {Parser}
 instead
 */
%define api.parser.class {Parser}
%define parse.assert
%define parse.error verbose
%output "parser.cc"
%token-table

%code requires{
	#include <list>
	#include "tokens.hpp"
	namespace holyc {
		class Scanner;
	}

//The following definition is required when 
// we don't use the %locations directive (which we won't)
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

//End "requires" code
}

%parse-param { holyc::Scanner &scanner }

%code{
   // C std code for utility functions
   #include <iostream>
   #include <cstdlib>
   #include <fstream>

   // Our code for interoperation between scanner/parser
   #include "scanner.hpp"
   //#include "tokens.hpp"

  //Request tokens from our scanner member, not 
  // from a global function
  #undef yylex
  #define yylex scanner.yylex
}

/*
The %union directive is a way to specify the 
set of possible types that might be used as
translation attributes that a symbol might take.
For this project, only terminals have types (we'll
have translation attributes for non-terminals in the next
project)
*/
%union {
   holyc::Token*                         transToken;
}

%token                   END	   0 "end file"
%token	<transToken>     AND
%token	<transToken>     AT
%token	<transToken>     ASSIGN
%token	<transToken>     BOOL
%token	<transToken>     BOOLPTR
%token	<transToken>     CARAT
%token	<transToken>     CHAR
%token	<transCharToken> CHARLIT
%token	<transToken>     CHARPTR
%token	<transToken>     COMMA
%token	<transToken>     CROSS
%token	<transToken>     CROSSCROSS
%token	<transToken>     DASH
%token	<transToken>     DASHDASH
%token	<transToken>     ELSE
%token	<transToken>     EQUALS
%token	<transToken>     FALSE
%token	<transToken>     FROMCONSOLE
%token	<transIDToken>   ID
%token	<transToken>     IF
%token	<transToken>     INT
%token	<transIntToken>  INTLITERAL
%token	<transToken>     INTPTR
%token	<transToken>     GREATER
%token	<transToken>     GREATEREQ
%token	<transToken>     LBRACE
%token	<transToken>     LCURLY
%token	<transToken>     LESS
%token	<transToken>     LESSEQ
%token	<transToken>     LPAREN
%token	<transToken>     NOT
%token	<transToken>     NOTEQUALS
%token	<transToken>     NULLPTR
%token	<transToken>     OR
%token	<transToken>     RBRACE
%token	<transToken>     RCURLY
%token	<transToken>     RETURN
%token	<transToken>     RPAREN
%token	<transToken>     SEMICOLON
%token	<transToken>     SLASH
%token	<transToken>     STAR
%token	<transStrToken>  STRLITERAL
%token	<transToken>     TOCONSOLE
%token	<transToken>     TRUE
%token	<transToken>     VOID
%token	<transToken>     WHILE

/* NOTE: Make sure to add precedence and associativity 
 * declarations
*/




%left         NOT
%left         STAR SLASH
%left         CROSS DASH
%nonassoc     LESS LESSEQ EQUALS NOTEQUALS GREATER GREATEREQ
%left         AND
%left         OR
%right        ASSIGN
%precedence   NEG



%%

/* TODO: add productions for the other nonterminals in the 
   grammar and make sure that all of the productions of the 
   given nonterminals are complete
*/
program : globals





globals : globals decl
		    | /* epsilon */




decl : varDecl SEMICOLON
     | fnDecl



varDecl : type id



type : INT
     | BOOL
     | CHAR
     | INTPTR
     | CHARPTR
     | VOID
     | BOOLPTR


formals : LPAREN RPAREN
        | LPAREN formalsList RPAREN


fnDecl : type id formals fnBody


formalsList : formalDecl
            | formalDecl COMMA formalsList




formalDecl : type id


fnBody : LCURLY stmtList RCURLY



stmt : varDecl SEMICOLON
     | FROMCONSOLE lval SEMICOLON
     | assignExp SEMICOLON
     | lval CROSSCROSS SEMICOLON
     | lval DASHDASH SEMICOLON
     | TOCONSOLE exp SEMICOLON
     | IF LPAREN exp RPAREN LCURLY stmtList RCURLY ELSE LCURLY stmtList RCURLY
     | RETURN SEMICOLON
     | WHILE LPAREN exp RPAREN LCURLY stmtList RCURLY
     | RETURN exp SEMICOLON
     | IF LPAREN exp RPAREN LCURLY stmtList RCURLY
     | fncall SEMICOLON

assignExp : lval ASSIGN exp

fncall : id LPAREN RPAREN             // function call with no args
       | id LPAREN actualList RPAREN  // function call with args

actualList : exp
           | actualList COMMA exp

exp : assignExp
    | exp DASH exp
    | exp STAR exp
    | exp NOTEQUALS exp
    | exp CROSS exp
    | exp OR exp
    | exp AND exp
    | exp LESS exp
    | exp EQUALS exp
    | exp SLASH exp
    | NOT exp
    | DASH term
    | exp GREATER exp
    | exp LESSEQ exp
    | exp GREATEREQ exp
    | term




stmtList : stmtList stmt
         | /* epsilon */




lval : id
     | CARAT id
     | AT id
     | id LBRACE exp RBRACE




term : lval
     | LPAREN exp RPAREN
     | INTLITERAL
     | STRLITERAL
     | FALSE
     | TRUE
     | NULLPTR
     | CHARLIT
     | fncall



id : ID


%%

void holyc::Parser::error(const std::string& err_message){
   /* For project grading, only report "syntax error"
      if a program has bad syntax. However, you will
      probably want better output for debugging. Thus,
      this error function prints a verbose message to 
      stdout, but only prints "syntax error" to stderr
   */
	std::cout << err_message << std::endl;
	std::cerr << "syntax error" << std::endl;
}
