========================================================================
    CONSOLE APPLICATION : TestTLS Project Overview
========================================================================

Ways to define a TLS variable

1)__declspec( thread ) declarator
http://msdn.microsoft.com/en-us/library/9w1sdazb.aspx

data is shared among all threads of a given process

2) TlsAlloc  TlsSetValue  TlsGetValue  TlsFree

TlsAlloc Just return TlsIndex, it can only contain 4byte data, to store more data, user need malloc by himself and store in the tlsindex