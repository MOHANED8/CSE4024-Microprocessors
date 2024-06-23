# STM32F401 Projects

## TASK 1: Three-bit Binary Counter and LED Pattern

### Part i: Three-bit Binary Counter
Create a three-bit binary counter using 3 LED lights. The counter will use 3 output pins to connect LEDs and the blue user button on the board to pause and resume the counter.

**Components:**
- 1x STM32F401
- 3x LEDs
- 1x Button
- Jumper cables

**Steps:**
1. Choose 3 output pins on the STM32F401 board.
2. Connect the LEDs to the chosen output pins.
3. Configure the blue user button to pause and resume the counter.
4. Implement the counter logic to update the LEDs in binary format.

**Video Reference:**
[Three-bit Binary Counter](https://github.com/MOHANED8/CSE4024-Microprocessors/assets/79280405/c828764c-975d-4418-99c0-474825fb8740)

### Part ii: LED Pattern with Custom Delay
Use the same LEDs and ports to create the LED pattern shown in the reference video. Ensure there is a 0.5-second delay between LED updates.

**Requirements:**
- Create a custom delay function using a TIMER. Do not use the `HAL_Delay()` function.

**Components:**
- 1x STM32F401
- 3x LEDs
- 1x Button
- Jumper cables

**Steps:**
1. Reuse the LEDs and output pins from Part i.
2. Implement the custom delay function using a TIMER.
3. Create the LED pattern as shown in the reference video, ensuring a 0.5-second delay between updates.

**Video Reference:**
[LED Pattern with Custom Delay](https://github.com/MOHANED8/CSE4024-Microprocessors/assets/79280405/c0925c79-6e9f-48a0-8660-03f6f767fab2)

## TASK 2: Analog Voltage Reading and LED Brightness Control

Read an analog voltage value from an ADC input connected to a potentiometer. The potentiometer applies a voltage in the range of 0--3.3V (VDD). Convert the voltage value into a digital value using the Analog-to-Digital Converter (ADC). Adjust the brightness of two LEDs based on the digital value: one LED dims while the other brightens.

**Components:**
- 1x STM32F401
- 2x LEDs
- 1x Potentiometer
- Jumper cables

**Steps:**
1. Connect the potentiometer to an ADC input on the STM32F401 board.
2. Implement the ADC conversion to read the analog voltage value and convert it to a digital value.
3. Connect the LEDs to two output pins on the STM32F401 board.
4. Implement the logic to adjust the brightness of the two LEDs based on the digital value from the ADC:
   - One LED starts at full brightness and the other at zero brightness.
   - As the analog voltage changes, the first LED dims and the second LED brightens proportionally.
5. Refer to the video for the desired behavior.

**Video Reference:**
[Analog Voltage Reading and LED Brightness Control](https://github.com/MOHANED8/CSE4024-Microprocessors/assets/79280405/d431cb3a-a333-421a-a729-3c1291402b9b)

