#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "hw2.h"

void print_packet(unsigned int packet[])
{
    unsigned int int0 = packet[0];
    unsigned int int1 = packet[1];
    unsigned int int2 = packet[2];


    unsigned int packet_type = (int0 >> 24) & 0xFF;
    unsigned int address = (int2);
    unsigned int length = int0 & 0x2FF;
    unsigned int requester_id = (int1 >> 16) & 0xFFFF;
    unsigned int tag = (int1 >> 8) & 0xFF;
    unsigned int last_BE = (int1 >> 4) & 0x0F;
    unsigned int first_BE = int1 & 0x0F;

    if (packet_type == 0100)
        printf("Packet Type: Write\n");
    else if (packet_type == 00)
        printf("Packet Type: Read\n");
    else if (packet_type == 01001010)
        printf("Packet Type: Completion\n");
    // else printf("error\n");

    printf("Address: %u\n", address);
    printf("Length: %u\n", length);
    printf("Requester ID: %u\n", requester_id);
    printf("Tag: %u\n", tag);
    printf("Last BE: %u\n", last_BE);
    printf("1st BE: %u\n", first_BE);
    printf("Data: ");

    if (packet_type == 0100) {
        for (unsigned int i = 0; i < length; i++) {
            unsigned int data = packet[3 + i];
            printf("%d ", (int)data);
        }
    }

    printf("\n");
    (void) packet;
}

void store_values(unsigned int packets[], char *memory)
{
    (void)packets;
    (void)memory;
}

unsigned int* create_completion(unsigned int packets[], const char *memory)
{
    (void)packets;
    (void)memory;
	return NULL;
}
