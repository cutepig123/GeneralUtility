#pragma once
#include <typeinfo>
#include <memory>

class noncopyable{
protected:
	noncopyable() {}
	~noncopyable() {}
private:
	noncopyable(const noncopyable&);
	noncopyable& operator=(const noncopyable &);
};

class Handle{

public:
	Handle();
	Handle(const Handle&);
	Handle& operator=(const Handle&);
	~Handle();

	void swap(Handle &rhs);

	template <class RPY>
	RPY* getReply(){
		return (RPY*)getReply(typeid(RPY));
	}

	template <class RPY>
	const RPY* getReply() const {
		return (const RPY*)getReply(typeid(RPY));
	}

	bool isFinished() const;

	class Impl;
	struct Access;
	friend struct Access;
private:
	std::unique_ptr<Impl> impl;

	void *getReply(const std::type_info &);
};

class MyTaskBase{
public:
	virtual ~MyTaskBase() = 0{};
	virtual void operator()() = 0;
	virtual void* getReply() = 0;
};

template <class CMD, class RPY>
class MyTask:public MyTaskBase{
public:
	typedef	void(*F)(CMD const *, RPY *);
private:
	CMD m_cmd;
	RPY m_rpy;
	F m_f;
public:
	MyTask(F f, CMD const &cmd)
		:m_cmd(cmd), m_f(f)
	{
	}
	virtual void operator()() {
		m_f(&m_cmd, &m_rpy);
	}
	virtual void* getReply() {
		return &m_rpy;
	}
};


template <class RPY>
class MyTask2 :public MyTaskBase{
public:
	typedef	void(*F)(RPY *);
private:
	RPY m_rpy;
	F m_f;
public:
	MyTask2(F f)
		:m_f(f)
	{
	}
	virtual void operator()() {
		m_f(&m_rpy);
	}
	virtual void* getReply() {
		return &m_rpy;
	}
};

class MyTaskScheduler :noncopyable{
	Handle	RunAsynTaskImpl(std::auto_ptr<MyTaskBase> p, const std::type_info &, Handle *pstPrevReply, const char *desc);
	class Impl;
	Impl *impl;
public:
	explicit MyTaskScheduler(int nThread/*, int const aSid[]*/);	// a task and it's dependency task must run at same thread
	~MyTaskScheduler();

	template <class CMD, class RPY>
	Handle	RunAsynTask(void (*pstF)(CMD const *, RPY *), CMD const &stCmd, Handle *pstPrevReply = NULL, const char *desc =NULL){
		return RunAsynTaskImpl(std::auto_ptr<MyTaskBase>(new MyTask<CMD, RPY>(pstF, stCmd)), typeid(RPY), pstPrevReply, desc);
	}

	template <class RPY>
	Handle	RunAsynTask(void(*pstF)(RPY *), Handle *pstPrevReply = NULL, const char *desc = NULL){
		return RunAsynTaskImpl(std::auto_ptr<MyTaskBase>(new MyTask2<RPY>(pstF, stCmd)), typeid(RPY), pstPrevReply, desc);
	}
};

