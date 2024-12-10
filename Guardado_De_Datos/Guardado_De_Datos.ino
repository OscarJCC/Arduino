#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <SPI.h>
#include <SD.h>

#define SSpin 10

File archivo;

int boton, i = 0, j, g = pow(2,12), lectura;
String l = "Seccion", N[26] = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","Y","Z"};
long ti;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.print("Iniciando SD ....");
  delay(500);
  lcd.clear();

  if (SD.begin(SSpin) == 0){
    lcd.print("Fallo Inico SD");
    delay(500);
    lcd.clear();
    return;
  }

  //SD.remove("Seccion.txt");
  lcd.print("Inicio Correcto");
  delay(500);
  lcd.clear();

  pinMode(2, INPUT_PULLUP);
  pinMode(3, OUTPUT);
  //pinMode(4, OUTPUT); No hay PWM
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  //pinMode(7, OUTPUT); No hay PWM
  //pinMode(8, OUTPUT); No hay PWM
  pinMode(9, OUTPUT);

  //digitalWrite(6,HIGH);
  analogWrite(3,76);//63 -> 25%, 76 -> 30%, 127 -> 50%, 178 -> 70%, 191 -> 75%, 229 -> 90%, 255 -> 100%
  //analogWrite(4,76);//63 -> 25%, 76 -> 30%, 127 -> 50%, 178 -> 70%, 191 -> 75%, 229 -> 90%, 255 -> 100%
  analogWrite(5,76);//63 -> 25%, 76 -> 30%, 127 -> 50%, 178 -> 70%, 191 -> 75%, 229 -> 90%, 255 -> 100%
  analogWrite(6,76);//63 -> 25%, 76 -> 30%, 127 -> 50%, 178 -> 70%, 191 -> 75%, 229 -> 90%, 255 -> 100%
  //analogWrite(7,76);//63 -> 25%, 76 -> 30%, 127 -> 50%, 178 -> 70%, 191 -> 75%, 229 -> 90%, 255 -> 100%
  //analogWrite(8,76);//63 -> 25%, 76 -> 30%, 127 -> 50%, 178 -> 70%, 191 -> 75%, 229 -> 90%, 255 -> 100%
  analogWrite(9,76);//63 -> 25%, 76 -> 30%, 127 -> 50%, 178 -> 70%, 191 -> 75%, 229 -> 90%, 255 -> 100%
  //tone(6,100);
}

void loop() {

  lectura = analogRead(A0);
  Serial.println(lectura);

  lcd.setCursor(0, 0);
  lcd.print("Pulsa Boton Para");
  lcd.setCursor(0, 1);
  lcd.print("Capturar Datos");

  boton = digitalRead(2);

  if (boton == 0){
    lcd.clear();
    l += N[i];
    //Serial.println(l);
    archivo = SD.open(l+".txt",FILE_WRITE);
    if(archivo != 0){
      ti = millis();

      for (j = 0; j < g; j += 1){
        archivo.println(analogRead(A0));
      }
      
      archivo.println(millis()-ti);

      lcd.clear();
      archivo.close();
      lcd.print("Archivo Cerrado");
      delay(500);
      lcd.clear();
    }
    else{
      lcd.print("ERROR");
      delay(1500);
      lcd.clear();
    }
  i += 1;
  l = "Seccion";
  }
}
