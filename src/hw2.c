#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "hw2.h"

void print_binary(unsigned int word) {
    unsigned int dword = word;
    for (int bit = 0; bit < 32; bit++) {
        printf("%d", word & 0x80000000 ? 1 : 0);
        word <<= 1;
    }
    printf(" %d", dword);
    printf("\n");
}

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
    if (!completions) return NULL;
    int index = 0;

    for (int i = 0; i < 6; i += 3 ) {
        
        unsigned int int0 = packets[i];
        unsigned int int1 = packets[i + 1];
        unsigned int int2 = packets[i + 2];

        unsigned int packet_type = (int0 >> 24) & 0xFF;
        if (packet_type != 00)
            break;

        unsigned int address = int2;
        unsigned int length = int0 & 0x3FF;
        unsigned int requester_id = (int1 >> 16); 
        unsigned int tag = (int1 >> 8) & 0xFF;
        unsigned int byte_count = length * 4;
        unsigned int last_BE = (int1 >> 4) & 0x0F;
        unsigned int first_BE = int1 & 0x0F;

        printf("%d %d\n", (int)first_BE, (int)last_BE);
        unsigned int not_first_BE = 0;
        if (!(first_BE & 0x01))
            not_first_BE++;
        else if (!(first_BE & 0x02))
            not_first_BE++;
        else if (!(first_BE & 0x04))
            not_first_BE++;
        else if (!(first_BE & 0x08))
            not_first_BE++;
        unsigned int not_last_BE = 0;
        if (!(last_BE & 0x01))
            not_last_BE++;
        else if (!(last_BE & 0x02))
            not_last_BE++;
        else if (!(last_BE & 0x03))
            not_last_BE++;
        else if (!(last_BE & 0x04))
            not_last_BE++;

        unsigned int remaining_bytes = byte_count;
        unsigned int current_address = address;

        unsigned int read_length = remaining_bytes / 4;
        if (current_address % 0x4000 != 0) {
            unsigned int to_boundary = 0x4000 - (current_address % 0x4000);
            to_boundary = to_boundary < remaining_bytes ? to_boundary : remaining_bytes;
            read_length = to_boundary / 4; 
        }

        unsigned int lower_address = current_address & 0x7F;
        completions[index++] = 0x4A000000 | read_length;
        completions[index++] = 0x00DC0000 | (remaining_bytes - not_first_BE - not_last_BE);
        completions[index++] = (((requester_id << 8) | tag) << 8) | lower_address;

        printf("read_length: %d\n", read_length);
        for (unsigned int j = 0; j < read_length; j++) {
            unsigned int data = 0;
            for (int k = 3; k >= 0; k--) {
                data |= (unsigned int) (memory[current_address + (4 * j) + k] & 0xFF) << (k * 8);
            }
            completions[index++] = data;
        }
        
        remaining_bytes -= read_length * 4;
        current_address += read_length * 4;

        if (read_length != length) { 
            lower_address = current_address & 0x7F;
            completions[index++] = 0x4A000000 | remaining_bytes / 4;
            completions[index++] = 0x00DC0000 | remaining_bytes;
            completions[index++] = (((requester_id << 8) | tag) << 8) | lower_address;

            for (unsigned int j = 0; j < remaining_bytes; j++) {
                unsigned int data = 0;
                for (int k = 3; k >= 0; k--) {
                    data |= (unsigned int) (memory[current_address + (j * 4) + k]  & 0xFF) << (8 * k);
                }
                completions[index++] = data;
            }
        }


        // i += 3;
    }
    (void)packets;
    (void)memory;
    for (int i = 0; i < 30; i++) {
        print_binary(completions[i]);
    }

	return completions;
}