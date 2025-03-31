# OS
# 1. System Call 의 이해
> 목표 : 리눅스 커널을 수정하여 System Call 동작 방식을 확인한다  

## 과제 
> 리눅스 커널에서 커스텀 시스템 콜을 등록하고 사용하는 예제
```scss
[call_my_queue.c]
     ↓ syscall(326, 3)
[syscall 번호] 
     ↓
[syscall_64.tbl] → 326 → __x64_sys_my_enqueue
     ↓
[my_queue_syscall.c] 내부 함수 실행
     ↓
[printk 출력] → dmesg 로 확인
```
### 1) 개발 환경 
- linux 4.4 kernel
### 2) 파일 설명
#### `call_my_queue.c` : 커널에 등록한 시스템 콜을 호출하여 테스트하는 유저 프로그램
- syscall(번호, 인자)를 이용해 커널 내부 함수(sys_my_enqueue, sys_my_dequeue, sys_my_initqueue) 호출 

#### `Makefile` : 커널 모듈 또는 사용자 프로그램을 컴파일하는 설정 파일
- my_queue_syscall.c 파일을 vmlinux 이미지에 정적 링크하도록 지시
- 커널 빌드시 이 오브젝트 파일이 포함되어 .text 섹션에 삽입됨
```make
obj-y += ... my_queue_syscall.o
```

#### `my_queue_syscall.c` : 커널 공간 코드, 시스템 콜 구현부
- 큐 자료구조 정의 및 printk()를 통한 커널 로그 출력
- 커널 모드에서 실행되며 asmlinkage 키워드 사용
```c
// sys_my_initqueue(), sys_my_enqueue(), sys_my_dequeue() 함수 구현 
```

#### `syscall_64.tbl` : 시스템 콜 테이블 정의 파일
- 커널 소스 내 위치: arch/x86/entry/syscalls/syscall_64.tbl
- 시스템 콜 번호 ↔ 커널 함수 매핑
```c
# os_eundms
326 common my_enqueue sys_my_enqueue
327 common my_dequeue sys_my_dequeue
328 common my_queueinit sys_my_initqueue
```

#### `syscalls.h` : 헤더 파일에 사용자 시스템 콜 함수 정의 추가 
```c
asmlinkage void sys_my_enqueue(int n);
asmlinkage int sys_my_dequeue(void);
asmlinkage void sys_my_initqueue(void);
```

### 3) 커널 컴파일 과정 
1. 시스템 콜 구현 추가 
```shell
/usr/src/linux-4.4/kernel $ sudo vi my_queue.syscall.c
```
- 구현 파일은 보통 kernel/ 폴더에 저장
- 구현 후, kernel/Makefile에 obj-y += my_queue_syscall.o 추가
- arch/x86/entry/syscalls/syscall_64.tbl에 번호 매핑 추가
- include/linux/syscalls.h에 함수 선언 추가

2. 전체 커널 빌드
```shell
/usr/src/linux-4.4 $ sudo make -j$(nproc)
```
- 병렬 빌드로 전체 커널(bzImage, 모듈 등) 컴파일
- -j는 CPU 코어 수만큼 빠르게 빌드 ($(nproc)는 자동 감지)

3. 모듈 설치
```shell
/usr/src/linux-4.4 $ sudo make modules
/usr/src/linux-4.4 $ sudo make modules_install
```
- 모듈(.ko) 파일들을 /lib/modules/<kernel-version>/ 아래에 설치
- 정적 시스템 콜만 구현했다면 이건 없어도 괜찮음

4. 커널 설치 
```shell
/usr/src/linux-4.4 $ sudo make install
```

5. 시스템 재부팅
```shell
sudo reboot
```

6. 확인 및 테스트 
```shell
$ uname -r       # 새 커널 버전 확인
$ dmesg          # printk 확인
$ ./call_my_queue
```
## 개념
### 1) Interrupt vs Trap

#### 공통점
- 현재 실행 중인 프로세스를 일시 중단하고 커널 모드로 전환하여 OS가 개입할 수 있게 한다 

#### 차이점
- Trap은 프로그램 실행 중 (동기적) 발생하며 현재 수행되는 명령어와 직접 연관되어 발생한다. (system call, 예외- 0으로 나누기) 핸들러는 현재 명령이 끝난 직후 실행된다. 현재 프로세스가 계속 실행될 수도 있다 
    - `int 0x80`, `divide by zero`, `page fault`
- Interrupt는 하드웨어적으로 프로그램 외부에서 발생하며 비동기적이다. 발생지점이 일정하지 않다. 인터럽트 처리에 앞서 현재 상태를 저장한다. 이유는 실행 중인 프로세스와 무관하게 외부에서 발생하기에 interrupt 이벤트 처리 후, 원래 실행 중이던 프로그램에 돌아와 이어서 작업을 수행하기 위해서이다. 각각의 인터럽트는 고유한 번호를 가지게 된다. 인터럽트 핸들러는 인터럽트가 발생한 물리적인 메모리의 위치를 기억하기 위한 세그먼트 어드레스를 포함하게 되는데 인터럽트 핸들러가 끝난 후, 중단된 프로세스에서 다시 하던 일을 계속하도록 지시할 수 있다
    - `timer interrupt`, `keyboard interrupt` 

### 2) 시스템 콜 
- User 영역에서 Kernel 영역으로 요청을 전달하는 인터페이스 

USER layer, KERNEL layer, HARDWARE layer로 층을 나누고 있으며, 하나의 layer는 인접한 layer와만 통신한다.
시스템 콜은 유저 모드는 커널 영역에 직접적으로 접근하지 못하기 때문에, 유저 레벨에서 커널에서 제공하는 서비스를 사용하기 위해 프로세스가 운영체제 커널에게 서비스를 요청하는 것이다. 프로세스 관리, 통신, 파일 관리, 정보 유지 등 다양한 목적을 위해 사용된다.

# 2. Procfs
## 과제 
> 목표 : 다양한 프로세스의 메모리 사용을 ㄴ모니터링하고 결과를 분석한다  

### 파일 설명
- get*.c: /proc/[pid]/statm 기반 메모리 정보 수집 도구
- makeobj.sh: 위 파일들 컴파일 자동화 스크립트
- makegraph.py: 측정된 데이터 시각화 (Jupyter 활용) 

## 개념
### 메모리 영역별 특징

| 항목 | 의미 | 실행 중 변화 | 
| --- | --- | --- | 
| Text | 코드 영역 (읽기 전용) | ❌ 불변 | 
| Data | 전역/정적 변수 | ⭕ 실행 중 변화 | 
| Stack | 지역변수, 함수 호출 | ⭕ 실행 중 변화 | 
| Shared | 라이브러리 등 | ⭕ or ❌ (공유) | 
| Virtual | 전체 주소 공간 | ⭕ | 
| Physical (Resident) | 실제 메모리에 올라간 페이지 수 | ⭕ 중요 |


- Firefox → 메모리 사용량 큼, Page Fault 가능성 높음
- Movie → 공유 메모리는 일정, data 변동 심함
- Text 편집기 → 일정 이상에서 memory 증가 (swp 파일)
- Bash → 거의 변화 없음 (stable background app)

