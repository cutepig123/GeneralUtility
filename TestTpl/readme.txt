����


��ΰ�װtpl��
https://www.nuget.org/packages/Microsoft.Tpl.Dataflow/
 To install Microsoft TPL Dataflow, run the following command in the Package Manager Console

PM> Install-Package Microsoft.Tpl.Dataflow
��װ���package�ڵ�ǰ��Ŀ����

ѧϰ����
����ʵ�� http://www.codeguru.com/csharp/article.php/c18601/NET-Framework-Task-Parallel-Library-Dataflow.htm
msdn�ο� http://msdn.microsoft.com/en-us/library/dd537608%28v=vs.110%29.aspx
http://msdn.microsoft.com/en-us/library/hh228603%28v=vs.110%29.aspx

����
actor model
Task Parallel Library (TPL) 
Task Parallel Library Dataflow (TDF)
"Blocks"�� TDF is composed of a set of "Blocks". Blocks receive, send, or both receive and send messages to other Blocks
	ActionBlock			һ��һ��
	BroadcastBlock		0��һ��
	TransformBlock		һ��һ��
	BufferBlock			һ��һ��
	JoinBlock			2��һ��
	LinkTo ������block������
	Post ��ĳһ��block����Ϣ
	Task.Factory.StartNew	���߳ǳ�������
Messages �� TDF Messages are instances of the class the particular block is configured to interact with. Messages are stored in TPL data structures. 
TaskScheduler�� I think of task classes as chunks of work. Blocks run tasks on a given TaskScheduler. Each Block exhibits different behavior in how it dispatches to the TaskScheduler and/or how it handles messages.

