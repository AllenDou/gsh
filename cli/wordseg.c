#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include "lib/hiredis.h"
#include <sys/stat.h>
#include <error.h>
#include "string.h"

#define NUM		6

typedef struct _ip_{
		char *ip;
		int port;
}IP;


IP arr[NUM] = {
		{"10.69.3.37",6526},
		{"10.69.3.37",6527},
		{"10.69.3.37",6528},
		{"10.55.37.38",6526},
		{"10.55.37.38",6527},
		{"10.55.37.38",6528}
};



#define FILEBUF (16*1024*1024)

redisContext *redis_c[NUM];
redisReply *reply;

char *ip;
char *fm_name;
int port;


static void parseOption(int argc, char* argv[])
{
		int i;
		for(i = 1 ; i<argc ; i++)
		{
				if(!strcmp(argv[i],"-h"))
						ip = argv[++i];
				else if(!strcmp(argv[i],"-p"))
						port = atoi(argv[++i]);
				else
				{
						fprintf(stderr,"[%s %s] is not supported.\r\n",argv[i],argv[i+1]);
						exit(1);
				}
		}

}

int main(int argc, char* argv[]){

		if(argc!=5)
		{
				fprintf(stderr,"wrong argument. please input arguments like '-h 127.0.0.1 -p 6522' \r\n");
				exit(-1);
		}
		parseOption(argc,argv);

		struct timeval timeout = { 1, 500 };
		int i;
		for(i=0;i<NUM;i++){
				redis_c[i] = redisConnectWithTimeout(arr[i].ip, arr[i].port, timeout);
				if (redis_c[i]->err){
						printf("Connection error: %s Port:%d\n", redis_c[i]->errstr,arr[i].port);
						exit(1);
				}
		}
		char f_name[1024];
		sprintf(f_name,"./lib%s.so",fm_name);
		char *p = malloc(FILEBUF);
		if(!p)
		{
				fprintf(stderr,"malloc failed.\r\n");
				exit(1);
		}
		char *str = malloc(2*1024*1024);
		memset(str,0,2*1024*1024);
		sprintf(str,"{\"time\":1349688926,\"ip\":\"127.0.0.1\",\
				      \"script\":\"test.php\",\
				      \"formula\":\"iask_wordseg\",\
				      \"programmer\":\"shunli\",\
				      \"data\":{\"text\":\"%s\"}}",STRING);

		for(i=0;i<1;i++){
				int idx = rand()%NUM;
				fprintf(stdout,"%d\r\n",idx);
				reply = redisCommand(redis_c[idx],"hget iask_wordseg %s",str);
				fprintf(stdout,"%s\r\n",reply->str);
				freeReplyObject(reply);
		}
		return 0;

}


