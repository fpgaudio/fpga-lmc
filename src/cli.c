#include "cli.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

const static char HELP_MESSAGE_FMT[] =
"Usage: %s --com COM-PORT [--verbose]\n\n"
""
"Parses and serializes LMC data into useful joint angles.\n"
"Options:\n"
"    --com, -c COM-PORT: The port to write serialized data into.\n"
"    --baud-rate, -b BAUD-RATE: The Baud Rate To Use in Hz\n"
"    --verbose, -v: Dumps the LMC data to the console.\n"
"\n%s\n";

cli_args_t cli_parse_args(int argc, char** argv) {
    struct cli_args args = {
        .com_port = NULL,
        .verbose = false,
        .baud_rate = 0
    };

    for (size_t i = 1; i < argc; i++) {
        const char* cur = argv[i];
        if (strcmp(cur, "--com") == 0 || strcmp(cur, "-c") == 0) {
            // The next argument should be a COM port.
            if (i == argc - 1) {
                // If we are missing the next argument, it is invalid.
                fprintf(stderr, HELP_MESSAGE_FMT, argv[0],
                        "No COM port given.");
                return NULL;
            }

            // Otherwise, populate the COM port and skip the next argument.
            args.com_port = argv[i + 1];
            i++;
        }
        else if (strcmp(cur, "--baud-rate") == 0 || strcmp(cur, "-b") == 0) {
            // Attempt to convert to integer.
            const unsigned long br = strtoul(argv[i + 1], NULL, 0);
            if (br == 0 || br == ULONG_MAX) {
                fprintf(stderr, HELP_MESSAGE_FMT, argv[0],
                        "Could not parse the baud rate.");
                return NULL;
            }

            args.baud_rate = br;
            i++;
        }
        else if (strcmp(cur, "--verbose") == 0 || strcmp(cur, "-v") == 0) {
            args.verbose = true;
        }
        else {
            // There is an unknown argument.
            fprintf(stderr, HELP_MESSAGE_FMT, argv[0],
                    "Unknown argument provided.");
            return NULL;
        }
    }

    if (args.com_port == NULL || args.baud_rate == 0) {
        fprintf(stderr, HELP_MESSAGE_FMT, argv[0], "");
        return NULL;
    }

    // args is populated, move it to the heap.
    cli_args_t h_args = (cli_args_t)malloc(sizeof(args));
    memcpy(h_args, &args, sizeof(args));
    return h_args;
}