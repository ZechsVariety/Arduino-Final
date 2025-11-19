#include <LiquidCrystal.h>

//Initialise the Serial LCD.
LiquidCrystal lcd(12,11,5,4,3,2); //These pin numbers are hard coded in on the serial backpack board.

int button1Pin = 6;
bool button1WasPressed = false;

int rotationPin = A0;
int dialValue = 0;

void setup()
{
  lcd.begin (16,2); //Initialize the LCD.
  
  pinMode(button1Pin, INPUT);
   
  pinMode(rotationPin, INPUT);
  
  Serial.begin(9600); //serial is just for debug--i was gonna use it for text but i like the immersion of using the lcd screen exclusively
}

void loop()
{
  //IntroSequence();
  
  DisplayCrosshair();
  delay(100);
}

void IntroSequence()
{
  LcdPrint("Welcome to Alien Shooter!", true);
  
  LcdPrint("After the alien invasion", false);
  LcdPrint("and years of survival", false);
  LcdPrint("humans had finally taken", false);
  LcdPrint("the upper hand...", true);
  
  LcdPrint("But we knew,", false);
  LcdPrint("it would never last.", true);
  
  LcdPrint("Now,", false);
  LcdPrint("the aliens are sending", false);
  LcdPrint("countless backup troops", false);
  LcdPrint("and they're about to", false);
  LcdPrint("rain from the skies", true);
  
  LcdPrint("We have but one weapon", false);
  LcdPrint("powerful enough to kill them", false);
  LcdPrint("and you...", true);
  
  LcdPrint("You are in control.", true);
  
  LcdPrint("Why don't you give it a try?", true);
  
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("- AIM WITH DIAL -");
  
  delay(10000);
  //TODO: add tutorial aiming
  
  LcdPrint("There are several alien types", true);
  LcdPrint("Use the dial below to aim", true);
  LcdPrint("You autofire in that direction", true);
  LcdPrint("Good luck!", true);
}

void DisplayCrosshair()
{
  ReadDialValue(300, 740);
  
  lcd.clear();
  lcd.setCursor(dialValue, 1);
  lcd.print("X");
}

/*
	This is how the potentiometer's value is converted to which pixel on the lcd screen it's aiming at
    
    min is the approximate value the potentiometer is at when it's pointed to the far left
    max is far right
*/
void ReadDialValue(int min, int max)
{
  dialValue = analogRead(rotationPin);
  //Serial.println(dialValue);
  
  if(dialValue < min) //far left
    dialValue = 0;
  else if(dialValue > max) //far right
    dialValue = 15;
  else
    dialValue = map(dialValue, min, max, 0, 15); //using map worked better than i thought--i thought id have to manually set the range of every pixel because of the potentiometer being non-linear, but this works great
}

/* LcdPrint() is my way of streamlining printing text

   It has the option to slowly print text which is cool and awesome

   It auto-formats text in 2 ways:
   1. removes unneccesary spaces on a new line
   2. adds a dash if a word is cut off
   These make it easier to write text
   
   It also uses the wait function to draw an indicator (>) to show it's waiting for a button input, or continue automatically if not
*/
void LcdPrint(String input, bool wait)
{
  lcd.clear();

  lcd.setCursor(0, 0);
  LcdSlowPrint(input, 0, 16, 50);
  
  //lcd.print(input.substring(0, 16)); //grabs the first 16 digits of the question for the top

  lcd.setCursor(0, 1);
  if(input[16] == ' ' || input[16] == NULL) //removes space
  {
    LcdSlowPrint(input, 17, 32, 50); //gabes the next 16 digits for the bottom.
  }
  else
  {
    if(input[15] == ' ' || input[15] == '.' || input[15] == '!' || input[15] == '?') //no dash if it's punctuation or spaces
    {
      lcd.setCursor(0, 1);
    }
    else
    {
      lcd.print("-");
      lcd.setCursor(1, 1);
    }
    LcdSlowPrint(input, 16, 31, 50); //gabes the next 16 digits for the bottom.
  }

  if(wait)
  {
    while(true)
    {
      delay(500);

      if(digitalRead(button1Pin) == HIGH)
        break;

      lcd.setCursor(15, 1);
      lcd.print(">");

      delay(500);

      if(digitalRead(button1Pin) == HIGH)
        break;

      lcd.setCursor(15, 1);
      lcd.print(" ");
    }
  }
  else
  {
    delay(1000);
  }
}
void LcdSlowPrint(String input, int start, int end, int speed)
{
  for(int i = start; i <= end; i++)
  {
    lcd.print(input.substring(i, i + 1));
    
    delay(speed);
  }
}