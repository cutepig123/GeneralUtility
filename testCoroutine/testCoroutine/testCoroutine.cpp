#include <stdio.h>
#include <setjmp.h>

static jmp_buf buf;

void second(void) {
	printf("second\n");         // 打印
	longjmp(buf, 1);             // 跳回setjmp的调用处 - 使得setjmp返回值为1
}

void first(void) {
	second();
	printf("first\n");          // 不可能执行到此行
}

int main() {
	if (!setjmp(buf)) {
		first();                // 进入此行前，setjmp返回0
	}
	else {                    // 当longjmp跳转回，setjmp返回1，因此进入此行
		printf("main\n");       // 打印
	}

	return 0;
}