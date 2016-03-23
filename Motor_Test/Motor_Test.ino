//Pins des Motor-Poti
const int DIRECTION_A = 10;  //B-IB
const int DIRECTION_B = 11;  //B-IA

/*
 * Geschwindigkeit des Poti. 255 ist Maximum, Werte unter 200 gingen bei mir gar nicht.
 * Aber auch bei 200 hakt das Poti schon.
 */
const int SPEED = 200;

/* ******************************************************************************************************************* */

unsigned long time = millis();
const unsigned long PERIOD = 5000; //alle 5000ms soll das Poti fahren.

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


