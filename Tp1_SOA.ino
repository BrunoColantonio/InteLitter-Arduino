//estados:
#define INIT                0
#define LIMPIO              1
#define ENTRANDO_GATO       2
#define GATO_AFUERA         3
#define LEVEMENTE_SUCIA     4
#define MEDIANAMENTE_SUCIA  5
#define ALTAMENTE_SUCIA     6 //inactivo.
#define VACIANDO            7
#define CONTINUE            8


//typedef enum {LIMPIO, ENTRANDO_GATO, LEVEMENTE_SUCIA, MEDIANAMENTE_SUCIA, ALTAMENTE_SUCIA, VACIANDO, LLENANDO, CONTINUE /*recomendado este por Esteban*/} Event;

//

//eventos:
#define ENTRANCE_DETECTED   1
#define EXIT_DETECTED       2
#define NO_DIRTINESS        3
#define LOW_DIRTINESS       4
#define MID_DIRTINESS       5
#define HIGH_DIRTINESS      6
#define BUTTON_1_ACTIVATED  7
#define BUTTON_2_ACTIVATED  8
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

typedef struct{
    int rojo;
    int verde;
    int azul;
}ledRGB;
//

void setup(){
  state = INIT;
}

void loop(){
  event = get_event();
  state_machine();
}

void state_machine() { //la lógica de lo que hace cada estado (cambiar el display, led, servo, etc.) puede estar acá adentro o en funciones que se llaman acá o en get_event().
  switch(state){
    case INIT:
      state = LIMPIO;
    break;
    case LIMPIO:
      switch(event){
        case ENTRANCE_DETECTED:
          state = ENTRANDO_GATO;
        break;
        default:
        break;
      }
    break;

    case ENTRANDO_GATO:
      switch(event){
        case EXIT_DETECTED:
          state = GATO_AFUERA;
        break;
        default:
        break;
      }
    break;

    case GATO_AFUERA:
      switch(event){
        case NO_DIRTINESS:
          state = LIMPIO;
        case LOW_DIRTINESS:
          state = LEVEMENTE_SUCIA;
        break;
        case MID_DIRTINESS:
          state = MEDIANAMENTE_SUCIA;
        break;
        case HIGH_DIRTINESS:
          state = ALTAMENTE_SUCIA;
        break;
        default:
        break;
      }
    break;

    case LEVEMENTE_SUCIA:
      switch(event){
        case ENTRANCE_DETECTED:
          state = ENTRANDO_GATO;
        case BUTTON_1_ACTIVATED:
          state = VACIANDO;
        break;
        default:
        break;
      }    
    break;

    case MEDIANAMENTE_SUCIA:
      switch(event){
        case ENTRANCE_DETECTED:
          state = ENTRANDO_GATO;
        case BUTTON_1_ACTIVATED:
          state = VACIANDO;
        break;
        default:
        break;
      }    
    break;

    case ALTAMENTE_SUCIA:
      switch(event){
        case BUTTON_1_ACTIVATED:
          state = VACIANDO;
        break;
        default:
        break;
      }    
    break;

    case VACIANDO:
      switch(event){
        case BUTTON_2_ACTIVATED:
          state = LIMPIO;
        break;
        default:
        break;
      }
    break;

    case CONTINUE: //sería como que quede en el mismo estado (capaz no va).
    break;

    default:
    break;
  }
}

int get_event(){
//iría la lógica para obtener un evento según el estado y las cosas que sucedan (leer sensores, mover actuadores, etc.).
//retorna el evento que corresponde según el estado actual y lo que ocurra en ese contexto.
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