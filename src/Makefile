LINKCOLOR="\033[34;1m"
BINCOLOR="\033[37;1m"
ENDCOLOR="\033[0m"

CC=gcc
CFLAGS= -O0 -g -rdynamic -ggdb
WALL= -Wall

LINK= -ldl -lm
TARGET=../bin
GSHSERVER=gsh-server
QUIET_LINK = @printf ' OMG!!! %b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR);

OBJ= ae.o ae_epoll.o anet.o command.o config.o db.o debug.o dict.o gsh.o networking.o object.o common/adlist.o common/cJSON.o common/sds.o common/util.o common/zmalloc.o

all:
	gcc -g3 -O0 -o ../bin/gsh-server gsh.c ae.c ae_epoll.c command.c config.c networking.c object.c db.c debug.c anet.c dict.c common/*.c  -I. iask_wordseg.c  -lm -lpthread -ldl -lstdc++ -m32 liblexicon.a libtouni.a libfromuni.a libcommon.a libustr.a
#$(GSHSERVER)

$(GSHSERVER): $(OBJ)
	$(QUIET_LINK) $(CC) $(CFLAGS) -o $(TARGET)/$@ $^ $(LINK)
clean:
	rm -rf *.o $(TARGET)/$(GSHSERVER) 

