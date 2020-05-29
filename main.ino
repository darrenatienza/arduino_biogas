
/**
 Arduino MQ4 gas sensor - Geekstips.com
 This example is for calculating R0 which is
 the resistance of the sensor at a known concentration 
 without the presence of other gases, or in fresh air
*/
#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
/* <mq4 variables> */
 float sensor_volt; //Define variable for sensor voltage
 float RS_air; //Define variable for sensor resistance
 float R0 = 10; //Define variable for R0
 float sensorValue; //Define variable for analog readings
 int pinLedAlert = 13;
 int pinLedNormal = 12;
/* </mq4 variables> */

 /** <GSM Variables> **/
#define BAUDRATE  9600
//irrigation receipient :+639273353259
//poultry receipient: +639500242431
//seabing receipient: +639482740591
#define PHONE_NUMBER "+639662414301"       //CELLPHONE NUMBER

#define MESSAGE  ""  //COMPOSE YOUR MESSAGE HERE
#define MESSAGE_LENGTH 160

//SERIAL PIN ASSIGNMENT, BAUDRATE, PHONE NUMBER, MESSAGE
//positive and negative must be close to sim800l module to obtain good startup powering
#define PIN_TX    2
#define PIN_RX    3
GPRS GSMTEST(PIN_TX,PIN_RX,BAUDRATE);//RX,TX,BAUDRATE
char message[MESSAGE_LENGTH];
int messageIndex = 0;
char phone[16];
char datetime[24];
bool enableSMS = true; // whether sms is enable or disable

int index_to_send_sms = 0;
int target_count_to_send = 10; 
bool is_ready_to_send_sms = false;

/** </GSM Variables> **/


void intSMS(){
  if(enableSMS){
    Serial.println("Initializing GSM Module");
    //lcd.clear();
    //lcd.setCursor(0,0);
    //lcd.print("Initializing GSM Module");
    while(!GSMTEST.init()) {
        delay(1000);
        Serial.println("GSM INIT ERROR");
        //lcd.clear();
        //lcd.setCursor(4,2); 
        //lcd.print("GSM INIT ERROR");
    }
     Serial.println("GSM INIT SUCCESSFUL");
    //lcd.clear();
    //lcd.setCursor(0,0);
    //lcd.print("GSM INIT SUCCESSFUL"); 
    delay(2000);
  }
}
/** <GSM Operations> */
void sms_try_to_send(){
            
           if(!is_ready_to_send_sms){ //if false (means sent sms done) delay by 30 cur_mins (1800 secs)
           // delay sms sending, if index_to_send_sms is greater than or equal to target_count_to_send
           // then send sms
               if(index_to_send_sms <= target_count_to_send){
                 index_to_send_sms++;
                  Serial.print("Sending after: ");
                  Serial.print(index_to_send_sms);
                   Serial.print("/");
                    Serial.println(target_count_to_send);
                   
               }else{
                   is_ready_to_send_sms = true;
                   index_to_send_sms = 0;
               }
           }else{ //if true(means ready to send sms) send message now
           Serial.println("Sending SMS Notif...");
               //lcd.clear();
               //lcd.setCursor(0,0);
               //lcd.print("Sending SMS Notif...");
               delay(2000); // delete this
              sendSMS("Biogas emission is HIGH!");
              Serial.println("SMS Sent");
               //lcd.setCursor(0,1);
               //lcd.print("SMS Sent");
               delay(1000);
               is_ready_to_send_sms = false;
               }
}

void sendSMS(String message){
 if(enableSMS){
    char charBuf[50];
      message.toCharArray(charBuf,50);
      delay(1000);
      GSMTEST.sendSMS(PHONE_NUMBER,charBuf); //DEFINE PHONE NUMBER AND TEXT
      delay(1000);
 }
  
}
 /** </GSM Operations> */   
void read_mq4(){
 
  for (int x = 0 ; x < 500 ; x++) //Start for loop
  {
    sensorValue = sensorValue + analogRead(A0); //Add analog values of sensor 500 times
  }
  sensorValue = sensorValue / 500.0; //Take average of readings
  sensor_volt = sensorValue * (5.0 / 1023.0); //Convert average to voltage
  RS_air = ((5.0 * 10.0) / sensor_volt) - 10.0; //Calculate RS in fresh air
  R0 = RS_air / 4.4; //Calculate R0
}

void setup() {
  Serial.begin(9600); //Baud rate
  pinMode(pinLedAlert, OUTPUT);
  pinMode(pinLedNormal, OUTPUT);
  intSMS();
}

void loop() {
  if(R0 < 2){
   
    sms_try_to_send();
    digitalWrite(pinLedAlert, HIGH);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(pinLedNormal, LOW);
  }else{
    // set index to zero to reset the current sms index value 
    index_to_send_sms = 0;
      digitalWrite(pinLedAlert, LOW);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(pinLedNormal, HIGH);
  }
  read_mq4();
  Serial.print("R0 = "); //Display "R0"
  Serial.println(R0); //Display value of R0
  delay(1000); //Wait 1 second
}