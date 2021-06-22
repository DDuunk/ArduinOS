#ifndef UTILS_H_
#define UTILS_H_

#include "Arduino.h"

/**
 * @brief Print ASCII strings
 * @param buffer
 * @param show_placeholder
 */
void PrintAscii(char* buffer, bool show_placeholder = true);

/**
 * @brief Prints error message in format [__scope__:Error] __error_msg__
 * @param scope (file)
 * @param data
 * @param new_line - For longer error messages that are unsupported by this function
 */
void ThrowError(char* scope, char* data, bool new_line = true);

#endif //UTILS_H_
