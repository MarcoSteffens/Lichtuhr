//Pins des Motor-Poti
const int DIRECTION_A = 10;  //B-IB
const int DIRECTION_B = 11;  //B-IA

const int SPEED = 200;         //Geschwindigkeit des Poti. Werte unter 200 gehen mit Stromversorgung über USB gar nicht, da regt sich der Motor nicht.

/* ******************************************************************************************************************* */

unsigned long time = millis();
const unsigned long PERIOD = 5000; //alle 5000ms soll der poti fahren.

const int WAITING = 0;
const int RUNNING = 1;
int status = WAITING;

const int NONE = 0;
const int UP = 1;
const int DOWN = 2;
int direction = NONE;
/****************************************************************************
 * void setup()
 ****************************************************************************/
void setup()
{

  Serial.begin(9600);

  //Poti in Startposition fahren:
  analogWrite(DIRECTION_A, SPEED);
  analogWrite(DIRECTION_B, 0);
  delay(400);
  analogWrite(DIRECTION_A, 0);
  analogWrite(DIRECTION_B, 0);

  delay(2000);
}
 
/****************************************************************************
 * void loop()
 ****************************************************************************/

void loop()
{

      if (direction == NONE)
      {
        direction = UP;
        analogWrite(DIRECTION_A, 0);
        analogWrite(DIRECTION_B, SPEED);
        delay(2000);
      }
      else if (direction == UP)
      {
        direction = DOWN; 
        analogWrite(DIRECTION_A, SPEED);
        analogWrite(DIRECTION_B, 0);
        delay(2000);
      }
      else if (direction == DOWN)
      {
        direction = NONE;
        status = WAITING;
        analogWrite(DIRECTION_A, 0);
        analogWrite(DIRECTION_B, 0);
        delay(PERIOD);
      }
}


