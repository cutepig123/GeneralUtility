F:\wamp\www\git_repos\GitHub\GeneralUtility\FileMerger\Debug>FileMerger.exe -mode append -src TestData0.txt -file Test.dat
Open Test.dat for append
File Sz 0
Read header Num File 0
Append File [0] Header Offset 12 Name TestData0.txt Size 1736

F:\wamp\www\git_repos\GitHub\GeneralUtility\FileMerger\Debug>FileMerger.exe -mode append -src TestData1.txt -file Test.dat
Open Test.dat for append
File Sz 2012
Read header Num File 1
File [0] Header Offset 12 Name TestData0.txt Size 1736
Append File [1] Header Offset 2012 Name TestData1.txt Size 11

F:\wamp\www\git_repos\GitHub\GeneralUtility\FileMerger\Debug>FileMerger.exe -mode getlist -file Test.dat
Open Test.dat for read
File Sz 2287
Read header Num File 2
File [0] Header Offset 12 Name TestData0.txt Size 1736
File [1] Header Offset 2012 Name TestData1.txt Size 11

F:\wamp\www\git_repos\GitHub\GeneralUtility\FileMerger\Debug>FileMerger.exe -mode getfile -idx 0 -des get0.txt -file Test.dat
Open Test.dat for read
File Sz 2287
Read header Num File 2
File [0] Header Offset 12 Name TestData0.txt Size 1736
Extract to Dest file get0.txt
File [1] Header Offset 2012 Name TestData1.txt Size 11

F:\wamp\www\git_repos\GitHub\GeneralUtility\FileMerger\Debug>FileMerger.exe -mode getfile -idx 1 -des get1.txt -file Test.dat
Open Test.dat for read
File Sz 2287
Read header Num File 2
File [0] Header Offset 12 Name TestData0.txt Size 1736
File [1] Header Offset 2012 Name TestData1.txt Size 11
Extract to Dest file get1.txt

F:\wamp\www\git_repos\GitHub\GeneralUtility\FileMerger\Debug>
