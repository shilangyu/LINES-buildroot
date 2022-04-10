#include <gpiod.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ERR(source)                                  \
    (perror(source),                                 \
     fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
     kill(0, SIGKILL),                               \
     exit(EXIT_FAILURE))

#define PROGRAM_SIZE 120

void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

void greet(char **argv) {
    printf("Welcome to %s, a small programmable language for flashing your diodes\n"
           "\n"
           "Buttons on the Raspi trigger the following actions:\n"
           "\tTODO) Start programming\n"
           "\tTODO) Run saved program\n"
           "\tTODO) Change frame duration, cycles between the following values: 1s [default], 500ms, 100ms\n"
           "\n"
           "The programming language consists of single-char instructions, available instructions:\n"
           "\t# - a single digit, index of the diode to turn on (1-indexed), possible values: [1-9], eg. `1`, `2`, `9`\n"
           "\t| - starts a new animation frame\n"
           "\n"
           "In each frame individual diodes can be turned on, state of a diode is not retained between frames. The program has always an implicit `|` at the very beginning which starts your first animation frame. Each animation frame has a fixed duration. If diode instructions repeat in a single frame, the behavior is equivalent as if it was specified only once. Within a single frame order of diodes plays no role.\n"
           "\n"
           "Example programs:\n"
           "\t1234||1234 - flashes 4 diodes, turns them off, and turned them back on\n"
           "\t1|2|3|4    - flashes a single diode per frame\n",
           argv[0]);
}

void read_program(void) {
    char buffer[PROGRAM_SIZE + 1];

    printf("Start programming:\n");
    while (1) {
        printf("> ");

        if (fgets(buffer, PROGRAM_SIZE + 1, stdin) == NULL) {
            ERR("fgets");
        }

        if (strlen(buffer) == PROGRAM_SIZE && buffer[PROGRAM_SIZE - 1] != '\n') {
            printf("Your program is too long, some instructions were cut-off, try again. Max program size: %d\n", PROGRAM_SIZE);
            flush_stdin();
        } else {
            // ignore trailing \n
            buffer[strlen(buffer) - 1] = '\0';

            bool valid = true;
            size_t i;

            for (i = 0; valid && i < strlen(buffer); i++) {
                switch (buffer[i]) {
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case '|':
                    break;
                default:
                    valid = false;
                    break;
                }
            }

            if (!valid) {
                printf("Given program is invalid, unrecognized instruction: %c\n", buffer[i - 1]);
            } else {
                break;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    greet(argv);

    read_program();

    return 0;
}
