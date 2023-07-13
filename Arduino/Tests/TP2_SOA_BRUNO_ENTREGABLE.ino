#include <Servo.h>
#include <Wire.h>
#include <rgb_lcd.h>
#include <SoftwareSerial.h>

//---------------------------
// DISPLAY
//---------------------------

#define DISPLAY_ADDRESS 32
#define DISPLAY_MAX_WIDTH 16
#define DISPLAY_MAX_HEIGHT 2
#define DISPLAY_WIDTH_0 0
#define DISPLAY_WIDTH_1 1
#define DISPLAY_WIDTH_2 2
#define DISPLAY_WIDTH_3 3
#define DISPLAY_WIDTH_4 4
#define DISPLAY_HEIGHT_0 0
#define DISPLAY_HEIGHT_1 1
rgb_lcd LCD;
void display_init();
void display_print(int event);


//---------------------------
// STATES
//---------------------------

#define INIT 0
#define CLEAN 2000
#define SLIGHTLY_DIRTY 2001
#define MEDIUMLY_DIRTY 2002
#define HIGHLY_DIRTY 2003
#define CLEANING  2004
#define CAT_INSIDE 2005
#define CAT_OUTSIDE 3
#define CONTINUE_STATE 8 


//---------------------------
// RGB HEADERS
//---------------------------

#define GREEN 1
#define YELLOW 2
#define ORANGE 3
#define RED 4
#define BLUE 5
#define RGB_255 255
#define RGB_0 0
#define RGB_100 100
#define PIN_LED_GREEN 5
#define PIN_LED_BLUE 6
#define PIN_LED_RED 3
void rgb_init();


//---------------------------
// EVENTS
//---------------------------

#define ENTRANCE_DETECTED   0
#define EXIT_DETECTED       1
#define NO_DIRTINESS        2
#define LOW_DIRTINESS       3
#define MID_DIRTINESS       4
#define HIGH_DIRTINESS      5
#define BUTTON_1_ACTIVATED  6
#define BUTTON_2_ACTIVATED  7
#define CONTINUE            8
#define GET_STATE           9
#define INITIAL             10


//---------------------------
// SERVO HEADER
//---------------------------

#define SERVO_PIN 9
#define SERVO_OPEN 0
#define SERVO_CLOSE 90
Servo Servomotor;
void servo_init();

//---------------------------
// MOISTURE HEADER
//---------------------------

#define MOISTURE_ERROR 50
#define POOP 3
#define LOW_DIRTINESS_MIN_VAL 1
#define MID_DIRTINESS_MIN_VAL 4
#define HIGH_DIRTINESS_MIN_VAL 6
#define INITIAL_MOISTURE 1017.0
#define LOW_MOISTURE_MIN_VAL 520
#define MID_MOISTURE_MIN_VAL 430
#define HIGH_MOISTURE_MIN_VAL 350
#define INITIAL_DIRTINESS 0
#define MOISTURE_MSG_SIZE 50
char moisture_msg[MOISTURE_MSG_SIZE];
const int moisture_sensor = A0;
float prev_moisture = 0;
float dirtiness_level = 0;
bool verify_moisture();
int moisture_get_points(float moisture);

//---------------------------
// DISTANCE SENSOR HEADERS
//---------------------------

#define DISTANCE_INSIDE  0
#define DISTANCE_OUTSIDE 1
#define DISTANCE_SENSOR 12
#define MIN_DISTANCE 20
#define SPEED_OF_SOUND 0.01723
#define PULSE_WIDTH_2 2
#define PULSE_WIDTH_10 10
int distance_state;
int dist;
long time_to_object;
void distance_sensor_init();
bool verify_distance();
long distance_read(int trigger_pin, int echo_pin);

//---------------------------
// BUTTON HEADER
//---------------------------

#define PRESSED_ONCE    0
#define PRESSED_TWICE   1
#define NOT_PRESSED     2
#define PIN_BUTTON 4
int button_state;
int prev_button_state = LOW;
int button_current_state = NOT_PRESSED;
void button_init();
bool verify_button();


//---------------------------
// BLUETOOTH HEADER
//---------------------------
#define PIN_BLUETOOTH_RX 10 // Arduino RX
#define PIN_BLUETOOTH_TX 11 // Arduino TX
#define BLUETOOTH_BPS 9600
#define BLUETOOTH_MSG_STOP 'S'
#define BLUETOOTH_MSG_CLEAN 'C'
#define BLUETOOTH_MSG_GET_STATE 's'
#define BLUETOOTH_MSG_GET_HUMIDITY 'h'
#define BLUETOOTH_MSG_SIZE 50
char bt_send_msg[BLUETOOTH_MSG_SIZE];
char bt_receive_msg;
void bluetooth_init();
bool verify_bluetooth();
void bluetooth_send_state();
SoftwareSerial Bluetooth(PIN_BLUETOOTH_RX, PIN_BLUETOOTH_TX);

//----------------------------
// TRANSITION FUNCTIONS HEADER
//----------------------------

void set_init();
void entrance_detected();
void exit_detected();
void button_1_activated();
void button_2_activated();
void no_dirtiness();
void low_dirtiness();
void mid_dirtiness();
void high_dirtiness();

//---------------------------
// Global Variables
//---------------------------

int state;
int event;
bool exited = false;

void setup()
{
  display_init();
  distance_sensor_init();
  rgb_init();
  button_init();
  servo_init();
  bluetooth_init();

  state = INIT;
}

void loop()
{
  get_event();
  state_machine();
}

void state_machine() { 
  switch(state)
  {
    case INIT:
      set_init();
    break;
    case CLEAN:
      switch(event)
      {
        case ENTRANCE_DETECTED:
          entrance_detected();
        break;
        case BUTTON_1_ACTIVATED:
          button_1_activated();
        break;
        case CONTINUE:
        break;
        case GET_STATE:
            bluetooth_send_state();
            break;
        default:
        break;
      }
    break;

    case CAT_INSIDE:
      switch(event)
      {
        case EXIT_DETECTED:
          exit_detected();
        break;
        case GET_STATE:
            bluetooth_send_state();
            break;
        default:
        break;
      }
    break;

    case CAT_OUTSIDE:
      switch(event)
      {
        case NO_DIRTINESS:
          no_dirtiness();
        break;
        case LOW_DIRTINESS:
           low_dirtiness();
        break;
        case MID_DIRTINESS:
          mid_dirtiness();
        break;
        case HIGH_DIRTINESS:
          high_dirtiness();
        break;
        case GET_STATE:
            bluetooth_send_state();
            break;
        default:
        break;
      }
    break;

    case SLIGHTLY_DIRTY: 
      switch(event)
      {
        case ENTRANCE_DETECTED:
          entrance_detected();
        break;
        case BUTTON_1_ACTIVATED:
          button_1_activated();
        break;
        case GET_STATE:
            bluetooth_send_state();
            break;
        case CONTINUE:
        break;
        default:
        break;
      }    
    break;

    case MEDIUMLY_DIRTY:
      switch(event)
      {
        case ENTRANCE_DETECTED:
          entrance_detected();
        break;
        case BUTTON_1_ACTIVATED:
          button_1_activated();
        break;
        case GET_STATE:
            bluetooth_send_state();
            break;
        case CONTINUE:
        break;
        default:
        break;
      }    
    break;

    case HIGHLY_DIRTY:
      switch(event)
      {
        case BUTTON_1_ACTIVATED:
          button_1_activated();
        break;
        case GET_STATE:
            bluetooth_send_state();
            break;
        case CONTINUE:
        break;
        default:
        break;
      }    
    break;

    case CLEANING:
      switch(event)
      {
        case BUTTON_2_ACTIVATED:
          button_2_activated();
        break;
        case GET_STATE:
            bluetooth_send_state();
            break;
        case CONTINUE:
        break;
        default:
        break;
      }
    break;

    default:
    break;
  }
}

void get_event()
{
  if(verify_distance() == true || verify_button() == true || verify_bluetooth() == true  || verify_moisture() == true)
    return;
  event = CONTINUE;
}

void change_state(int new_state)
{
  state = new_state;
  if(state != CAT_OUTSIDE)
  {
    bluetooth_send_state();
  }
}

bool verify_bluetooth()
{
  if(Bluetooth.available())
  {
    bt_receive_msg = Bluetooth.read();
    if(bt_receive_msg == BLUETOOTH_MSG_STOP)
    {
        event = BUTTON_1_ACTIVATED;
        return true;
    }
    if(bt_receive_msg == BLUETOOTH_MSG_CLEAN)
    {
        event = BUTTON_2_ACTIVATED;
        return true;
    }

    if(bt_receive_msg == BLUETOOTH_MSG_GET_STATE)
    {
        event = GET_STATE;
        return true;
    }


    return false;
  }

  return false;
}

bool verify_distance()
{
  time_to_object = distance_read(DISTANCE_SENSOR, DISTANCE_SENSOR);
  dist = time_to_object * SPEED_OF_SOUND;

  switch(distance_state)
  {
    case DISTANCE_OUTSIDE:
      if(dist < MIN_DISTANCE)
      {
        distance_state = DISTANCE_INSIDE;
        event = ENTRANCE_DETECTED;
        return true;
      }
      return false;
    break;

    case DISTANCE_INSIDE:
      if(dist >= MIN_DISTANCE)
      {
        distance_state = DISTANCE_OUTSIDE;
        event = EXIT_DETECTED;
        return true;
      }
      return false;
    break;
  }
}

bool verify_button()
{
  button_state = digitalRead(PIN_BUTTON);
  if (button_state != prev_button_state) 
  {
    if (button_state == HIGH)
    {
      switch (button_current_state) 
      {
        case PRESSED_ONCE:
          button_current_state = PRESSED_TWICE;
          event = BUTTON_2_ACTIVATED;
          prev_button_state = button_state;
          return true;
        break;

        case PRESSED_TWICE:
          button_current_state = PRESSED_ONCE;
          event = BUTTON_1_ACTIVATED;
          prev_button_state = button_state;
          return true;
        break;
        
        case NOT_PRESSED:
          button_current_state = PRESSED_ONCE;
          event = BUTTON_1_ACTIVATED;
          prev_button_state = button_state;
          return true;
        break;
      }
    }
  }

  prev_button_state = button_state;
  return false;
}

bool verify_moisture()
{ 
  float moisture = analogRead(moisture_sensor);
  float moisture_val_out_of_ten;

  if(exited == true)
  {
    if(moisture < prev_moisture - MOISTURE_ERROR)
    {
      moisture_val_out_of_ten = moisture_get_points(moisture);
      dirtiness_level += moisture_val_out_of_ten;
      prev_moisture = moisture;
    }
    else
    {
      dirtiness_level += POOP;
    }

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

  return false;
}

void changeLED(int color)
{
  switch(color){
    case GREEN:
      analogWrite(PIN_LED_GREEN, RGB_255);
      analogWrite(PIN_LED_RED, RGB_0);
      analogWrite(PIN_LED_BLUE, RGB_0);
    break;
    case YELLOW:
      analogWrite(PIN_LED_GREEN, RGB_255);
      analogWrite(PIN_LED_RED, RGB_255);
      analogWrite(PIN_LED_BLUE, RGB_0);
    break;
    case ORANGE:
      analogWrite(PIN_LED_GREEN, RGB_100);
      analogWrite(PIN_LED_RED, RGB_255); 
      analogWrite(PIN_LED_BLUE, RGB_0);
    break;
    case RED:
      analogWrite(PIN_LED_GREEN, RGB_0);
      analogWrite(PIN_LED_RED, RGB_255);
      analogWrite(PIN_LED_BLUE, RGB_0);
    break;
    case BLUE:
      analogWrite(PIN_LED_GREEN, RGB_0);
      analogWrite(PIN_LED_RED, RGB_0);
      analogWrite(PIN_LED_BLUE, RGB_255);
    break;
    default:
    break;
  }
}



//---------------------------
// SERVO IMPLEMENTATION
//---------------------------

void servo_init()
{
  Servomotor.attach(SERVO_PIN);
  Servomotor.write(SERVO_OPEN);
}

//---------------------------
// DISPLAY IMPLEMENTATION
//---------------------------

void display_init()
{
  LCD.begin(DISPLAY_MAX_WIDTH, DISPLAY_MAX_HEIGHT);
}

void print_in_display(int event)
{
  switch(event)
  {
    case INITIAL:
      LCD.clear();
      display_print(DISPLAY_WIDTH_0, DISPLAY_HEIGHT_0, "INICIANDO...");
      LCD.clear();
      display_print(DISPLAY_WIDTH_1, DISPLAY_HEIGHT_0, "ESTADO: LIMPIO");
      break;
    case ENTRANCE_DETECTED:
      LCD.clear();
      display_print(DISPLAY_WIDTH_2, DISPLAY_HEIGHT_0, "GATO DENTRO");
      display_print(DISPLAY_WIDTH_2, DISPLAY_HEIGHT_1, "NO MOLESTAR");
      break;
    case BUTTON_1_ACTIVATED:
      LCD.clear();
      display_print(DISPLAY_WIDTH_4, DISPLAY_HEIGHT_0, "LIMPIANDO");
      display_print(DISPLAY_WIDTH_2, DISPLAY_HEIGHT_1, "SENSORES: OFF");
      break;
    case BUTTON_2_ACTIVATED:
      LCD.clear();
      display_print(DISPLAY_WIDTH_1, DISPLAY_HEIGHT_0, "ESTADO: LIMPIO");
      break;
    case EXIT_DETECTED:
       LCD.clear();
       display_print(DISPLAY_WIDTH_0, DISPLAY_HEIGHT_0, "CALCULANDO .....");
       display_print(DISPLAY_WIDTH_0, DISPLAY_HEIGHT_1, "Espere Por Favor");
       break;
    case NO_DIRTINESS:
          LCD.clear();
          display_print(DISPLAY_WIDTH_1, DISPLAY_HEIGHT_0, "ESTADO: LIMPIO");
        break;
        case LOW_DIRTINESS:
          LCD.clear();
          display_print(DISPLAY_WIDTH_2, DISPLAY_HEIGHT_0, "ESTADO: LEVE");
        break;
        case MID_DIRTINESS:
          LCD.clear();
          display_print(DISPLAY_WIDTH_1, DISPLAY_HEIGHT_0, "ESTADO: MEDIO");
        break;
        case HIGH_DIRTINESS:
          LCD.clear();
          display_print(DISPLAY_WIDTH_0, DISPLAY_HEIGHT_0, "ESTADO: CRITICO");
  }

}
void display_print(int width, int height, String msg)
{
  LCD.setCursor(width, height);
  LCD.print(msg);
}


//---------------------------
// MOISTURE SENSOR IMPLEMENTATION
//---------------------------

int moisture_get_points(float moisture)
{
    if(moisture <= HIGH_MOISTURE_MIN_VAL)
    return HIGH_DIRTINESS_MIN_VAL;
  else if (moisture <= MID_MOISTURE_MIN_VAL)
    return MID_DIRTINESS_MIN_VAL;
  else if (moisture <= LOW_MOISTURE_MIN_VAL)
    return LOW_DIRTINESS_MIN_VAL;
  else
    return 0;
}

//---------------------------
// DISTANCE SENSOR IMPLEMENTATION
//---------------------------

void distance_sensor_init()
{
  distance_state = DISTANCE_OUTSIDE;
}

long distance_read(int trigger_pin, int echo_pin)
{
  pinMode(trigger_pin, OUTPUT);

  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(PULSE_WIDTH_2);

  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(PULSE_WIDTH_10);
  
  digitalWrite(trigger_pin, LOW);
  pinMode(echo_pin, INPUT);

  return pulseIn(echo_pin, HIGH);
}


//---------------------------
// RGB IMPLEMENTATION
//---------------------------

void rgb_init()
{
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_BLUE, OUTPUT);

  changeLED(GREEN);
}

//---------------------------
// BUTTON IMPLEMENTATION
//---------------------------

void button_init()
{
  pinMode(PIN_BUTTON, INPUT);
}

//---------------------------
// BLUETOOTH IMPLEMENTATION
//---------------------------
void bluetooth_init()
{
    Bluetooth.begin(BLUETOOTH_BPS);
}


void bluetooth_send_state()
{
  snprintf(bt_send_msg, BLUETOOTH_MSG_SIZE, "%d", state);
  Bluetooth.write(bt_send_msg);
}

//---------------------------
// TRANSITION FUNCTIONS
//---------------------------
void set_init()
{
  state = CLEAN;
  print_in_display(INIT);
}
void entrance_detected()
{
  change_state(CAT_INSIDE);
  print_in_display(ENTRANCE_DETECTED);
}
void button_1_activated()
{
  print_in_display(BUTTON_1_ACTIVATED);
  changeLED(BLUE);
  change_state(CLEANING);
  Servomotor.write(SERVO_CLOSE);
}
void exit_detected()
{
  print_in_display(EXIT_DETECTED);
  exited = true;            
  change_state(CAT_OUTSIDE);
}
void no_dirtiness()
{
  print_in_display(NO_DIRTINESS);
  change_state(CLEAN);
}
void low_dirtiness()
{
  print_in_display(LOW_DIRTINESS);
  change_state(SLIGHTLY_DIRTY);
  changeLED(YELLOW);
}
void mid_dirtiness()
{
  print_in_display(MID_DIRTINESS);
  change_state(MEDIUMLY_DIRTY);
  changeLED(ORANGE);
}
void high_dirtiness()
{
  print_in_display(HIGH_DIRTINESS);
  change_state(HIGHLY_DIRTY);
  changeLED(RED);
  Servomotor.write(SERVO_CLOSE);
}
void button_2_activated()
 {
   print_in_display
   (BUTTON_2_ACTIVATED);
   change_state(CLEAN);
   prev_moisture = INITIAL_MOISTURE;
   dirtiness_level = INITIAL_DIRTINESS;
   changeLED(GREEN);
   Servomotor.write(SERVO_OPEN);
}
