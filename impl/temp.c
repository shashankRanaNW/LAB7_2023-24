// #include <stdio.h>
// #include <stdlib.h>
// #include "header.h"

// void print_binary(unsigned int num) {
//     for (int i = 15; i >= 0; --i) {
//         printf("%d", (num >> i) & 1);
//         if (i % 4 == 0) printf(" ");
//     }
//     printf("\n");
// }

// int checksumCalc(unsigned char* buffer){
//     struct packet *p = deserialize(buffer);
//     int len = p->totalLength;
//     free( p);
//     unsigned int sum = 0;
//     // printf("Len:\t");
//     // print_binary(len);
//     for( int i = 0; i < len; i++){
//         // printf("current buffer:\t");
//         // print_binary(buffer[i]);
//         if( i == 2 ){
//             sum+= (buffer[i] & 0b11111100) >> 2;
//         }
//         else if( i == 3 ){
//             //continue, checksum is not included in the checksum calculation
//         }
//         else{
//             sum += buffer[i];
//         }
//         // first 10 bits + remaining bits right shifted
//         // printf( "pre sum:\t");
//         // print_binary(sum);
//         sum = (sum & 0b1111111111) | (sum & 0xFFFFFC00)>>10;
//         // printf( "current sum:\t");
//         // print_binary(sum);
//     }

//     // flip the bits of the sum
//     sum = ~sum;
//     return sum;
// }

// void ser_test( unsigned char *buffer, int size)
// {
//     printf("Serialized buffer:\n");
//     int c = size;
//     char *m = buffer;
//     while (c--)
//     {
        
//         for (int i = 7; i >= 0; --i) {
//             if( i == 3)
//                 printf(" ");
//             printf("%d", (*m >> i) & 1);
//         }
//         printf("\n");
//         m++;
//     }

// }

// unsigned char* getPacket(char* input, int srcCampus, int srcDept,
// int* validDept[3], int numOfValidDept[3]);

// int main(){
    
//     struct packet *p = generatePacket(0b1111, 6, 20, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0, 0b1111, 0b1111,(unsigned char[]){ 0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111,0b1111});
//     unsigned char* buffer_ptr = serialize(p) ;
//     ser_test(buffer_ptr, p->totalLength);
//     int check_sum = checksumCalc(buffer_ptr);
//     printf("Checksum: %d\n", check_sum);
//     print_binary(check_sum);
//     free( buffer_ptr);
//     free( p);
//     return 0;
// }
#include <stdio.h>
#include <stdlib.h>
#include "header.h"

int main(){
    char str[] = "happy";
    printf("%s\n", str);
    return 0;
}