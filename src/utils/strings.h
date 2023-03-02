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

#endif // STRINGS_H
