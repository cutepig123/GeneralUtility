set File=SimpleCalc

path %path%;F:\Program Files\Java\jdk1.7.0_07\bin
SET CLASSPATH=.;F:\wamp\www\git_repos\GitHub\GeneralUtility\ANTLR3Test\antlr-3.5.2-complete.jar;%CLASSPATH%

md tmp
cd tmp

copy ..\%File%.g . /y
java org.antlr.Tool %File%.g
pause

javac Test.java ExprLexer.java ExprParser.java
pause

java Test <..\in.txt
pause
