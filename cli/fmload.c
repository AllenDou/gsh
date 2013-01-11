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
int port;
char *fm_name;

off_t get_file_size(char *fname)
{
		struct stat buff;

		if (0 != stat(fname, &buff))
		{
				fprintf(stderr,"wrong.\r\n" );
				exit(-1);
		}
		return buff.st_size;
}


static void parseOption(int argc, char* argv[])
{
		int i;
		for(i = 1 ; i<argc ; i++)
		{
				if(!strcmp(argv[i],"-h"))
						ip = argv[++i];
				else if(!strcmp(argv[i],"-p"))
						port = atoi(argv[++i]);
				else if(!strcmp(argv[i],"-f"))
						fm_name = argv[++i];
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
				fprintf(stderr,"wrong argument. please input arguments like '-h 127.0.0.1 -p 6522 -f libsina.so\r\n");
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
		FILE *f =fopen(f_name,"r");
		char *p = malloc(FILEBUF);
		if(!p)
		{
				fprintf(stderr,"malloc failed.\r\n");
				exit(1);
		}
		int fsize = get_file_size(f_name);
		int len=0;
		int i;
		int ch;
		for(i =0;i<fsize; i++)
		{
				ch = fgetc(f);
				len += sprintf(p+len,"%d,",ch);
		}

		/*char *cmd ="{\"time\":1349688926,\
					\"ip\":\"127.0.0.1\",\
					\"script\":\"test.php\",\
					\"formula\":\"carsvm\",\
					\"programmer\":\"shunli\",\
					\"data\":{\
					\"tag\":\"qiche\",\
					\"parameter\":\"+1\t1:0.6\t2:0.3\"}}";
		*/
		reply = redisCommand(redis_c,"load %s %s",fm_name,p);
		freeReplyObject(reply);
		/*reply = redisCommand(redis_c,"grun aaa %s",cmd);
		freeReplyObject(reply);*/
		return 0;

}


