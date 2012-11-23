/*** hash functions from redis. ***/

#include <ctype.h>
#include <stdint.h>
#include <string.h>
#define JHASH_GOLDEN_RATIO  0x9e3779b9
#define __jhash_mix(a, b, c) \
{ \
		a -= b; a -= c; a ^= (c>>13); \
		b -= c; b -= a; b ^= (a<<8); \
		c -= a; c -= b; c ^= (b>>13); \
		a -= b; a -= c; a ^= (c>>12);  \
		b -= c; b -= a; b ^= (a<<16); \
		c -= a; c -= b; c ^= (b>>5); \
		a -= b; a -= c; a ^= (c>>3);  \
		b -= c; b -= a; b ^= (a<<10); \
		c -= a; c -= b; c ^= (b>>15); \
}


/* Thomas Wang's 32 bit Mix Function */
unsigned int dictIntHashFunction(unsigned int key)
{
		key += ~(key << 15);
		key ^=  (key >> 10);
		key +=  (key << 3);
		key ^=  (key >> 6);
		key += ~(key << 11);
		key ^=  (key >> 16);
		return key;
}

/* Identity hash function for integer keys */
unsigned int dictIdentityHashFunction(unsigned int key)
{
		return key;
}

/* Generic hash function (a popular one from Bernstein).  89  * I tested a few and this was the best. */
unsigned int dictGenHashFunction(const unsigned char *buf, int len) {
		unsigned int hash = 5381;

		while (len--)
				hash = ((hash << 5) + hash) + (*buf++); /* hash * 33 + c */
		return hash;
}

/* And a case insensitive version */
unsigned int dictGenCaseHashFunction(const unsigned char *buf, int len) {
		unsigned int hash = 5381;

		while (len--)
				hash = ((hash << 5) + hash) + (tolower(*buf++)); /* hash * 33 + c */
		return hash;
}

/*ELF hash*/
unsigned int ELFhash(char *key)
{
		unsigned long h=0;
		while(*key)
		{
				h=(h<<4)+*key++;
				unsigned long g=h&0Xf0000000L;
				if(g)
						h^=g>>24;
				h&=~g;
		}
		return h;
}

/*One-Way Hash from Blizzard*/
unsigned long cryptTable[0x500];
unsigned long HashString(char *lpszFileName, unsigned long dwHashType)
{
		unsigned char *key = (unsigned char *)lpszFileName;
		unsigned long seed1 = 0X7FED7FED, seed2 = 0xEEEEEEEE;
		int ch;
		while(*key != 0)
		{
				ch = toupper(*key++);
				seed1 = cryptTable[(dwHashType << 8) + ch]^(seed1 + seed2);
				seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
		}
		return seed1;
}

/*hash func from openssl*/
unsigned int lh_strhash(void *src)
{
		int i, l;
		unsigned long ret = 0;
		unsigned short *s;
		char *str = (char *)src;
		if (str == 0)
				return(0);
		l = (strlen(str) + 1) / 2;
		s = (unsigned short *)str;

		for (i = 0; i < l; i++) 
				ret ^= s[i]<<(i&0x0f);

		return(ret);
}
/*jhash from linux kernel*/
unsigned int jhash2(const unsigned int *k, unsigned int length, unsigned int initval)
{
		unsigned int a, b, c, len;

		a = b = JHASH_GOLDEN_RATIO;
		c = initval;
		len = length;

		while (len >= 3) {
				a += k[0];
				b += k[1];
				c += k[2];
				__jhash_mix(a, b, c);
				k += 3; len -= 3;
		}

		c += length * 4;

		switch (len) {
				case 2 : b += k[1];
				case 1 : a += k[0];
		};

		__jhash_mix(a,b,c);

		return c;
}

/* murmur hash from nginx
 * murmur is from google.*/
unsigned int ngx_murmur_hash2(unsigned char *data, size_t len)
		//unsigned int ngx_murmur_hash2(char *data, size_t len)
{
		unsigned int  h, k;

		h = 0 ^ len;

		while (len >= 4) {
				k  = data[0];
				k |= data[1] << 8;
				k |= data[2] << 16;
				k |= data[3] << 24;

				k *= 0x5bd1e995;
				k ^= k >> 24;
				k *= 0x5bd1e995;

				h *= 0x5bd1e995;
				h ^= k;

				data += 4;
				len -= 4;
		}

		switch (len) {
				case 3:
						h ^= data[2] << 16;
				case 2:
						h ^= data[1] << 8;
				case 1:
						h ^= data[0];
						h *= 0x5bd1e995;
		}

		h ^= h >> 13;
		h *= 0x5bd1e995;
		h ^= h >> 15;

		return h;
}

unsigned int RSHash(char* str, unsigned int len)   
{   
		unsigned int b    = 378551;   
		unsigned int a    = 63689;   
		unsigned int hash = 0;   
		unsigned int i    = 0;   
		for(i = 0; i < len; str++, i++)   
		{   
				hash = hash * a + (*str);   
				a    = a * b;   
		}   
		return hash;   
}   
/* End Of RS Hash Function */  

unsigned int JSHash(char* str, unsigned int len)   
{   
		unsigned int hash = 1315423911;   
		unsigned int i    = 0;   
		for(i = 0; i < len; str++, i++)   
		{   
				hash ^= ((hash << 5) + (*str) + (hash >> 2));   
		}   
		return hash;   
}   
/* End Of JS Hash Function */  

unsigned int PJWHash(char* str, unsigned int len)   
{   
		const unsigned int BitsInUnsignedInt = (unsigned int)(sizeof(unsigned int) * 8);   
		const unsigned int ThreeQuarters     = (unsigned int)((BitsInUnsignedInt  * 3) / 4);   
		const unsigned int OneEighth         = (unsigned int)(BitsInUnsignedInt / 8);   
		const unsigned int HighBits          = (unsigned int)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);   
		unsigned int hash              = 0;   
		unsigned int test              = 0;   
		unsigned int i                 = 0;   
		for(i = 0; i < len; str++, i++)   
		{   
				hash = (hash << OneEighth) + (*str);   
				if((test = hash & HighBits)  != 0)   
				{   
						hash = (( hash ^ (test >> ThreeQuarters)) & (~HighBits));   
				}   
		}   
		return hash;   
}   
/* End Of  P. J. Weinberger Hash Function */  

//unsigned int ELFHash(char* str, unsigned int len)   
//{   
//		unsigned int hash = 0;   
//		unsigned int x    = 0;   
//		unsigned int i    = 0;   
//		for(i = 0; i < len; str++, i++)   
//		{   
//				hash = (hash << 4) + (*str);   
//				if((x = hash & 0xF0000000L) != 0)   
//				{   
//						hash ^= (x >> 24);   
//				}   
//				hash &= ~x;   
//		}   
//		return hash;   
//}   
/* End Of ELF Hash Function */  

unsigned int BKDRHash(char* str, unsigned int len)   
{   
		unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */  
		unsigned int hash = 0;   
		unsigned int i    = 0;   
		for(i = 0; i < len; str++, i++)   
		{   
				hash = (hash * seed) + (*str);   
		}   
		return hash;   
}   
/* End Of BKDR Hash Function */  

unsigned int SDBMHash(char* str, unsigned int len)   
{   
		unsigned int hash = 0;   
		unsigned int i    = 0;   
		for(i = 0; i < len; str++, i++)   
		{   
				hash = (*str) + (hash << 6) + (hash << 16) - hash;   
		}   
		return hash;   
}   
/* End Of SDBM Hash Function */  

unsigned int DJBHash(char* str, unsigned int len)   
{   
		unsigned int hash = 5381;   
		unsigned int i    = 0;   
		for(i = 0; i < len; str++, i++)   
		{   
				hash = ((hash << 5) + hash) + (*str);   
		}   
		return hash;   
}   
/* End Of DJB Hash Function */  

unsigned int DEKHash(char* str, unsigned int len)   
{   
		unsigned int hash = len;   
		unsigned int i    = 0;   
		for(i = 0; i < len; str++, i++)   
		{   
				hash = ((hash << 5) ^ (hash >> 27)) ^ (*str);   
		}   
		return hash;   
}   
/* End Of DEK Hash Function */  

unsigned int BPHash(char* str, unsigned int len)   
{   
		unsigned int hash = 0;   
		unsigned int i    = 0;   
		for(i = 0; i < len; str++, i++)   
		{   
				hash = hash << 7 ^ (*str);   
		}   
		return hash;   
}   
/* End Of BP Hash Function */  

unsigned int FNVHash(char* str, unsigned int len)   
{   
		const unsigned int fnv_prime = 0x811C9DC5;   
		unsigned int hash      = 0;   
		unsigned int i         = 0;   
		for(i = 0; i < len; str++, i++)   
		{   
				hash *= fnv_prime;   
				hash ^= (*str);   
		}   
		return hash;   
}   
/* End Of FNV Hash Function */  

unsigned int APHash(char* str, unsigned int len)   
{   
		unsigned int hash = 0xAAAAAAAA;   
		unsigned int i    = 0;   
		for(i = 0; i < len; str++, i++)   
		{   
				hash ^= ((i & 1) == 0) ? (  (hash <<  7) ^ (*str) * (hash >> 3)) :   
						(~((hash << 11) + (*str) ^ (hash >> 5)));   
		}   
		return hash;   
}   
/* End Of AP Hash Function */


/* MurmurHash2, by Austin Appleby
 * Note - This code makes a few assumptions about how your machine behaves -
 * 1. We can read a 4-byte value from any address without crashing
 * 2. sizeof(int) == 4
 *
 * And it has a few limitations -
 *
 * 1. It will not work incrementally.
 * 2. It will not produce the same results on little-endian and big-endian
 *    machines.
 */
static uint32_t dict_hash_function_seed = 5381;
unsigned int dictGenHashFunction_mur(const void *key, int len) {
    /* 'm' and 'r' are mixing constants generated offline.
     They're not really 'magic', they just happen to work well.  */
    uint32_t seed = dict_hash_function_seed;
    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    /* Initialize the hash to a 'random' value */
    uint32_t h = seed ^ len;

    /* Mix 4 bytes at a time into the hash */
    const unsigned char *data = (const unsigned char *)key;

    while(len >= 4) {
        uint32_t k = *(uint32_t*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    /* Handle the last few bytes of the input array  */
    switch(len) {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0]; h *= m;
    };

    /* Do a few final mixes of the hash to ensure the last few
     * bytes are well-incorporated. */
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return (unsigned int)h;
}
