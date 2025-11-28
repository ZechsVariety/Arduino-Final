#include <LiquidCrystal.h>

//Initialise the Serial LCD.
LiquidCrystal lcd(12,11,5,4,3,2); //These pin numbers are hard coded in on the serial backpack board.

int button1Pin = 6;
bool button1WasPressed = false;

int rotationPin = A0;
int dialValue = 0;
int prevDialValue = 0;
int shootSpeed = 10; //ticks
bool shotThisTick = false;

//alien stuff

int alienType[16];
int alienPos[16]; //no y pos needed
int alienHealth[16];
int alienTime[16];

int alienSpecial1[16]; //this int is used for alien's abilities (ex: for the hyper alien, it's used for its run direction)
int alienSpecial2[16];
int alienSpecial3[16];

//special alien modifiers
int hyperFreq = 35; //35 instead of 40 makes it FEEL random, but you can actually get good at predicted when itll run and get an extra hit, adding a layer of skill :D
int prowlerDuration = 20;

int score = 0; //score is just the amount of aliens you've killed
bool gameOver = false;

//SPRITES
//ALIENS
byte basic[] = {
  B00000,
  B00000,
  B00000,
  B01110,
  B11111,
  B10101,
  B01110,
  B10101
};
byte hyper[] = {
  B00000,
  B00100,
  B01110,
  B10101,
  B01110,
  B01110,
  B11111,
  B01010
};
byte prowler[] = {
  B00000,
  B01110,
  B11111,
  B00000,
  B11111,
  B01110,
  B01110,
  B10101
};
byte prowlerTrans[] = {
  B00000,
  B00100,
  B01010,
  B00000,
  B01010,
  B00100,
  B01010,
  B10101
};
byte multiplier[] = {
  B00000,
  B00000,
  B10001,
  B01010,
  B01110,
  B01010,
  B10001,
  B00000
};
byte splitted[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B01110,
  B10001,
  B00000,
  B00000
};
byte shielder[] = { //shield sprite is just 'o'
  B00000,
  B00000,
  B00000,
  B11101,
  B01010,
  B11110,
  B11110,
  B10101
};

//SET AT GAME START
int tick;
int spawnFrequency; //how often an alien spawns
int previousSpawnTick;

void LcdPrint(String input, bool wait, bool clearScreen = true); //this allows the use of default arguments, and it must be above any uses of the function (thanks to the 8th post for the solution: https://forum.arduino.cc/t/default-arguments/40909/8)

void setup()
{
  lcd.begin (16,2); //Initialize the LCD.
  
  pinMode(button1Pin, INPUT);
   
  pinMode(rotationPin, INPUT);
  
  Serial.begin(9600); //serial is just for debug--i was gonna use it for text but i like the immersion of using the lcd screen exclusively

  randomSeed(analogRead(0));
  
  //INITIALIZE SPRITES (up to 8)
  //ALIENS
  lcd.createChar(1, basic);
  lcd.createChar(2, hyper);
  lcd.createChar(3, prowler);
  lcd.createChar(4, prowlerTrans);
  lcd.createChar(5, multiplier);
  lcd.createChar(6, splitted);
  lcd.createChar(7, shielder);
  //lcd.createChar(8, basic); //thisll prolly be explosion
}

void loop()
{
  if(!gameOver) //gameover will only be false here if the game was just started
  	//IntroSequence();
  
  Serial.println("GAME START!");
  
  tick = 0; //1 tick is 100 milliseconds (.1 second)
  spawnFrequency = 100; //100 ticks is 10 seconds
  previousSpawnTick = -50; //set to -50 so that first alien spawn happens after 5 secs
  score = 0;
  gameOver = false;
  
  while(!gameOver) //game
  {
    lcd.clear();
    
    Aliens();
    Crosshair(true);

    delay(100); //this is one tick
    
    tick++;
    
    //spawn new alien
    if(tick - previousSpawnTick == spawnFrequency)
    {
      spawnFrequency--; //aliens spawn 1 tick faster each time
      previousSpawnTick = tick;
      
      //find a free slot
      for(int i = 0; i < 16; i++)
      {
        if(alienType[i] == 0) //no alien assigned to spot
        {
          Serial.println("Alien spawned!");
          
          alienType[i] = random(1, 4); //1-3
          
          RandomAlienPos(i, false);
          
          alienHealth[i] = 5;
          
          alienTime[i] = 200; //200 ticks is 20 seconds
          
          if(alienType[i] == 2)
          {
          	alienSpecial1[i] = 1; //used for run dir. left = -1, right = 1, can't move = 0
            //alienSpecial2[i] = 0; //binary if left is available
            //alienSpecial3[i] = 0; //binary if right is available
          	Serial.println(alienSpecial1[i]);
          }
          if(alienType[i] == 3)
          {
          	alienSpecial1[i] = 0; //tick timer for when it's invisible
          }
            
          break;
        }
      }
    }
  }
}

void RandomAlienPos(int i, bool originalPosFailsafe)
{
  int originalPos = alienPos[i];
  
  //find a free position
  for(int timeOut = 0; timeOut < 20; timeOut++) //times out after 20 attempts
  {
    alienPos[i] = random(0, 16);
    Serial.print("attempt: ");
    Serial.print(timeOut);
    Serial.print(" pos: ");
    Serial.println(alienPos[i]);

    bool posTaken = false;
    for(int j = 0; j < 16; j++) //run through each alien to check if the spot is taken
    {
      if(j == i) //ignore itself
        continue;

      if(alienPos[i] == alienPos[j])
      {
        posTaken = true;
        break;
      }
    }
    
    if(!posTaken)
      return;
  }
  
  Serial.print("Failsafe position finder: ");
  
  //failsafe
  if(originalPosFailsafe) //prowler uses this; if it cant find a new random spot, it just uses its original spot
  {
    alienPos[i] = originalPos; //alien wont move
  }
  else
  {
    for(int chronological = 0; chronological < 16; chronological++) //go from 0-15 chronologically to find a free space
    {
      bool posTaken = false;
      for(int j = 0; j < 16; j++) //run through each alien to check if the spot is taken
      {
        if(j == i) //if it's on its own tile, it's free
          break;

        if(chronological == alienPos[j])
        {
          posTaken = true;
          break;
        }
      }
      
      if(!posTaken)
      {
        alienPos[i] = chronological;
        Serial.println(chronological);
        return;
      }
    }
  }
  
  Serial.println("ERROR: alien couldn't spawn");
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
  //LcdPrint("powerful enough to kill them", false);
  LcdPrint("      THE        POTENTIOMETER!", true);
  
  //LcdPrint("and you...", false);
  //LcdPrint("You are in control.", true);
  
  LcdPrint("Why don't you give it a try?", true);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" AIM USING DIAL ");
  
  delay(500);
  
  while(digitalRead(button1Pin) == LOW)
  {
    Crosshair(false);
    delay(100);
  }
  
  LcdPrint("All you need to do is aim", false);
  LcdPrint("I'll fire for ya !", true);
  
  LcdPrint("Let me tell you about the aliens", false);
  //LcdPrint("Each one takes 5 hits to kill,", false);
  //LcdPrint("but there are several breeds", true);
  LcdPrint("First up is the BASIC alien", false);
  
  //TODO: type "BASIC" and show 2 scurrying below
  //TODO: other alien types
  
  LcdPrint("I haven't coded that yet tho", false);
  
  LcdPrint("You ready to take them on?", true);
  LcdPrint("Good luck!", false);
}

void Crosshair(bool shooting)
{
  ReadDialValue(300, 740);
  
  if(shooting)
  {
    if(tick % shootSpeed == 0)
    {
      shotThisTick = true;
    }
    else
    {
      shotThisTick = false;
    }
  }
  else
  {
    //lcd.clear();
    lcd.setCursor(prevDialValue, 1);
    lcd.print(" "); //clears only the previous tile
  }
  
  lcd.setCursor(dialValue, 1);
  
  //i noticed ! kinda looked like a . shooting upwards lol
  if(shotThisTick)
  	lcd.print("!");
  else
    lcd.print(".");
  
  //Serial.println(shotThisTick);
}

/*
	This is how the potentiometer's value is converted to which pixel on the lcd screen it's aiming at
    
    min is the approximate value the potentiometer is at when it's pointed to the far left
    max is far right
*/
void ReadDialValue(int min, int max)
{
  prevDialValue = dialValue;
  
  dialValue = analogRead(rotationPin);
  //Serial.println(dialValue);
  
  if(dialValue < min) //far left
    dialValue = 0;
  else if(dialValue > max) //far right
    dialValue = 15;
  else
    dialValue = map(dialValue, min, max, 0, 15); //using map worked better than i thought--i thought id have to manually set the range of every pixel because of the potentiometer being non-linear, but this works great
}

void Aliens()
{
  for(int i = 0; i < 16; i++)
  {
    if(alienType[i] != 0) //alien exists
    {
      //alien time limit
      alienTime[i]--;
      //Serial.println(alienTime[i]);
      if(alienTime[i] < 100) //if you have less than 10 seconds left
      {
        lcd.setCursor(alienPos[i], 1);
        //if(alienTime[i] > 10)
          if(!(alienType[i] == 3 && alienSpecial1[i] > 0)) //for the prowler, ensure the number doesnt show unless the prowler is visible
          	lcd.print(alienTime[i] / 10); //prints a timer underneath
        //else
          //lcd.print("!"); //i forgot that ! is the shooting sprite lol
      }
      
      //game over
      if(alienTime[i] == 0)
      {
        //show alien get to bottom row
        lcd.setCursor(alienPos[i], 1);
        
        if(alienType[i] == 1)
          lcd.write(1);
        else if(alienType[i] == 2)
          lcd.write(2);
        else if(alienType[i] == 3)
          lcd.write(3);
        
        gameOver = true;
      }
      else
      {
        //draw alien

        lcd.setCursor(alienPos[i], 0);
		
        //if shot
        if(shotThisTick && dialValue == alienPos[i])
        {
          if(alienType[i] == 3) //prowler
          {
            if(alienSpecial1[i] > 0) //if prowler is in hidden state
            {
              break;
            }
            else
            {
              alienSpecial1[i] = prowlerDuration;
              alienTime[i] += prowlerDuration; //prowler's time invisible doesn't count towards its timer
            }
          }
          
          alienHealth[i]--;
          alienTime[i] += 10; //add 1 second to the timer (so that if you start attacking with less than 5 seconds left, you still have a chance)

          //hit sprite
          if(alienHealth[i] > 0)
          {
            lcd.print("x");
          }
          else //explode
          {
            lcd.print("@");
            
            score++;

            alienType[i] = 0; //deletes alien, other values don't have to be deleted though
          }
        }
        else
        {
          //lcd.print("O");
          
          //alien type-specific code
          if(alienType[i] == 1)
          	lcd.write(1);
          else if(alienType[i] == 2) //hyper
          {
          	if(
              tick % hyperFreq == 0
              || (tick - 1) % hyperFreq == 0 
              || (tick - 2) % hyperFreq == 0 
              || (tick - 3) % hyperFreq == 0 
              || (tick - 4) % hyperFreq == 0) //every 3 seconds for 5 ticks
            {
              alienSpecial2[i] = 1;
              alienSpecial3[i] = 1;
              
              //detect walls
              if(alienPos[i] - 1 < 0) //left
                alienSpecial2[i] = 0;
              else if(alienPos[i] + 1 > 15) //right
                alienSpecial3[i] = 0;
              
              //detect other aliens
              for(int j = 0; j < 16; j++)
              {
                if(i == j || alienType[j] == 0) //ignores itself and any despawned aliens
                  continue;
                
                //Serial.println(j);
                //alien to left
                if(alienPos[i] - 1 == alienPos[j]) //left aint available
                {
                  alienSpecial2[i] = 0;
                  //Serial.println("ALIEN LEFT!");
                }
                //alien to right
                if(alienPos[i] + 1 == alienPos[j]) //right aint available
                {
                  alienSpecial3[i] = 0;
                  //Serial.println("ALIEN RIGHT!");
                }
              }
              
              if(alienSpecial2[i] + alienSpecial3[i] == 0) //alien to both sides
              {
                alienSpecial1[i] = 0; //cant move
                //Serial.println("can't move");
              }
              else if(alienSpecial2[i] == 0) //alien only to left
              {
                alienSpecial1[i] = 1; //right
                //Serial.println("right");
              }
              else if(alienSpecial3[i] == 0) //alien only to right
              {
                alienSpecial1[i] = -1; //left
                //Serial.println("left");
              }
              
              //move
              alienPos[i] += alienSpecial1[i];
            }
            
            lcd.write(2); //hyper sprite
          }
          else if(alienType[i] == 3) //prowler
          {
            if(alienSpecial1[i] == 0) //regular
              lcd.write(3); //prowler sprite
            else
              alienSpecial1[i]--;
            
            //transition sprites
            if(alienSpecial1[i] == 2 || alienSpecial1[i] == prowlerDuration - 1) //shows on 2nd and 2nd last fram
            {
              lcd.write(4); //prowler invisible transition sprite
            }
            //else, it just doesnt render
            
            if(alienSpecial1[i] == prowlerDuration - 2) //teleports to a random spot on the frame before its transition sprite. if it teleported earlier, the other aliens' spawn positions could give away its new position
            {
              RandomAlienPos(i, true);
            }
          }
        }
      }
    }
  }
  
  if(gameOver) //doesnt do death sequence until all the aliens are rendered on screen for the sake of consistency
  {
    Serial.println("game over");
    
    delay(2000); //pause for a moment
        
    GameOver();
  }
}

void GameOver()
{
  //disabling the screen clearing leads to a super easy but cool effect :D
  LcdPrint("@@@@@@@@@@@@@@@@ @@@@@@@@@@@@@@@@", false, false); //this is supposed to look like explosions
  delay(500);
  LcdPrint("      YOU              DIED      ", true, false);
  
  LcdPrint("SCORE:          " + String(score), true); //thank you PaulRB for the solution for adding an int to a string value: https://forum.arduino.cc/t/help-with-int-to-string/928406
  
  //score messages
  if(score == 0)
  {
  //  LcdPrint("Zero?? Did you even try?", false);
  //  LcdPrint("All you gotta do is aim bud.", false);
  }
  else if(score <= 1)
  {
  //  LcdPrint("Hey, one's better than nothing!", false);
  }
  else if(score <= 5)
  {
  //  LcdPrint(String(score) + "? I know you can do better!", false);
  }
  else if(score <= 15)
  {
  //  LcdPrint(String(score) + "? That ain't bad!", false);
  }
  else if(score <= 30)
  {
  //  LcdPrint(String(score) + "? You're incredible!", false);
  }
  else if(score <= 50)
  {
  //  LcdPrint(String(score) + "? Literally how??", false);
  }
  else if(score <= 75)
  {
  //  LcdPrint(String(score) + "? Amazing. Just amazing.", false);
  }
  else if(score <= 100)
  {
  //  LcdPrint(String(score) + "? You're at the top!", false);
  }
  else //This is where i realized i had hit the string memory limit :( i'll have to phase out string values whenever i add more text
  {
  //  LcdPrint(String(score) + "? You gotta be cheating...", false);
  //  LcdPrint("Past 100 must be impossible", false);
  //  LcdPrint("If you really weren't cheating", false);
  //  LcdPrint("You must love the game lol", false);
  }
  
  LcdPrint("You ready to go again?", true);
  LcdPrint("Alrighty then!", false);
  
  //delete all aliens
  for(int i = 0; i < 16; i++)
  {
    alienType[i] = 0;
  }
}

/* LcdPrint() is my way of streamlining printing text

   It has the option to slowly print text which is cool and awesome

   It auto-formats text in 2 ways:
   1. removes unneccesary spaces on a new line
   2. adds a dash if a word is cut off
   These make it easier to write text
   
   It also uses the wait function to draw an indicator (>) to show it's waiting for a button input, or continue automatically if not
*/
void LcdPrint(String input, bool wait, bool clearScreen)
{
  Serial.println(input);
  
  if(clearScreen)
  	lcd.clear();

  lcd.setCursor(0, 0);
  LcdSlowPrint(input, 0, 16, 50);
  
  //lcd.print(input.substring(0, 16)); //grabs the first 16 digits of the question for the top

  lcd.setCursor(0, 1);
  if(input[16] == ' ' || input[16] == NULL) //removes space
  {
    LcdSlowPrint(input, 17, 32, 30); //gabes the next 16 digits for the bottom.
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
    LcdSlowPrint(input, 16, 31, 30); //gabes the next 16 digits for the bottom.
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
    delay(500);
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