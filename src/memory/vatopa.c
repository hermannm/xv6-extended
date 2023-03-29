/**
 * @file
 * Takes a virtual memory address and an optional process ID, and prints the corresponding physical
 * address from the process's pagetable. If no process ID is provided, gets the address from the
 * current process.
 */

#include "../../lib/user/user.h"
#include "../utils/strings.h"
#include "sys_vatopa.h"

#define MAX_VIRTUAL_ADDRESS_LENGTH 20

int main(int argc, char **argv)
{
    if (argc < 2 || argc > 3) {
        printf("Usage: vatopa virtual_address [pid]\n");
        exit(1);
    }

    char *virtual_address_string = argv[1];
    if (strnlen(virtual_address_string, MAX_VIRTUAL_ADDRESS_LENGTH + 1) >
        MAX_VIRTUAL_ADDRESS_LENGTH)
    {
        printf("Provided virtual address exceeded maximum length %d\n", MAX_VIRTUAL_ADDRESS_LENGTH);
        exit(1);
    }

    uint64 virtual_address = 0;

    enum strntoul_result parse_result =
        strntoul(&virtual_address, virtual_address_string, MAX_VIRTUAL_ADDRESS_LENGTH, 10);

    if (parse_result != STRNTOUL_SUCCESS) {
        char *error_message = strntoul_error_message(parse_result);
        printf("Failed to parse provided virtual address: %s\n", error_message);
    }

    int process_id = 0;
    if (argc == 3) {
        process_id = atoi(argv[2]);
    }

    uint64 physical_address = vatopa(virtual_address, process_id);

    printf("0x%x\n", physical_address);

    exit(0);
}