#include <Wire.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <LCD.h>
// definisco il pin del sensore

#define DHTPIN 2                                                                                                                                                                                                                    
#define BUTTON1 3            // pin di input dove è collegato il pulsante  
#define BUTTONup  4          // pin di input dove è collegato il pulsante  
#define BUTTONdown 5  
#define ReleSerra 12
#define ReleVentilatore 11

#define DHTTYPE DHT11

// pin di input dove è collegato il pulsante  
boolean buttonUpPressed;
boolean buttonDownPressed; 

boolean buttonUpState; 
boolean buttonDownState;
int misurazionePioggia=0;
int pippo=0;
int a = 0;
boolean val1 =HIGH;                  // si userà val per conservare lo stato del pin di input  
boolean val2 = HIGH;          // si userà vecchio_val per conservare lo stato del pin di input al passo precedente  
boolean val3 = HIGH;
int stato =0;
const int statoMax = 45;                     //limite massimo valore della variabile
const int statoMin = 15; 
int valoreVentilatore = LOW;
//definisco il tipo di sensore

DHT dht(DHTPIN, DHTTYPE);
// definisco a quali pin è connesso il display
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
void(* software_reboot)(void) = 0;


int setTemperatura01 = 0;
int setTemperatura02 = 0;
int menuAttivo = 0;
int SogliaTemperaturaSerra = 0;
int SogliaTemperaturaVentilatore = 0;

void setup() {

    Serial.begin (9600);
    delay(2000);
    lcd.begin(16, 2);
    lcd.print("Temp.");
    lcd.setCursor(0,1);
    lcd.print("Umid.");
    lcd.setCursor(11,0);
    lcd.print("Serra");
    pinMode(ReleSerra, OUTPUT);
    pinMode(ReleVentilatore, OUTPUT);
    // EEPROM.write(5, stato);
    pinMode(BUTTON1, INPUT);     // imposta il pin digitale come input  -
    pinMode(BUTTONup, INPUT);     // imposta il pin digitale come input  -
    pinMode(BUTTONdown, INPUT);     // imposta il pin digitale come input  -
    digitalWrite(BUTTON1, HIGH); // Abilità res. pull-up
    digitalWrite(BUTTONup, HIGH); // Abilità res. pull-up
    digitalWrite(BUTTONdown, HIGH); // Abilità res. pull-up
    SogliaTemperaturaSerra = EEPROM.read(5);
    stato = SogliaTemperaturaSerra;

}


void menu(){
    // Recupero gli stati dei vari bottoni
    val1 = digitalRead(BUTTON1);
    val2 = digitalRead(BUTTONup);
    val3 = digitalRead(BUTTONdown);
    // Ritardo lettura
    delay(25);

    // Se è premuto il tasto Menu
    if(val1 == LOW){

      // Verifico se la temperatura serra è stata impostata
      if(setTemperatura01 == 0){
            // Imposto il display
            lcd.begin(16, 2); 
            lcd.print("Val.Temp.Serra   ");
            lcd.setCursor(0,1);
            // Leggo la temperatura in memoria
            SogliaTemperaturaSerra = EEPROM.read(5);
            lcd.setCursor(8,1);
            lcd.print(SogliaTemperaturaSerra);
            // Setto la lettura come effettuata
            setTemperatura01 = 1;
            // Avvio il ciclo per l'incremento/decremento del valore
            while(digitalRead(BUTTON1) == HIGH){
               
                val2 = digitalRead(BUTTONup);
                val3 = digitalRead(BUTTONdown);
                // Incremento
                if(val2 == LOW){
                    SogliaTemperaturaSerra+=1;
                    lcd.setCursor(8,1);
                    lcd.print(SogliaTemperaturaSerra);  
                }
                // Decremento
                if(val3 == LOW){
                    SogliaTemperaturaSerra-=1;
                    lcd.setCursor(8,1);
                    lcd.print(SogliaTemperaturaSerra);  
                }
                // Ritardo di lettura
                delay(300);
            }
      }
      // Verifico se la temperatura ventilatore è stata impostata
      if(setTemperatura02 == 0){
            // Imposto il display
            lcd.begin(16, 2); 
            lcd.print("Val.Temp.Ventilatore   ");
            lcd.setCursor(0,1);
            // Leggo la temperatura in memoria
            SogliaTemperaturaVentilatore = EEPROM.read(8);
            lcd.setCursor(8,1);
            lcd.print(SogliaTemperaturaVentilatore); 
            // Setto la lettura come effettuata
            setTemperatura02 = 1;
            // Avvio il ciclo per l'incremento/decremento del valore
            while(digitalRead(BUTTON1) == HIGH){
               
                val2 = digitalRead(BUTTONup);
                val3 = digitalRead(BUTTONdown);
                // Incremento
                if(val2 == LOW){
                    SogliaTemperaturaVentilatore+=1;
                    lcd.setCursor(8,1);
                    lcd.print(SogliaTemperaturaVentilatore);   
                }
                // Decremento
                if(val3 == LOW){
                    SogliaTemperaturaVentilatore-=1;
                    lcd.setCursor(8,1);
                    lcd.print(SogliaTemperaturaVentilatore);   
                }
                // Ritardo di lettura
                delay(300);
            }
            return;
      }

      // Scrivo i valori in memoria
      EEPROM.write(5, SogliaTemperaturaSerra);   
      EEPROM.write(8, SogliaTemperaturaVentilatore);   

      // Reinizializzo le variabili di lettura
      setTemperatura01=0;
      setTemperatura02=0;
      // Disattivo il menu
      menuAttivo = 0;
      // Imposto il display con un ritardo alto per evitare che il bottone Menu venga letto 
      // due volte e rientri nel loop
      delay(600);
      lcd.begin(16, 2); 
      lcd.print("Salvataggio...");
      delay(900);
      lcd.begin(16, 2); 
    }
   
}




void loop() {

    if(menuAttivo == 1){
        menu();       
    }else{

      SogliaTemperaturaSerra = EEPROM.read(5);
      SogliaTemperaturaVentilatore = EEPROM.read(8);
      lcd.setCursor(0,0);
      lcd.print("Temp.");
      lcd.setCursor(0,1);
      lcd.print("Vent."); 
      lcd.setCursor(11,0);
      lcd.print("Serra");  

      int t = dht.readTemperature();
      //int h = dht.readHumidity();
                                            // posiziono il cursore alla colonna 12 e riga 0
      lcd.setCursor(5, 0);
      lcd.print(t);
      lcd.setCursor(5, 1);
      lcd.print("Off");
      lcd.setCursor(11,0);
      lcd.print("Serra");
      lcd.setCursor(10, 1);
      lcd.print("Chiusa"); 
      //Serial.println(SogliaTemperaturaSerra);
     //Serial.println( SogliaTemperaturaVentilatore);
    // Serial.println(misurazionePioggia);
    // Serial.println(t);
    // Serial.println(h);
       //Serial.println(ReleSerra);

  
       misurazionePioggia = analogRead(A0);
      // Se il valore è minore di 400 stà piovendo quindi Chiudo tutto
      if(misurazionePioggia < 400){
          // Spengo la serra
          digitalWrite(ReleSerra, LOW);
          // Spengo anche il ventilatore
          digitalWrite(ReleVentilatore, LOW);
          lcd.setCursor(10, 1);
          lcd.print("Chiusa");
          lcd.setCursor(5, 1);
          lcd.print("Off");    
      }else{
  
          // Verifico la temperatura se minore del valore in eeprom chiudo la serra
          if (t <= (SogliaTemperaturaSerra-5))   
          {
            digitalWrite(ReleSerra, LOW);
                // Spengo anche il ventilatore
            digitalWrite(ReleVentilatore, LOW);
            lcd.setCursor(10, 1);
            lcd.print("Chiusa"); 
            lcd.setCursor(5, 1);
            lcd.print("Off");    
          }
  
          // Verifico la temperatura e maggiore del valore in eeprom apro la serra
          if (t >= SogliaTemperaturaSerra)  
          {
              digitalWrite(ReleSerra, HIGH);   
              lcd.setCursor(10, 1);
              lcd.print("Aperta");
          }
  
          // Recupero lo stato del rele della serra
          valoreVentilatore = digitalRead(ReleSerra);
  
         
  
          // Se lo stato dell rele della serra e ALTO e la temperatura e <= il valore dell'eeprom spengo il ventilatore
          if( t < (SogliaTemperaturaVentilatore-3) ){
              digitalWrite(ReleVentilatore, LOW);
             lcd.setCursor(5, 1);
             lcd.print("Off");    
          }
  
         // Se lo stato dell rele della serra e ALTO e la temperatura e 5 gradi sopra il valore dell'eeprom avvio il ventilatore
          if( t >= (SogliaTemperaturaVentilatore) ){
              digitalWrite(ReleVentilatore, HIGH);
             lcd.setCursor(5, 1);
             lcd.print(" On");    
          }
      }
    }

    val1 = digitalRead(BUTTON1);
    delay(30);
    if (val1 == LOW) { menuAttivo=1; }

    

}    





