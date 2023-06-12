# GPS TripMeter
A tripmeter that uses GPS to calculate distance traveled and current speed.

It is possible to reset the trip by pressing the button

The data are shown on a 1.8" TFT display

![TripMeter](tripMeter_gif.gif)

## Motivation

Need to calculate the distance traveled in order to follow road notes.

## Display wiring

| Dsiplay | Arduino Nano |
|---------|--------------|
| VCC     | 5V           |
| GND     | GND          |
| CS      | D10          |
| RESET   | D8           |
| A0      | D9           |
| SDA     | D11          |
| SCK     | D13          |
| LED     | 3.3V         |


## GPS Wiring

| GPS | Arduino Nano |
|-----|--------------|
| VCC | 5V           |
| GND | GND          |
| TX  | D6           |
| RX  | D7           |


## Button Wiring

| Button | Arduino Nano           |
|--------|------------------------|
| VCC    | 5V                     |
| GND    | GND + 10K Ohm Resistor |
| PIN    | D2                     |


## Hardware

- Arduino Nano
- Touchscreen Display of your choice.
- GPS module