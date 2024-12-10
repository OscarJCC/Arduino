#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

int boton, i = 0, l = 0, lectura1, lectura2, A, PA, sensor, sensora;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("VELOCIDAD");
  lcd.setCursor(5, 1);
  lcd.print("MOTOR");
  delay(1500);
  lcd.clear();

  pinMode(2, INPUT_PULLUP);
  pinMode(4, INPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);

}

void loop() {
  sensor = digitalRead(4);
  boton = digitalRead(2);
  sensora = analogRead(A0);
  lectura1 = analogRead(A1);
  lectura2 = analogRead(A2);
  A = map(lectura2,0,1023,0,255);
  PA = A*100/255;
  //tone(5,A);
  analogWrite(5,A);
  analogWrite(6,A);

  Serial.println(sensor);

  if (boton == 0){
    delay(500);
    lcd.clear();
    if (l == 0){
      l = 1;
    } else if (l == 1){
      l = 0;
    }
  }

  if (l == 0){
    lcd.setCursor(0, 0);
    lcd.print("Senal PWM");
    lcd.setCursor(0, 1);
    lcd.print("Porcentaje");
    lcd.setCursor(13, 1);
    lcd.print(PA);

    if (PA < 9 and i == 1){
      lcd.clear();
      i = 0;
    }
    else if (PA <= 99 and PA >= 10 and i == 2){
      lcd.clear();
      i = 1;
    }
    else if (PA <= 9){
      i = 0;
    }
    else if (PA <= 99 and PA >= 10){
      i = 1;
    }
    else if(PA >= 100){
      i = 2;
    }

  }else if (l == 1){
    lcd.setCursor(0, 0);
    lcd.print("Rev / Min");
    lcd.setCursor(0, 1);
    lcd.print(sensor);
  }
}

