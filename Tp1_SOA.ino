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


//typedef enum {LIMPIO, ENTRANDO_GATO, LEVEMENTE_SUCIA, MEDIANAMENTE_SUCIA, ALTAMENTE_SUCIA, VACIANDO, LLENANDO, CONTINUE /*recomendado este por Esteban*/} Event;

//estados propios del pulsador:
#define PRESSED_ONCE    0
#define PRESSED_TWICE   1
#define NOT_PRESSED     2 //capaz no hace falta

//

//colores:
#define GREEN 1
#define YELLOW 2
#define ORANGE 3
#define RED 4
#define BLUE 5


//eventos:
#define ENTRANCE_DETECTED   1
#define EXIT_DETECTED       2
#define NO_DIRTINESS        3
#define LOW_DIRTINESS       4
#define MID_DIRTINESS       5
#define HIGH_DIRTINESS      6
#define BUTTON_1_ACTIVATED  7
#define BUTTON_2_ACTIVATED  8
#define CONTINUE            10 //para que quede en los estados pasivos (los de suciedad, limpio o vaciando).
//

//pines:
const int distance_sensor = 12;
const int servo = 9;

const int ledG = 5;
const int ledB = 6;
const int ledR = 7;
const int button = 4;

//const int servo2 = 10; //YA NO HAY SERVO 2

const int humidity_sensor = A0;

const int displaySDA = A4;
const int displaySCL = A5;

//


//inicializaciones:
int state;
int event;


bool dentro = false; //capaz no está bien, ya que también es un estado, preguntar o revisar si no son estados internos del sensor de presencia..



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
}

void loop()
{
  /*event = */
  get_event();
  state_machine();
}

void state_machine() { //la lógica de lo que hace cada estado (cambiar el display, led, servo, etc.) puede estar acá adentro o en funciones que se llaman acá o en get_event().
  switch(state)
  {
    case INIT:
      state = LIMPIO;
      changeLED(GREEN);
      //inicializa el SERVO.
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
          //cambia el LED, el DISPLAY, mueve el SERVO.
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
          changeLED(GREEN);
          //reinicia o reposiciona el SERVO.
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
  if(verify_distance() == true || verify_button() == true || verify_humidity() == true)
    return;
  event = CONTINUE; //nunca entraría acá por como funciona verify_humidity(), ver si se puede sacar o lo necesitamos.
}

bool verify_distance()
{
  //lógica del lector de distancia. Retorna true si detecta que el gato está adentro y debería retornar true si detecta que el gato salió (no se puede medir contra el máximo)
  //ya que retornaría true siempre. Hay que usar algún timer o algo para detectar que salió (un booleano creo que queda perfecto).

  float min = 3; //distancia de la puerta de la caja.

  float dist = digitalRead(distanciometro) / 63; //ver como era que funcionaba

  if(dist < min) //&& dentro tambien?
  {
    event = ENTRANCE_DETECTED;
    dentro = true;
    return true;
  }

  if(dist >= min && dentro)
  {
    event = EXIT_DETECTED;
    dentro = false; //para indicar que salió, nos ahorramos el problema de detectar la distancia maxima constantemente y que eso retorne un evento EXIT_DETECTED. retorna solo si ya estaba dentro.
    return true;
  }

  return false;
}

bool verify_button() //podria inicializarse el boton en PRESSED_TWICE.
{
  get_button_state(); //capaz no hace falta y se puede leer de acá.
  
  switch(button_state)
  {
    case PRESSED_ONCE:
      event = BUTTON_1_ACTIVATED;
      return true;
    break;

    case PRESSED_TWICE:
      event = BUTTON_2_ACTIVATED;
      return true;
    break;

    default:
      return false; //NOT_PRESSED. CAPAZ NO VA, PODRÍA SER LO QUE DEVUELVE GET_BUTTON_STATE(), REVISARLO.
    break;
  }

  return false;
}

bool verify_humidity() //también tendría que verificar la cantidad de veces que entró y salió (como un contador asumiendo que caga siempre para facilitarnos la vida)
{
  int MIN_HUMIDITY = 1000;
  int MID_HUMIDITY = 600;
  int MAX_HUMIDITY = 200;
  int CLEAN = 9999;

  float humedad = analogRead(sensorHumedad);

//capaz al pedo este if.
  if(humedad == CLEAN)
  {
    event = CONTINUE;
    return true;
  }

//valores de ejemplo, creo que cuanto menor, mas humedo.


  if(humedad >= MIN_HUMIDITY && humedad < MID_HUMIDITY)
  {
    event = LOW_DIRTINESS;
    return true;
  }

  if(humedad >= MID_HUMIDITY && humedad < MAX_HUMIDITY)
  {
    event = MID_DIRTINESS;
    return true;
  }

  event = HIGH_DIRTINESS;
  return true;
}

void changeLED(int color){
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
      digitalWrite(ledG, 69);
      digitalWrite(ledR, HIGH); //magic number, hay que cambiarlo (al pedo).
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
