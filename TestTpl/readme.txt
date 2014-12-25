介绍


如何安装tpl？
https://www.nuget.org/packages/Microsoft.Tpl.Dataflow/
 To install Microsoft TPL Dataflow, run the following command in the Package Manager Console

PM> Install-Package Microsoft.Tpl.Dataflow
安装后的package在当前项目下面

学习资料
入门实例 http://www.codeguru.com/csharp/article.php/c18601/NET-Framework-Task-Parallel-Library-Dataflow.htm
msdn参考 http://msdn.microsoft.com/en-us/library/dd537608%28v=vs.110%29.aspx
http://msdn.microsoft.com/en-us/library/hh228603%28v=vs.110%29.aspx

名词
actor model
Task Parallel Library (TPL) 
Task Parallel Library Dataflow (TDF)
"Blocks"： TDF is composed of a set of "Blocks". Blocks receive, send, or both receive and send messages to other Blocks
	ActionBlock			一进一出
	BroadcastBlock		0进一出
	TransformBlock		一进一出
	BufferBlock			一进一出
	JoinBlock			2进一出
	LinkTo 把两个block连起来
	Post 向某一个block发消息
	Task.Factory.StartNew	在线城池里运行
Messages ： TDF Messages are instances of the class the particular block is configured to interact with. Messages are stored in TPL data structures. 
TaskScheduler： I think of task classes as chunks of work. Blocks run tasks on a given TaskScheduler. Each Block exhibits different behavior in how it dispatches to the TaskScheduler and/or how it handles messages.

