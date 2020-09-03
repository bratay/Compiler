%option c++
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

////////////////////////////////////////////////////////////////////
//Symbol Operators
////////////////////////////////////////////////////////////////////

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
		int tokenKind = TokenKind::STRBRACEAR;
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

		return tokenKind
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
		int tokenKind = TokenKind::AND;
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
		int tokenKind = TokenKind::STAR;
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

////////////////////////////////////////////////////////////////////
//String Literals
////////////////////////////////////////////////////////////////////
"\n"		{
		//Update the line number, reset the columnNumber
		Token * token  = new StrToken(lineNum, colNum, yylval);
		this->yylval->tokenValue = token;
		lineNum++;
		colNum = 0;

		return TokenKind::STRLITERAL;
		}

"\t"	{
		Token * token  = new StrToken(lineNum, colNum, yylval);
		this->yylval->tokenValue = token;
		colNum += 5;

		return TokenKind::STRLITERAL;
		}

"\'" 	{
		//Single quote
		Token * token  = new StrToken(lineNum, colNum, yylval);
		this->yylval->tokenValue = token;
		colNum++;

		return TokenKind::STRLITERAL;
		}

"\"" 	{
		//Double quote
		Token * token  = new StrToken(lineNum, colNum, yylval);
		this->yylval->tokenValue = token;
		colNum++;

		return TokenKind::STRLITERAL;
		}

"\\"	{
			Token * token  = new StrToken(lineNum, colNum, yylval);
			this->yylval->tokenValue = token;
			colNum++;

			return TokenKind::STRLITERAL;
		}


////////////////////////////////////////////////////////////////////
//Char Literals
////////////////////////////////////////////////////////////////////
"'a"	{
		Token * token  = new Token(lineNum, colNum, yylval);
		this->yylval->tokenValue = token;

		//What does `a do?
		colNum++;

		return TokenKind::CHARLIT;	
		}

"'\t" |	"'\		" 
		{
		Token * token  = new Token(lineNum, colNum, yylval);
		this->yylval->tokenValue = token;
		colNum += 4;

		return TokenKind::CHARLIT;	
		}

"'\\"	{
		Token * token  = new Token(lineNum, colNum, yylval);
		this->yylval->tokenValue = token;
		colNum++;

		return TokenKind::CHARLIT;	
		}

"'\n"	{
		Token * token  = new Token(lineNum, colNum, yylval);
		this->yylval->tokenValue = token;
		lineNu++;
		colNum = 0;

		return TokenKind::CHARLIT;
		}

"'\ "	{
		Token * token  = new Token(lineNum, colNum, yylval);
		this->yylval->tokenValue = token;
		colNum++;

		return TokenKind::CHARLIT;
		}

////////////////////////////////////////////////////////////////////
// Identifier
////////////////////////////////////////////////////////////////////

{ID}	{
			int tokenKind = TokenKind::ID;
			Token * token  = new IDToken(lineNum, colNum, yylval);
			this->yylval->tokenValue = token;
			colNum += yylval.length();

			return tokenKind;
		}

{DIGIT}+	
		{
			//TODO: The following computation of the integer
			// value does not check for overflow. It should do so.
			int intVal = atoi(yytext);


			//Note that the IntLitToken does not take a TokenKind argument
			// since it's implicitly known that the token represents an
			// INTLITERAL		
			yylval->tokenValue = new IntLitToken(lineNum, colNum, intVal);

			//TODO: update the colNum count according to the number of
			// characters consumed. 

			colNum += s = to_string(intVal).length();

			//As before, alert the client program that an INTLITERAL 
			// token was matched
			return TokenKind::INTLITERAL;
		}

////////////////////////////////////////////////////////////////////
//Illegal character
////////////////////////////////////////////////////////////////////
.   {
		//TODO: Add the rest of the rules above this point so that legal characters
		// are properly accounted for. 
		std::string msg = "Illegal character ";
		msg += yytext;
		error(lineNum,colNum,msg);
		colNum += yyleng;
    }
%%
