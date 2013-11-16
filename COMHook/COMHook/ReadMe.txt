========================================================================
    控制台应用程序：COMHook 项目概述
========================================================================


本程序实现了
COMHook.cpp
- 如何调用COM来调用IDisplatch接口
- 如何通过修改vtable指针项目来实现comhook （HookMethod函数）
- 如何编写hook的callback函数
- typedef function pointer
TestVirtualFtn.cpp
- 测试是否同一类型的虚函数表指针都一样

todo
- 如何确定field offset？

note
- 需要关闭编译器的堆栈检查功能， 否则会出错


