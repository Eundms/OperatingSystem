#include<linux/kernel.h>
#include<linux/linkage.h>
# define QLEN 5
int queue[QLEN]; // 원형 큐
int head=0; // 큐에서 제일 오래된 원소의 위치.
int tail=0; //다음 원소가 들어올 위치
void print_queue(void) {
	printk(KERN_CRIT "Queue:");
	// 큐가 비어있다면
	if (head == tail) { printk(KERN_CRIT "[EMPTY]\n"); return; }
	int i = head;
	//큐에 있는 내용을 다 프린트 할 때까지 진행
	while(1){
		printk(KERN_CRIT "%d\t", queue[i]);
		i = (i + 1) % QLEN;
		if (i==head||i == tail) { break; }
	}
	printk(KERN_CRIT "\n");

}
int isexist(int n){
	int i = head;
	for (i=head;i<tail;i++){
	if (n==queue[i])return -1;
	}
	return 0;
}
void sys_my_enqueue(int n) {
	if (isexist(n)==-1){printk(KERN_CRIT "[EXIST]%d exist.\n",n);return;}
	//큐가 꽉차있다면,
	if ((tail + 1) % QLEN == head){
		printk(KERN_CRIT "FULL-can't add %d\n", n);
		return;
	}
	//tail이 가르키는 공간에 값을 넣는다
	printk(KERN_CRIT "Enqueue");
	queue[tail] = n;
	tail = (tail + 1) % QLEN;
	
	print_queue();
}

int sys_my_dequeue(void) {
	printk(KERN_CRIT "Dequeue");
	// 큐가 비어있다면,
	if (head == tail) {
		printk(KERN_CRIT "EMPTY-can't remove\n");
	}
	// 큐에서 제일 오래된 값을 없앤다
	else{
	int old=queue[head];
	queue[head] = -1;
	head = (head + 1) % QLEN;
	print_queue();

	return old;
	}	
}
