#include <Keypad.h>
#include <LiquidCrystal.h>


const int SWITCH = 1;
const int BUZZER = 0;
const int LED = 9;
const int ROW_NUM = 4;
const int COLUMN_NUM = 3;
const int contrast = A3;

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'x','0','#'}
};

char code[7] = {'*','*','*','*','*','*','*'};



byte pin_rows[ROW_NUM] = {5, 7, 8, 6};
byte pin_column[COLUMN_NUM] = {2, 3, A6};

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

const int rs = 10, en = 11, d4 = 12, d5 = A0, d6 = A1, d7 = A2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

bool armed = false;
bool count = false;

void setup() {
  Serial.begin(115200);
  Serial.println(1);
  pinMode(SWITCH,INPUT_PULLUP);
  pinMode(LED,OUTPUT);
  pinMode(contrast,OUTPUT);
  analogWrite(contrast, 255);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("*******");
  lcd.noDisplay();
  digitalWrite(LED,LOW);
}

int second = 0;
int last = millis();
int lastBeep = millis();
int beepLength = 125;
bool beeping = false;
void loop() {
  
  if (!armed && !count && digitalRead(SWITCH) == 0) {
    armed = true;
    lcd.display();
    Serial.println("ARMED");
    tone(BUZZER,2500,100);
    delay(200);
    tone(BUZZER,2500,100);
    delay(200);
  }
  if (armed) {
    char key = keypad.getKey();

    if (key){
      tone(BUZZER,2000);
      delay(50);
      noTone(BUZZER);
      //Serial.println(key);
      int code_length = sizeof(code) / sizeof(code[0]);
      String toDisplay = "";
      for(int i = 0; i < code_length; i++) {
        if (code[i] == '*') {
          code[i] = key;
          for(int j = 0; j < code_length;j++) {
            if(code[j] != '*'){
              toDisplay += code[j];
            }
            
          }
          int starLength = code_length - toDisplay.length();
          for(int z = 0; z < starLength; z++) {
            toDisplay = "*" + toDisplay;
          }
          lcd.clear();
          lcd.setCursor(4,0);
          lcd.print(toDisplay);
          break;
        }
      }
      Serial.println(code[6]);
      if(code[6] != '*') { // FULL
        Serial.println("FULL");
        if (code[0] == '7' && code[1] == '3' && code[2] == '5' && code[3] == '5' && code[4] == '6' && code[5] == '0' && code[6] == '8') {
          //CORRECT
          lcd.clear();
          lcd.setCursor(2,0);
          lcd.print("BOMB PLANTED");
          
          count = true;
          armed = false;
          
        }
        else {
          // INCORRECT
          lcd.clear();
          lcd.setCursor(4,0);
          lcd.print("*******");
          tone(BUZZER,2500,100);
          delay(200);
          tone(BUZZER,2500,100);
          delay(200);
        }
        for(int i = 0; i < code_length; i++) {
          code[i] = '*';
        }
      }
    }
    // set count to true if button press
    //
  }
  if (count) {
    
    if(millis()-last >= 1000) {
      last = millis();
      second++;
    }

    int bps;
    if (second >= 0 && second <= 20) {
        beepLength = 125;
        bps = 1;
    }
    if (second > 20 && second <= 30) {
        beepLength = 100;
        bps = 2;
    }
    if (second > 30 && second <= 35) {
        beepLength = 75;
        bps = 4;
    }
    if (second > 35 && second < 38)  {
        beepLength = 50;
        bps = 10;
    }
    if (second >= 38 && second < 40)  {
        beepLength = 25;
        bps = 20;
    }
    if (second >= 40) {
      second = 0;
      digitalWrite(LED,HIGH);
      for(int i = 150; i <10000;i+=50 ) {
        tone(BUZZER,i);
        delay(10);
      }
      for(int i = 0; i < 8;i++) {
        tone(BUZZER,5000,50);
        delay(100);
      }
      armed = false;
      count = false;
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("*******");
      lcd.noDisplay();
      digitalWrite(LED,LOW);
    }  
    
    else{
      int wait = (int) 1000.0 / bps;
     
      if(millis()-lastBeep > wait + beepLength) {
        lastBeep = millis();
        digitalWrite(LED,HIGH);
        tone(BUZZER,2146,beepLength);
        delay(beepLength);
        digitalWrite(LED,LOW);
      }
    }
  }
}
