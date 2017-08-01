#include <OneWire.h>
#include <LiquidCrystal.h>

int DS18S20_Pin = 9; //DS18S20 Signal pin on digital 2
int relay = 13; // Fan relay on pin 12
int pump_on = 7;
int pump_off = 6;
int pump_in = 8;

LiquidCrystal lcd(11, 10, 5, 4, 3, 2);  // initialize the library with the numbers of the interface pins

//Temp settings in celsius you must keep it with dot and value after dot
int high_temp = 99.00;
int norm_temp = 94.00;

//Temperature chip i/o
OneWire ds(DS18S20_Pin);  // ds18b20 on digital pin 2 or change to your ditial pin number

void setup(void) {
	
	// set up the LCD's number of columns and rows: 
	lcd.begin(16, 2);
	
	
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW); // Make relay off be default to prevent clicking at startup
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(8, INPUT_PULLUP);
  
}

void loop(void) {
  int temperature = getTemp();
  Serial.println(temperature); //Print in serial current temp
  Serial.println(high_temp); //Print in serial predefined high temp when fan will turn on
  Serial.println(norm_temp); //Print in serial predefined normal temp when fan will turn off
  
  
  int sensorVal = digitalRead(8);
  
  
  	lcd.setCursor(0, 0);
	lcd.print("** EFC Filter **");
	
	
  
  lcd.setCursor(0, 1);
	lcd.print("H: ");
	lcd.setCursor(7, 1);
	lcd.print("T: ");	
	lcd.setCursor(10,1);
	lcd.print(temperature, DEC);
	lcd.print((char)223);
	lcd.print("C");
  
  
  delay (5000); // Make some delay to see values in serial monitor
 
  if (getTemp() >= high_temp) // FAN goes ON  when water reach high temp
         {
          digitalWrite(relay, HIGH); 
          lcd.setCursor(3, 1);
	      lcd.print("OFF ");
         }
  if (getTemp() <= norm_temp) // Fan goes OFF when temp become normal
         {     
          digitalWrite(relay, LOW);
          lcd.setCursor(3, 1);
	      lcd.print("ON ");
          }
          
          
     if (sensorVal == HIGH) {
		digitalWrite(7,HIGH);
          
	} else {
		digitalWrite(6,LOW);
		delay(1); 
                
	}
         
          
  }

//////////////////////////////////////////////////////////////////////////////////////////////////////// Working with DS18B20
int getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);   
  ds.write(0xBE); // Read Scratchpad

 
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
 
  ds.reset_search();
 
  byte MSB = data[1];
  byte LSB = data[0];

  int tempRead = ((MSB << 8) | LSB); //using two's compliment
  int TemperatureSum = tempRead / 16;
 
  return TemperatureSum;
  
 
 
}