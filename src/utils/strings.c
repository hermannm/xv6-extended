#include "strings.h"
#include "../../lib/kernel/types.h"
#include "../../lib/user/user.h"

uint strnlen(const char* const string, const uint max_length)
{
    int length = 0;

    while (length < max_length) {
        if (string[length] == '\0') {
            break;
        }

        length++;
    }

    return length;
}

char* strncpy(char* const target_string, const char* const source_string, const uint max_length)
{
    for (int i = 0; i < max_length; i++) {
        char source_char = source_string[i];
        target_string[i] = source_char;

        if (source_char == '\0') {
            return target_string;
        }
    }

    target_string[max_length] = '\0';
    return target_string;
}

char* strncat(char* const target_string, const char* const source_string, const uint max_length)
{
    uint target_length = strlen(target_string);
    uint source_length = strlen(source_string);

    if (source_length < max_length) {
        strcpy(&target_string[target_length], source_string);
    } else {
        strncpy(&target_string[target_length], source_string, max_length);
        target_string[target_length + max_length] = '\0';
    }

    return target_string;
}
