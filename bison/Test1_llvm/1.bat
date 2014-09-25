""F:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"" x86

path F:\wamp\www\git_repos\GitHub\GeneralUtility\bison\bison-2.4.1-bin\bin;%path%

flex -otokens.cpp <tokens.l
pause

bison -d -o parser.cpp parser.y
pause


cl /I. /IF:\llvm\llvm-3.5.0.src\llvm-3.5.0.src\tmp\include /IF:\llvm\llvm-3.5.0.src\llvm-3.5.0.src\include /link F:\llvm\llvm-3.5.0.src\llvm-3.5.0.src\tmp\Debug\lib\LLVMCore.lib parser.cpp tokens.cpp main.cpp
pause
