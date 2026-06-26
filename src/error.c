/**
 * @file error.c
 * @brief Error handling module for libcftext.
 *
 * This file implements the error-state management and message translation
 * functions declared in the public header. It maintains a module-level
 * (static) error code that is set by other parts of the library whenever
 * an operation fails, and provides read access to it via
 * libcftext_get_error().
 *
 * Because the state is stored in a single global variable, the library is
 * **not** thread-safe. In a multithreaded application the user must
 * serialize calls to `is_text_file()` and the associated error functions.
 *
 * @version 1.0.0
 */

#include "../include/libcftext.h"

/*===========================================================================
 * MODULE-LEVEL ERROR STATE
 *===========================================================================*/

/**
 * @brief Holds the last error code produced by any library function.
 *
 * Initialised to `LIBCFTEXT_OK` (0) at load time, meaning “no error”.
 *
 * It is updated exclusively via `libcftext_set_error()` and read via
 * `libcftext_get_error()`. The storage class is `static` so that the
 * variable has file scope – only this translation unit can access it
 * directly, preserving encapsulation.
 */
static int last_error = LIBCFTEXT_OK;

/*===========================================================================
 * PUBLIC FUNCTION IMPLEMENTATIONS
 *===========================================================================*/

/**
 * @brief Return the current error code.
 *
 * The returned value is one of the `LIBCFTEXT_ERR_*` macros:
 * - #LIBCFTEXT_OK (0)
 * - #LIBCFTEXT_ERR_NULL_PTR (1)
 * - #LIBCFTEXT_ERR_OPEN (2)
 * - #LIBCFTEXT_ERR_READ (3)
 * - #LIBCFTEXT_ERR_UNKNOWN (99) – rarely used, reserved.
 *
 * @return The last error code set by a library function.
 *
 * @note The error code persists until the next library call that
 *       modifies it. Always read it immediately after a function
 *       returns an error indication (e.g., after `is_text_file()`
 *       returns -1).
 */
int libcftext_get_error(void)
{
    return last_error;
}

/**
 * @brief Set the internal error code.
 *
 * This function is **meant for internal use by the library** (called from
 * `core.c` when an error is detected). It is exposed in the public header
 * so that the implementation can be kept in a separate translation unit.
 *
 * Regular library users should **not** call this function – doing so would
 * overwrite the real error state and lead to misleading diagnostics.
 *
 * @param[in] err One of the `LIBCFTEXT_ERR_*` error codes.
 */
void libcftext_set_error(int err)
{
    last_error = err;
}

/**
 * @brief Convert an error code to a human-readable string.
 *
 * This function maps the numeric error codes used internally to short,
 * descriptive English strings that can be shown directly to an end user
 * or written to a log file.
 *
 * The returned pointer points to a statically allocated string literal;
 * **do not free it** and **do not modify it**.
 *
 * @param[in] err An error code, e.g. #LIBCFTEXT_ERR_OPEN.
 *
 * @return A null-terminated string describing the error. If the code is
 *         not recognised, `"Unknown error"` is returned.
 *
 * @par Example
 * @code
 * int err = libcftext_get_error();
 * printf("Failed: %s\n", libcftext_strerror(err));
 * @endcode
 */
const char *libcftext_strerror(int err)
{
    switch (err) {
    case LIBCFTEXT_OK:
        return "No error";
    case LIBCFTEXT_ERR_NULL_PTR:
        return "Null pointer argument";
    case LIBCFTEXT_ERR_OPEN:
        return "Cannot open file";
    case LIBCFTEXT_ERR_READ:
        return "Read error";
    default:
        return "Unknown error";
    }
}