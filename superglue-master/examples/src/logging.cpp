#include "sg/superglue.hpp"
#include "sg/option/instr_trace.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <cstddef>

using namespace std;

//
// This example activates logging and execute some dummy tasks.
//
// The application creates a files "output.log" which contains
// execution trace information such as which thread executes
// which task, at what time and for how long.
//
// There is a python script in scripts/ that can be used to
// draw the execution trace. It can be used like this:
//
//    ../../script/drawsched.py execution.log
//

//===========================================================================
// Task Library MyOptions
//===========================================================================
struct MyOptions : public DefaultOptions<MyOptions> {
    typedef Enable TaskName;
    typedef Trace<MyOptions> Instrumentation;
};

//===========================================================================
// Tasks
//===========================================================================
class TaskA : public Task<MyOptions, 1> {
public:
    TaskA(Handle<MyOptions> &h) {
        register_access(ReadWriteAdd::write, h);
    }

    void run() {
        Time::TimeUnit t = Time::getTime();
        while (Time::getTime() < t + 1000000);
    }

    std::string get_name() { return "A"; }
};

class TaskB : public Task<MyOptions, 2> {
private:
    size_t delay;

public:
    TaskB(Handle<MyOptions> &h, Handle<MyOptions> &h1, size_t delay_)
      : delay(delay_) {
        register_access(ReadWriteAdd::read, h);
        register_access(ReadWriteAdd::write, h1);
    }

    void run() {
        Time::TimeUnit t = Time::getTime();
        while (Time::getTime() < t + delay);
    }
    std::string get_name() { return "B"; }
};

class TaskD : public Task<MyOptions, 1> {
public:
    TaskD(Handle<MyOptions> &h) {
        register_access(ReadWriteAdd::read, h);
    }

    void run() {
        Time::TimeUnit t = Time::getTime();
        while (Time::getTime() < t + 1000000);
    }
    std::string get_name() { return "D"; }
};

class TaskE : public Task<MyOptions, 2> {
public:
    TaskE(Handle<MyOptions> &h1, Handle<MyOptions> &h2) {
        register_access(ReadWriteAdd::read, h1);
        register_access(ReadWriteAdd::read, h2);
    }

    void run() {
        Time::TimeUnit t = Time::getTime();
        while (Time::getTime() < t + 1000000);
    }
    std::string get_name() { return "E"; }
};

//===========================================================================
// main
//===========================================================================
int main_logging(int argc, char *argv[]) {

    int num_threads = -1;
    if (argc == 2) {
        num_threads = (size_t) atoi(argv[1]);
    }
    else if (argc != 1) {
        printf("usage: %s [num_cores]\n", argv[0]);
        exit(0);
    }

    SuperGlue<MyOptions> sg(num_threads);
    Handle<MyOptions> a, b, c;
    sg.submit(new TaskA(a));
    sg.submit(new TaskB(a, b, 1000000));
    sg.submit(new TaskB(a, c, 2000000));
    sg.submit(new TaskD(b));
    sg.submit(new TaskE(b, c));
    sg.barrier();

    Trace<MyOptions>::dump("execution.log");
    return 0;
}
