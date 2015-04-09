@echo off
set File=%1
set Rule=%2

path %path%;F:\Program Files\Java\jdk1.7.0_07\bin
SET CLASSPATH=.;F:\wamp\www\git_repos\GitHub\GeneralUtility\ANTLR3Test\antlr-3.5.2-complete.jar;%CLASSPATH%

md tmp
cd tmp

echo Select Choice:
echo 1: Compile
echo 2: Test
echo 3: Cmd
set /p Choice=

if %Choice%==1 (
copy ..\%File%.g . /y
java org.antlr.Tool %File%.g
javac %File%*.java
)

if %Choice%==2 (
rem java org.antlr.runtime.misc.TestRig  %File% r -tree
java org.antlr.runtime.misc.TestRig %File% %Rule% -gui <..\in.txt
)

if %Choice%==3 (
cmd
)
pause
