extern HWND hMyWnd;

void RTCInit();
void RTCTimerThread();			//这个函数给Display去SetTimer，不过这不是一个低耦合的方法。