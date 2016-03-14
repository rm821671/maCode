#include <iostream>
void free_ram() {
     int *p;
     int counter = 0;
     while(1) {
         if( !(counter%1000) ) { // print status
             std::cout << counter << "\r" << std::flush;
         }
         int inc=1024*1024*sizeof(char);
         p=(int*) calloc(1,inc);
         if(!p) break;
         counter ++;
     }
 }
