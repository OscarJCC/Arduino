#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int lectura = 0, boton = 0, estado = 0; // Define variables enteras
float voltaje, angulo, b = 180, A; // Define variables flotantes

void setup() {
  Serial.begin(115200); // Inicia la comunicacion Serial del arduino
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("OSCAR JOEL");
  lcd.setCursor(0, 1);
  lcd.print("CASTRO CONTRERAS");
  delay(3000);
  lcd.clear();
// Usa un pin Di, i = 1,2,3,...,13 como una entrada con INPUT y como una
// salida con OUTPUT
  pinMode(3, INPUT_PULLUP);
  pinMode(6, OUTPUT);

  //tone(6,60,10000); // Genera una funcion cuadrada de minimo 31HZ, recibe de argumento (Pin de salida, Frecuencia, Tiempo de duracion)
  //digitalWrite(6,HIGH); // Activa la salida de voltaje de un ciero pin con HIGH y lo apaga con LOW
  analogWrite(6,191); // Genera una señal PWM de la cual sale por un pin y puede haver un rango de 0 - 255, donde 255 significa 100 de la señal en alto
  }

void loop() {
  boton = digitalRead(3); // Lee el valor que entra por el pin usado
  lectura = analogRead(A0); // Lee el valor que entra por un pin analigico Ai, i = 0,1,2,...,7

  Serial.println(boton); // Imprime en Monitor serial el valor de boton

  lcd.setCursor(0, 0);
  lcd.print("Lectura:");
  lcd.setCursor(12, 0);
  lcd.print(lectura);
  lcd.setCursor(0, 1);
  lcd.print("Boton  :");
  lcd.setCursor(15, 1);
  lcd.print(boton);
}
