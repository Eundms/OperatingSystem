#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int is_pidndur_ok(int argc, char**argv, int*pid, int*dur){

	// pid값을 입력하지 않았을 때
	if(argc<2){printf("%s pid duration: duration default=10",*(argv)); return -1;}
	else if(argc!=2){*dur = atoi(*(argv+2));}

	*pid = atoi(*(argv+1));

	// 문자를 입력했을 때
	if(*pid<=0||*dur<=0)return -1;

	return 0;
}


void print_int(char * buffer){
	for(int i=0;i<strlen(buffer);i++){
		if(buffer[i]>='0' && buffer[i]<='9'){
			printf("%c",buffer[i]);
		}
	}
	
}
/* [status] */
int get_dstinfo(char *spid)/* VmData, VmStk, VmExe*/
{ 
	char buffer[1024];
	char dest[30];
	sprintf(dest, "/proc/%s/status", spid);

	FILE *fp=fopen(dest,"r");
	if(fp==NULL){return -1;}
	while(fgets(buffer,1024,fp)!=NULL){
		if(strstr(buffer,"VmRSS")!=NULL){
			print_int(buffer);
			printf("\t");
		}

		printf("%s",buffer);
		memset(buffer,0,sizeof(buffer));
	}
	fclose(fp);
return 0;
}
/* [statm] statm의 data, text 확인-> text는 고정 data(stack+data(heap))는 변함.을 확인하기 위해서 */
int get_textNdata(char *spid){/*text=code, data=data+stack*/
 
	char buffer[1024];
	char dest[30];
	sprintf(dest, "/proc/%s/statm", spid);
	int size, resident, shared,text,lib,data ;
	FILE *fp=fopen(dest,"r");
	if(fp==NULL){return -1;}
	while(fgets(buffer,1024,fp)!=NULL){
		sscanf(buffer,"%d %d %d %d %d %d",&size,&resident,&shared,&text,&lib,&data);//공백 기준으로 2번째 숫자이용.
		printf("%d\t%d\n",text,data);
		memset(buffer,0,sizeof(buffer));
	}
	fclose(fp);
return 0;
}

int main(int argc, char* argv[]){
	int pid = -1;
	int dur = 10; //default값
	
	if(is_pidndur_ok(argc, argv, &pid, &dur)==-1){return -1;}

	char spid[10];//stat, statm, status
	sprintf(spid, "%d", pid);
	
	for(int time=0; time<dur*2; time++){
		//get_dstinfo(spid);
		if (get_textNdata(spid)==-1){break;}
		usleep(500000);//usleep(1000000 ) 1초
	}
}
