#pragma once

class noncopyable{
protected:
	noncopyable() {}
	~noncopyable() {}
private:
	noncopyable(const noncopyable&);
	noncopyable& operator=(const noncopyable &);
};
