/**
 * This header defines functions used for CLI behavior.
 */

#ifndef CLI_H
#define CLI_H

#include <stdbool.h>

struct cli_args {
    char* com_port;
    unsigned long baud_rate;
    bool verbose;
};

typedef struct cli_args* cli_args_t;

/**
 * Parses the command line arguments.
 * 
 * @return A pointer to the parsed arguments. Must be freed.
 *         If the arguments are malformed, this will return NULL and print
 *         the error message.
 */
cli_args_t cli_parse_args(int argc, char** argv);

#endif CLI_H