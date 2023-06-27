#include <LiquidCrystal_I2C.h>
#include <Wire.h>

unsigned long  counts; //variable for GM Tube events
unsigned long previousMillis; //variable  for measuring time
float averageCPM;
float sdCPM;
int currentCPM;
float  calcCPM;
LiquidCrystal_I2C lcd(0x27, 20, 4);
float CPMArray[100];

#define  LOG_PERIOD 30000 // count rate (in milliseconds)

void setup() { //setup
  counts = 0;
  currentCPM = 0;
  averageCPM = 0;
  sdCPM = 0;
  calcCPM  = 0;
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  pinMode(2,  INPUT);
  attachInterrupt(digitalPinToInterrupt(2), impulse, FALLING); //define  external interrupts
}

void loop() { //main cycle
  lcd.setCursor(0,2);
  lcd.print("CPM Count: ");
  lcd.print(counts);
  unsigned long currentMillis  = millis();
  if (currentMillis - previousMillis > LOG_PERIOD) {
    previousMillis  = currentMillis;
    CPMArray[currentCPM] = counts * 2;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("uSv/hr: ");
    lcd.print(outputSieverts(CPMArray[currentCPM]));
    counts = 0;
    averageCPM = 0;
    sdCPM = 0;
    //calc avg and sd
    for (int x=0;x<currentCPM+1;x++)  {
      averageCPM = averageCPM + CPMArray[x];
    }
    averageCPM = averageCPM / (currentCPM + 1);
    for (int x=0;x<currentCPM+1;x++)  {
      sdCPM = sdCPM + sq(CPMArray[x] - averageCPM);
    }
    sdCPM  = sqrt(sdCPM / currentCPM) / sqrt(currentCPM+1);

    Serial.println("Avg:  " + String(averageCPM) + " +/- " + String(sdCPM) + "  ArrayVal: " + String(CPMArray[currentCPM]));
    currentCPM = currentCPM + 1;
    displayAverageCPM();
  } 
}

void  impulse() {
  counts++;
}
void displayAverageCPM()  {
  lcd.setCursor(0,1);
  lcd.print("Avg: ");
  lcd.print(outputSieverts(averageCPM));
  lcd.print("+/-");
  lcd.print(outputSieverts(sdCPM));
}
float outputSieverts(float x)  {
  float y = x * 0.0057;
  return y;
}