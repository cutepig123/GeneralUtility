rem @echo off
set File=%1
set Rule=%2

path %path%;F:\Program Files\Java\jdk1.7.0_07\bin
SET CLASSPATH=.;F:\wamp\www\git_repos\GitHub\GeneralUtility\ANTLRTest\antlr-4.4-complete.jar;%CLASSPATH%

md tmp
cd tmp

echo Select Choice:
echo 1: Compile
echo 2: Test
set /p Choice=

if %Choice%==1 (
copy ..\%File%.g4 . /y
java org.antlr.v4.Tool %File%.g4
javac %File%*.java
)

if %Choice%==2 (
rem java org.antlr.v4.runtime.misc.TestRig  %File% r -tree
java org.antlr.v4.runtime.misc.TestRig %File% %Rule% -gui <..\in.txt
)

pause
