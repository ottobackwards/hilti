
%{
// Disable: scanner.ll:41:1: warning: expression result unused [-Wunused-value]
#pragma clang diagnostic ignored "-Wunused-value"

#include <string>

#include "parser/scanner.h"
#include "parser/driver.h"

#define yyterminate() return token::END

%}

%option c++
%option prefix="BinPAC"
%option noyywrap nounput batch debug yylineno

%x RE
%s IGNORE_NL

%{
#define YY_USER_ACTION yylloc->columns(yyleng);
%}

address4  ({digits}"."){3}{digits}
address6  \[("::")?({digits}"."){3}{digits}|({hexs}:){7}{hexs}|0x{hexs}({hexs}|:)*"::"({hexs}|:)*|(({digits}|:)({hexs}|:)*)?"::"({hexs}|:)*\]
attribute \&[a-zA-Z_][a-zA-Z_0-9]*
blank     [ \t]
comment   [ \t]*#[^\n]*
digits    [0-9]+
hexs      [0-9a-fA-F]+
id        [a-zA-Z_][a-zA-Z_0-9]*|[$][$]
int       [+-]?[0-9]+
property  %[a-zA-Z_][a-zA-Z_0-9]*
string    \"(\\.|[^\\"])*\"

%%
%{
    yylloc->step ();
%}

%{
    typedef binpac_parser::Parser::token token;
    typedef binpac_parser::Parser::token_type token_type;
%}

{blank}+              yylloc->step();
[\n]+                 yylloc->lines(yyleng);
{comment}             /* Eat. */

<RE>(\\.|[^\\\/])*    yylval->sval = yytext; return token::CREGEXP;
<RE>[/\\\n]	          return (token_type) yytext[0];

addr                  return token::ADDR;
any                   return token::ANY;
arrow                 return token::ARROW;
attribute             return token::ATTRIBUTE;
bitset                return token::BITSET;
bool                  return token::BOOL;
bytes                 return token::BYTES;
caddr                 return token::CADDR;
caddress              return token::CADDRESS;
cast                  return token::CAST;
catch                 return token::CATCH;
cbool                 return token::CBOOL;
cbytes                return token::CBYTES;
cdouble               return token::CDOUBLE;
cinteger              return token::CINTEGER;
const                 return token::CONST;
constant              return token::CONSTANT;
cport                 return token::CPORT;
cregexp               return token::CREGEXP;
cstring               return token::CSTRING;
debug                 return token::DEBUG_;
declare               return token::DECLARE;
double                return token::DOUBLE;
else                  return token::ELSE;
enum                  return token::ENUM;
exception             return token::EXCEPTION;
export                return token::EXPORT;
file                  return token::FILE;
for                   return token::FOR;
foreach               return token::FOREACH;
global                return token::GLOBAL;
ident                 return token::IDENT;
if                    return token::IF;
import                return token::IMPORT;
in                    return token::IN;
int                   return token::INT;
interval              return token::INTERVAL;
iter                  return token::ITER;
list                  return token::LIST;
local                 return token::LOCAL;
map                   return token::MAP;
mod                   return token::MOD;
module                return token::MODULE;
net                   return token::NET;
new                   return token::NEW;
on                    return token::ON;
port                  return token::PORT;
print                 return token::PRINT;
property              return token::PROPERTY;
regexp                return token::REGEXP;
return                return token::RETURN;
set                   return token::SET;
string                return token::STRING;
switch                return token::SWITCH;
time                  return token::TIME;
timer                 return token::TIMER;
try                   return token::TRY;
tuple                 return token::TUPLE;
type                  return token::TYPE;
unit                  return token::UNIT;
var                   return token::VAR;
vector                return token::VECTOR;
void                  return token::VOID;

!=                    return token::NEQ;
\&\&                  return token::AND;
\+=                   return token::PLUSASSIGN;
--                    return token::MINUSMINUS;
-=                    return token::MINUSASSIGN;
\<\<                  return token::SHIFTLEFT;
\<=                   return token::LEQ;
==                    return token::EQ;
\>=                   return token::GEQ;
\>\>                  return token::SHIFTRIGHT;
\?\.                  return token::HASATTR;
\*\*                  return token::POW;
\+\+                  return token::PLUSPLUS;
\|\|                  return token::OR;

False                 yylval->bval = 0; return token::CBOOL;
True                  yylval->bval = 1; return token::CBOOL;

{attribute}           yylval->sval = yytext; return token::ATTRIBUTE;
{property}            yylval->sval = yytext; return token::PROPERTY;
{digits}\/(tcp|udp)   yylval->sval = yytext; return token::CPORT;
{address4}            yylval->sval = yytext; return token::CADDRESS;
{address6}            yylval->sval = string(yytext, 1, strlen(yytext) - 1); return token::CADDRESS;

[-+]?{digits}\.{digits} yylval->dval = strtod(yytext, 0); return token::CDOUBLE;

{int}                 yylval->ival = strtoll(yytext, 0, 10); return token::CINTEGER;
{string}              yylval->sval = util::expandEscapes(string(yytext, 1, strlen(yytext) - 2)); return token::CSTRING;
b{string}             yylval->sval = util::expandEscapes(string(yytext, 2, strlen(yytext) - 3)); return token::CBYTES;

{id}                   yylval->sval = yytext; return token::IDENT;
{id}(::{id}){1,}       yylval->sval = yytext; return token::SCOPED_IDENT;
{id}(::{property}){1,} yylval->sval = yytext; return token::SCOPED_IDENT;

[,=:;<>(){}/|*/&^%!+-] return (token_type) yytext[0];

.                     driver.error("invalid character", *yylloc);

%%

int BinPACFlexLexer::yylex()
{
    assert(false); // Shouldn't be called.
    return 0;
}

void binpac_parser::Scanner::enablePatternMode()
{
    yy_push_state(RE);
}

void binpac_parser::Scanner::disablePatternMode()
{
    yy_pop_state();
}