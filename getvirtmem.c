#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void parse_data(char *name);
void get_info(char *info);
int is_pidndur_ok(int argc, char**argv, int*pid, int*dur);
int is_pid_exist(int pid);

void parse_data(char *name){
	for(int i=0; i<strlen(name); i++){
		if(name[i]>='0'&&name[i]<='9'){printf("%c",name[i]);}
	}
}
void get_info(char *info)
{ 
	char dest[30];
	sprintf(dest, "/proc/%s", info);

	char buffer[1024];
 	size_t bytes_read=0;
 	int count = 0;
	
	FILE *fp=fopen(dest,"r");

	while(feof(fp)==0){
		fgets(buffer,1024,fp);
		if(strstr(buffer,"VmRSS")!=NULL){
			parse_data(buffer); /*printf("%s",buffer);*/}
		memset(buffer,0,sizeof(buffer));
	}
	fclose(fp);
}

int is_pidndur_ok(int argc, char**argv, int*pid, int*dur){

	// pid값을 입력하지 않았을 때
	if(argc<2){printf("%s pid duration: duration default=10",*(argv)); return -1;}
	else if(argc!=2){*dur = atoi(*(argv+2));}

	*pid = atoi(*(argv+1));

	// 문자를 입력했을 때
	if(*pid<=0||*dur<=0)return -1;

	for(int i=1; i<=argc; i++)
		printf("%s\t", *(argv+i-1));
	return 0;
}

int is_pid_exist(int pid){
	char command[15];
	sprintf(command,"ps %d", pid);
	if(system(command)!=0){printf("%d isn't exist.\n",pid);return-1;}
	return 0;
}

int main(int argc, char* argv[]){
	int pid = -1;
	int dur = 10; //default값
	
	if(is_pidndur_ok(argc, argv, &pid, &dur)==-1){return -1;}
	
	if(is_pid_exist(pid)==-1){return -1;}
	
	//clock_t start_clk = clock();//sleep(), system()과 같이 CPU가 점유하지 않을 때는 시간이 측정되지 않는다.
	
	char psstat[10];//stat, statm, status
	sprintf(psstat, "%d/status", pid);
	
	for(int time=0; time<dur*2; time++){
		printf("\n\n>>>%d\n", time);
		//get_info("meminfo");
		get_info(psstat);
		usleep(500000);//usleep(1000000 ) 1초
	}
	//clock_t end_clk = clock();
	//printf("%d\n",(int)((end_clk - start_clk)/CLOCKS_PER_SEC));
}