#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x20,16,2);
#include <SPI.h>
#include <SD.h>
#include <MCP_DAC.h>
MCP4921 myDAC;
const byte dacPin=9;
long t, t0;
byte sample;
int no_of_samples = 0;
int RECPin = 2;
int PLAYPin = 3;
bool RECStatus = LOW;
bool PLAYStatus = LOW;
//String line;
float fs;
int CSPin = 10;
int welcome=0;
int dt=1000;
int playstate=0;
File recFile;


void setup()
{
  lcd.begin(16,2);
 
  Serial.begin(115200);
  Serial.println("Serial Start...");
  
  Serial.print( "Initializing SD card...");

  if (!SD.begin(CSPin)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  myDAC.begin(9);
  while(welcome==0){
    lcd.setCursor(0,0);
    lcd.print("Welcome");
    delay(dt);
    welcome=1;
  
  }
  
//  ADCSRA = 0;             // clear ADCSRA register
//  ADCSRB = 0;             // clear ADCSRB register
//  ADMUX |= (0 & 0x07);    // set A0 analog input pin
//  //ADMUX |= (1 << REFS0);  // set reference voltage
//  ADMUX |= (1 << ADLAR);  // left align ADC value to 8 bits from ADCH register

  // sampling rate is [ADC clock] / [prescaler] / [conversion clock cycles]
  // for Arduino Uno ADC clock is 16 MHz and a conversion takes 13 clock cycles
  //ADCSRA |= (1 << ADPS2) | (1 << ADPS0);    // 32 prescaler for 38.5 KHz
  //ADCSRA |= (1 << ADPS2);                     // 16 prescaler for 76.9 KHz
  //ADCSRA |= (1 << ADPS1) | (1 << ADPS0);    // 8 prescaler for 153.8 KHz

//  ADCSRA |= (1 << ADATE); // enable auto trigger
//  ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
//  ADCSRA |= (1 << ADEN);  // enable ADC
//  ADCSRA |= (1 << ADSC);  // start ADC measurements

}

  
void loop(){
  playstate=0;
  Serial.println("Waiting for input...");
  lcd.noBlink();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Which option?");
  delay(dt);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Record");
  lcd.setCursor(0,1);
  lcd.print("Play");

  do{
    RECStatus = digitalRead(RECPin);
    PLAYStatus = digitalRead(PLAYPin);
    delay(100);
  }while(RECStatus == LOW && PLAYStatus == LOW);
  if (RECStatus == HIGH){
    record();
  }else if (PLAYStatus == HIGH){
    playfile();
  }
  Serial.println("Restarting...");
  Serial.println("-------------------------------");
  delay(500);
  
}

void record(){
  SD.remove("rec.txt");
  recFile = SD.open("rec.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (recFile) {
    Serial.println("File opened sucessfully");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening REC.txt");
    while(1){
      
    }
  }
  
  Serial.println("Recording...");
  delay(500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Recording");
  lcd.setCursor(9,0);
  lcd.blink();
  RECStatus = digitalRead(RECPin);
  t0 = micros();
  while(RECStatus == LOW){
    sample = analogRead(A0); //sample = ADCH;
    recFile.println(sample);
    no_of_samples++;
    RECStatus = digitalRead(RECPin);
  }
  t = micros() - t0; // calculate elapsed time

  recFile.close();
  Serial.println("Recording Stopped.");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("COMPLETE");
  delay(dt);
  fs = (float)no_of_samples*1000/t;
  Serial.print(no_of_samples);
  Serial.print(" samples recorded in ");
  Serial.print(t);
  Serial.println(" microseconds");
  Serial.print("Sampling frequency: ");
  Serial.print(fs);
  Serial.println(" KHz");
  Serial.println("");
  no_of_samples = 0;
}

void playfile(){
  while(playstate==0){
    
    String line;
    recFile = SD.open("rec.txt", FILE_READ);

  // if the file opened okay, write to it:
    if (recFile) {
      Serial.println("File opened sucessfully");
  } else {
    // if the file didn't open, print an error:
      Serial.println("error opening REC.txt");
  }
    recFile=SD.open("rec.txt");
    Serial.println("Playing...");
    lcd.noBlink();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Playing");
    lcd.setCursor(7,0);
    lcd.blink();
    while(recFile.available()){
      t0= micros();
      line = recFile.readStringUntil('\n');
      myDAC.analogWrite(line.toInt());
      Serial.print(line);
      t = micros() - t0;
      delayMicroseconds(1000000/fs - t);
  }
    recFile.close();
    playstate=1;
  }
  
}

//#include <SD.h>                     
//#include <SPI.h>
//#include <MCP_DAC.h>
//MCP4921 myDAC;
//#define SD_ChipSelectPin 4 
//
//unsigned long t0, ts;
//File testFile;

//void setup(){ 
//  String temp;
//  
//  Serial.begin(9600);
//  if (!SD.begin(SD_ChipSelectPin)) {  
//    Serial.println("SD fail");  
//    return;  
//  }
//  else{   
//    Serial.println("SD ok");   
//  }
//
//  myDAC.begin(9);
//  //testFile = SD.open("test3.txt");
//}
//
//
//
//void loop(){ 
//  String full;
//  testFile = SD.open("rec.txt");
//    while (testFile.available()){
//      t0= micros();
//      full = testFile.readStringUntil('\n');
//      //temp = testFile.readStringUntil("\n"); 
//      Serial.println(full); 
//      myDAC.analogWrite(full.toInt());
//      t = micros() - t0;
//      delayMicroseconds(1000000/fs - t);
//  }
//  testFile.close();
//  while(1){
//    
//  }
//  
//}
