#include "strings.h"
#include "../../lib/kernel/types.h"
#include "../../lib/user/user.h"

uint strnlen(const char *const string, const uint max_length)
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

char *strncpy(char *const target_string, const char *const source_string, const uint max_length)
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

char *strncat(char *const target_string, const char *const source_string, const uint max_length)
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

enum strntoul_result
strntoul(uint64 *const target_int, const char *source_string, const uint max_length, uint base)
{
    if (max_length == 0) {
        return STRNTOUL_ERR_NO_NUMBER_WITHIN_MAX_LENGTH;
    }

    int digit = *source_string++;
    uint characters_parsed = 1;

    // Disallow leading - for unsigned integer
    if (digit == '-') {
        return STRNTOUL_ERR_NEGATIVE_NUMBER;
    }

    // Strip leading +
    if (digit == '+') {
        if (characters_parsed >= max_length) {
            return STRNTOUL_ERR_NO_NUMBER_WITHIN_MAX_LENGTH;
        }

        digit = *source_string++;
        characters_parsed++;
    }

    // Strips leading 0x for base 0 or 16
    if ((base == 0 || base == 16) && digit == '0' && (characters_parsed < max_length) &&
        (*source_string == 'x' || *source_string == 'X'))
    {
        if (characters_parsed + 2 > max_length) {
            return STRNTOUL_ERR_NO_NUMBER_WITHIN_MAX_LENGTH;
        }

        base = 16;
        digit = source_string[1];
        source_string += 2;
        characters_parsed += 2;
    }

    // Deduce base from leading digit if base was not given and was not hexadecimal
    if (base == 0) {
        base = digit == '0' ? 8 : 10;
    }

    const uint64 uint64_max = (uint64)-1;

    // Max value to parse for the given base
    uint64 cutoff = uint64_max / (uint64)base;

    // If at cutoff: max remainder to allow (since / rounds down for integers)
    int max_remainder_at_cutoff = uint64_max % (uint64)base;

    uint64 parsed_int = 0;
    while (digit != '\0' && characters_parsed <= max_length) {
        // Exploits the fact that chars are integers, to translate characters to digits
        if (digit >= '0' && digit <= '9') {
            digit -= '0';
        } else if (digit >= 'A' && digit <= 'Z') {
            digit = digit - 'A' + 10;
        } else if (digit >= 'a' && digit <= 'z') {
            digit = digit - 'a' + 10;
        } else {
            return STRNTOUL_ERR_INVALID_DIGIT;
        }

        if (digit >= base) {
            return STRNTOUL_ERR_INVALID_DIGIT;
        }

        if (parsed_int > cutoff || (parsed_int == cutoff && digit > max_remainder_at_cutoff)) {
            return STRNTOUL_ERR_OUT_OF_RANGE;
        }

        parsed_int *= base;
        parsed_int += digit;

        digit = *source_string++;
        characters_parsed++;
    }

    *target_int = parsed_int;
    return STRNTOUL_SUCCESS;
}

char *strntoul_error_message(const enum strntoul_result result)
{
    switch (result) {
    case STRNTOUL_SUCCESS:
        return "Success";
    case STRNTOUL_ERR_NO_NUMBER_WITHIN_MAX_LENGTH:
        return "Failed to parse number from string within provided max length";
    case STRNTOUL_ERR_NEGATIVE_NUMBER:
        return "Cannot parse negative number to unsigned integer";
    case STRNTOUL_ERR_INVALID_DIGIT:
        return "Invalid digit encountered in parsed string";
    case STRNTOUL_ERR_OUT_OF_RANGE:
        return "Parsed number exceeded maximum value for unsigned integer";
    }

    return "Undefined error code";
}
