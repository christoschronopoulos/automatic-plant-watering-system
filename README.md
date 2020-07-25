# automatic-plant-watering-system
## Installation
The code is tested with an Arduino Mega and an itead.cc LCD 1602 Shield with 6 buttons, and can be installed to the board via the Arduino IDE software [download](https://www.arduino.cc/en/main/software).

```
code code code
```

 This can be set up in the SETUP menu item. In R1 and R2 DURATION menu items the duration of the pumps operation can be set up. The purpose of this is to allow for different watering schemes depending on the water needs of different plants. In the RELAYS STATE menu item the user can see the current state of the Relays and enable or disable them, by pressing the "Right" button (select between R1 and R2) and the "Up" / "Down" buttons (enable or disable).

## How to use it
The code allows scheduling of the operation of two 5V submersible pumps connected to Arduino through two 5V Relays.
## Menu
The menu is based on the code by Paul Siewert, retrieved at https://www.instructables.com/id/Arduino-Uno-Menu-Template/ . The menu consists of six menu items as follows:
- WTR INTERVAL
- R1 DURATION
- R2 DURATION
- RELAYS STATE
- SETUP
- WATERING MODE

### WTR INTERVAL
WTR INTERVAL (Watering interval) is the time between two watering sessions, and it can be either in minutes (1-59) or hours (1-9). The reason I included the **_minutes_** function is purely for testing purposes. However, there might be cases that enabling the relays that often makes perfect sense. Changing from minutes to hours and vice versa can be set up in the SETUP menu item.

One can change the current limitation of hours (1-9) and increase it as desired by changing the **_constrain_** of the **_wateringInt_** variable.

```
wateringInt = constrain(wateringInt, 1, 9);
```

### R1 DURATION

### R2 DURATION

### RELAYS STATE

### SETUP

### WATERING MODE


## Schematic
![Schematic](https://github.com/christoschronopoulos/automatic-plant-watering-system/blob/master/schematic.png)

## License
[MIT](https://choosealicense.com/licenses/mit/)
