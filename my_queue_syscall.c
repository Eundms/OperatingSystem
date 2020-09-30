#include<linux/kernel.h>
#include<linux/linkage.h>
# define QLEN 5
int queue[QLEN];
int head=0;
int tail=0;
void print_queue(void) {
	printk(KERN_CRIT "Queue:");
	if (head == tail) { printk(KERN_CRIT "[EMPTY]\n"); return; }
	int i = head;
	while(1){
		printk(KERN_CRIT "%d\t", queue[i]);
		i = (i + 1) % QLEN;
		if (i==head||i == tail) { break; }
	}
	printk(KERN_CRIT "\n");

}
void sys_my_enqueue(int n) {
	if ((tail + 1) % QLEN == head){
		printk(KERN_CRIT "FULL-can't add %d\n", n);
		return;
	}
	printk(KERN_CRIT "Enqueue");
	queue[tail] = n;
	tail = (tail + 1) % QLEN;
	
	print_queue();
}

int sys_my_dequeue(void) {
	printk(KERN_CRIT "Dequeue");
	if (head == tail) {
		printk(KERN_CRIT "EMPTY-can't remove\n");
		return -1;
	}
	queue[head] = -1;
	head = (head + 1) % QLEN;
	print_queue();

	return 0;
}
