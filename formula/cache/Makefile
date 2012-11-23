
all:
	gcc -Wall -g -O0 cache.c dsl_assoc.c hash.c rbtree.c common.c ../../src/common/cJSON.c -fPIC -shared -o ../../lib/libcache.so
clean:
	rm -rf ../../lib/libcache.so

