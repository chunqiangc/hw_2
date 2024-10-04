#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hw2.h"

int main() {
	//Write your code here
    unsigned int packet[] = {
    0x40000003,
    0x00000C75,
    0x000000CC,
    0x00845FED,
    0xFFFF668F,
    0x05888192
    };

    char *mem = (char*)malloc(1000000);

    print_packet(packet);
    store_values(packet, mem);
    return 0;
}
