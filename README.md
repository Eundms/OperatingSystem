# OS
2020-2 OS

## 과제 01 Kernel System Call 이해와 구현

* 인터럽트와 trap 비교

interrupt에는 크게 외부 인터럽트와 내부 인터럽트로 나누어지는데, 내부 인터럽트(=software interrupt)와 트랩은 거의 유사하므로, 외부 인터럽트와 trap에 대해서 비교한다. 

trap은 프로그램 내에서 발생하며 동기적이다. 발생지점이 프로그램의 일정한 지점이다. 현재 수행되는 명령어와 직접 연관되어 원인이 발생한다. 

interrupt는 하드웨어적으로 프로그램 외부에서 발생하며 비동기적이다. 발생지점이 일정하지 않다. 각각의 인터럽트는 고유한 번호를 가지게 된다. 인터럽트 핸들러는 
인터럽트가 발생한 물리적인 메모리의 위치를 기억하기 위한 세그먼트 어드레스를 포함하게 되는데 인터럽트 핸들러가 끝난 후, 중단된 프로세스에서 다시 하던 일을 계속하도록 지시할 수 있다.

* 시스템 콜

시스템 호출은 유저 모드는 커널 영역에 직접적으로 접근하지 못하기 때문에, 유저 레벨에서 커널에서 제공하는 서비스를 사용하기 위해 프로세스가 운영체제 커널에게 요청하는 것이다. 프로세스 관리, 통신, 파일 관리, 정보 유지 등 다양한 목적을 위해 사용된다.
libc.a(GNU C 표준 라이브러리)에 정의된 wrapper function(포장함수)를 이용하여 호출하는 방법이 있고, syscall() 함수를 이용하여 직접 호출하는 방법이 있다. 
1. libc에 정의되어 있는 open(),read(),write(),close() 이용
2. #include<unistd.h> #include<sys/syscall.h> syscall(SYS_open) 이용
이때, SYS_open은 [syscall.h](https://unix.superglobalmegacorp.com/Net2/newsrc/sys/syscall.h.html)에 define되어 있다.


1을 이용하여 시스템 콜이 진행되는 과정은 다음과 같다.
USER level에서 실행된 프로세스는 libc에 저장되어있는 read()함수를 호출한다. 그러면, 어셈블리어로 작성된 read()가 실행된다. read() 시스템 콜의 고유번호를 eax레지스터에 저장하고, 어셈블리 코드에서 Trap형식으로 시스템 콜이 실행된다.(Trap 이라고 호칭하는 이유는 이벤트와 별개로 내가 실행하던 flow가 있는 게 아니고, 수행 중이던 프로그램 내에서 발생한 동기적 이벤트이기 때문이다. intel에서는 int 0x80와 같은 int명령으로 트랩을 발생시킨다.) 이때, 발생된 trap은 interrupt나 trap이 걸렸을 때, 수행해야 할 핸들러의 주소를 저장해 두는 구조체인 IDT(=Interrupt Descriptor Table)의 0x80에 대응되는 entry에 등록되어 있는 System_call를 호출한다. system_call() 함수에서는 호출된 시스템 콜 번호와 모든 레지스터를 스택에 저장하고 올바른 시스템 콜 번호인지 검사한 후, eax에 저장되어 있는 시스템 콜 번호를 인덱스로 sys_call_table을 탐색하여 그 번호에 해당하는 함수(sys_read())를 호출한다. 이 함수가 종료하면 entry에 정의되어 있는 ret_from_sys_call()함수에 의해 사용자 프로세서로 돌아간다. 

간단하게 정리하자면, libc의 read()함수 호출->0xNN trap 발생->IDT의 0xNN에 대응되는 함수 호출->eax값(시스템 콜 번호)을 인덱스로 sys_call_table을 찾아서 번호에 해당하는 시스템 콜(sys_read()) 호출 순으로 진행된다.



# 과제 02 가상메모리의 이해와 procfs를 사용한 분석
* getvirtmem.c
