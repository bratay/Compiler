%{
#include <string>
#include <limits.h>

/* Get our custom yyFlexScanner subclass */
#include "scanner.hpp"
#include "tokens.hpp"
#undef  YY_DECL
#define YY_DECL int holyc::Scanner::yylex( holyc::Parser::semantic_type * const lval )

/* define yyterminate as this instead of NULL */
#define yyterminate() return( TokenKind::END )

/* Exclude unistd.h for Visual Studio compatability. */
#define YY_NO_UNISTD_H

/* typedef to make the reference to token kinds shorter */
using TokenKind = holyc::Parser::token;

%}

%option debug
%option nodefault
%option yyclass="holyc::Scanner"
%option noyywrap
%option c++

DIGIT [0-9]
ID    [a-z][a-z0-9]*

%%
%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}

"+"		{
		// Keep track of the "kind" of token we matched. In this case,
		// it's a CROSS (aka the plus symbol). The full list of token
		// kinds is declared around line 197 of grammar.hh. The testing
		// functionScanner::outputTokens also indicates what each token
		// represents (declared in scanner.cpp line 9)
		int tokenKind = TokenKind::CROSS;

		//Instatiate a token. There are 4 classes of tokens defined based on 
		// - a literal string (StringLitToken)
		// - a literal integer (IntLitToken)
		// - an identifier (IDToken)
		// - a base Token (any other type)
		// The token object is a handy place to keep information about the
		// token (i.e. token metadata) such as the line and column on which
		// the token character started (which can be used later in debug 
		// messages, etc). 
		Token * token = new Token(lineNum, colNum, tokenKind);

		//Assign the token to a instance variable of the scanner.
		// (When Flex processes a .l file in C++ mode, it creates 
		// a class called Scanner, which is a subclass of yyFlexScanner)
		// By assigning a value to this field, we put the token
		// metadata in a place where scanner's client program can find it.
		this->yylval->tokenValue = token;

		//Since we consumed 1 character, increase the column number by 1
		colNum++;

		//Let the scanner's client program know that a CROSS token
		// has been matched
		return tokenKind;
		}

"-" 	{
		//Essentially the same thing as CROSS, but without the verbose
		// comments so it doesn't look so scary.
		int tokenKind = TokenKind::DASH;
		this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
		colNum++;
		return tokenKind;
		}

"--" 	{
		int tokenKind = TokenKind::DASHDASH;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum += 2;

		return tokenKind;
		}

"*" 	{
		int tokenKind = TokenKind::STAR;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum++;

		return tokenKind;
		}	

"/" 	{
		int tokenKind = TokenKind::SLASH;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum++;

		return tokenKind;
		}

"@" 	{
		int tokenKind = TokenKind::AT;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum++;

		return tokenKind;
		}

"^" 	{
		int tokenKind = TokenKind::CARAT;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum++;

		return tokenKind;
		}

"[" 	{
		int tokenKind = TokenKind::LBRACE;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum++;

		return tokenKind;
		}

"]" 	{
		int tokenKind = TokenKind::RBRACE;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum++;

		return tokenKind;
		}

"{"   	{
		int tokenKind = TokenKind::LCURLY;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum++;

		return tokenKind;
		}

"}" 	{
		int tokenKind = TokenKind::RCURLY;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum++;

		return tokenKind;
		}

"(" 	{
		int tokenKind = TokenKind::LPAREN;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum++;

		return tokenKind;
		}

")" 	{
		int tokenKind = TokenKind::RPAREN;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum++;

		return tokenKind;
		}

";" 	{
		int tokenKind = TokenKind::SEMICOLON;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum++;

		return tokenKind;
		}

"," 	{
		int tokenKind = TokenKind::COMMA;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum++;

		return tokenKind;
		}

"++" 	{

		int tokenKind = TokenKind::CROSSCROSS;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum += 2;

		return tokenKind;
		}

"!" 	{
		int tokenKind = TokenKind::NOT;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum++;

		return tokenKind;
		}

"==" 	{
		int tokenKind = TokenKind::EQUALS;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum += 2;

		return tokenKind;
		}

"!=" 	{
		int tokenKind = TokenKind::NOTEQUALS;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum += 2;

		return tokenKind;
		}

">" 	{
		int tokenKind = TokenKind::GREATER;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum++;

		return tokenKind;
		}

"<" 	{
		int tokenKind = TokenKind::LESS;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum++;

		return tokenKind;
		}

"&" 	{
		int tokenKind = TokenKind::AMP;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum++;

		return tokenKind;
		}

"<=" 	{
		int tokenKind = TokenKind::LESSEQ;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum += 2;

		return tokenKind;
		}

">=" 	{
		int tokenKind = TokenKind::GREATEREQ;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum += 2;

		return tokenKind;
		}

"=" 	{
		int tokenKind = TokenKind::ASSIGN;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum++;

		return tokenKind;
		}

"&&" 	{
		int tokenKind = TokenKind::AND;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum += 2;

		return tokenKind;
		}

"||" 	{
		int tokenKind = TokenKind::OR;
		Token * token = new Token(lineNum, colNum, tokenKind);
		this->yylval->tokenValue = token;
		colNum += 2;

		return tokenKind;
		}









\\ {
	colNum++;
	}

\n	{
		//Update the line number, reset the columnNumber
		//Token * token  = new StrToken(lineNum, colNum, yytext);
		//this->yylval->tokenValue = token;
		lineNum++;
		colNum = 0;

		// return TokenKind::STRLITERAL;
		}

\t	{
		// Token * token  = new StrToken(lineNum, colNum, yytext);
		// this->yylval->tokenValue = token;
		colNum += 5;

		// return TokenKind::STRLITERAL;
		}

\' 	{
		//Single quote
		// Token * token  = new StrToken(lineNum, colNum, yytext);
		// this->yylval->tokenValue = token;
		colNum++;

		// return TokenKind::STRLITERAL;
		}

\" 	{
		//Double quote
		// this->yylval->tokenValue = new StrToken(lineNum, colNum, yytext);
		colNum++;

		// return TokenKind::STRLITERAL;
		}

\\	{
			colNum++;
		}
















" " {
	colNum++;
	}


"'a"	{
		//What does `a do?
		colNum++;

		return TokenKind::CHARLIT;	
		}

'\t  {
		colNum += 4;

		return TokenKind::CHARLIT;	
		}

'\"	{
		colNum++;

		return TokenKind::CHARLIT;	
		}

'\n	{
		lineNum++;
		colNum = 0;

		return TokenKind::CHARLIT;
		}

'\  {
		// Tab
		colNum++;

		return TokenKind::CHARLIT;
		}

















{ID}	{
			int tokenKind = TokenKind::ID;
			Token * token  = new IDToken(lineNum, colNum, yytext);
			this->yylval->tokenValue = token;
			colNum += strlen(yytext);

			return tokenKind;
		}

{DIGIT}+	{
			//TODO: The following computation of the integer
			// value does not check for overflow. It should do so.
			int intVal = atoi(yytext);

			if( intVal > INT_MAX ){
				std::string msg = "Integer overflow";
				error(lineNum, colNum, msg);
			}

			//Note that the IntLitToken does not take a TokenKind argument
			// since it's implicitly known that the token represents an
			// INTLITERAL		
			yylval->tokenValue = new IntLitToken(lineNum, colNum, intVal);

			//TODO: update the colNum count according to the number of
			// characters consumed. 

			colNum += strlen(yytext);

			//As before, alert the client program that an INTLITERAL 
			// token was matched
			return TokenKind::INTLITERAL;
		}

["][\n|\t|\'|\"|\\|	|a-zA-Z0-9|!|@|#|$|%|^|&|*]*["] {
	int tokenKind = TokenKind::STRLITERAL;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += strlen(yytext);
	return tokenKind;
}






"int" {
	int tokenKind = TokenKind::INT;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += strlen(yytext);
	return tokenKind;
}
"intptr" {
	int tokenKind = TokenKind::INTPTR;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += strlen(yytext);
	return tokenKind;
}

"bool" {
	int tokenKind = TokenKind::BOOL;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += strlen(yytext);
	return tokenKind;
}

"char" {
	int tokenKind = TokenKind::CHAR;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += strlen(yytext);
	return tokenKind;
}

"charptr" {
	int tokenKind = TokenKind::CHARPTR;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += strlen(yytext);
	return tokenKind;
}

"void" {
	int tokenKind = TokenKind::VOID;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += strlen(yytext);
	return tokenKind;
}

"if" {
	int tokenKind = TokenKind::IF;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += strlen(yytext);
	return tokenKind;
}

"else" {
	int tokenKind = TokenKind::ELSE;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += strlen(yytext);
	return tokenKind;
}

"while" {
	int tokenKind = TokenKind::WHILE;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += strlen(yytext);
	return tokenKind;
}

"return" {
	int tokenKind = TokenKind::RETURN;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += strlen(yytext);
	return tokenKind;
}

"true" {
	int tokenKind = TokenKind::TRUE;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += strlen(yytext);
	return tokenKind;
}

"false" {
	int tokenKind = TokenKind::FALSE;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += strlen(yytext);
	return tokenKind;
}

"FROMCONSOLE" {
	int tokenKind = TokenKind::FROMCONSOLE;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += strlen(yytext);
	return tokenKind;
}

"TOCONSOLE" {
	int tokenKind = TokenKind::TOCONSOLE;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += strlen(yytext);
	return tokenKind;
}

"NULLPTR" {
	int tokenKind = TokenKind::NULLPTR;
	this->yylval->tokenValue = new Token(lineNum, colNum, tokenKind);
	colNum += strlen(yytext);
	return tokenKind;
}







` {
    std::string msg = "Illegal Character";
    // Report::fatal(lineNum, colNum, msg);
    error(lineNum, colNum, msg);
    colNum += yyleng;
}

'"\\ " {
    std::string msg = "Empty char literal escape sequence";
    error(lineNum, colNum, msg);
    colNum += yyleng;
}

'"\\ " {
    std::string msg = "Bad escape sequence";
    error(lineNum, colNum, msg);
    colNum += yyleng;
}

["][\n|\t|\'|\"|\\| |a-zA-Z0-9|!|@|#|$|%|^|&|*]* {
	std::string msg = "Unterminated string literals";
    error(lineNum, colNum, msg);
    colNum += yyleng;
}



["] {
    std::string msg = "Unterminated string literals";
    error(lineNum, colNum, msg);
    colNum += yyleng;
}

.  {
		//TODO: Add the rest of the rules above this point so that legal characters
		// are properly accounted for.
		// Illegal character	
		std::string msg = "Illegal character ";
		msg += yytext;
		error(lineNum,colNum,msg);
		colNum += yyleng;
    }
%%
