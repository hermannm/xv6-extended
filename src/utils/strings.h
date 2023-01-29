#ifndef STRINGS_H
#define STRINGS_H

#include "../../lib/kernel/types.h"

uint strnlen(const char* string, uint max_length);

char* strncpy(char* target_string, const char* source_string, uint max_length);

char* strncat(char* target_string, const char* source_string, uint max_length);

#endif // STRINGS_H
