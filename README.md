# automatic-plant-watering-system
## Installation
The code is tested with an Arduino Mega and an itead.cc LCD 1602 Shield with 6 buttons, and can be installed to the board via the Arduino IDE software [download](https://www.arduino.cc/en/main/software).

```
code code code
```

 This can be set up in the SETUP menu item. In R1 and R2 DURATION menu items the duration of the pumps operation can be set up.  

## How to use it
The code allows scheduling of the operation of two 5V submersible pumps connected to Arduino through two 5V Relays.
## Menu
The menu is based on the code by Paul Siewert, retrieved at https://www.instructables.com/id/Arduino-Uno-Menu-Template/ . It consists of six menu items as follows:
- WTR INTERVAL
- R1 DURATION
- R2 DURATION
- RELAYS STATE
- SETUP
- WATERING MODE

### WTR INTERVAL
WTR INTERVAL (Watering interval) is the time between two watering sessions, and it can be either in minutes (1-59) or hours (1-9). The reason I included the **_minutes_** function is purely for testing purposes. However, there might be cases that enabling the relays that often makes perfect sense. Changing from minutes to hours and vice versa can be set up in the SETUP menu item.

One can change the current limitation of hours (1-9) and increase it as desired by changing the **_constrain_** of the **_wateringInt_** variable in the code.

```
wateringInt = constrain(wateringInt, 1, 9);
```

When pressing the **_Left_** button, it returns to the main menu. It is important to note that the value changes are saved!

### R1 DURATION
In R1 DURATION menu item the duration of the pump operation can be set up. In other words, is the duration that the Relay 1 stays ON. This actually means that the code can easily be used to handle the operation of any device connected to the relays, and not only the submersible pumps. The purpose of the different Relay duration variables is to allow for different watering schemes depending on the water needs of various plants.

> **IMPORTANT:** The relays must be suitable for the Input Voltage (e.g. 5V or 220V) to be used with defferent devices. For example, in this project 5V Relays are used together with 5V submersible pumps. However, if one would like to control a lamp or other household appliance **MUST** use 220V relays with **EXTREME CAUTION**.

The maximum duration is 30 seconds, which can be changed as desired by changing the **_constrain_** of the **_r1Duration_** variable in the code.

```
r1Duration = constrain(r1Duration, 1, 30);
```

When pressing the **_Left_** button, it returns to the main menu. It is important to note that the value changes are saved!

### R2 DURATION
In R2 DURATION menu item the duration of the pump operation can be set up. In other words, is the duration that the Relay 2 stays ON. This actually means that the code can easily be used to handle the operation of any device connected to the relays, and not only the submersible pumps. The purpose of the different Relay duration variables is to allow for different watering schemes depending on the water needs of various plants. 

> **IMPORTANT:** The relays must be suitable for the Input Voltage (e.g. 5V or 220V) to be used with defferent devices. For example, in this project 5V Relays are used together with 5V submersible pumps. However, if one would like to control a lamp or other household appliance **MUST** use 220V relays with **EXTREME CAUTION**.

The maximum duration is 30 seconds, which can be changed as desired by changing the **_constrain_** of the **_r2Duration_** variable in the code.

```
r2Duration = constrain(r2Duration, 1, 30);
```

When pressing the **_Left_** button, it returns to the main menu. It is important to note that the value changes are saved!

### RELAYS STATE
In the RELAYS STATE menu item the user can see the current state of the Relays and enable or disable them, by pressing the **_Right_** button (select between R1 and R2) and the **_Up_** / **_Down_** buttons (enable or disable).

### SETUP
In SETUP one can change the time interval between the watering sessions from minutes to hours and vice versa.

When pressing the **_Left_** button, it returns to the main menu. It is important to note that the value changes are saved!

### WATERING MODE
The WATERING MODE menu item shows the current state of the relays (and hence, of the pumps or any connected device)

## Schematic
![Schematic](https://github.com/christoschronopoulos/automatic-plant-watering-system/blob/master/schematic.png)

## License
[MIT](https://choosealicense.com/licenses/mit/)
