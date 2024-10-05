#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hw2.h"

int main() {
	//Write your code here
    unsigned int packet[] = {
    0x00000001,
    0x0000020F,
    0x00000010
    };

    char *mem = (char*)malloc(1000000);

    create_completion(packet, mem);
    return 0;
}
