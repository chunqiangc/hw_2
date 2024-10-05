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
        unsigned int last_BE = (int1 >> 4) & 0x0F;
        unsigned int first_BE = int1 & 0x0F;

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
    unsigned int* completions = (unsigned int *)malloc(1000000 * sizeof(unsigned int));
    int index = 0;

    for (int i = 0; ; ) {
        
        unsigned int int0 = packets[i];
        unsigned int int1 = packets[i + 1];
        unsigned int int2 = packets[i + 2];

        unsigned int packet_type = (int0 >> 24) & 0xFF;
        if (packet_type != 00)
            break;

        unsigned int address = (int2 >> 2);
        unsigned int length = int0 & 0x2FF;
        unsigned int requester_id = (int1 >> 16) & 0xFFFF;
        unsigned int tag = (int1 >> 8) & 0xFF;
        unsigned int lower_address = address & 0x7F;
        unsigned int byte_count = length * 4;

        unsigned int beforeBound = 0;
        for (unsigned int j = address; j < address + length * 4; j++) {
            if (j == 0x4000) {
                beforeBound = (j - address) / 4;
                break;
            }
            beforeBound = length;
        }

        completions[index++] = 0x4A000000 | length;
        completions[index++] = 0x00DC0000 | byte_count;
        completions[index++] = (((requester_id << 8) | tag) << 8) | lower_address;

        for (unsigned int j = 0; j < beforeBound; j++) {
            unsigned int data = 0;
            for (unsigned int k = 0; k < 4; k++) {
                data = data | (memory[address + j * 4 + k] << (8 * k));
            }
            completions[index++] = data;
        }
        
        if (beforeBound != length) {
            unsigned int afterBound = length - beforeBound;
            byte_count = afterBound * 4;
            lower_address = 0x4000 & 0x7F;

            completions[index++] = 0x4A000000 | afterBound;
            completions[index++] = 0x00DC0000 | byte_count;
            completions[index++] = (((requester_id << 8) | tag) << 8) | lower_address;

            for (unsigned int j = 0; j < afterBound; j++) {
                unsigned int data = 0;
                for (unsigned int k = 0; k < 4; k++) {
                    data = data | (memory[0x4000 + k] << (8 * k));
                }
                completions[index++] = data;
            }
        }
    

        i += length + 3;
    }
        // if (packet_type == 00)
        //     printf("Packet Type: Read\n");
        // printf("Address: %u\n", address);
        // printf("Length: %u\n", beforeBound);
        // printf("Requester ID: %u\n", requester_id);
        // printf("Tag: %u\n", tag);
        // printf("Data: \n");


        // for (int j = 0; j < index; j++ ) {
        //     for (int k = 0; k < 32; ++k) {
        //         if (completions[j] >> k & 0x1) putchar('1');
        //         else putchar('0');
        //     }
        //     printf("\n");
        // }
    (void)packets;
    (void)memory;
	return completions;
}
