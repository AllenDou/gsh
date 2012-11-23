#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include "hiredis.h"
#include <sys/stat.h>
#include <error.h>


#define FILEBUF (16*1024*1024)

redisContext *redis_c;
redisReply *reply;

char *ip;
char *fm_name;
char *pexp;
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
				else if(!strcmp(argv[i],"-e"))
						pexp = argv[++i];
				else
				{
						fprintf(stderr,"[%s %s] is not supported.\r\n",argv[i],argv[i+1]);
						exit(1);
				}
		}

}

int main(int argc, char* argv[])
{
		if(argc!=7)
		{
				fprintf(stderr,"wrong argument. please input arguments like '-h 127.0.0.1 -p 6522 -e \"1+2+3\"' \r\n");
				exit(-1);
		}
		parseOption(argc,argv);

		struct timeval timeout = { 1, 500 };
		redis_c = redisConnectWithTimeout(ip, port, timeout);
		if (redis_c->err) 
		{
				printf("Connection error: %s Port:%d\n", redis_c->errstr,port);
				exit(1);
		}
		char f_name[1024];
		sprintf(f_name,"./lib%s.so",fm_name);
		char *p = malloc(FILEBUF);
		if(!p)
		{
				fprintf(stderr,"malloc failed.\r\n");
				exit(1);
		}

		char *bc ="{\"time\":1349688926,\
					\"ip\":\"127.0.0.1\",\
					\"script\":\"test.php\",\
					\"formula\":\"bc\",\
					\"programmer\":\"shunli\",\
					\"data\":{\
					\"exp\":\"%s\"}}";
		char cmd[BUFSIZ];
		sprintf(cmd,bc,pexp);
		int i;
		for(i=0;i<1;i++)		
		{
				reply = redisCommand(redis_c,"grun aaa %s",cmd);
				fprintf(stdout,"%s\r\n",reply->str);
				freeReplyObject(reply);
		}
		return 0;

}


