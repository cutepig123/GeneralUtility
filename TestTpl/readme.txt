======介绍

======如何安装tpl？
https://www.nuget.org/packages/Microsoft.Tpl.Dataflow/
 To install Microsoft TPL Dataflow, run the following command in the Package Manager Console

PM> Install-Package Microsoft.Tpl.Dataflow
安装后的package在当前项目下面

======学习资料
入门实例 http://www.codeguru.com/csharp/article.php/c18601/NET-Framework-Task-Parallel-Library-Dataflow.htm
msdn参考 http://msdn.microsoft.com/en-us/library/dd537608%28v=vs.110%29.aspx
http://msdn.microsoft.com/en-us/library/hh228603%28v=vs.110%29.aspx

======名词
actor model
	An actor is an entity that you can send messages to. In response to a message an actor can do any of the following:
* Send messages to other actors
* Create new actors
* Set behaviour for the next message it receives, (that is, change its internal state)

Task Parallel Library (TPL) 
Task Parallel Library Dataflow (TDF)
"Blocks"： TDF is composed of a set of "Blocks". Blocks receive, send, or both receive and send messages to other Blocks
	ActionBlock			一进0出
	BroadcastBlock		0进一出
	TransformBlock		一进一出
	BufferBlock			一进一出
	JoinBlock			2进一出
	LinkTo 把两个block连起来
	Post 向某一个block发消息
	Task.Factory.StartNew	在线城池里运行
Messages ： TDF Messages are instances of the class the particular block is configured to interact with. Messages are stored in TPL data structures. 
TaskScheduler： I think of task classes as chunks of work. Blocks run tasks on a given TaskScheduler. Each Block exhibits different behavior in how it dispatches to the TaskScheduler and/or how it handles messages.
Complete method：All DataflowBlocks have a Complete method that will prevent additional data from being received。一旦标记为完成，就不会继续接受新的消息了
tpl之本质：
	each Dataflow block component is technically running code on the thread pool at one time or another throughout the course of the pipeline
	框架应该包括，线程池，消息处理，并行化分析引擎（确定要将哪些task放到线程池里面）
	
======algmodule设计方面，可以参考tpl的地方以及不足
可参考的地方：tpl内部如何并行化的？ 要不要以同样地通过发消息的方式向模块发消息？（貌似不需要）
可以很容易的cancel task的运行（貌似不需要）
不足：如何实现几个block组合成新的block？这个新的block有几个输入几个输出？这种用法tpl貌似没有提供

======todo：
tpl内部如何并行化的？有啥算法可以参考吗
搜到几个链接：
	Data-flow analysis - Wikipedia, the free encyclopedia
	Lecture 2 Introduction to Data Flow Analysis - Suif
消息如何传递的？传值还是指针？	

