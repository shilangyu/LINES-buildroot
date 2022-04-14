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

static const char CHIP[] = "gpiochip0";

unsigned int button_lines[] = {25, 10, 17, 18};
unsigned int led_lines[]    = {24, 22, 23, 27};

struct gpio_context_t {
    struct gpiod_line_bulk *buttons;
    struct gpiod_line_bulk *leds;
};

char program[PROGRAM_SIZE];

void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

void greet(char **argv) {
    printf("Welcome to %s, a small programmable language for flashing your LEDs\n"
           "\n"
           "Buttons on the Raspi trigger the following actions:\n"
           "\t%d) Start programming\n"
           "\t%d) Run saved program\n"
           "\t%d) Change frame duration, cycles between the following values: 1s [default], 500ms, 100ms\n"
           "\n"
           "The programming language consists of single-char instructions, available instructions:\n"
           "\t# - a single digit, index of the LED to turn on (1-indexed), possible values: [1-9], eg. `1`, `2`, `9`\n"
           "\t| - starts a new animation frame\n"
           "\n"
           "In each frame individual LEDs can be turned on, state of a LED is not retained between frames. The program has always an implicit `|` at the very beginning which starts your first animation frame. Each animation frame has a fixed duration. If LED instructions repeat in a single frame, the behavior is equivalent as if it was specified only once. Within a single frame order of LEDs plays no role.\n"
           "\n"
           "Example programs:\n"
           "\t1234||1234 - flashes 4 LEDs, turns them off, and turned them back on\n"
           "\t1|2|3|4    - flashes a single LED per frame\n",
           argv[0],
           button_lines[0],
           button_lines[1],
           button_lines[2]);
}

void run_program(struct gpiod_line_bulk *leds) {
    printf("Running program %s\n", program);

    for (int i = 0; i < strlen(program); i++) {
        switch (program[i]) {
        case '1':
        case '2':
        case '3':
        case '4': {
            struct gpiod_line *curr = leds->lines[program[i] - '1'];

            int ret = gpiod_line_set_value(curr, 1);

            if (ret) {
                ERR("gpiod_line_set_value");
            }
            break;
        }
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            printf("Following LED does not exist: %c\n", program[i]);
            break;
        case '|':
            sleep(1);
            break;
        default:
            ERR("Unexpected instruction");
            break;
        }


        for (int i = 0; i < leds->num_lines; i++) {
            int ret = gpiod_line_set_value(leds->lines[i], 0);
            if (ret) {
                ERR("gpiod_line_set_value");
            }
        }
    }
}

void read_program(void) {
    char buffer[PROGRAM_SIZE + 1];

    printf("Start programming:\n");
    while (1) {
        printf("> ");

        if (fgets(buffer, PROGRAM_SIZE + 1, stdin) == NULL) {
            ERR("fgets");
        }

        if (strlen(buffer) == 1) {
            printf("Empty program!\n");
        } else if (strlen(buffer) == PROGRAM_SIZE && buffer[PROGRAM_SIZE - 1] != '\n') {
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

    strcpy(program, buffer);
}

static void handle_event(struct gpio_context_t *ctx,
                         int event_type,
                         unsigned int line,
                         const struct timespec *timestamp) {
    if (line == button_lines[0]) {
        read_program();
    } else if (line == button_lines[1]) {
        run_program(ctx->leds);
    } else if (line == button_lines[2]) {
        // TODO: change frame rate
    } else {
        ERR("Unexpected line event");
    }

    printf("Something happened %d!\n", line);
}

static int event_callback(int event_type, unsigned int line,
                          const struct timespec *timestamp, void *data) {
    struct gpio_context_t *ctx = data;

    printf("Got event %d\n", line);

    switch (event_type) {
    case GPIOD_CTXLESS_EVENT_CB_FALLING_EDGE:
        handle_event(ctx, event_type, line, timestamp);
        break;
    default:
        ERR("Unexpected gpio event");
        return GPIOD_CTXLESS_EVENT_CB_RET_STOP;
    }

    return GPIOD_CTXLESS_EVENT_CB_RET_OK;
}

int main(int argc, char *argv[]) {
    int ret;
    greet(argv);

    struct timespec timeout = {100000, 0};

    struct gpiod_chip *chip;
    struct gpiod_line_bulk buttons;
    struct gpiod_line_bulk leds;

    chip = gpiod_chip_open_lookup(CHIP);
    if (!chip) {
        ERR("chip_open_lookup");
    }

    // TODO: segmentation fault here
    ret = gpiod_chip_get_lines(chip, button_lines, 4, &buttons);
    if (ret) {
        ERR("gpiod_chip_get_lines:buttons");
    }

    ret = gpiod_chip_get_lines(chip, led_lines, 4, &leds);
    if (ret) {
        ERR("gpiod_chip_get_lines:butledstons");
    }

    for(int i = 0; i < 4; i++) {
        ret = gpiod_line_request_output(leds.lines[i], "CONSUMER", 0);
    if (ret) {
        ERR("gpiod_chip_get_lines:butledstons");
    }
    }


    struct gpio_context_t ctx = {
        .buttons = &buttons,
        .leds    = &leds,
    };

    ret = gpiod_ctxless_event_monitor_multiple_ext(
        CHIP,
        GPIOD_CTXLESS_EVENT_FALLING_EDGE,
        button_lines,
        4,
        false,
        "cool_marcin",
        &timeout,
        NULL, // TODO: check if this is important
        event_callback,
        &ctx,
        0 // TODO: check if the flag should be different
    );

    if (ret) {
        ERR("gpiod_ctxless_event_monitor_multiple_ext");
    }

    return 0;
}
