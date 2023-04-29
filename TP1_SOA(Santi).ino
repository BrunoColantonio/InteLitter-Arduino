#include <Servo.h>

//estados:
#define INIT                0
#define LIMPIO              1
#define ENTRANDO_GATO       2
#define GATO_AFUERA         3
#define LEVEMENTE_SUCIA     4
#define MEDIANAMENTE_SUCIA  5
#define ALTAMENTE_SUCIA     6 //inactivo.
#define VACIANDO            7
#define CONTINUE_STATE      8 //DUDOSO, puede salir


String states[] = {"INIT", "LIMPIO", "ENTRANDO_GATO", "GATO_AFUERA", "LEVEMENTE_SUCIA", "MEDIANAMENTE_SUCIA", "ALTAMENTE_SUCIA", "VACIANDO", "CONTINUE_STATE"};

//typedef enum {LIMPIO, ENTRANDO_GATO, LEVEMENTE_SUCIA, MEDIANAMENTE_SUCIA, ALTAMENTE_SUCIA, VACIANDO, LLENANDO, CONTINUE /*recomendado este por Esteban*/} Event;

//estados propios del pulsador:
#define PRESSED_ONCE    0
#define PRESSED_TWICE   1
#define NOT_PRESSED     2 //capaz no hace falta

//

//estados propios del distanciometro:
#define DISTANCE_INSIDE  0
#define DISTANCE_OUTSIDE 1

//colores:
#define GREEN 1
#define YELLOW 2
#define ORANGE 3
#define RED 4
#define BLUE 5


//eventos:
#define ENTRANCE_DETECTED   0
#define EXIT_DETECTED       1
#define NO_DIRTINESS        2
#define LOW_DIRTINESS       3
#define MID_DIRTINESS       4
#define HIGH_DIRTINESS      5
#define BUTTON_1_ACTIVATED  6
#define BUTTON_2_ACTIVATED  7
#define CONTINUE            8 //para que quede en los estados pasivos (los de suciedad, limpio o vaciando).
//

String events[] = {"ENTRANCE_DETECTED", "EXIT_DETECTED", "NO_DIRTINESS", "LOW_DIRTINESS",
                    "MID_DIRTINESS", "HIGH_DIRTINESS", "BUTTON_1_ACTIVATED", "BUTTON_2_ACTIVATED", "CONTINUE"};

//---------------------------
// SERVO HEADERS
//---------------------------
#define SERVO_PIN 9
#define SERVO_OPEN 0
#define SERVO_CLOSE 180
Servo Servomotor;
void servo_init();

//---------------------------
// MOISTURE HEADERS
//---------------------------
#define MAX_MOISTURE_VALUE      539.0
#define POOP                    2       // valores de ejemplo para que la prueba en tinker sea facil
#define LOW_DIRTINESS_MIN_VAL   1
#define MID_DIRTINESS_MIN_VAL   4
#define HIGH_DIRTINESS_MIN_VAL  6
float prev_moisture = 0.0; // guarda lectura de humedad previa
float dirtiness_level = 0; // detecta nivel de suciedad

//---------------------------
// DISTANCE SENSOR HEADERS
//---------------------------
#define DISTANCE_SENSOR 12

//---------------------------
// BUTTON HEADERS
//---------------------------

int buttonState;
int prevButtonState = LOW;
int buttonCurrentState = NOT_PRESSED;

//pines:
const int distance_sensor = 12;
const int servo = 9;
const int pin_button = 4;
const int ledG = 5;
const int ledB = 6;
const int ledR = 7;

//const int servo2 = 10; //YA NO HAY SERVO 2

const int moisture_sensor = A0;

const int displaySDA = A4;
const int displaySCL = A5;

//


//inicializaciones:
int state;
int event;


bool exited = false; //capaz no está bien, ya que también es un estado, preguntar o revisar si no son estados internos del sensor de presencia..
int distance_state;

int dist;
long time_to_object;
const int min_distance = 50; //sería la distancia a la que estaría la puerta desde el sensor de distancia.
const float speed_of_sound = 0.01723; //0,03446 / 2, por la fórmula distancia = velocidad * tiempo / 2, donde velocidad = 340m/seg = 0,034cm/seg

typedef struct //esto no sirve
{
    int rojo;
    int verde;
    int azul;
}ledRGB;
//



void setup()
{
  state = INIT;
  distance_state = DISTANCE_OUTSIDE;
  pinMode(pin_button, INPUT);
  Serial.begin(9600);
}

void loop()
{
  /*event = */
  get_event();
  state_machine();

  show_state_and_event();
  delay(1000);
}

void show_state_and_event()
{
  Serial.print("Ocurre el evento: ");
  Serial.print(events[event]);
  Serial.print(", Se encuentra en el estado: ");
  Serial.println(states[state]);
  
}

void state_machine() { //la lógica de lo que hace cada estado (cambiar el display, led, servo, etc.) puede estar acá adentro o en funciones que se llaman acá o en get_event().
  switch(state)
  {
    case INIT:
      state = LIMPIO;
      changeLED(GREEN);
      servo_init();
    break;
    case LIMPIO:
      switch(event)
      {
        case ENTRANCE_DETECTED:
          state = ENTRANDO_GATO;
          //no hace mucho más.
        break;
        case BUTTON_1_ACTIVATED:
          state = VACIANDO;
          //comienza a ignorar los sensores, puede cambiar el DISPLAY, trabar la puerta.
        break;
        case CONTINUE:
          //se queda en este estado.
        break;
        default:
        break;
      }
    break;

    case ENTRANDO_GATO:
      switch(event)
      {
        case EXIT_DETECTED:
        //calcular nivel de suciedad
          exited = true;            
          state = GATO_AFUERA;
          //no hace mucho más. Podría incrementarse un acumulador con la cantidad de veces que ingresó (sería más facil calcular la suciedad si asumimos que siempre caga).
        break;
        default:
        break;
      }
    break;

    case GATO_AFUERA:
      switch(event)
      {
        case NO_DIRTINESS:
          state = LIMPIO; //capaz que no va, CONSIDERAR.
          //no hace mucho.
        break;
        case LOW_DIRTINESS:
          state = LEVEMENTE_SUCIA;
          changeLED(YELLOW);
          //cambia el LED y el DISPLAY.
        break;
        case MID_DIRTINESS:
          state = MEDIANAMENTE_SUCIA;
          changeLED(ORANGE);
          //cambia el LED y el DISPLAY.
        break;
        case HIGH_DIRTINESS:
          state = ALTAMENTE_SUCIA;
          changeLED(RED);
          Servomotor.write(SERVO_CLOSE);
        break;
        default:
        break;
      }
    break;

    case LEVEMENTE_SUCIA: 
      switch(event)
      {
        case ENTRANCE_DETECTED:
          state = ENTRANDO_GATO;
          //no hace mucho más.
        break;
        case BUTTON_1_ACTIVATED:
          state = VACIANDO;
          //comienza a ignorar los sensores, puede cambiar el DISPLAY, mueve el SERVO.
        break;
        case CONTINUE: //podría un evento ser en vez de CONTINUE ser LOW_DIRTINESS y que se quede en este estado.
          //se queda en este estado.
        break;
        default:
        break;
      }    
    break;

    case MEDIANAMENTE_SUCIA:
      switch(event)
      {
        case ENTRANCE_DETECTED:
          state = ENTRANDO_GATO;
        break;
        case BUTTON_1_ACTIVATED:
          state = VACIANDO;
          //comienza a ignorar los sensores, puede cambiar el DISPLAY, mueve el SERVO.
        break;
        case CONTINUE: //podría un evento ser en vez de CONTINUE ser MID_DIRTINESS y que se quede en este estado.
          //se queda en este estado.
        break;
        default:
        break;
      }    
    break;

    case ALTAMENTE_SUCIA:
      switch(event)
      {
        case BUTTON_1_ACTIVATED:
          state = VACIANDO;
          //comienza a ignorar los sensores, puede cambiar el DISPLAY.
        break;
        case CONTINUE: //podría un evento ser en vez de CONTINUE ser HIGH_DIRTINESS y que se quede en este estado.
          //se queda en este estado.
        break;
        default:
        break;
      }    
    break;

    case VACIANDO:
      switch(event)
      {
        case BUTTON_2_ACTIVATED:
          state = LIMPIO;
          prev_moisture = 0;
          dirtiness_level = 0;
          changeLED(GREEN);
          Servomotor.write(SERVO_OPEN);
        break;
        case CONTINUE:
          //se queda en este estado.
        break;
        default:
        break;
      }
    break;

    case CONTINUE_STATE: //sería como que quede en el mismo estado (capaz no va).
    break;

    default:
    break;
  }
}

void get_event()
{
//NO SE PUEDE HACER UN SWITCH CASE.
//iría la lógica para obtener un evento según el estado y las cosas que sucedan (leer sensores, mover actuadores, etc.).
//PUEDE MANEJARSE CON UN CASE PARA LEER SEGÚN CADA ESTADO, IGUAL QUE state_machine().
//retorna el evento que corresponde según el estado actual y lo que ocurra en ese contexto.

//revisar la prioridad que queremos darle.
  if(verify_distance() == true || verify_button() == true || verify_moisture() == true)
    return;
  event = CONTINUE; //nunca entraría acá por como funciona verify_humidity(), ver si se puede sacar o lo necesitamos.
}

//FUNCIONA BIEN. PROBADO EN TINKERCAD.
bool verify_distance()
{
//lógica del lector de distancia. Retorna true si detecta que el gato está adentro y debería retornar true si detecta que el gato salió (no se puede medir contra el máximo)
//ya que retornaría true siempre. Hay que usar algún timer o algo para detectar que salió (un booleano creo que queda perfecto).

//con maquina de estados: ESTEBAN ME DIJO QUE ASÍ ESTÁ BIEN Y CON EL BOOLEANO NO ESTARÍA BIEN, PERO JUSTIFICAR POR QUÉ LO HICIMOS ASÍ EN EL INFORME Y EN EL COLOQUIO SABER DEFENDERLO.
  time_to_object = read_distance_sensor(distance_sensor, distance_sensor);
  dist = time_to_object * speed_of_sound;

  switch(distance_state)
  {
    case DISTANCE_OUTSIDE:
      if(dist < min_distance)
      {
        distance_state = DISTANCE_INSIDE;
        event = ENTRANCE_DETECTED;
        return true;
      }
      return false; //si estaba afuera y no entró entonces no retorna un estado que nos interese.
    break;

    case DISTANCE_INSIDE:
      if(dist >= min_distance)
      {
        distance_state = DISTANCE_OUTSIDE;
        event = EXIT_DETECTED;
        return true;
      }
      return false; //si estaba dentro y no salió entonces no retorna un estado que nos interese.
    break;
  }



//SIN MAQUINA DE ESTADOS:
/* 
  time_to_object = read_distance_sensor(distance_sensor, distance_sensor);
  dist = time_to_object * speed_of_sound;
  if(dist < min_distance) //&& dentro tambien?
  {
    event = ENTRANCE_DETECTED;
    dentro = true;
    return true;
  }
  if(dist >= min_distance && dentro) //esto no se puede?
  {
    event = EXIT_DETECTED;
    dentro = false; //para indicar que salió, nos ahorramos el problema de detectar la distancia maxima constantemente y que eso retorne un evento EXIT_DETECTED.
    return true; //retorna solo si ya estaba dentro.
  }
  return false;
*/
}

bool verify_button()  // podria inicializarse el boton en PRESSED_TWICE.
{
  buttonState = digitalRead(pin_button); // Lee en LOW (no lee en low).
  
  Serial.println("Lectura del boton:");
  Serial.println(buttonState);
  Serial.println("------------------");
  Serial.println("prevButtonState:");
  Serial.println(prevButtonState);
  Serial.println("------------------");
  
  if (buttonState != prevButtonState) 
  {
    if (buttonState == HIGH)
    {
      switch (buttonCurrentState) 
      {
        case PRESSED_ONCE:
          buttonCurrentState = PRESSED_TWICE;
          event = BUTTON_2_ACTIVATED;
          prevButtonState = buttonState;
          return true;
        break;

        case PRESSED_TWICE:
          buttonCurrentState = PRESSED_ONCE;
          event = BUTTON_1_ACTIVATED;
          prevButtonState = buttonState;
          return true;
        break;
        
        case NOT_PRESSED:
          buttonCurrentState = PRESSED_ONCE;
          event = BUTTON_1_ACTIVATED;
          prevButtonState = buttonState;
          return true;
        break;
      }
    }
  }

  prevButtonState = buttonState;
  return false;
}

bool verify_moisture()
{ 
  // leo la humedad
  float moisture = analogRead(moisture_sensor);
  // nivel de humedad de 0 a 10 puntos
  float moisture_val_out_of_ten;

  // si el evento es que salio el gato, comparo las humedades y actualizo el valor
  // sino por cada loop me actualizaría el valor de la suciedad (si no cambia la humedad asumiria que cago)
  // y estaria tomando como que el gato cagó cuando ni siquiera entró
  //if(event == EXIT_DETECTED)
  if(exited == true)
  {
    if(moisture > prev_moisture)
    {
      // calculo el porcentaje y lo divido por 10 para tener un puntaje y que sea mas facil pasarlo a "suciedad"
      moisture_val_out_of_ten = (((moisture/MAX_MOISTURE_VALUE)*100)/10.0);
      dirtiness_level += moisture_val_out_of_ten;
      // como se modifico la humedad, actualizo el valor previo
      prev_moisture = moisture;

    }
    // si no cambió la humedad y el gato entró y salió, asumo que cagó
    else
    {
      Serial.println("Cague");
      dirtiness_level += POOP;
    }
    
    // comparo los valores de suciedad para determinar en que estado está
    if(dirtiness_level < LOW_DIRTINESS_MIN_VAL)
          event = NO_DIRTINESS;
    else if (dirtiness_level < MID_DIRTINESS_MIN_VAL)
          event = LOW_DIRTINESS;
    else if (dirtiness_level < HIGH_DIRTINESS_MIN_VAL)
          event = MID_DIRTINESS;
    else
          event = HIGH_DIRTINESS;

    exited = false;
    return true;
  }
// si hago la cuenta solo cuando sale el gato, podria retornar false y "no haber leido nada".
//¿Que conviene? Evento "continue"? O que siempre lea por mas que no modifique ningun evento?

  return false;
}
void changeLED(int color)
{
  switch(color){
    case GREEN:
      digitalWrite(ledG, HIGH);
      digitalWrite(ledR, LOW);
      digitalWrite(ledB, LOW);
    break;
    case YELLOW:
      digitalWrite(ledG, HIGH);
      digitalWrite(ledR, HIGH);
      digitalWrite(ledB, LOW);
    break;
    case ORANGE:
      digitalWrite(ledG, 69); //magic number, hay que cambiarlo (al pedo).
      digitalWrite(ledR, HIGH); 
      digitalWrite(ledB, LOW);
    break;
    case RED:
      digitalWrite(ledG, LOW);
      digitalWrite(ledR, HIGH);
      digitalWrite(ledB, LOW);
    break;
    case BLUE:
      digitalWrite(ledG, LOW);
      digitalWrite(ledR, LOW);
      digitalWrite(ledB, HIGH);
    break;
    default:
    break;
  }
}

//retorna una duración de tiempo.
long read_distance_sensor(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);

  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  
   //Desactivo el trigger
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT); //se tiene que cambiar el pinMode por código ya que físicamente es el mismo PIN, si fueran 2 pines por separado no hace falta.
  
  //Leo el pin de Echo, y retorno el tiempo en
  //microsegundos en donde se encuentra el objeto
  //pulseIn: espera a que la señal en echoPin pase de LOW a HIGH y luego toma el tiempo que tarda de pasar de HIGH a LOW y retorna ese tiempo.
  //SEGUN LA PAGINA DE ARDUINO HACE ESO, REPASAR PORQUE NO LO ENTENDÍ
  return pulseIn(echoPin, HIGH);
}

//---------------------------
// SERVO IMPLEMENTATION
//---------------------------
void servo_init()
{
  Servomotor.attach(SERVO_PIN);
  Servomotor.write(SERVO_OPEN);
}
