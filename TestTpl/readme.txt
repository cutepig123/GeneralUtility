======����

======��ΰ�װtpl��
https://www.nuget.org/packages/Microsoft.Tpl.Dataflow/
 To install Microsoft TPL Dataflow, run the following command in the Package Manager Console

PM> Install-Package Microsoft.Tpl.Dataflow
��װ���package�ڵ�ǰ��Ŀ����

======ѧϰ����
����ʵ�� http://www.codeguru.com/csharp/article.php/c18601/NET-Framework-Task-Parallel-Library-Dataflow.htm
msdn�ο� http://msdn.microsoft.com/en-us/library/dd537608%28v=vs.110%29.aspx
http://msdn.microsoft.com/en-us/library/hh228603%28v=vs.110%29.aspx

======����
actor model
	An actor is an entity that you can send messages to. In response to a message an actor can do any of the following:
* Send messages to other actors
* Create new actors
* Set behaviour for the next message it receives, (that is, change its internal state)

Task Parallel Library (TPL) 
Task Parallel Library Dataflow (TDF)
"Blocks"�� TDF is composed of a set of "Blocks". Blocks receive, send, or both receive and send messages to other Blocks
	ActionBlock			һ��0��
	BroadcastBlock		0��һ��
	TransformBlock		һ��һ��
	BufferBlock			һ��һ��
	JoinBlock			2��һ��
	LinkTo ������block������
	Post ��ĳһ��block����Ϣ
	Task.Factory.StartNew	���߳ǳ�������
Messages �� TDF Messages are instances of the class the particular block is configured to interact with. Messages are stored in TPL data structures. 
TaskScheduler�� I think of task classes as chunks of work. Blocks run tasks on a given TaskScheduler. Each Block exhibits different behavior in how it dispatches to the TaskScheduler and/or how it handles messages.
Complete method��All DataflowBlocks have a Complete method that will prevent additional data from being received��һ�����Ϊ��ɣ��Ͳ�����������µ���Ϣ��
tpl֮���ʣ�
	each Dataflow block component is technically running code on the thread pool at one time or another throughout the course of the pipeline
	���Ӧ�ð������̳߳أ���Ϣ�������л��������棨ȷ��Ҫ����Щtask�ŵ��̳߳����棩
	
======algmodule��Ʒ��棬���Բο�tpl�ĵط��Լ�����
�ɲο��ĵط���tpl�ڲ���β��л��ģ� Ҫ��Ҫ��ͬ����ͨ������Ϣ�ķ�ʽ��ģ�鷢��Ϣ����ò�Ʋ���Ҫ��
���Ժ����׵�cancel task�����У�ò�Ʋ���Ҫ��
���㣺���ʵ�ּ���block��ϳ��µ�block������µ�block�м������뼸������������÷�tplò��û���ṩ

======todo��
tpl�ڲ���β��л��ģ���ɶ�㷨���Բο���
�ѵ��������ӣ�
	Data-flow analysis - Wikipedia, the free encyclopedia
	Lecture 2 Introduction to Data Flow Analysis - Suif
��Ϣ��δ��ݵģ���ֵ����ָ�룿	

