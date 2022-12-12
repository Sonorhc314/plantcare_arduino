#include <LiquidCrystal.h> //LCD
#include "pitches.h" //library to play melidies with piezo

//I use define instead of declaring constants to save memory
#define BUZZER 13 //piezo music
#define UPDATE_B 2 //update button
#define RED_PIN 9 //red RGB LED pin
#define GREEN_PIN 7 //green RGB LED pin
#define BLUE_PIN 8 //blue RGB LED pin

//-----------LCD screeen------------
int lcd_counter = 0;//switch condition for lcd update
LiquidCrystal lcd(12, 11, 5, 4, 3,6);
//----------------------------------

//-----------------BUZZER settings--
//idea created by Robson Couto 
//https://github.com/robsoncouto/arduino-songs

int melody[] = {  //melody when there are problems encountered, can be modified
 NOTE_B4,5,NOTE_B4,5, //simple alarm, else user can set more melody below
 // NOTE_B4,5, NOTE_E5, 4, NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,
};
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};
int notes=sizeof(melody)/sizeof(melody[0])/2; 
int tempo=144; 

// this calculates the duration of a whole note in ms (60s/tempo)*4 beats
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;

//----------------------------------

//---------RGB_LED-
int red_rgb = 0;
int green_rgb = 0;
int blue_rgb = 0;
//-----------------

//-----------------
int temp = 0; //temperature
int humidity = 0;
int light = 0;
int problem_counter; //how many problems encountered per loop
String serial_string; //string from the arduino-sender
//-----------------


void setup() {
  Serial.begin(9600);// Begin the Serial at 9600 Baud
  //lcd initial--
  lcd.begin(16, 2);
  lcd.print("Plant care");
  lcd.setCursor(0,1);
  lcd.print("project! ^.^");
  //-------------

  pinMode(BUZZER, OUTPUT);
  pinMode(UPDATE_B, INPUT); //button that updates lcd

  //RGB LED------
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  //-------------
  attachInterrupt(0, updateLCD, FALLING); // interrupt routine if button clicked
  }
  
void loop() {
  problem_counter=0;
  serial_string = Serial.readString();//takes string arduino-sender gives

  analyze_string(serial_string); //find value of temp, humidity and light from string


  if(600<light || light<200) //normal light within the range [200...600]
  {
    problem_counter++;
  }

  if(28<temp || temp<10)
  {
    problem_counter++;
  }

  if(75<humidity || humidity<30)
  {
    problem_counter++;
  }

  switch(problem_counter)
  {
    case 0:
      RGB_color(0,255,0); //no problem - green
      break;
    case 1:
      RGB_color(40,255,0); //1 problem - yellow
      break;
    default:
      RGB_color(40,0,0); //2+ problems - red
      problem_sound(); //call buzzer
      break;
  }

  delay(3000);
}


void updateLCD()//updates LCD screen when button is pressed
{
  switch(lcd_counter)
  {
    case 0: //pressed for first time etc.
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Humidity: ");
      lcd.print(humidity);
      lcd.print(" %");
      //problem detected
      if(humidity>75)
      {
        lcd.setCursor(0,1);
        lcd.print("!!Too damp!!");
      }
      else if(humidity<30)
      {
        lcd.setCursor(0,1);
        lcd.print("!!Too dry!!");
      }
      lcd_counter++;
      break;
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Temp: ");
      lcd.print(temp);
      lcd.print(" C");
      //problem detected
      if(temp>28)
      {
        lcd.setCursor(0,1);
        lcd.print("!!Too warm!!");
      }
      else if(temp<10)
      {
        lcd.setCursor(0,1);
        lcd.print("!!Too cold!!");
      }
      lcd_counter++;
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Light: ");
      lcd.print(light);
      //problem detected
      if(light>600)
      {
        lcd.setCursor(0,1);
        lcd.print("!!Too bright!!");
      }
      else if(light<200)
      {
        lcd.setCursor(0,1);
        lcd.print("!!Too dark!!");
      }
      else
      {
        lcd.print("(fine)");
      }

      lcd_counter=0; //counter set back to 0, begin from humidity
      break;
  }
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {//function to use RGB LED in an easier way
  analogWrite(RED_PIN, red_light_value);
  analogWrite(GREEN_PIN, green_light_value);
  analogWrite(BLUE_PIN, blue_light_value);
}

void problem_sound() //created by https://www.arduino.cc/en/Tutorial/BuiltInExamples/toneMelody
{
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(BUZZER, melody[thisNote], noteDuration*0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);
    
    // stop the waveform generation before the next note.
    noTone(BUZZER);
  }
}

void analyze_string(String str) //string from serial divided into variables
{
  for(int i=0;i<str.length();i++)
  {
    if(str[i]=='M')
    {
      i=i+3;
      String humidity_str = "aaaa"; //4 symbols so we have enough space for numbers like -120, 1230
      int j=0;
      while(str[i]!=';')
      {
        humidity_str[j] = str[i];
        j++;
        i++;
      }
      humidity = humidity_str.toInt(); //get integer from string
    }
    else if(str[i]=='T')
    {
      i=i+3;
      String temp_str = "aaaa";
      int j=0;
      while(str[i]!=';')
      {
         temp_str[j] = str[i];
         i++;
         j++;
      }
      temp = temp_str.toInt();
    }
    if(str[i]=='L')
    {
      i=i+3;
      String light_str="aaaa";
      int j=0;
      while(str[i]!=';')
      {
         light_str[j] = str[i];
         i++;
         j++;
      }
      light = light_str.toInt();
    }
  }
}