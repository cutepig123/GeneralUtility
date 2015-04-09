grammar SimpleCalc;
 
options
{
    language=C;
}
 
tokens
{
    PLUS    = '+' ;
    MINUS   = '-' ;
    MULT    = '*' ;
    DIV = '/' ;
}
 
@members
{
 
 #include "SimpleCalcLexer.h"
 
 int main(int argc, char * argv[])
 {
 
    pANTLR3_INPUT_STREAM           input;
    pSimpleCalcLexer               lex;
    pANTLR3_COMMON_TOKEN_STREAM    tokens;
    pSimpleCalcParser              parser;
 
    input  = antlr3AsciiFileStreamNew          ((pANTLR3_UINT8)argv[1]);
    lex    = SimpleCalcLexerNew                (input);
    tokens = antlr3CommonTokenStreamSourceNew  (ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
    parser = SimpleCalcParserNew               (tokens);
 
    parser  ->expr(parser);
 
    // Must manually clean up
    //
    parser ->free(parser);
    tokens ->free(tokens);
    lex    ->free(lex);
    input  ->close(input);
 
    return 0;
 }
 
}
 
/*------------------------------------------------------------------
 * PARSER RULES
 *------------------------------------------------------------------*/
 
expr    : term   ( ( PLUS | MINUS )  term   )*
        ;
 
term    : factor ( ( MULT | DIV   )  factor )*
        ;
 
factor  : NUMBER
        ;
 
 
/*------------------------------------------------------------------
 * LEXER RULES
 *------------------------------------------------------------------*/
 
NUMBER      : (DIGIT)+
            ;
 
WHITESPACE  : ( '\t' | ' ' | '\r' | '\n'| '\u000C' )+
              {
                 $channel = HIDDEN;
              }
            ;
 
fragment
DIGIT       : '0'..'9'
            ;