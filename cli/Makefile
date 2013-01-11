LINKCOLOR="\033[34;1m"
BINCOLOR="\033[37;1m"
ENDCOLOR="\033[0m"

CC=gcc
CFLAGS=-Wall -g -O0
LINK= -ldl -lm
bin = bin

PREDICT=cli_predict
CACHE=cli_cache
FMLOAD=cli_fmload
CARSVM=cli_carsvm
BC=cli_bc
QUIET_LINK = @printf ' %b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR);

SHARE_OBJ=lib/cJSON.o lib/hiredis.o lib/net.o lib/sds.o

PREDICT_OBJ=predict.o $(SHARE_OBJ)
CACHE_OBJ=cache.o $(SHARE_OBJ)
FMLOAD_OBJ=fmload.o $(SHARE_OBJ)
CARSVM_OBJ=carsvm.o $(SHARE_OBJ)
BC_OBJ=bc.o  $(SHARE_OBJ)

all:$(PREDICT) $(FMLOAD) $(CARSVM) $(BC)

$(PREDICT):$(PREDICT_OBJ)
	$(QUIET_LINK) $(CC) $(CFLAGS) -o bin/$@ $^ $(LINK)
$(FMLOAD):$(FMLOAD_OBJ)
	$(QUIET_LINK) $(CC) $(CFLAGS) -o bin/$@ $^ $(LINK)
$(CARSVM):$(CARSVM_OBJ)
	$(QUIET_LINK) $(CC) $(CFLAGS) -o bin/$@ $^ $(LINK)
$(BC):$(BC_OBJ)
	$(QUIET_LINK) $(CC) $(CFLAGS) -o bin/$@ $^ $(LINK)
clean:
	rm -rf $(bin)/* *.o lib/*.o
