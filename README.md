# automatic-plant-watering-system
## Installation
Text text text [url](https://github.com/christoschronopoulos/automatic-plant-watering-system/edit/master/README.md) text text.

```
code code code
```

This code is tested with an Arduino Mega and an itead.cc 1602 Shield with 6 buttons.   WTR INTERVAL (Watering interval) is the time between two watering sessions, and it can be either in minutes or hours. This can be set up in the SETUP menu item. In R1 and R2 DURATION menu items the duration of the pumps operation can be set up. The purpose of this is to allow for different watering schemes depending on the water needs of different plants. In the RELAYS STATE menu item the user can see the current state of the Relays and enable or disable them, by pressing the "Right" button (select between R1 and R2) and the "Up" / "Down" buttons (enable or disable).

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
### R1 DURATION
### R2 DURATION
### RELAYS STATE
### SETUP
### WATERING MODE

## Schematic
![Schematic](https://github.com/christoschronopoulos/automatic-plant-watering-system/blob/master/schematic.png)

## License
[MIT](https://choosealicense.com/licenses/mit/)
