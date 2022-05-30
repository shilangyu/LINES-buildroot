import sys
import threading
import time
from pathlib import Path

import gpiod

LED_CHIP = "gpiochip0"

BUTTON_LINES = [25, 10, 17, 18]
LED_LINES = [24, 22, 23, 27]
MOTION_LINE = 21
CONSUMER = "cool_consumer"

logs_file = Path("motion_activity_server.log")


chip = gpiod.Chip(LED_CHIP)
leds = chip.get_lines(LED_LINES)
motion = chip.get_lines([MOTION_LINE])
button = chip.get_lines([BUTTON_LINES[0]])

system_enabled = True


def blink(off=False):
    if off:
        leds.set_values([0, 0, 0, 0])
    else:
        leds.set_values([1, 1, 1, 1])


def handle_event(event):
    if event.type == gpiod.LineEvent.RISING_EDGE:
        msg = 'DETECTED INTRUDER!'
        evstr = 'RISING EDGE'
        blink()
    elif event.type == gpiod.LineEvent.FALLING_EDGE:
        msg = 'INTRUDER LEFT.'
        evstr = 'FALLING EDGE'
        blink(off=True)
    else:
        raise TypeError('Invalid event type')

    print('event: {} offset: {} timestamp: [{}.{}]'.format(evstr,
                                                           event.source.offset(),
                                                           event.sec, event.nsec))

    with open(logs_file, 'a') as f:
        f.write(f'{msg} timestamp: [{event.sec}.{event.nsec}]\n')


def listen_motion():
    try:
        while True:
            ev_lines = motion.event_wait(sec=1)
            if ev_lines:
                for line in ev_lines:
                    event = line.event_read()
                    if system_enabled:
                        handle_event(event)
    except KeyboardInterrupt:
        sys.exit(130)


def listen_button():
    global system_enabled

    while True:
        ev_lines = button.event_wait(sec=1)
        if ev_lines:
            for line in ev_lines:
                event = line.event_read()

                if event.type == gpiod.LineEvent.RISING_EDGE:
                    system_enabled = not system_enabled

                    leds.set_values([0, 0, 0, 0 if system_enabled else 1])

                    msg = 'System was {}'.format(
                        'enabled' if system_enabled else 'disabled')
                    print(msg)
                    with open(logs_file, 'a') as f:
                        f.write(
                            f'{msg} timestamp: [{event.sec}.{event.nsec}]\n')

                time.sleep(1)


def init_gpio():
    leds.request(consumer=CONSUMER, type=gpiod.LINE_REQ_DIR_OUT)
    motion.request(consumer=CONSUMER, type=gpiod.LINE_REQ_EV_BOTH_EDGES)
    button.request(consumer=CONSUMER, type=gpiod.LINE_REQ_EV_BOTH_EDGES)

    th = threading.Thread(target=listen_button)
    th.start()

    with open(logs_file, "w") as f:
        pass


if __name__ == "__main__":
    init_gpio()
    listen_motion()
