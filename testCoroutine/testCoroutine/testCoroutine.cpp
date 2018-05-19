#include <stdio.h>
#include <setjmp.h>

static jmp_buf buf;

void second(void) {
	printf("second\n");         // ��ӡ
	longjmp(buf, 1);             // ����setjmp�ĵ��ô� - ʹ��setjmp����ֵΪ1
}

void first(void) {
	second();
	printf("first\n");          // ������ִ�е�����
}

int main() {
	if (!setjmp(buf)) {
		first();                // �������ǰ��setjmp����0
	}
	else {                    // ��longjmp��ת�أ�setjmp����1����˽������
		printf("main\n");       // ��ӡ
	}

	return 0;
}