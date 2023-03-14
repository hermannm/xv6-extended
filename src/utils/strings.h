#ifndef STRINGS_H
#define STRINGS_H

#include "../../lib/kernel/types.h"

/**
 * Counts the length of the given string, but only up to `max_length`.
 */
uint strnlen(const char *string, uint max_length);

/**
 * Copies at most `max_length` characters from `source_string` to `target_string`, then returns
 * `target_string`. Ensures that the returned string is properly null terminated.
 */
char *strncpy(char *target_string, const char *source_string, uint max_length);

/**
 * Concatenates at most `max_length` characters from `source_string` to `target_string`, then
 * returns `target_string`. Ensures that the returned string is properly null terminated.
 *
 * Adapted from Apple's libc implementation:
 * https://opensource.apple.com/source/Libc/Libc-262/ppc/gen/strncat.c.auto.html
 */
char *strncat(char *target_string, const char *source_string, uint max_length);

enum strntoul_result {
    STRNTOUL_SUCCESS = 0,
    STRNTOUL_ERR_NO_NUMBER_WITHIN_MAX_LENGTH = -1,
    STRNTOUL_ERR_NEGATIVE_NUMBER = -2,
    STRNTOUL_ERR_INVALID_DIGIT = -3,
    STRNTOUL_ERR_OUT_OF_RANGE = -4,
};

/**
 * Attempts to parse the given `source_string`, up to `max_length` characters, as a number of the
 * given `base`, into the unsigned long `target_int`. If base 0 is provided, tries to deduce the
 * base from the string, defaulting to base 10.
 *
 * If parsing failed, returns one of the error codes from `strntoul_result`.
 *
 * Adapted from Apple's libc implementation of `strtoul` and `strtol`:
 * - https://opensource.apple.com/source/Libc/Libc-262/stdlib/strtoul.c.auto.html
 * - https://opensource.apple.com/source/Libc/Libc-825.25/stdlib/FreeBSD/strtol.c.auto.html
 */
enum strntoul_result
strntoul(uint64 *target_int, const char *source_string, uint max_length, uint base);

/**
 * Returns an appropriate error message for the given result of a `strntoul` call.
 * Assumes the result has already been checked as not a success.
 */
char *strntoul_error_message(const enum strntoul_result result);

#endif // STRINGS_H
