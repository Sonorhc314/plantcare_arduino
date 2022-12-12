//I use define instead of declaring constants to save memory
#define PUMP_PIN 4 // Water Pump Pin
#define AIR_VALUE 590 //soil moisture sensor shows when in dry air
#define WATER_VALUE 300 //soil moisture sensor shows when in water
#define SENSE_TEMP_PIN A2
#define SENSE_LIGHT_PIN A1
#define SENSE_SOIL_PIN A0
#define START_PUMP 2 //update button

//maxLampLight = 1008; minlamp = 230, mediumlamp = 720

int TempValue; //The variable we will use to store the temp sensor input
int LightValue;
int soilMoistureValue;
int soilmoisturepercent;
double temp; //The variable we will use to store temperature in degrees.
int temp_int;
int last_watering; //time when water pump was ON the last time
 
int watering_flag = 0;

void setup() {
  Serial.begin(9600); // Serial Port setup
  pinMode(PUMP_PIN, OUTPUT); // Set pin as OUTPUT
  pinMode(START_PUMP, INPUT); //button that starts pump if dry soil detected
  attachInterrupt(0, start_thepump, FALLING); // interrupt routine if button clicked
}


void loop() {
  TempValue = analogRead(SENSE_TEMP_PIN);
  LightValue = analogRead(SENSE_LIGHT_PIN);
  soilMoistureValue = analogRead(SENSE_SOIL_PIN);  
  soilmoisturepercent = map(soilMoistureValue, AIR_VALUE, WATER_VALUE, 0, 100);
  
  //if water pump was ON recently(watering_flag==2) and soil moisture is still < 30%, then set it to 30
  //because it should not be read as a problem with dryness if soil has not soaked up yet
  if(watering_flag==2 && soilmoisturepercent<30) 
  {
    soilmoisturepercent = 30;
  }

  temp = (double)TempValue / 1024; //find percentage of input reading
  temp = temp * 5; //multiply by 5V to get voltage
  temp = temp - 0.5; //Subtract the offset
  temp = temp * 100; //Convert to degrees
  temp_int = round(temp); //get a rounded integer
  
  //-----------------------------
  //sends data to arduino-receiver
  Serial.print("M: "); //moisture
  Serial.print(soilmoisturepercent);Serial.print(";");
  Serial.println();
  Serial.print("T: "); //temperature
  Serial.print(temp_int);Serial.print(";");
  Serial.println();
  Serial.print("L: "); //light
  Serial.print(LightValue);Serial.print(";");
  Serial.println();
  //-----------------------------
  
  //check if enough time passed after last watering(4 hours = 4*3600000 milliseconds)
  //if so set system to be available for watering
  if(watering_flag==2 && (millis()-last_watering)>4*3600000)
  { 
    watering_flag=0;
  }
  
  //if button was pressed and flag is set to 1, we can water the plant
  if(watering_flag==1)
  {
    digitalWrite(PUMP_PIN, HIGH);//call water pump
    delay(25000);
    digitalWrite(PUMP_PIN, LOW);
    last_watering = millis();
    watering_flag=2;
  }

  delay(3000);
}

void start_thepump()
{
  if(soilmoisturepercent < 30 || watering_flag == 0)
  {//attach interupt function will work only if moisture < 30% and flaf is 0
    watering_flag =1;
  }
}
