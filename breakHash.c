#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <locale.h>
#include "sha256.h"

//I assume that the platform on which this runs have sizeof(int) at least 4, so
//then i can use regular int as index into tables which exceed 2^24.
//Shortly said if the int is 2^32 then saving numbers of 2^24 - 2^28 is safe.
//If the platform would have int 2^16 then this application will not work for
//such large number. Porbably shrinking LASTBYTES and TABLE_ROWS would help to make it work.

// you can reconfigure these values and run ./compile to run it again in different configuration
#define LASTBYTES 6                             //6 bytes = 6*8 = 48bits, how many bytes to store from hash (and compare for collision)
#define DATA_BLOCK (LASTBYTES+sizeof(int))
// #define TABLE_ROWS (16*16*1000000)
#define TABLE_ROWS (32*1000000)             
#define DOT_EVERY 100000                        // will make 1 dot after given X iterations

//Explanations the Sha-256 has 128bit of security, if I want find collision,
//in last 48bits then there is 50% chance of collision inside 2^24 of combinations.
//That's about 16M combinations, therefore increasing it into 268M combinations (2^28)
//Increasing the table Will increase chances to find the collision with the birthday attack.

//Normaly I'm againts global varibales, but I think i will be faster when function
//calls do not have to pass anything back and forth. Not even pointers.
//Probably this is not as much needed, because GCC can do -finline-functions to inline the 
//functions when optimalisations are enabled. But this will benfit the speed a bit even without
//optimalisations. The -O3 optimalisations should gain bit more performance because it will save time on function 
//calls on my code, plus the hashing library will be optimised as well.
//
//For 2^25 combinations and 6 bytes of hash the difference between no optimalisations 
//and -O3 is significant. 47s with -O3 optimations vs 83s without optimalisations.
//Still both of them found the hash pretty quickly.
//
//It creates 2 version dynamicly linked and static linked, the static linking speeds ups maybe by a
//second, which maybe just noise. Makes executable file 64 times bigger, but is not depending on
//system libraries, so it can be copied to older linux and will run without need for recompiling.



BYTE buf[SHA256_BLOCK_SIZE];
BYTE input[16];
unsigned char* table;

//Will calculate hash of input buffer and save just part of it into buf
void sha256Fast() {
  SHA256_CTX ctx;
  sha256_init(&ctx);
  sha256_update(&ctx, input, strlen(input));
  sha256_finalFast(&ctx, buf);	
}

//Will calculate hash of input buffer and save it into buf
void sha256() {
  SHA256_CTX ctx;
  sha256_init(&ctx);
  sha256_update(&ctx, input, strlen(input));
  sha256_final(&ctx, buf);  
}

//Will print human friendly hex representation of the hash in buf
void printSha() {
  for (int k=0;k<SHA256_BLOCK_SIZE;k++) {
    printf("%02x", (unsigned) buf[k]);
  }  
  printf("\n");
}

//Print the table with hashes and index as well 
void printTable() {
  for (unsigned int k=0;k<TABLE_ROWS*DATA_BLOCK;k++) {
    if (k%DATA_BLOCK==0) printf("\n");
    printf("%02x", (unsigned) table[k]);
  }  
  printf("\n");
}

int compareHashes (const void * a, const void * b) {
   return memcmp(a,b,LASTBYTES);
}


int main() {
  unsigned int index;

  //will help me printf to print numbers with spacings (i want comma for each 1000)
  setlocale(LC_NUMERIC, "");

  printf("Trying to find collision in the last %d bits (%d bytes).\n", LASTBYTES*8,LASTBYTES);
  // printf("%d bits should have collision in 2^%d  \n", LASTBYTES*8,(LASTBYTES*8)/2);
  printf("1/4 Allocating %'u kB of ram for %'u entries !\n", (TABLE_ROWS*DATA_BLOCK)/(1024),TABLE_ROWS);

  //this is not the cleanest way to allocate 2D array, but it will do the job and
  //probably will have same assembler end result like: table[x][y] and table[x+(y*WIDTH)]
  //But I do not waste resources on any datastructure and the code is crude, but simple.
  if ((table = calloc(TABLE_ROWS,DATA_BLOCK))==NULL) {
      printf("Failed to allocate so much memorry into single block.\n");
      exit(1);    
  }

  printf("2/4 Generating hashes (dot every %'d hashes)\n",DOT_EVERY);
  for (unsigned int j=0,index=0;j<TABLE_ROWS;j++,index+=DATA_BLOCK) {
    sprintf(input, "%d", j);
    sha256Fast();
    // printSha();
    
    //save hash into table
    memcpy(table+index,buf+SHA256_BLOCK_SIZE-LASTBYTES,LASTBYTES);

    //save index into table
    memcpy(table+index+LASTBYTES,&j,sizeof(int));

    if (j%DOT_EVERY==0) {
      printf(".");
      fflush(stdout);
    }
  }


  printf("\n3/4 Sorting hashes\n");
  // printTable();
  qsort(table, TABLE_ROWS, DATA_BLOCK, compareHashes);
  // printTable();


  printf("4/4 Searching for collision\n");
  //because they are sorted the same hashes will be next to each other.
  //plus I don't need to check hashes which were checked already, 
  //so that will halve the number of iterations as well.
  for (unsigned int i=0,index=0;i<(TABLE_ROWS-1);i++,index+=DATA_BLOCK) {
    if (compareHashes(table+index,table+index+DATA_BLOCK)==0) {

      unsigned int itemIndex1,itemIndex2;
      memcpy(&itemIndex1,table+index+LASTBYTES,sizeof(int));
      memcpy(&itemIndex2,table+index+LASTBYTES+DATA_BLOCK,sizeof(int));
      printf("\n===========\nCollission found for ascii inputs \"%d\" and \"%d\"\n============\n",itemIndex1,itemIndex2);

      sprintf(input, "%d", itemIndex1);
      sha256();
      printSha();
      sprintf(input, "%d", itemIndex2);
      sha256();
      printSha();
      printf("=============\nTo verify type:\necho -n \"%d\" | sha256sum\necho -n \"%d\" | sha256sum\n=========\n",itemIndex1,itemIndex2);
      printf("Exiting... have a nice day\n");

      free(table);
      exit(0);
    }
  }

  printf("\n\nSorry nothing found in the set of %'u of hashes.\n",TABLE_ROWS);
  printf("Maybe increase the TABLE_ROWS, recompile and rerun again.");
  free(table);
}
