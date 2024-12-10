#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

int lectura = 0,boton = 0, i = 0, l = 0, j = 1, m = 0;
float voltaje, angulo, b = 180, A;

void setup(){
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("OSCAR JOEL");
  lcd.setCursor(2, 1);
  lcd.print("JUAN ARMANDO");
  delay(1000);
  lcd.clear();
  pinMode(2, INPUT_PULLUP);
}
void loop() {
  if (l == 1 and A == angulo){
    lcd.clear();
  }
  else if (l == 0 and j == 0){
    lcd.clear();
    j = 1;
  }

  boton = digitalRead(2);
  lectura = analogRead(A0);

  Serial.println(lectura);

  if(boton == 0){
    delay(500);
    if (l == 0){
      A = angulo;
      l = 1;
    }
    else if (l == 1){
      A = 0;
      l = 0;
      j = 0;
    }
  }
  
  if (abs(angulo) < 9 and i == 1){
    lcd.clear();
    i = 0;
  }
  else if (abs(angulo) <= 99 and abs(angulo) >= 10 and i == 2){
    lcd.clear();
    i = 1;
  }
  else if (abs(angulo) <= 9){
    i = 0;
  }
  else if (abs(angulo) <= 99 and abs(angulo) >= 10){
    i = 1;
  }
  else if(abs(angulo) >= 100){
    i = 2;
  }
  
  lcd.setCursor(0, 0);
  lcd.print("ANGULO :");
  if (angulo < 0){
    lcd.setCursor(13, 0);
  }
  else if (angulo >= 0){
    lcd.setCursor(13, 0);
  }
  lcd.print(abs(angulo),0);
  lcd.setCursor(0, 1);
  lcd.print("VOLTAJE:");
  lcd.setCursor(12, 4);
  lcd.print(voltaje);
}