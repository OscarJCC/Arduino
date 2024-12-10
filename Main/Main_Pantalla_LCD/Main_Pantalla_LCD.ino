#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init(); // Inicia la LCD
  lcd.backlight(); // Prende la LCD
  lcd.setCursor(3, 0); // Coloca el cursor en las coordenadas de la LCD
  lcd.print("OSCAR JOEL"); // Imprime un mensaje en la LCD
  lcd.setCursor(0, 1);
  lcd.print("CASTRO CONTRERAS");
  delay(3000); // Detiene el Arduino en segundos
  lcd.clear(); // Limpia la pantalla de la LCD
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("ANGULO :");
  lcd.setCursor(13, 0);
  lcd.print(360);
  lcd.setCursor(0, 1);
  lcd.print("VOLTAJE:");
  lcd.setCursor(13, 1);
  lcd.print(5.0);
}
