#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>
#include <TMRpcm.h>
bool flag = false;
const byte rows = 2;
const byte cols = 3;
char keys[rows][cols] = {
  {'1', '2', '3'},
  {'4', '5', '6'}
};
byte rowPins[rows] = {A1, A2};
byte colPins[cols] = {A3, A4, A5};
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

Keypad gajo = Keypad(makeKeymap(keys), rowPins, colPins, 2, 3);
TMRpcm audio;

const byte dacPin = 9;
long t, t0;
uint8_t sample;
int no_of_samples = 0;
String line;
float fs;
int CSPin = 10;
int welcome = 0;
int dt = 100;
int playstate = 0;
int clips = 1;
char chosenKey;
char playKey;
File recFile;
char c[4][10] = {"rec1.csv", "rec2.csv", "rec3.csv"};
int i = 0;

File lfilter;
float y_pre=0;

void setup()
{
   
  audio.speakerPin = 9;
  lcd.begin(16, 2);
  if (!SD.begin(CSPin)) {
    lcd.print("SD Failed");
    while (1);
  }
  lcd.print("SD Loaded");
  delay(1000);
  audio.CSPin = CSPin;

  while (welcome == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Welcome");
    delay(dt);
    welcome = 1;
  }
  File root = SD.open("/");
  //deleteFiles(root);    // delete files initial
  root.close();
  DDRD = 0B11111111;
}

ISR(TIMER2_OVF_vect) {
  flag = true;
  //TCNT2 = 0x05;
}

void loop() {
  playstate = 0;
  lcd.noBlink();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Which option?");
  delay(dt);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("6-Record 5-effects");
  lcd.setCursor(0, 1);
  lcd.print("4-Play");
  do {
    chosenKey = gajo.getKey();
    delay(100);
  } while (chosenKey != '6' && chosenKey != '4' && chosenKey != '5');
  if (chosenKey == '6') {
    maincode_record();
  } else if (chosenKey == '4') {
    maincode_playfile(0);
  }
  else if (chosenKey == '5') {
    maincode_playfile(1);
  }
  
/*  else if (chosenKey == '5') {

    File root = SD.open("/");
    deleteFiles(root);
    root.close();
    lcd.clear();
    lcd.setCursor(0, 1);
    clips = 1;
    i = 0;
    lcd.print("Deleted all");
    delay(2000);
    lcd.clear();
  }*/
  playstate = 0;
  delay(100);
}

void maincode_record() {
  if (clips == i + 1 && clips != 4) {
    record();
  }
  else {
    lcd.clear();
    lcd.print("Delete first");
    delay(1000);
    lcd.clear();
  }
}
void record() {
// =======================================
//  recFile = SD.open(c[i], FILE_WRITE);
//  if (recFile) {
//  } else {
//    while (1) {
//    }
//  }
// =======================================
  delay(100);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Recording");
  lcd.setCursor(9, 0);
  lcd.blink();
  delay(200);
// =======================================
//  ADC_init();
//  while (1) {
//    ADC_getSample(); //sample = ADCH;
//    no_of_samples++;
//    if (no_of_samples == 20000) {
//      break;
//    }
//  }
//  recFile.close();
// =======================================
  audio.startRecording(c[i],16000,A0);
  delay(5000);
  audio.stopRecording(c[i]);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("COMPLETE");
  delay(dt);
  no_of_samples = 0;
  i += 1;
  clips += 1;
}

void maincode_playfile(int m) {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Which clip?");
  while (1) {
    playKey = gajo.getKey();
    if (playKey == '1' || playKey == '2' || playKey == '3') {
      delay(100);
      lcd.clear();
      break;
    }
  }
  if (playKey == '1') {
    if (m==0){
    playfile(0);
    }else{
    play_effects(0);
    }
  }
  else if (playKey == '2') {
    if (m==0){
    playfile(1);
    }else{
    play_effects(1);
    }
  }
  else if (playKey == '3') {
    if (m==0){
    playfile(2);
    }else{
    play_effects(2);
    }
  }
}

int playfile(int d) {

  audio.play(c[d]);
// =======================================
//  while (playstate == 0) {
//    uint8_t line;
//    recFile = SD.open(c[d], FILE_READ);
//    if (recFile) {
//      lcd.noBlink();
//      lcd.clear();
//      lcd.setCursor(0, 0);
//      lcd.print("Playing");
//      lcd.setCursor(7, 0);
//      lcd.blink();
//      PORTD = B00000000;
//      while (recFile.available()) {
//        line = recFile.read();
//        while (1) {
//          if (flag) {
//            PORTD = line;
//            break;
//          }
//        }
//        playstate = 1;
//      }
//    } else {
//      //Serial.println("error opening REC.txt");
//      lcd.clear();
//      lcd.setCursor(0, 1);
//      lcd.print("File n/a");
//      delay(2000);
//      lcd.clear();
//      playstate = 1;
//    }
//    recFile.close();
//  }
// =======================================
}

int play_effects(int R){
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Processing");
   lowfilter_main(R);
   lfilter = SD.open("lowpass.csv",FILE_READ);
    PORTD = B00000000;
    while (lfilter.available()) {
        line = lfilter.read();
        while (1) {
          if (flag) {
            PORTD = line.toInt();
            break;
          }
        }
      }
  lfilter.close(); 
  SD.remove("lowpass.txt");  
}

int lowfilter_main(int l){
   recFile = SD.open(c[l], FILE_READ);
   lfilter = SD.open("lowpass.csv",FILE_WRITE);
   while(1){
            line = recFile.read();
            int line_=line.toInt();
            //int y_current=(0.5335*y_pre)+(0.4665*line_);//2000
            //int y_current=(0.7304*y_pre)+(0.2696*line_);//200-25
            int y_current=(0.8546*y_pre)+(0.1454*line_);//200-125
            lfilter.write((y_current));
            y_pre=y_current;
          if (!recFile.available()){break;}
        }
   lfilter.close();
   recFile.close();
}

void deleteFiles(File r) {
  while (true) {
    File dir = r.openNextFile();
    if (!dir) {
      dir.close();
      clips = 1;
      lcd.clear();

      break;
    }
    SD.remove(dir.name());
    dir.close();
  }
}

void ADC_init() {
  sei();
  TCCR2B = 0b000000001; // Timer Frequency = 16 MHz / 8 = 2 MHz
  TIMSK2 |= (1 << TOIE2); //Enable the timer overflow interrupt
  Serial.println("Timer interrupts configured");
  ADCSRA |= (1 << ADPS2) |  (1 << ADPS1) |  (1 << ADPS0);
  ADMUX |= 0x00;
  ADMUX |= (1 << REFS0);
  ADMUX |= (1 << ADLAR);
  ADCSRA |= (1 << ADEN);
  Serial.println("ADC Initialized");
}

void ADC_getSample() {
  ADCSRA |= (1 << ADSC);
  //Serial.println("ABC");
  // Starting conversion
  recFile.write(sample);
  while (1) {
    if (flag) {
      // writing previous reading to file while converting
      sample = ADCH;
      break;
    }
  }
  //read upper 8bits
}

void ADC_stop() {
  //ADCSRA |= (1<<ADEN);
  //Serial.println("DEF");
  //TIMSK2 |= (0 << TOIE2);
  cli();
}
