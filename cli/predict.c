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


#define FILEBUF (16*1024*1024)

redisContext *redis_c;
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

int main(int argc, char* argv[])
{
		if(argc!=5)
		{
				fprintf(stderr,"wrong argument. please input arguments like '-h 127.0.0.1 -p 6522' \r\n");
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

		char *str ="{\"time\":1349688926,\
					\"ip\":\"127.0.0.1\",\
					\"script\":\"test.php\",\
					\"formula\":\"suggest_predict\",\
					\"programmer\":\"shunli\",\
					\"data\":{\"blogid\":\"5f56a4640100md37\",\"blog_pubdate\":\"1282028281\",\"classid\":\"15\",\"body\":\"-\",\"keyWords\":[{\"word\":\"发射系统\",\"count\":19,\"tfidf\":0.7192128244436503},{\"word\":\"导弹\",\"count\":47,\"tfidf\":0.27454768431526294}]}}";
		int i;
		for(i=0;i<1;i++)		
		{
				reply = redisCommand(redis_c,"hget suggest_predict %s",str);
				fprintf(stdout,"%s\r\n",reply->str);
				freeReplyObject(reply);
		}
		return 0;

}


