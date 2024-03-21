#include "header.h"

#define VERSION 2

void printPacket(struct packet *p){  //always follow this format for printing the packet
    printf("{\n");
    printf("    Version: %d\n", p->version);
    printf("    Header Length: %d\n", p->headerLength);
    printf("    Total Length: %d\n", p->totalLength);
    printf("    Source Department: %d\n", p->srcDept);
    printf("    Destination Department: %d\n", p->destDept);
    printf("    CheckSum: %d\n", p->checkSum);
    printf("    Hops: %d\n", p->hops);
    printf("    Type: %d\n", p->type);
    printf("    ACK: %d\n", p->ACK);
    if(p->headerLength == 6){
        printf("    Source Campus: %d\n", p->srcCampus);
        printf("    Destination Campus: %d\n", p->destCampus);
    }
    printf("    Data: %s\n", p->data);
    printf("}\n");
}

int checksumCalc(unsigned char* buffer){
    struct packet *p = deserialize(buffer);
    int len = p->totalLength;
    free( p);
    unsigned int sum = 0;
    // printf("Len:\t");
    // print_binary(len);
    for( int i = 0; i < len; i++){
        // printf("current buffer:\t");
        // print_binary(buffer[i]);
        if( i == 2 ){
            sum+= (buffer[i] & 0b11111100) >> 2;
        }
        else if( i == 3 ){
            //continue, checksum is not included in the checksum calculation
        }
        else{
            sum += buffer[i];
        }
        // first 10 bits + remaining bits right shifted
        // printf( "pre sum:\t");
        // print_binary(sum);
        sum = (sum & 0b1111111111) ;//| (sum & 0xFFFFFC00)>>10;
        // printf( "current sum:\t");
        // print_binary(sum);
    }

    // flip the bits of the sum
    sum = ~sum+1;
    return sum&0x3FF;
}


unsigned char* serialize(struct packet* p){
    unsigned char *buffer;
    buffer = malloc(p->totalLength);
    memset(buffer, 0, p->totalLength);
    
    buffer[0] = (unsigned char)((p->version << 4) | p->headerLength);
    buffer[1] = (unsigned char)(p->totalLength);
    buffer[2] = (unsigned char)((p->srcDept << 5 | p->destDept << 2 | ((p->checkSum & 0b1100000000) >> 8)));
    buffer[3] = (unsigned char)((p->checkSum & 0b11111111));
    buffer[4] = (unsigned char)((p->hops << 5) | p->type << 2 | p->ACK);
    
    if (p->headerLength == 6)
    {
        buffer[5] = (unsigned char)(p->srcCampus << 4 | p->destCampus);
        strcpy((char*)(buffer+6), p->data);
    }
    else
    {
        strcpy((char*)(buffer+5), p->data);
    }

    int checkSum=checksumCalc(buffer);
    buffer[2]=(buffer[2] & 0b00000011) | (checkSum & 0b1100000000);
    buffer[3]=checkSum & 0xFF;
    return buffer;
}

struct packet *deserialize(unsigned char* buffer){
    struct packet* p_ptr = (struct packet*) malloc( sizeof(struct packet));
    // code for deserialization
    p_ptr->version =        (int)((buffer[0] & 0b11110000) >> 4);
    p_ptr->headerLength =   (int)(buffer[0] & 0b00001111);
    p_ptr->totalLength =    (int)(buffer[1]);
    p_ptr->srcDept =        (int)((buffer[2] & 0b11100000) >> 5);
    p_ptr->destDept =       (int)((buffer[2] & 0b00011100) >> 2);
    p_ptr->checkSum =       (int)(((buffer[2] & 0b00000011) << 8) | buffer[3]);
    p_ptr->hops =           (int)((buffer[4] & 0b11100000) >> 5);
    p_ptr->type =           (int)((buffer[4] & 0b00011100) >> 2);
    p_ptr->ACK =            (int)(buffer[4] & 0b00000011);
    if (p_ptr->headerLength == 6) {
        p_ptr->srcCampus =      (int)((buffer[5] & 0b11110000) >> 4);
        p_ptr->destCampus =     (int)(buffer[5] & 0b00001111);
        // + 8 as the header might contain a 0x00 somewhere as value, which might be taken as null by the strcpy function
        strcpy(p_ptr->data, (char*)(buffer+6));
    }
    else{
        strcpy(p_ptr->data, (char*)(buffer+5));
    }
    return p_ptr;
}

struct packet *generatePacket(int version, int headerLength, int totalLength, 
                              int srcDept, int destDept, int checkSum, int hops, 
                              int type, int ACK, int srcCampus, int destCampus, char* data) {
    //feel free to write your own function with more customisations. This is a very basic func 
    struct packet *p;
    p = (struct packet *)malloc(sizeof(struct packet));
    p->version = version;
    p->headerLength = headerLength;
    p->totalLength = totalLength;
    p->srcDept = srcDept;
    p->destDept = destDept;
    p->checkSum = checkSum;
    p->hops = hops;
    p->type = type;
    p->ACK = ACK;
    p->srcCampus = srcCampus;
    p->destCampus = destCampus;
    strcpy(p->data, data);
    return p;
}


unsigned char* getPacket(char* input, int srcCampus, int srcDept,
int* validDept[3], int numOfValidDept[3]){

    if( srcCampus < 0 || srcCampus > 2 || srcDept < 0 || srcDept > 7 ){
        // printf("Invalid source campus or department\n");
        return NULL;
    }
    char type_str[1024] = {0};
    strncpy( type_str, input, 1);
    int type = atoi( type_str );
    if( 1 == type || 2 == type ){
        return generateUnicastPacket(input, srcCampus, srcDept, validDept, numOfValidDept);
    }
    else if( 3 == type || 4 == type ){
        return generateBroadcastPacket(input, srcCampus, srcDept, validDept, numOfValidDept);
    }
    else if( 5 == type ){
        return generateControlPacket(input, srcCampus, srcDept, validDept, numOfValidDept);
    }

}

unsigned char* generateUnicastPacket(char* input, int srcCampus, int srcDept, int* validDept[3], int numOfValidDept[3]){
    int type = atoi(strtok(input, "."));
    if( 1 == type ){
        // inter campus
        int dest_campus = atoi(strtok( NULL, ":"));
        int dest_dept = atoi(strtok( NULL, ":"));
        if( dest_campus < 0 || dest_campus > 2 || dest_dept < 0 || dest_dept > 7 ){
            // printf("Invalid destination campus or department\n");
            return NULL;
        }
        int found_dept_in_dest_campus = 0;
        for( int i = 0; i < numOfValidDept[dest_campus]; i++){
            if( dest_dept == validDept[dest_campus][i] ){
                found_dept_in_dest_campus = 1;
                break;
            }
        }
        if( !found_dept_in_dest_campus ){
            // printf("Invalid destination department\n");
            return NULL;
        }
        char payload[1024] = {0};
        strcpy( payload, strtok( NULL, "\n"));
        struct packet *p = generatePacket( VERSION, 6, 6+strlen(payload), srcDept, dest_dept, 0, 0, 0, 0, srcCampus, dest_campus, payload);
        char* buff = serialize(p);
        free(p);
        return buff;
    }
    else{
        // intra campus
        int dest_dept = atoi(strtok( NULL, ":"));
        if( dest_dept < 0 || dest_dept > 7 ){
            // printf("Invalid destination department\n");
            return NULL;
        }
        char payload[1024] = {0};
        strcpy( payload, strtok( NULL, "\n"));
        struct packet *p = generatePacket( VERSION, 5, 5+strlen(payload), srcDept, dest_dept, 0, 0, 0, 0, 0, 0, payload);
        char* buff = serialize(p);
        free(p);
        return buff;
    }
}

unsigned char* generateBroadcastPacket(char* input, int srcCampus, int srcDept, int* validDept[3], int numOfValidDept[3]){
    int type = atoi(strtok(input, "."));
    if( 3 == type ){
        // campuse level broadcast
        char payload[1024] = {0};
        strcpy( payload, strtok( NULL, "\n"));
        struct packet *p = generatePacket( VERSION, 5, 5+strlen(payload), srcDept, 0b111, 0, 0, 0, 0, 0, 0, payload);
        char* buff = serialize(p);
        free(p);
        return buff;
    }
    else{
        // university broadcast
        char payload[1024] = {0};
        strcpy( payload, strtok( NULL, "\n"));
        struct packet *p = generatePacket( VERSION, 6, 6+strlen(payload), srcDept, 0b111, 0, 0, 0, 0, srcCampus, 0b1111, payload);
        char* buff = serialize(p);
        free(p);
        return buff;
    }
}

unsigned char* generateControlPacket(char* input, int srcCampus, int srcDept, int* validDept[3], int numOfValidDept[3]){
    int type = atoi( strtok(input, "."));
    if( 5 == type ){
        struct packet *p = generatePacket( VERSION, 5, 5, srcDept, 0b000, 0, 0, 0b001, 0, 0, 0, "");
        char* buff = serialize(p);
        free(p);
        return buff;
    }
}