/**
 * @file core.c
 * @brief Core implementation of the text/binary detection function.
 *
 * This file implements is_text_file(), which examines the raw bytes of a file
 * and determines whether its content is text or binary. The heuristic is based
 * on the presence of null bytes and the proportion of non-printable characters.
 *
 * @version 1.0.1
 */

#include <stdio.h>      /* FILE, fopen, fclose, fread, ferror, NULL */
#include <stddef.h>     /* size_t */
#include "../include/libcftext.h"   /* Public API, error codes, libcftext_set_error() */

/*===========================================================================
 * LOCAL CONFIGURATION MACROS
 *===========================================================================*/

/**
 * @brief Read buffer size in bytes.
 *
 * 4 KB is a typical filesystem block size, offering a good balance between
 * I/O performance and stack memory usage. Can be adjusted for specific needs.
 */
#define BUFFER_SIZE               4096

/**
 * @brief Determines if a byte is considered "printable" within a text file.
 *
 * Bytes that are considered normal for a text file:
 * - ASCII printable characters (32–126): letters, digits, punctuation, space.
 * - Horizontal tab (9), line feed (10), carriage return (13).
 * - High bytes >= 128 (assumed to be part of multi-byte encodings like UTF-8).
 *
 * Bytes NOT included (suspicious binary control characters):
 * - Other control characters like bell (7), backspace (8), escape (27),
 *   delete (127), and anything else below 32 that is not explicitly listed.
 * - Null byte (0) is handled separately because it immediately marks a binary file.
 */
#define IS_PRINTABLE(c)          (((c) >= 32 && (c) <= 126) || \
                                  (c) == '\t' || (c) == '\n' || (c) == '\r' || \
                                  (c) >= 128)

/**
 * @brief Maximum allowed percentage of non-printable bytes.
 *
 * If the percentage of bytes that fail the IS_PRINTABLE check exceeds this
 * threshold, the file is classified as binary. The value 10% is a common
 * heuristic used by tools like the Unix `file` command.
 */
#define MAX_NONPRINTABLE_PERCENT 10

/*===========================================================================
 * PUBLIC FUNCTION IMPLEMENTATION
 *===========================================================================*/

/**
 * @brief Check whether a file is text or binary.
 *
 * Algorithm overview:
 * 1. Open the file in binary mode ("rb") to avoid text mode translations.
 * 2. Read the file in chunks and scan for a null byte (0x00). If found,
 *    close the file and immediately return 0 (binary).
 * 3. Count the number of "printable" bytes (as defined by IS_PRINTABLE)
 *    and the total number of bytes processed.
 * 4. After the read loop, check for a read error using ferror().
 * 5. Close the file. An empty file (total == 0) is considered text.
 * 6. Compute the percentage of non-printable bytes. If it exceeds
 *    MAX_NONPRINTABLE_PERCENT (10%), return 0 (binary). Otherwise, return 1 (text).
 *
 * @param[in] filename Path to the file to examine (null-terminated C string).
 *                     Must not be NULL.
 * @return 1 if the file is text (or empty),
 *         0 if the file is binary,
 *         -1 on error (call libcftext_get_error() for the specific error code).
 *
 * @note Thread safety: This function calls libcftext_set_error(), which
 *       modifies a global error state. It is therefore not safe to use
 *       concurrently from multiple threads without external synchronization.
 */
int is_text_file(const char *filename)
{
    /* --- Parameter validation --- */
    if (filename == NULL) {
        /* NULL pointer is not allowed. Set error and return -1. */
        libcftext_set_error(LIBCFTEXT_ERR_NULL_PTR);
        return -1;
    }

    /* --- Open the file --- */
    /*
     * Use binary mode "rb" to read the raw bytes exactly as stored.
     * On Windows this prevents automatic conversion of CRLF to LF,
     * which could hide null bytes or alter the printable count.
     */
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        /* File could not be opened (missing, permission denied, etc.) */
        libcftext_set_error(LIBCFTEXT_ERR_OPEN);
        return -1;
    }

    /* --- Initialize buffer and counters --- */
    unsigned char buf[BUFFER_SIZE];  /* Stack-allocated read buffer; no dynamic allocation overhead */
    size_t n;                        /* Bytes read in the current chunk */
    size_t printable = 0;            /* Running count of printable bytes */
    size_t total = 0;                /* Total bytes processed so far */

    /* --- Main read-analyze loop --- */
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        for (size_t i = 0; i < n; i++) {
            unsigned char c = buf[i];

            /* Null byte detection: if we see 0x00, the file is definitely binary. */
            if (c == 0) {
                fclose(f);   /* Close the file before returning */
                return 0;
            }

            /* Increment printable count if the byte passes the heuristic check */
            if (IS_PRINTABLE(c))
                printable++;

            /* Always increment total; this byte was examined (and wasn't a null byte) */
            total++;
        }
    }

    /* --- Check for read errors --- */
    if (ferror(f)) {
        /* The loop terminated because of an I/O error, not a clean EOF. */
        libcftext_set_error(LIBCFTEXT_ERR_READ);
        fclose(f);
        return -1;
    }

    /* --- Close the file normally --- */
    fclose(f);

    /* --- Handle empty file --- */
    if (total == 0)
        return 1;   /* An empty file contains no suspect bytes → text */

    /* --- Calculate percentage of non-printable bytes --- */
    size_t non_printable = total - printable;

    /*
     * Use 100ULL to force the multiplication to be done in the domain of
     * unsigned long long, preventing overflow on platforms where size_t is
     * narrower (e.g., 32-bit systems). The division is safe because total > 0.
     */
    if ((non_printable * 100ULL) / total > MAX_NONPRINTABLE_PERCENT)
        return 0;   /* Too many non-printable characters → binary */

    /* All checks passed; the file is considered a text file */
    return 1;
}