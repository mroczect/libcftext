/**
 * @file main.c
 * @brief Command-line interface to the libcftext library.
 *
 * This small utility takes a filename as an argument, calls
 * is_text_file() on it, and prints either "text" or "binary" to
 * standard output. On error, it prints an error message and exits
 * with a non-zero status.
 *
 * Usage:
 * @code
 * ./demo <file>
 * @endcode
 *
 * @version 1.0.3
 */

#include <stdio.h>
#include "./include/libcftext.h"

/**
 * @brief Program entry point.
 *
 * @param argc Argument count.
 * @param argv Argument vector. argv[1] should be the path to the file
 *             to examine.
 * @return 0 on success,
 *         1 if no filename was given (usage error),
 *         2 if is_text_file() reported a library error.
 */
int main(int argc, char *argv[])
{
    /* --- Validate command-line arguments --- */
    if (argc < 2) {
        /*
         * No filename provided. Print a short usage message and the
         * library version for reference.
         */
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        fprintf(stderr, "Version: %s\n", libcftext_version());
        return 1;   /* Indicates a usage error to the caller/shell */
    }

    /* --- Call the library to test the file --- */
    int result = is_text_file(argv[1]);

    if (result == -1) {
        /*
         * The library encountered an error while processing the file.
         * Retrieve the error code, translate it to a human-readable
         * message, and print it to stderr. Exit with status 2 to
         * distinguish this from a usage error.
         */
        int err = libcftext_get_error();
        fprintf(stderr, "Error: %s\n", libcftext_strerror(err));
        return 2;
    }

    /*
     * No error: result is either 1 (text) or 0 (binary).
     * Print the corresponding label to stdout.
     */
    printf("%s\n", result ? "text" : "binary");

    return 0;   /* Successful execution */
}