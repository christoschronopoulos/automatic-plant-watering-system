/***************************************************************************************
    Name    : Automatic Plant Watering System
    Author  : Christos Chronopoulos
    Created : July 21, 2020
    Last Modified: July 24, 2020
    Version : 1.0
    Available : GitHub https://github.com/christoschronopoulos/automatic-plant-watering-system
    Notes   : This code is tested with an Arduino Mega and an itead.cc 1602 Shield with 6 buttons.
              The menu is based on the code by Paul Siewert, retrieved at
              https://www.instructables.com/id/Arduino-Uno-Menu-Template/ .
              The code allows scheduling of the operation of two 5V submersible pumps connected
              to Arduino through two 5V Relays. WTR INTERVAL (Watering interval) is the time
              between two watering sessions, and it can be either in minutes or hours. This can
              be set up in the SETUP menu item. In R1 and R2 DURATION menu items the duration of
              the pumps operation can be set up. The purpose of this is to allow for different
              watering schemes depending on the water needs of different plants. In the RELAYS STATE
              menu item the user can see the current state of the Relays and enable or disable them,
              by pressing the "Right" button (select between R1 and R2) and the "Up" / "Down" buttons
              (enable or disable).
    License : MIT License
              Copyright (c) 2020 Christos Chronopoulos

              Permission is hereby granted, free of charge, to any person obtaining a copy
              of this software and associated documentation files (the "Software"), to deal
              in the Software without restriction, including without limitation the rights
              to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
              copies of the Software, and to permit persons to whom the Software is
              furnished to do so, subject to the following conditions:

              The above copyright notice and this permission notice shall be included in all
              copies or substantial portions of the Software.

              THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
              IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
              FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
              AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
              LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
              OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
              SOFTWARE.

 ***************************************************************************************/

// You can have up to 10 menu items in the menuItems[] array below without having to change the base programming at all. Name them however you'd like. Beyond 10 items, you will have to add additional "cases" in the switch/case
// section of the operateMainMenu() function below. You will also have to add additional void functions (i.e. menuItem11, menuItem12, etc.) to the program.
String menuItems[] = {"WTR INTERVAL", "R1 DURATION", "R2 DURATION", "RELAYS STATE", "SETUP", "WATERING MODE"};

// Navigation button variables
int readKey;

// Menu control variables
int menuPage = 0;
int maxMenuPages = round(((sizeof(menuItems) / sizeof(String)) / 2) + .5);
int cursorPosition = 0;

int setupMode = 0;
int waterState = 0;
int relaySel = 0;
int counterR1 = 0;
int counterR2 = 0;

// Relay Timer variables
long startingTime = 0;
long wateringInt = 3; // minutes or hours. Selected in SETUP menu item
long r1Duration = 5; // seconds
long r2Duration = 10; // seconds
long previousMillis = 0; // will store last time Relay was updated
long previousR1Millis = 0;
long previousR2Millis = 0;

//Relay variables
int relay1Pin = 22;
int relay2Pin = 28;
volatile byte relay1State = HIGH;
volatile byte relay2State = HIGH;

//Backlight
int backlightPin = 10;
volatile byte backlightState = HIGH;
long backlightOff = 60000; // in millis 60 seconds

// Creates custom characters for the menu display
byte downArrow[8] = {
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b10101, // * * *
  0b01110, //  ***
  0b00100  //   *
};

byte upArrow[8] = {
  0b00100, //   *
  0b01110, //  ***
  0b10101, // * * *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100  //   *
};

byte waterOn[8] = {
  B00100, //   *
  B00100, //   *
  B01110, //  ***
  B01010, //  * *
  B10111, // * ***
  B10111, // * ***
  B01110, //  ***
  B00000  //
};

byte waterOff[8] = {
  B00000, //
  B00000, //
  B10001, // *   *
  B01010, //  * *
  B00100, //   *
  B01010, //  * *
  B10001, // *   *
  B00000  //
};

byte timerBell[8] = {
  B00100, //   *
  B01110, //  ***
  B01110, //  ***
  B01110, //  ***
  B11111, // *****
  B00000, //
  B00100, //   *
  B00000  //
};

byte seconds[8] = {
  B11011, // ** **
  B01001, //  *  *
  B10010, // *  *
  B00000, //
  B00000, //
  B00000, //
  B00000, //
  B00000  //
};

#include <Wire.h>
#include <LiquidCrystal.h>

// Setting the LCD shields pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {

  // Initializes serial communication
  Serial.begin(9600);

  // Initializes and clears the LCD screen
  lcd.begin(16, 2);
  lcd.clear();

  // Creates the byte for the custom characters
  //lcd.createChar(0, );
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);
  lcd.createChar(3, waterOn);
  lcd.createChar(4, waterOff);
  lcd.createChar(5, timerBell);
  lcd.createChar(7, seconds);

  // Pin for relay module set as output
  pinMode(relay1Pin, OUTPUT);
  digitalWrite(relay1Pin, HIGH);
  pinMode(relay2Pin, OUTPUT);
  digitalWrite(relay2Pin, HIGH);

  pinMode(backlightPin, OUTPUT);
  digitalWrite(backlightPin, HIGH);
}

void loop() {
  mainMenuDraw();
  drawCursor();
  operateMainMenu();

}

// This function will generate the 2 menu items that can fit on the screen. They will change as you scroll through your menu. Up and down arrows will indicate your current menu position.
void mainMenuDraw() {
  Serial.println(menuPage);

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(menuItems[menuPage]);
  lcd.setCursor(1, 1);
  lcd.print(menuItems[menuPage + 1]);
  if (menuPage == 0) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
  } else if (menuPage > 0 and menuPage < maxMenuPages) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  } else if (menuPage == maxMenuPages) {
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  }
}

// When called, this function will erase the current cursor and redraw it based on the cursorPosition and menuPage variables.
void drawCursor() {

  for (int x = 0; x < 2; x++) {     // Erases current cursor
    lcd.setCursor(0, x);
    lcd.print(" ");
  }

  // The menu is set up to be progressive (menuPage 0 = Item 1 & Item 2, menuPage 1 = Item 2 & Item 3, menuPage 2 = Item 3 & Item 4), so
  // in order to determine where the cursor should be you need to see if you are at an odd or even menu page and an odd or even cursor position.
  if (menuPage % 2 == 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is even and the cursor position is even that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(char(0x3E));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is even and the cursor position is odd that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(char(0x3E));
    }
  }
  if (menuPage % 2 != 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is odd and the cursor position is even that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(char(0x3E));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is odd and the cursor position is odd that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(char(0x3E));
    }
  }
}


void operateMainMenu() {

  int activeButton = 0;
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 0: // When button returns as 0 there is no action taken
        break;
      case 1:  // This case will execute if the "forward" button is pressed
        button = 0;
        switch (cursorPosition) { // The case that is selected here is dependent on which menu page you are on and where the cursor is.
          case 0:
            if (setupMode == 0) {
              menuItem1minutes();
            }
            else
              menuItem1hours();
            break;
          case 1:
            menuItem2();
            break;
          case 2:
            menuItem3();
            break;
          case 3:
            menuItem4();
            break;
          case 4:
            menuItem5();
            break;
          case 5:
            menuItem6();
            break;
          case 6:
            menuItem7();
            break;
          case 7:
            menuItem8();
            break;
          case 8:
            menuItem9();
            break;
          case 9:
            menuItem10();
            break;
        }
        activeButton = 1;
        mainMenuDraw();
        drawCursor();
        break;

      case 2:
        button = 0;
        if (menuPage == 0) {
          cursorPosition = cursorPosition - 1;
          cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        }
        if (menuPage % 2 == 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        cursorPosition = cursorPosition - 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));

        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;

      case 3:
        button = 0;
        if (menuPage % 2 == 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        cursorPosition = cursorPosition + 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
    }
  }
}

// This function is called whenever a button press is evaluated. The LCD shield works by observing a voltage drop across the buttons all hooked up to A0.
int evaluateButton(int x) {
  int result = 0;
  if (x < 50) {
    result = 1; // right
  } else if (x < 195) {
    result = 2; // up
  } else if (x < 380) {
    result = 3; // down
  } else if (x < 790) {
    result = 4; // left
  }
  return result;
}

// If there are common usage instructions on more than 1 of your menu items you can call this function from the sub
// menus to make things a little more simplified. If you don't have common instructions or verbage on multiple menus
// I would just delete this void. You must also delete the drawInstructions()function calls from your sub menu functions.
void drawInstructions() {
  lcd.setCursor(0, 1); // Set cursor to the bottom line
  lcd.print("Use ");
  lcd.write(byte(1)); // Up arrow
  lcd.print("/");
  lcd.write(byte(2)); // Down arrow
  lcd.print(" buttons");
}

// ===================== WATERING INTERVAL FOR MINUTES =====================
void menuItem1minutes() { // Function executes when you select the 1st item from main menu
  int activeButton = 0;
  lcd.clear();
  lcd.setCursor(0, 1);
  drawInstructions();
  lcd.setCursor(0, 0);
  lcd.print("Interval:");
  lcd.print(wateringInt);
  lcd.print("' ");
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 2:
        button = 0;
        wateringInt = wateringInt + 1;
        wateringInt = constrain(wateringInt, 1, 59);
        lcd.setCursor(9, 0);
        lcd.print("     ");
        lcd.setCursor(9, 0);
        lcd.print(wateringInt);
        lcd.print("' ");
        break;
      case 3:
        button = 0;
        wateringInt = wateringInt - 1;
        wateringInt = constrain(wateringInt, 1, 59);
        lcd.setCursor(9, 0);
        lcd.print("     ");
        lcd.setCursor(9, 0);
        lcd.print(wateringInt);
        lcd.print("' ");
        break;
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Values saved!");
        delay(1500);
        activeButton = 1;
        break;
    }
  }
}

// ===================== WATERING INTERVAL FOR HOURS =====================
void menuItem1hours() { // Function executes when you select the 1st item from main menu
  int activeButton = 0;
  lcd.clear();
  lcd.setCursor(0, 1);
  drawInstructions();
  lcd.setCursor(0, 0);
  lcd.print("Interval:");
  lcd.print(wateringInt);
  lcd.print("h ");
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 2:
        button = 0;
        wateringInt = wateringInt + 1;
        wateringInt = constrain(wateringInt, 1, 9);
        lcd.setCursor(9, 0);
        lcd.print("     ");
        lcd.setCursor(9, 0);
        lcd.print(wateringInt);
        lcd.print("h ");
        break;
      case 3:
        button = 0;
        wateringInt = wateringInt - 1;
        wateringInt = constrain(wateringInt, 1, 9);
        lcd.setCursor(9, 0);
        lcd.print("     ");
        lcd.setCursor(9, 0);
        lcd.print(wateringInt);
        lcd.print("h ");
        break;
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Values saved!");
        delay(1500);
        activeButton = 1;
        break;
    }
  }
}

// ===================== RELAY 1 DURATION =====================
void menuItem2() { // Function executes when you select the 2nd item from main menu
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(0, 1);
  drawInstructions();
  lcd.setCursor(0, 0);
  lcd.print("R1 Duration:");
  lcd.print(r1Duration);
  //lcd.print("'");
  lcd.write(byte(7));

  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 2:
        button = 0;
        r1Duration = r1Duration + 1;
        r1Duration = constrain(r1Duration, 1, 30);
        lcd.setCursor(12, 0);
        lcd.print("     ");
        lcd.setCursor(12, 0);
        lcd.print(r1Duration);
        //lcd.print("'");
        lcd.write(byte(7));
        break;
      case 3:
        button = 0;
        r1Duration = r1Duration - 1;
        r1Duration = constrain(r1Duration, 1, 30);
        lcd.setCursor(12, 0);
        lcd.print("     ");
        lcd.setCursor(12, 0);
        lcd.print(r1Duration);
        lcd.write(byte(7));
        break;
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Values saved!");
        delay(1500);
        activeButton = 1;
        break;
    }
  }
}

// ===================== RELAY 2 DURATION =====================
void menuItem3() { // Function executes when you select the 3rd item from main menu
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(0, 1);
  drawInstructions();
  lcd.setCursor(0, 0);
  lcd.print("R2 Duration:");
  lcd.print(r2Duration);
  //lcd.print("'");
  lcd.write(byte(7));

  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 2:
        button = 0;
        r2Duration = r2Duration + 1;
        r2Duration = constrain(r2Duration, 1, 30);
        lcd.setCursor(12, 0);
        lcd.print("     ");
        lcd.setCursor(12, 0);
        lcd.print(r2Duration);
        //lcd.print("'");
        lcd.write(byte(7));
        break;
      case 3:
        button = 0;
        r2Duration = r2Duration - 1;
        r2Duration = constrain(r2Duration, 1, 30);
        lcd.setCursor(12, 0);
        lcd.print("     ");
        lcd.setCursor(12, 0);
        lcd.print(r2Duration);
        //lcd.print("'");
        lcd.write(byte(7));
        break;
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Values saved!");
        delay(1500);
        activeButton = 1;
        break;
    }
  }
}

// ===================== RELAYS STATE =====================
void menuItem4() { // Function executes when you select the 4th item from main menu
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Relays State");
  lcd.setCursor(0, 1); // Set cursor to the bottom line
  if (relay1State == LOW) {
    lcd.setCursor(1, 1);
    lcd.print("R1:");
    lcd.setCursor(4, 1);
    lcd.write(byte(3)); //ON
    relay1Enable();
  }
  else {
    lcd.setCursor(1, 1);
    lcd.print("R1:");
    lcd.setCursor(4, 1);
    lcd.write(byte(4)); //OFF
    relay1Disable();
  }

  if (relay2State == LOW) {
    lcd.setCursor(7, 1);
    lcd.print("R2:");
    lcd.setCursor(10, 1);
    lcd.write(byte(3)); //ON
    relay2Enable();
  }
  else {
    lcd.setCursor(7, 1);
    lcd.print("R2:");
    lcd.setCursor(10, 1);
    lcd.write(byte(4)); //OFF
    relay2Disable();
  }

  if (relaySel == 0) {
    lcd.setCursor(0, 1);
    lcd.write(char(0x3E));
    lcd.setCursor(6, 1);
    lcd.print(" ");

  }
  else {
    lcd.setCursor(6, 1);
    lcd.write(char(0x3E));
    lcd.setCursor(0, 1);
    lcd.print(" ");
  }

  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 1: // This case will execute if the "right" button is pressed
        button = 0;
        if (relaySel == 0) {
          relaySel = 1;
          lcd.setCursor(0, 1);
          lcd.print(" ");
          lcd.setCursor(6, 1);
          lcd.write(char(0x3E));
        }
        else {
          relaySel = 0;
          lcd.setCursor(0, 1);
          lcd.write(char(0x3E));
          lcd.setCursor(6, 1);
          lcd.print(" ");
        }
        break;
      case 2:
        button = 0;
        if (relaySel == 0) {
          if (relay1State == HIGH) {
            relay1Enable();
            lcd.setCursor(1, 1);
            lcd.print("R1:");
            lcd.setCursor(4, 1);
            lcd.write(byte(3)); //ON
          }
          else {
            relay1Disable();
            lcd.setCursor(1, 1);
            lcd.print("R1:");
            lcd.setCursor(4, 1);
            lcd.write(byte(4)); //OFF
          }
        }
        else {
          if (relay2State == HIGH) {
            relay2Enable();
            lcd.setCursor(7, 1);
            lcd.print("R2:");
            lcd.setCursor(10, 1);
            lcd.write(byte(3)); //ON
          }
          else {
            relay2Disable();
            lcd.setCursor(7, 1);
            lcd.print("R2:");
            lcd.setCursor(10, 1);
            lcd.write(byte(4)); //OFF
          }
        }
        break;
      case 3:
        button = 0;
        if (relaySel == 0) {
          if (relay1State == HIGH) {
            relay1Enable();
            lcd.setCursor(1, 1);
            lcd.print("R1:");
            lcd.setCursor(4, 1);
            lcd.write(byte(3)); //ON
          }
          else {
            relay1Disable();
            lcd.setCursor(1, 1);
            lcd.print("R1:");
            lcd.setCursor(4, 1);
            lcd.write(byte(4)); //OFF
          }
        }
        else {
          if (relay2State == HIGH) {
            relay2Enable();
            lcd.setCursor(7, 1);
            lcd.print("R2:");
            lcd.setCursor(10, 1);
            lcd.write(byte(3)); //ON
          }
          else {
            relay2Disable();
            lcd.setCursor(7, 1);
            lcd.print("R2:");
            lcd.setCursor(10, 1);
            lcd.write(byte(4)); //OFF
          }
        }
        break;
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Values saved!");
        delay(1500);
        activeButton = 1;
        break;
    }
  }
}

// ===================== SETUP =====================
void menuItem5() { // Function executes when you select the 5th item from main menu
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(0, 1);
  drawInstructions();
  lcd.setCursor(0, 0);
  lcd.print("Setup:");
  if (setupMode == 0) {
    lcd.print("minutes");
  }
  else
    lcd.print("hours");

  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 2:
        button = 0;
        setupMode = setupMode + 1;
        setupMode = constrain(setupMode, 0, 1);
        lcd.setCursor(6, 0);
        lcd.print("     ");
        lcd.setCursor(6, 0);
        //lcd.print(setupMode);
        if (setupMode == 0) {
          lcd.print("minutes");
        }
        else
          lcd.print("hours  ");
        break;
      case 3:
        button = 0;
        setupMode = setupMode - 1;
        setupMode = constrain(setupMode, 0, 1);
        lcd.setCursor(6, 0);
        lcd.print("     ");
        lcd.setCursor(6, 0);
        //lcd.print(setupMode);
        if (setupMode == 0) {
          lcd.print("minutes");
        }
        else
          lcd.print("hours");
        break;
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Values saved!");
        delay(1500);
        activeButton = 1;
        break;
    }
  }
}

// ===================== WATERING MODE MENU ITEM =====================
void menuItem6() { // Function executes when you select the 6th item from main menu
  int activeButton = 0;

  startingTime = millis(); //Resets the starting time every time the user access the Watering Mode menu item

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Watering Mode");

  while (activeButton == 0) {
    if (setupMode == 0) {
      wateringMode();
    }
    else
      wateringMode1();
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
        previousMillis = 0; // Resets the timer
        
        backlightState = HIGH;
        digitalWrite(backlightPin, backlightState);
        
        break;
    }
  }
}

void menuItem7() { // Function executes when you select the 7th item from main menu
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Sub Menu 7");

  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
        break;
    }
  }
}

void menuItem8() { // Function executes when you select the 8th item from main menu
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Sub Menu 8");

  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
        break;
    }
  }
}

void menuItem9() { // Function executes when you select the 9th item from main menu
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Sub Menu 9");

  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
        break;
    }
  }
}

void menuItem10() { // Function executes when you select the 10th item from main menu
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Sub Menu 10");

  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
        break;
    }
  }
}

// ===================== WATERING MODE FOR MINUTES =====================
void wateringMode() { //Function to operate the relays according to the values submitted in the menu
  //Serial.print("operateRelays ");
  unsigned long currentMillis = millis();

  if (previousMillis == 0) {
    previousMillis = startingTime;
  }

  // Turn the backlight off
  if (currentMillis - previousMillis >= backlightOff) {
    backlightState = LOW;
    digitalWrite(backlightPin, backlightState);
  }

  if (currentMillis - previousMillis >= (wateringInt * 60000)) {

    counterR1 = 1;
    counterR2 = 1;

    relay1Enable();
    relay2Enable();

    // Save the last time you turned ON the RELAYS 1 & 2
    previousMillis = currentMillis;
    previousR1Millis = currentMillis;
    previousR2Millis = currentMillis;

    if (waterState == 0) {
      waterState = 1;
    }
    else
      waterState = 0;
  }

  //r1Duration and r2Duration are counted in seconds and thus, we multiply them by 1000 (millis * 1000 = seconds)
  if (currentMillis - previousR1Millis >= (r1Duration * 1000) && counterR1 == 1) {
    relay1Disable();
    counterR1 == 0;
  }

  if (currentMillis - previousR2Millis >= (r2Duration * 1000) && counterR2 == 1) {
    relay2Disable();
    counterR2 == 0;
  }

  lcd.setCursor(0, 1);
  lcd.print("R1:");
  lcd.setCursor(3, 1);
  if (relay1State == 0) {
    lcd.write(byte(3)); //ON
  }
  else {
    lcd.write(byte(4)); //OFF
  }
  lcd.setCursor(5, 1);
  lcd.print("R2:");
  lcd.setCursor(8, 1);
  if (relay2State == 0) {
    lcd.write(byte(3)); //ON
  }
  else {
    lcd.write(byte(4)); //OFF
  }

  lcd.setCursor(10, 1);
  lcd.write(byte(5));
  lcd.setCursor(11, 1);

  long remainingMillis = (wateringInt * 60000) - (currentMillis - previousMillis); //minutes in millis
  int remainingSeconds = (remainingMillis / 1000);
  int minutes = (remainingSeconds / 60);
  int seconds = (remainingSeconds % 60);

  // Print millis to hours and minutes
  if (remainingSeconds < 60) {
    lcd.setCursor(11, 1);
    lcd.print(seconds);
    lcd.write(7);
    lcd.print("   ");
  }
  else {
    lcd.print(minutes);
    lcd.print("'");
    lcd.print(seconds);
    lcd.write(7);
    lcd.print("   ");
  }
}

// ===================== WATERING MODE FOR HOURS =====================
void wateringMode1() { //Function to operate the relays according to the values submitted in the menu
  //Serial.print("operateRelays ");
  unsigned long currentMillis = millis();

  if (previousMillis == 0) {
    previousMillis = startingTime;
  }

  // Turn the backlight off
  if (currentMillis - previousMillis >= backlightOff) {
    backlightState = LOW;
    digitalWrite(backlightPin, backlightState);
  }

  //if (currentMillis - previousMillis >= (wateringInt * 1000)) {
  if (currentMillis - previousMillis >= (wateringInt * 3600000)) {

    counterR1 = 1;
    counterR2 = 1;

    relay1Enable();
    relay2Enable();

    // Save the last time you turned ON the RELAYS 1 & 2
    previousMillis = currentMillis;
    previousR1Millis = currentMillis;
    previousR2Millis = currentMillis;

    if (waterState == 0) {
      waterState = 1;
    }
    else
      waterState = 0;
  }

  if (currentMillis - previousR1Millis >= (r1Duration * 1000) && counterR1 == 1) {
    relay1Disable();
    counterR1 == 0;
  }

  if (currentMillis - previousR2Millis >= (r2Duration * 1000) && counterR2 == 1) {
    relay2Disable();
    counterR2 == 0;
  }

  lcd.setCursor(0, 1);
  lcd.print("R1:");
  lcd.setCursor(3, 1);
  if (relay1State == 0) {
    lcd.write(byte(3)); //ON
  }
  else {
    lcd.write(byte(4)); //OFF
  }
  lcd.setCursor(5, 1);
  lcd.print("R2:");
  lcd.setCursor(8, 1);
  if (relay2State == 0) {
    lcd.write(byte(3)); //ON
  }
  else {
    lcd.write(byte(4)); //OFF
  }

  lcd.setCursor(10, 1);
  lcd.write(byte(5));
  lcd.setCursor(11, 1);

  long remainingMillis = (wateringInt * 3600000) - (currentMillis - previousMillis); //hours in millis
  int remainingMinutes = (remainingMillis / 60000);
  int hours = (remainingMinutes / 60);
  int minutes = (remainingMinutes % 60);

  //print millis to hours and minutes
  if (remainingMinutes < 60) {
    lcd.setCursor(11, 1);
    lcd.print(minutes);
    lcd.print("'");
    lcd.print("   ");
  }
  else {
    //lcd.print(((wateringInt * 60000) - (currentMillis - previousMillis)) / 60000);
    lcd.print(hours);
    lcd.print("h");
    //lcd.print(((wateringInt - currentMillis) / 1000) - ((currentMillis - previousMillis) / 60000) * 60);
    lcd.print(minutes);
    lcd.write("'");
    lcd.print("   ");
  }
}

// ===================== RELAYS ENABLE/DISABLE FUNCTIONS =====================
void relay1Enable() {
  if (relay1State == HIGH) {
    relay1State = LOW;
    digitalWrite(relay1Pin, relay1State);
    Serial.print("relay1State: ");
    Serial.println(relay1State);
  }
}

void relay2Enable() {
  if (relay2State == HIGH) {
    relay2State = LOW;
    digitalWrite(relay2Pin, relay2State);
    Serial.print("relay2State: ");
    Serial.println(relay2State);
  }
}

void relay1Disable() {
  if (relay1State == LOW) {
    relay1State = HIGH;
    digitalWrite(relay1Pin, relay1State);
    Serial.print("relay1State: ");
    Serial.println(relay1State);
  }
}

void relay2Disable() {
  if (relay2State == LOW) {
    relay2State = HIGH;
    digitalWrite(relay2Pin, relay2State);
    Serial.print("relay2State: ");
    Serial.println(relay2State);
  }
}
