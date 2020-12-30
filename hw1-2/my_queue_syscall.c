#include <linux/slab.h>
#include<linux/kernel.h>
#include<linux/linkage.h>

typedef struct node {
	int data;
	struct node* next;
}NODE;

typedef struct linked_queue {
	NODE* head;
	NODE* tail;
	int size; //queue의 사이즈
}QUEUE;

QUEUE* queue; //linked_queue선언

asmlinkage void print_queue(void) {
	NODE* pp=queue->head;
	printk(KERN_CRIT "Queue:\t");
	int i=0;
	for (i = 0; i < queue->size; i++) {
		printk(KERN_CRIT "%d\t",pp->data);
		pp=pp->next;
	}
	printk(KERN_CRIT "\n");
}

asmlinkage void sys_my_initqueue(void) {
	queue = (QUEUE*)kmalloc(sizeof(QUEUE),GFP_USER); //동적배열할당
	if (queue == NULL) { printk(KERN_CRIT "Queue 생성 실패"); return -1; }
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;
}

asmlinkage void sys_my_enqueue(int n) {
	
	NODE *new_node = (NODE*)kmalloc(sizeof(NODE),GFP_USER);
	if (new_node == NULL) { printk(KERN_CRIT "new_node생성 실패"); return -1; }
	
	new_node->data = n;
	new_node->next = NULL;

	if (queue->head == NULL) {//제일 처음 입력하는 값
		queue->head = new_node;
		queue->tail = new_node;
	}
	else {
		queue->tail->next = new_node;
		queue->tail = queue->tail->next;
	}
	queue->size = (queue->size) + 1;
	printk(KERN_CRIT "Enqueue: %d\n", n);
	print_queue();
}

asmlinkage int sys_my_dequeue(void) {
	NODE* temp;
	if (queue->head==queue->tail) {//dequeue할 것이 없음.
		printk(KERN_CRIT "[EMPTY] Can't remove data.\n");
		return -1;
	}
	int deletedata=queue->head->data;
	temp = queue->head->next;
	kfree(queue->head);
	queue->head=temp;
	queue->size = (queue->size) - 1;
	
	printk(KERN_CRIT "Dequeue:%d\n",deletedata);
	print_queue();

}
