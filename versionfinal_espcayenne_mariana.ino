/*  Proyecto 
 *  
 *  Caracas, julio del 2018
 *  
 *  Facultad de Ingenieria de la UCV
 *  Escuela de Ingenieria Electrica
 *  Semestre I-2018
 *  Laboratorio de Proyectos; Profesor Simon Morales
 *  
 *  ---Mariana C. Marquez Azuaje---
*/


#define PIN_ANALOGICO A0          // el pin del esp a la que conectamos la entrada LDR

// definiendo para la coomunicacion con Cayenne
#define CAYENNE_PRINT Serial
#include <CayenneMQTTESP8266.h>

#define CANAL_ESCRITURA 4
#define PIN_VALVULA D1 

// incluyendo la libreria del sensor sht15
#include <SHT1x.h>              
#define PIN_DATO  D5         //especificar pines de dato y sincronizacion del SHT15
#define PIN_CLOCK D6
SHT1x sht1x(PIN_DATO, PIN_CLOCK);       

//definiendo variables para la medicion de la luminosidad
unsigned int luminosidad;
float coeficiente_porcentaje=100.0/1023.0; // El valor de la entrada analógica va de 0 a 1023 y se quiere convertir a porcentaje que va de cero a 100
float luz;

//definiendo variables para la lectura de la temperatura y humedad del sensor SHT15
float temp_c;
float temp_f;
float humedad;

//definiendo variables para el monitoreo del nivel de agua del tanque
#define PIN_NIVEL D2
int edo_agua=0;

// WiFi network info.
char ssid[] = "ABACANTVWIFIA624";
char wifiPassword[] = "iraimamogollon";

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "acffbab0-5fda-11e8-a25e-d5e347246797";
char password[] = "a92aed9ee6d2ef0fe119272e639158a36391efa3";
char clientID[] = "81e956b0-6606-11e8-9b75-e32a21a125ca";

void setup() {
  
	Serial.begin(115200);
	Cayenne.begin(username, password, clientID, ssid, wifiPassword);  //se estable conexion con Cayenne
  pinMode(PIN_VALVULA, OUTPUT);     //definiendo el modo de los pines de interes
  pinMode(PIN_NIVEL,INPUT);
  
}

void loop() {
  
	Cayenne.loop();

    // lee temperatura y humedad del sensor
    temp_c = sht1x.readTemperatureC();
    temp_f = sht1x.readTemperatureF();
    humedad = sht1x.readHumidity();
    
    // lee luminosidad del pin A0 (LDR)
    luz=analogRead(PIN_ANALOGICO);                      
    luminosidad=luz*coeficiente_porcentaje;

    // muestra luminosidad en el puerto serial
    Serial.print("La luminosidad es del ");
    Serial.print(luminosidad);
    Serial.println("%");
    
    // muestra temperatura y humedad en el puerto serial
    Serial.print("Temperatura: ");
    Serial.print(temp_c, DEC);
    Serial.print("C / ");
    Serial.print(temp_f, DEC);
    Serial.print("F. Humedad: ");
    Serial.print(humedad);
    Serial.println("%");
    
    delay(500);        // retardo de 500ms para que tome los valores y los reproduzca en la pagina
    
    Cayenne.virtualWrite(0,luminosidad);          // en el canal 0 se muestra la luminosidad
    Cayenne.virtualWrite(1,temp_c);       // en el canal 1 se muestra la temperatura en celsius
    Cayenne.virtualWrite(2,temp_f);       // en el canal 2 se muestra la temperatura en farenheit
    Cayenne.virtualWrite(3,humedad);      // en el canal 3 se muestra la humedad

//muestra el estado del tanque en el puerto serial
edo_agua=digitalRead(PIN_NIVEL);
if(edo_agua==HIGH){
Serial.println("El tanque tiene agua");
}
else {
  Serial.println("Debes llenar el tanque");
}
delay(500);

}

    CAYENNE_IN(CANAL_ESCRITURA)
{
  int value = getValue.asInt();
  CAYENNE_LOG("Channel %d, pin %d, value %d", CANAL_ESCRITURA, PIN_VALVULA, value);
  // Write the value received to the digital pin.
  digitalWrite(PIN_VALVULA, value);
}
   
