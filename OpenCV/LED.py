#Robert Nguyen
#Embedded Systems
#Lab11_Blink_LED
#7/15/24
from gpiozero import LED
import time

LEDs = [LED(14)]


while True:  # Loop indefinitely
    for x in range(4):  # 4 short pulses
        LEDs[0].on  # Turn on the LED
        time.sleep(0.5)  # Wait for 0.5 seconds
        LEDs[0].off  # Turn off the LED
        time.sleep(0.5)  # Wait for 0.5 seconds