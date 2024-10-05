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
    else {
        printf("No Output (invalid packet)");
        return;
    }

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
}

void store_values(unsigned int packets[], char *memory)
{

    for (unsigned int i = 0; ; ) {
        unsigned int int0 = packets[i];
        unsigned int int1 = packets[i+1];
        unsigned int int2 = packets[i+2];

        unsigned int packet_type = (int0 >> 24) & 0xFF;
        if (packet_type != 0100)
            break;

        unsigned int address = (int2);
        unsigned int length = int0 & 0x2FF;
        // unsigned int requester_id = (int1 >> 16) & 0xFFFF;
        // unsigned int tag = (int1 >> 8) & 0xFF;
        unsigned int last_BE = (int1 >> 4) & 0x0F;
        unsigned int first_BE = int1 & 0x0F;

        // printf("Address: %u\n", address);
        // printf("Length: %u\n", length);
        // printf("Requester ID: %u\n", requester_id);
        // printf("Tag: %u\n", tag);
        // printf("Last BE: %u\n", last_BE);
        // printf("1st BE: %u\n", first_BE);
        // printf("Data: ");

        // if (packet_type == 0100) {
        //     for (unsigned int j = 0; j < length; j++) {
        //         unsigned int data = packets[i + 3 + j];
        //         printf("%d ", (int)data);
        //     }
        // }

        // printf("\n");

        if (packet_type != 0100)
            break;

        if (address > 1000000) {
            i += 3 + length;
            continue;
        }

        for (unsigned int j = 0; j < length; j++) {
            unsigned int data = packets[i + 3 + j];

            int start_address = address + j * 4;

            if (j == 0) {
                if (first_BE & 0x01) 
                    memory[start_address] = data & 0xFF;
                if (first_BE & 0x02) 
                    memory[start_address + 1] = (data >> 8) & 0xFF;
                if (first_BE & 0x04) 
                    memory[start_address + 2] = (data >> 16) & 0xFF;
                if (first_BE & 0x08) 
                    memory[start_address + 3] = (data >> 24) & 0xFF;
            }
            else if (j == length - 1) {
                if (last_BE & 0x01) 
                    memory[start_address] = data & 0xFF;
                if (last_BE & 0x02) 
                    memory[start_address + 1] = (data >> 8) & 0xFF;
                if (last_BE & 0x04) 
                    memory[start_address + 2] = (data >> 16) & 0xFF;
                if (last_BE & 0x08) 
                    memory[start_address + 3] = (data >> 24) & 0xFF;
            }
            else {
                for (unsigned int byte = 0; byte < 4; byte++) {
                    memory[start_address + byte] = (data >> (byte * 8)) & 0xFF;
                }
            }
        }        
        i += length + 3;
    }
    (void)packets;
    (void)memory;
}

unsigned int* create_completion(unsigned int packets[], const char *memory)
{
    
    (void)packets;
    (void)memory;
	return NULL;
}
