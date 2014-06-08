-module(geometry).
-export([area/1, main/0]).
area({rectangle,Width,Hit}) -> Width*Hit;
area({circle,R}) -> 3.14159*R*R.

main() ->area({circle,100}).
