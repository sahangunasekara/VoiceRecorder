/*
   WROTE BY : VIDURA MUNASINGHE
   https://github.com/vidurawarna
   Definitions and implimentations all the functions used in recorder
*/

//void setupKeyPad() {
//  /*
//     This function is used to initialize the keypad
//  */
//  for (char r_ = 0; r_ < r; r_++) {
//    pinMode(rPins[r_], INPUT);    //set the row pins as input
//    digitalWrite(rPins[r_], HIGH);    //turn on the pullups
//  }
//
//  for (char c_ = 0; c_ < c; c_++) {
//    pinMode(cPins[c_], OUTPUT);   //set the column pins as output
//  }
//}
//char keyInput() {
//  /*
//     This function detects a keypress and return the corrosponding
//
//  */
//  char k = 0;
//
//  for (char c_ = 0; c_ < c; c_++) {
//    digitalWrite(cPins[c_], LOW);
//    for (char r_ = 0; r_ < r; r_++) {
//      if (digitalRead(rPins[r_]) == LOW) {
//        delay(20);    //20ms debounce time
//        while (digitalRead(rPins[r_]) == LOW);
//        k = keys[r_][c_];
//      }
//    }
//    digitalWrite(cPins[c_], HIGH);
//  }
//  return k;
//}

char keyInput() {
  /*
    This function detects a keypress and return the corrosponding
  */
  char k = 0;
  //int PinVal = analogRead(keypadPin);
  //Serial.println(PinVal);
  if (analogRead(keypadPin) < 1023) {
    for (char i = 0; i < 16; i++) {
      if (abs(analogRead(keypadPin) - realVals[i]) < 5) {
        k = keys[i];
        while (analogRead(keypadPin) < 1000);
      }
    }
  }
  return k;
}

void readFromFile()
{
  /*This function reads data from the specified file and display*/
  String f = getKeyInput();
  String line, ch;
  test_File = SD.open(f);
  if (test_File) {
    clrDisplay("Playing Track " + String(f[0]));

    while (test_File.available()) {
      st = micros();
      char key = keyInput();
      if (key && key == '*') {
        break;
      }
      line = test_File.readStringUntil('\n');
      PORTD = line.toInt();

      //Serial.print(test_File.read());

      t = micros();
    if ( t - st < fsDelayout) {

      delayMicroseconds(fsDelayout + st - t);
    }
      //Serial.println(micros() - st);
    }

    // close the file:
    secondLine("End of play");
    test_File.close();
    delay(1000);


  } else {
    // if the file didn't open, print an error:
    secondLine("No such file");
    delay(1000);
  }
}


void record() {
  /*Used to record the data got from input into a file*/

  // open the file.
  File root = SD.open("/");
  int fcount = countFiles(root);
  root.close();

  test_File2 = SD.open(checkDuplicates(fcount), FILE_WRITE);//this function is used to check duplicates when new files are created

  // if the file opened okay, write to it:
  if (!test_File2) {
    clrDisplay("error !!");
    delay(1000);
  }
  else {
    clrDisplay("Recording....");
  }



  while (true) {
    st = micros();

    pot_Read = analogRead(pot) * (255. / 1023.);
    test_File2.println(pot_Read);

    char key = keyInput();

    if (key && key == '*') {
      break;
    }
t = micros();
    if ( t - st < fsDelayin) {

      delayMicroseconds(fsDelayin + st - t);
    }
    //Serial.println(t - st);
  }
  
  test_File2.close();
  clrDisplay("Data recorded.");
  delay(1000);

}

/*From here the functions countFiles, deleteAll,getTrackList and
  deleteFile use the same itteration method to handle file operations*/

int countFiles(File r) {
  /*This loop checks for next file to open and
    when there are no fles it stops counting, then return the count*/
  int c = 0;
  while (true) {
    File dir = r.openNextFile();
    if (!dir) {
      dir.close();
      break;
    }
    c++;
    dir.close();
  }
  return c;
}



void deleteAll(File r) {
  while (true) {
    File dir = r.openNextFile();
    if (!dir) {
      dir.close();
      break;
    }
    SD.remove(dir.name());
    dir.close();
  }
}


void deleteFile(File r) {
  String n = getKeyInput();
  while (true) {
    File dir = r.openNextFile();
    if (!dir) {
      dir.close();
      clrDisplay("No such file.");
      delay(1000);
      break;
    } else if (String(dir.name()) == n) {
      SD.remove(dir.name());
      dir.close();
      clrDisplay("File removed");
      delay(1000);
      break;
    }
    dir.close();
  }
}

String getKeyInput() {
  //used to get the user inputs from the keypad
  String res = "";
  while (true) {
    //char key_input = keypad.getKey();
    char key_input = keyInput();
    if (key_input == '*') {
      break;
    }
    else if (key_input) {
      clrDisplay(String(key_input));
      res += String(key_input);
    }
  }
  return res + ".TXT";
}

void getTrackList(File r) {
  while (true) {
    File dir = r.openNextFile();
    if (!dir) {
      dir.close();
      clrDisplay("End of files !!");
      delay(1000);
      break;
    }

    secondLine("Track " + String(dir.name()[0]));
    delay(1000);
    dir.close();
  }
}

/*This function checks if the new file to be made is existing,
  if does it generates a new name for the file*/

String checkDuplicates(int count) {
  String fname_temp = String(count + 1) + ".txt";
  if (SD.exists(fname_temp)) {
    fname_temp = checkDuplicates(count + 1);
  }
  else {
    return fname_temp;
  }
}

void firstLine(String msg) {
  //Prints the string passed in the first line of the LCD display
  lcd.setCursor(0, 0);
  lcd.print(msg);
}

void clrDisplay(String msg) {
  //Clears the LCD and displays the msg in first line
  lcd.clear();
  firstLine(msg);
}

void secondLine(String msg) {
  //Prints the string passed in the second line of the LCD display
  lcd.setCursor(0, 1);
  lcd.print(msg);
}
