/**
 * @file version.c
 * @brief Library version information.
 *
 * Provides the libcftext_version() function, which returns a pointer to a
 * static string containing the current library version. This is the same
 * string that is available at compile time through the `LIBCFTEXT_VERSION`
 * macro.
 *
 * Keeping the version string in its own compilation unit allows it to be
 * changed in one place when a new release is cut, without touching the
 * public header (which already contains the identical version information
 * in macro form for compile‑time checks).
 *
 * @version 1.0.0
 */

#include "../include/libcftext.h"

/*===========================================================================
 * PUBLIC FUNCTION IMPLEMENTATION
 *===========================================================================*/

/**
 * @brief Get the library version as a static string.
 *
 * The version string has the format `"MAJOR.MINOR.PATCH"`, e.g. `"1.0.0"`.
 *
 * Because the returned string is statically allocated, it remains valid for
 * the entire lifetime of the program and must not be freed or modified by
 * the caller.
 *
 * @return A null‑terminated string representing the library version.
 *
 * @note This string mirrors the `LIBCFTEXT_VERSION` macro defined in
 *       `libcftext.h`. The macro is suitable for compile‑time checks,
 *       while this function is convenient for runtime display.
 *
 * @par Example
 * @code
 * printf("Using libcftext version %s\n", libcftext_version());
 * @endcode
 */
const char *libcftext_version(void)
{
    return "1.0.0";
}