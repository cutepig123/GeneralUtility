from multiprocessing import Process, Queue
import time

def f(q):

    cmd=''
    data=''
    while 1:
        try:
            [cmd, data] =q.get(True, 1)
        except:
            pass

        print cmd, data

        if cmd=='quit':
            break

if __name__ == '__main__':
    q = Queue()
    p = Process(target=f, args=(q,))
    p.start()

    q.put(['start', 'case1'])
    time.sleep(3)

    q.put(['start', 'case2'])
    time.sleep(3)

    q.put(['stop', ''])
    time.sleep(3)

    q.put(['quit', ''])

    p.join()