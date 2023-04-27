//estados:
#define INIT                0
#define LIMPIO              1
#define ENTRANDO_GATO       2
#define GATO_AFUERA         3
#define LEVEMENTE_SUCIA     4
#define MEDIANAMENTE_SUCIA  5
#define ALTAMENTE_SUCIA     6 //inactivo.
#define VACIANDO            7
#define CONTINUE_STATE      8 //DUDOSO


//typedef enum {LIMPIO, ENTRANDO_GATO, LEVEMENTE_SUCIA, MEDIANAMENTE_SUCIA, ALTAMENTE_SUCIA, VACIANDO, LLENANDO, CONTINUE /*recomendado este por Esteban*/} Event;

//estados propios del pulsador:
#define PRESSED_ONCE    0
#define PRESSED_TWICE   1
#define NOT_PRESSED     2

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
const int sigPIR = 12;

const int ledG = 5;
const int ledB = 6;
const int ledR = 7;

const int servo1 = 9;
//const int servo2 = 10; //YA NO HAY SERVO 2

const int pulsadorDisable = 4;
const int pulsadorLlenar = 13;


/*
acá irían los pines del display:
*/

const int sensorHumedad = A0;
//


//inicializaciones:
int state;
int event;

typedef struct
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
          state = LIMPIO;
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
          //comienza a ignorar los sensores, puede cambiar el DISPLAY.
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
          //comienza a ignorar los sensores, puede cambiar el DISPLAY.
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
  event = CONTINUE;
}

bool verify_distance()
{
  //lógica del lector de distancia. Retorna true si detecta que el gato está adentro y debería retornar true si detecta que el gato salió (no se puede medir contra el máximo)
  //ya que retornaría true siempre. Hay que usar algún timer o algo para detectar que salió.
  
  float min = 3; //distancia de la puerta de la caja.
  float timer = 0; //ver cómo manejar esto.

  float dist = 3;
  if(dist < min)
  {
    event = ENTRANCE_DETECTED;
    return true;
  }

  if(dist >= min && timer)
  {
    event = EXIT_DETECTED;
    return false;
  }

  return false;
}

bool verify_button()
{
  get_button_state();
  
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

bool verify_humidity()
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
      digitalWrite(ledG, HIGH);
      digitalWrite(ledR, 69);
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



/*EJEMPLO: 
LiquidCrystal lcd(8,9,10,11,12,13);
byte colPins[4] = {3, 2, 10, 11};
byte rowPins[4] = {7, 6, 5, 4};
Keypad myKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
typedef enum {INITIAL, WAITING, LOADING, SUCCESS, ERROR, NONE, SECOND_FACTOR} generalStatus;
generalStatus STATUS = INITIAL;

typedef enum {NO_EVENT,KEY_PRESS, CHANGE_TEMP, ACCEPT, START, OPENED, PROCCESSING, RESET, WARNING} Event;
Event EVENT = NO_EVENT;
*/

/* void setup()
{
	Serial.begin(9600);
  	pinMode(ledLoading, OUTPUT);
  	pinMode(ledSuccess, OUTPUT);
  	pinMode(ledError, OUTPUT);
  	pinMode(buzzer, OUTPUT);
  	lcd.begin(16,2);
}

void loop()
{
  if(ldrValue != previousLdrValue){
    if (STATUS == SECOND_FACTOR)
    	EVENT = CHANGE_TEMP;
    ldrValue= analogRead(pinLDR);
  }
  key = myKeypad.getKey();
  if(key){
    switch(STATUS){
     
      case INITIAL:
        if (key == keyStart)
        	EVENT = START;
        else if (key == keyReset)
      		EVENT = RESET;
        else
        	EVENT = KEY_PRESS;
    	break;
    case WAITING:
      if (key == keyAccept)
        	EVENT = ACCEPT;
        else if (key == keyReset)
      		EVENT = RESET;
        else
        	EVENT = KEY_PRESS;
    	break;
       default:
    	if (key == keyReset)
      		EVENT = RESET;
    	break;
    }
  }
  executeSystem();
}

void executeSystem(){
  switch(STATUS){
   	case INITIAL:
        switch(EVENT){
          case RESET:
    		resetSystem();
    		break;
          case NO_EVENT:
          	initializeSystem();
          	break;
          case START:
          	verifyStart();
          	break;
        }
    break;
    case WAITING:
    	switch(EVENT){
          case RESET:
    		resetSystem();
    		break;
          case NO_EVENT:
          	break;
          case ACCEPT:
          	acceptPassword();
          	break;
          case KEY_PRESS:
          	writePassword();
          	break;
        }
    break;
    case LOADING:
    	switch(EVENT){
          case RESET:
    		resetSystem();
    		break;
          case PROCCESSING:
          	loadingProccess();
          	break;
        }
		break;
    case SECOND_FACTOR:
    	switch(EVENT){
          case RESET:
    		resetSystem();
    		break;
          case CHANGE_TEMP:
          	verifySecondFactorSecurity();
          	break;
        }
    	break;
    case SUCCESS:
    	switch(EVENT){
          case RESET:
    		resetSystem();
    		break;
          case OPENED:
          	openBox();
          	break;
        }
		break;
    case ERROR:
    switch(EVENT){
          case RESET:
    		resetSystem();
    		break;
          case WARNING:
          	errorPassword();
          	break;
        }
		break;
  }  
}

void errorPassword(){
  changeStatusLed(ledError,TURN_ON); 
  prepareToTone();
  writeLcd("LA CLAVE","ES INCORRECTA");
}

void resetSystem(){
 lcd.clear();
 STATUS = INITIAL;
 EVENT = NO_EVENT;
}

void openBox(){
  changeStatusLed(ledSuccess,TURN_ON); 
  writeLcd("LA CLAVE","ES CORRECTA");
    
}

void verifySecondFactorSecurity(){
  writeLcd("ESPERANDO","HUELLA DIGITAL");
  if(ldrValue > fingerPrintTem){
    lcd.clear();
    STATUS=SUCCESS;
    EVENT=OPENED;
  } 
}

void loadingProccess(){
  changeStatusLed(ledLoading,TURN_ON); 
  writeLcd("PROCESANDO","");
  currentMillis = millis();
  timeToProccess();       
}

void writePassword(){
  lcd.clear();
  password = password + key;
  passwordToShow = passwordToShow + '-';
  writeLcd("CLAVE:", passwordToShow);
  EVENT = NO_EVENT;
}
-
 */
