#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main(int argc, char* argv[]){
	int pid=-1;
	int dur=10;	//default값

	// pid랑 duration 입력안함.
	if(argc<2){printf("%s pid duration: duration default=10",argv[0]); return -1;}
	else if(argc!=2){dur = atoi(argv[2]);}
	pid = atoi(argv[1]);
	
	//pid 랑 dur에 문자가 들어있을 때
	if(pid<=0||dur<=0){return -1;}
	printf("pid:%d,dur:%d\n",pid,dur);

	//pid is not exist
	char command[15];
	strcpy(command,"ps ");
	char strpid[10];
	sprintf(strpid,"%d",pid);
	strcat(command,strpid);
	int exist = system(command);
	if(exist!=0){printf("[%d]%d isn't exist.\n",exist,pid);return-1;}
	
	system("cat /proc/meminfo");		
}
