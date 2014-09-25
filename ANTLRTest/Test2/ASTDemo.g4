grammar ASTDemo;

parse
  :  orExp
  ;

orExp
  :  andExp ('||' andExp)* // Make `||` root
  ;

andExp
  :  atom ('&&'  atom)* // Make `&&` root
  ;

atom
  :  'true'
  |  'false'
  |  '(' orExp ')'  // Just a single token, no need to do `^(...)`, 
                            // we're removing the parenthesis. Note that
                            // `'('! orExp ')'!` will do exactly the same.
  ;

// ignore white space characters
WS : [ \t\r\n]+ -> skip ;