grammar ASTDemo;

parse
  :  orExp
  ;

orExp
  :  andExp ('||' andExp)*
	// Make `||` root
  ;

andExp
  :  addsubExp ('&&'  addsubExp)* // Make `&&` root
  ;
  
addsubExp
	: mulDivExp ('+'  mulDivExp)*
	| mulDivExp ('-'  mulDivExp)*
	;

mulDivExp
	: compareExp ('*'  compareExp)*
	| compareExp ('/'  compareExp)*
	;
	
compareExp
	: atom ('>'  atom)*
	| atom ('>='  atom)*
	| atom ('<='  atom)*
	| atom ('<'  atom)*
	;
	
atom
  :  ID
  |  INT
  |  '(' parse ')'  // Just a single token, no need to do `^(...)`, 
                            // we're removing the parenthesis. Note that
                            // `'('! orExp ')'!` will do exactly the same.
  ;

ID  :   ('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*
    ;

INT :   '0'..'9'+
    ;  
	
// ignore white space characters
WS : [ \t\r\n]+ -> skip ;