#include <LiquidCrystal.h>

//Initialise the Serial LCD.
LiquidCrystal lcd(12,11,5,4,3,2); //These pin numbers are hard coded in on the serial backpack board.

int button1Pin = 6;
bool button1WasPressed = false;

int rotationPin = A0;

void setup()
{
  lcd.begin (16,2); //Initialize the LCD.
  
  pinMode(button1Pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(button1Pin), button1Pressed, RISING);
  
  pinMode(rotationPin, INPUT);
}

void loop()
{
  LcdPrint("Welcome to Alien Shooter!", true);
  LcdPrint("TEEESSSTTT!", true);
}

void LcdPrint(String input, bool wait)
{
  while(true)
  {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(input.substring(0, 16)); //grabs the first 16 digits of the question for the top

    lcd.setCursor(0, 1);
    if(input[16] == ' ' || input[16] == NULL)
    {
      lcd.print(input.substring(17, 32)); //gabes the next 16 digits for the bottom.
    }
    else
    {
      lcd.print("-");
      lcd.setCursor(1, 1);
      lcd.print(input.substring(16, 31)); //gabes the next 16 digits for the bottom.
    }

    if(wait)
    {
      delay(500);
      
      if(WasButton1Pressed())
        break;

      lcd.setCursor(15, 1);
      lcd.print(">");
      
      if(WasButton1Pressed())
        break;
      
      delay(500);
      
      if(digitalRead(button1Pin) == HIGH)
      {
        delay(100);
      }
    }
    else
    {
      break;
    }
  }
}

//interrupts
void button1Pressed()
{
  button1WasPressed = true;
}

bool WasButton1Pressed()
{
  if(button1WasPressed)
  {
    button1WasPressed = false;
    return true;
  }
  else
  {
    return false;
  }
}