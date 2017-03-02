/* This piece of code receives phonecalls in order to turn off LED attached on PIN 13 on Arduino UNO board using
 SIM800L-EVB Module.

If phone number is not on phonebook (contacts) the system rejects anything from them.

if on phonebook turns off LED.


/OJO/ CHANGES THIS! //////////

Check out the "Add contact" using AT commands code on: (Here comes the GitHub repo with that software, but ot has not been made yet)
This only processes national numbers (ones that don't include the +  prefix).

For example (AT+CPBW=1,"04161587896",129,"name") where 129 means national numbers.
It is IMPERATIVE to save the number with a name or alias and not to leave it empty.
If by mistake someone tries something like (AT+CPBW= 1,"+584161587896",129,"name") or (AT+CPBW= 1,"04161587896",145,"name")
it won't work at all since it has a "+" sign in the first case and the 145 integer in the second.

/OJO/ CHANGES THIS! //////////



We will use the GPRS library from SEEEDUINO. It has a lot of functions for the SIM800L board that will make the development process easier and faster.
You can get it from https://github.com/Seeed-Studio/Seeeduino_GPRS.git

In case you don't know or remember how to implement a library on Arduino IDE,
go on https://www.arduino.cc/en/hacking/libraries. Everything is explanied there very detailed.

*/


/****************************************************************Start of the code********************************************************/



//--------------------------------Start-Libraries Section----------------------------------------------------//

#include <gprs.h>
#include <SoftwareSerial.h>

//--------------------------------End-Libraries Section----------------------------------------------------//


//--------------------------------Start-Definition Section--------------------------------------------------//

#define TIMEOUT    15000
#define LED_PIN    13

//--------------------------------End-Definition Section--------------------------------------------------//


//--------------------------------Start-Object Creation--------------------------------------------------//

GPRS gprs;

//--------------------------------End-Object Creation--------------------------------------------------//


//--------------------------------Start-Variable Declaration-------------------------------------------//

//Boolean to be set to false if number is on phonebook
bool ledStatus = HIGH;

////Variable to hold last line of serial output from SIM800
char currentLine[500] = "";
int currentLineIndex = 0;

//Integers value to determine whether the number who sends SMS belongs the PhoneBook entry previously set.
int firstComma = -1;
int secondComma = -1;

//Boolean to be set to true if number is found on phonebook
bool isInPhonebook = false;

// Variable to hold phonebook from call
char phonebook[13];

// Variable to send SMS
char *number = NULL;

////Boolean to be set to true if call notificaion was found and next
bool nextLineIsCall = false;


//--------------------------------End-Variable Declaration-------------------------------------------//



//------------------------------End-Libraries Section--------------------------------------------------------//


//--------------------------------Start-Setup Section-----------------------------------------------//

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  while (!Serial);

  // initialize digital pin 13 as an output.
  pinMode(LED_PIN, OUTPUT);

  //Write current status to LED pin
  digitalWrite(LED_PIN, ledStatus);

  Serial.println("Starting SIM800 SMS Command Processor");
  gprs.preInit();
  delay(1000);

  while (0 != gprs.init()) {
    delay(1000);
    Serial.print("init error\r\n");
  }

  //Set call notification
  if (0 != gprs.sendCmdAndWaitForResp("AT+CLIP=1\r\n", "OK", TIMEOUT)) {
    ERROR("ERROR:CLIP");
    return;
  }
  Serial.println("Init success");

}

//--------------------------------End-Setup Section----------------------------------------------//


void loop() {
  //Write current status to LED pin
  digitalWrite(LED_PIN, ledStatus);
  static char phonebook[13];
  int firstComma = -1;
  int secondComma = -1;
  int thirdComma = -1;
  int forthComma = -1;
  int j = 0;
  ////char *number = NULL;

  //If there is serial output from SIM800
  if (gprs.serialSIM800.available()) {
    char lastCharRead = gprs.serialSIM800.read();
    //Read each character from serial output until \r or \n is reached (which denotes end of line)
    if (lastCharRead == '\r' || lastCharRead == '\n') {
      String lastLine = String(currentLine);

      //If last line read +CMT, New SMS Message Indications was received.
      //Hence, next line is the message content.
      if (lastLine.startsWith("RING")) {

        Serial.println(lastLine);
        nextLineIsCall = true;

      } else if ((lastLine.length() > 0) && (nextLineIsCall)) {
          Serial.println(lastLine);
          
          firstComma = lastLine.indexOf(',');
          Serial.println(firstComma);  //For debugging
          secondComma = lastLine.indexOf(',', firstComma + 1);
          Serial.println(secondComma); //For debugging
          thirdComma = lastLine.indexOf(',', secondComma + 1);
          Serial.println(thirdComma);  //For debugging
          forthComma = lastLine.indexOf(',', thirdComma + 1);
          Serial.println(forthComma); //For debugging
          
          nextLineIsCall = false;
          isInPhonebook = false;
      }

      //Clear char array for next line of read
      for ( int i = 0; i < sizeof(currentLine);  ++i ) {
        currentLine[i] = (char)0;
      }
      currentLineIndex = 0;
    } else {
      currentLine[currentLineIndex++] = lastCharRead;
    }
  }
}


//Start Sending SMS Section

//       firstComma = lastLine.indexOf(34); //ASCII code for ""
//       Serial.println(firstComma);  //For debugging
//       secondComma = lastLine.indexOf(34, firstComma + 1);
//      Serial.println(secondComma); //For debugging
//
//       for (int i = firstComma+1; i < secondComma; ++i) {
//        phonebook[j] = lastLine[i];
//          ++j;
//        }
//       phonebook[j] = '\0';
//       Serial.println(phonebook);
//       number = phonebook;


//End Sending SMS Section


