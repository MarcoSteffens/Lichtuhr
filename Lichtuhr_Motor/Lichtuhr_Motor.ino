/*
 * Autor: Marco Steffens (marco.steffens@gmail.com)
 * Sketch: Lichtuhr_Motor
 * Datum: 23.03.2016
 * Version: 1.0
 * Lizenz: cc zero (http://creativecommons.org/publicdomain/zero/1.0/deed.de)
 */
 
// DS3231: 
// SDA pin -> Arduino Analog 4 or the dedicated SDA pin
// SCL pin -> Arduino Analog 5 or the dedicated SCL pin

#include <DS3231.h>
DS3231 rtc(SDA, SCL); // Init the DS3231 using the hardware interface

String strTime = "";
String strTimeTemp = "";
char arrCharTime[6] = { '8', '8', ':', '8', '8', '\0' }; // "88:88";

const int POTI_PIN= A0;   //Pin des Schiebepoti
int poti_wert = 0;

//Pin-Belegung der LEDs
const int LED_8 = 2; //Unterste LED
const int LED_7 = 3; //
const int LED_6 = 4; //
const int LED_5 = 5; //
const int LED_4 = 6; //
const int LED_3 = 7; //
const int LED_2 = 8; //
const int LED_1 = 9; //Oberste LED

//Pins des Motor-Poti
const int DIRECTION_A = 10;
const int DIRECTION_B = 11;

const int SPEED = 210;         //Geschwindigkeit des Poti. Werte unter 200 gehen mit Stromversorgung über USB gar nicht, da regt sich der Motor nicht.

unsigned long time = millis();
const unsigned long PERIOD = 5000; //alle 5000ms soll der poti fahren.

const int WAITING = 0;
const int RUNNING = 1;
int status = WAITING;

const int NONE = 0;
const int UP = 1;
const int DOWN = 2;
int direction = NONE;


/* Im zweidimensionalen Array "arrDisplay[][]" wird das abgelegt, was letztlich
 *  Angezeigt wird. Also "hh:mm", und außerdem die Leerräume bzw. das Trenn- 
 *  zeichen. 
 */
byte arrDisplay[8][31];

/* Eine Ziffer ist 7 Einheiten hoch und 5 breit. Damit lassen sich Zahlen 
 * vernünftig anzeigen. Repräsentiert wird so eine Zahl durch ein Array
 * mit 5x8 Feldern - 8, weil es 8 LED gibt und ich die Möglichkeit 
 * erhalten wollte, einfach auch ein anderes Zahlen-Design zu verwenden. Die 
 * unterste Reihe ist immer leer, wird also nie angeleuchtet. Bei der ersten
 * Zahl schreib ich die Reihen mal untereinander, dann ist das Prinzip 
 * erkennbar. 
*/
const int DIGIT_WIDTH = 5;      //Breite einer ZIFFER. Die Trennzeichen (" " und ":") haben je eine eigene Breite.
const int DIGIT_HEIGHT = 8;     //Höhe der anzuzeigenden Zeichen, entspricht der Anzahl der LED
const int DISPLAY_WIDTH = 31;   //Anzahl der "Pixel" im Display, waagerecht
int displayIndex = 0;           //Zähler für das große Array, 

const byte DIGIT_0[DIGIT_HEIGHT][DIGIT_WIDTH] = { { 1, 1, 1, 1, 1 }, 
                                                  { 1, 0, 0, 0, 1 }, 
                                                  { 1, 0, 0, 0, 1 }, 
                                                  { 1, 0, 0, 0, 1 }, 
                                                  { 1, 0, 0, 0, 1 }, 
                                                  { 1, 0, 0, 0, 1 }, 
                                                  { 1, 1, 1, 1, 1 }, 
                                                  { 0, 0, 0, 0, 0 } } ;
const byte DIGIT_1[DIGIT_HEIGHT][DIGIT_WIDTH] = { { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 0 } } ;
const byte DIGIT_2[DIGIT_HEIGHT][DIGIT_WIDTH] = { { 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 1, 1, 1, 1, 1 }, { 1, 0, 0, 0, 0 }, { 1, 0, 0, 0, 0 }, { 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 0 } } ;
const byte DIGIT_3[DIGIT_HEIGHT][DIGIT_WIDTH] = { { 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 0 } } ;
const byte DIGIT_4[DIGIT_HEIGHT][DIGIT_WIDTH] = { { 1, 0, 0, 0, 1 }, { 1, 0, 0, 0, 1 }, { 1, 0, 0, 0, 1 }, { 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 0 } } ;
const byte DIGIT_5[DIGIT_HEIGHT][DIGIT_WIDTH] = { { 1, 1, 1, 1, 1 }, { 1, 0, 0, 0, 0 }, { 1, 0, 0, 0, 0 }, { 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 0 } } ;
const byte DIGIT_6[DIGIT_HEIGHT][DIGIT_WIDTH] = { { 1, 0, 0, 0, 0 }, { 1, 0, 0, 0, 0 }, { 1, 0, 0, 0, 0 }, { 1, 1, 1, 1, 1 }, { 1, 0, 0, 0, 1 }, { 1, 0, 0, 0, 1 }, { 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 0 } } ;
const byte DIGIT_7[DIGIT_HEIGHT][DIGIT_WIDTH] = { { 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 0 } } ;
const byte DIGIT_8[DIGIT_HEIGHT][DIGIT_WIDTH] = { { 1, 1, 1, 1, 1 }, { 1, 0, 0, 0, 1 }, { 1, 0, 0, 0, 1 }, { 1, 1, 1, 1, 1 }, { 1, 0, 0, 0, 1 }, { 1, 0, 0, 0, 1 }, { 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 0 } } ;
const byte DIGIT_9[DIGIT_HEIGHT][DIGIT_WIDTH] = { { 1, 1, 1, 1, 1 }, { 1, 0, 0, 0, 1 }, { 1, 0, 0, 0, 1 }, { 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 0 } } ;
const byte DELIMITER[DIGIT_HEIGHT][5]    = { { 0, 0, 0, 0, 0}, { 0, 0, 0, 0, 0}, { 0, 0, 0, 0, 0}, { 0, 0, 1, 0, 0}, { 0, 0, 0, 0, 0}, { 0, 0, 1, 0, 0}, { 0, 0, 0, 0, 0}, { 0, 0, 0, 0, 0} };
const byte SPACE[DIGIT_HEIGHT][3]   = { { 0, 0, 0}, { 0, 0, 0}, { 0, 0, 0}, { 0, 0, 0}, { 0, 0, 0}, { 0, 0, 0}, { 0, 0, 0}, { 0, 0, 0} };

/* Offset ist der Bereich am Anfang und am Ende der Poti-Strecke, 
 * der nicht für die Anzeige genutzt wird. Dieser Bereich deckt einmal ab, 
 * dass der Poti nicht immer Werte zwischen 0 und 1023 lieferte, sondern
 * zwischen 9 und 1019 und ähnliches. Zum Anderen deckt dieser Wert aber
 * auch den Bereich ab, der durch den LED-Arm überdeckt wird. 
 */
const int OFFSET = 140;

/****************************************************************************
 * void setup()
 ****************************************************************************/
void setup()
{

  Serial.begin(9600);
  
  pinMode(LED_1,OUTPUT);
  pinMode(LED_2,OUTPUT);
  pinMode(LED_3,OUTPUT);
  pinMode(LED_4,OUTPUT);
  pinMode(LED_5,OUTPUT);
  pinMode(LED_6,OUTPUT);
  pinMode(LED_7,OUTPUT);
  pinMode(LED_8,OUTPUT);

  //Das Display-Array wird erstmal mit lauter Null-Werten initialisiert
  for (int i=0; i<8; ++i)
  {
    for (int j=0; j<DISPLAY_WIDTH; ++j)
    {
      arrDisplay[i][j] = 0;
    }
  }

  rtc.begin();
  // Mit den folgenden Zeilen kann das RTC-Modul konfiguriert werden:
  //rtc.setTime(19, 41, 0);     // Setzt die Zeit auf hh, mm, ss (24-Stunden-Format)
  //rtc.setDate(5, 3, 2016);    // Setzt das Datum auf den 5. März 2016
  //rtc.setDOW();               // Setzt den Wochentag (Day-of-Week) in Abhängigkeit vom aktuellen Datum


  /*
  analogWrite(motorA, 255);
  analogWrite(motorB, 0);
  delay(200);
  analogWrite(motorA, 0);
  analogWrite(motorB, 255);
  delay(200);
  analogWrite(motorA, 0);
  analogWrite(motorB, 0);
*/

  // Pins für Motorenrichtungen als Ausgang
//  pinMode(DIRECTION_A, OUTPUT);
//  pinMode(DIRECTION_B, OUTPUT);
  
  // Geschwindigkeiten setzen
//  analogWrite(geschwindigkeitA, geschwindigkeit);
//  analogWrite(geschwindigkeitB, geschwindigkeit);

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

  switch (status) {

    case WAITING:
      Serial.println("WAITING");
      //wenn die zeit gekommen ist
      //setzte den status auf running und
      //break;
      //sonst:
      //delay(1000);
      if (millis() - time > PERIOD)
      {
        status = RUNNING;
      }
      else
      {
        delay(1000);
      }
      break;

    case RUNNING:
      Serial.println("RUNNING");
      //wenn direction NONE ist
      //setzte direction auf up
      //schalte die motoren entsprechend
      //wenn direction UP ist und wenn das obere offset erreicht ist
      //setze direction auf down
      //schalte motoren entsprechend
      //wenn direction auf down ist und wenn das untere offset erreicht ist
      //setze direction auf NONE
      //und schalte motoren aus.
      //und merk dir die zeit.
      poti_wert =analogRead(POTI_PIN);
      Serial.print("Poti-Wert: ");Serial.println(poti_wert);
      
      if (direction == NONE)
      {
        Serial.print("direction: ");Serial.println("NONE");
        direction = UP;
        analogWrite(DIRECTION_A, 0);
        analogWrite(DIRECTION_B, SPEED);
      }
      else if ((direction == UP) && (poti_wert > (1023 - OFFSET/2)))
      {
        Serial.print("direction: ");Serial.println("UP");
        direction = DOWN; 
        analogWrite(DIRECTION_A, SPEED);
        analogWrite(DIRECTION_B, 0);
      }
      else if ((direction == DOWN) && (poti_wert < OFFSET/2))
      {
        Serial.print("direction: ");Serial.println("DOWN");
        direction = NONE;
        status = WAITING;
        analogWrite(DIRECTION_A, 0);
        analogWrite(DIRECTION_B, 0);
        time = millis();
      }
      break;
    default: 
      // if nothing else matches, do the default
      // default is optional
      break;


  }
  /*
   * Der Poti bewegt sich im Bereich 0-(1/2 offset) bzw. 1023-(1/2 offset)
   * 
   */

/*

  
  poti_wert =analogRead(POTI_PIN);

  analogWrite(DIRECTION_A, 215);
  analogWrite(DIRECTION_B, 0);

  Serial.print("Poti-Wert: "); Serial.println(poti_wert);

  if(poti_wert == 1023)
  {
    analogWrite(DIRECTION_A, 0);
    analogWrite(DIRECTION_B, 0);
    delay(40000);
  }
  */
/*  
  //movingForwards
  //movingBackwards
  //for (int i = 0; i < 1024; ++i)
  //{
    //analogWrite(motorA, 255);
    //analogWrite(motorB, 0);
    
  analogWrite(DIRECTION_A, 215);
  analogWrite(DIRECTION_B, 0);
  delay(500);
  analogWrite(DIRECTION_A, 0);
  analogWrite(DIRECTION_B, 215);
  delay(500);
  analogWrite(DIRECTION_A, 0);
  analogWrite(DIRECTION_B, 0);
    drive(); 
  //}

  delay(5000);
  */
    drive();
}


void drive()
{

  /*
   * Holt die aktuelle Zeit im Format HH:MM von der RTC und speichert sie zwischen
   * TODO: nicht in jedem Loop machen sondern alle 1000 Millis 
   * (Sekundengenau dann, kommt eh nicht drauf an und spart vllt. loop()-Zeit.)
   */
  strTimeTemp = rtc.getTimeStr(FORMAT_SHORT);

  /*
   * Wenn sich die Zeit im Vergleich zum letzten Durchlauf verändert hat,
   * muss die Anzeige aktualisiert werden.
   */
  if (!strTime.equals(strTimeTemp))
  {
    //Serial.println("Anzeige wird aktualisiert");

    strTime = strTimeTemp;          //angezeigte Zeit wird für den nächsten Loop gespeichert

    setDisplay();                   //Das letztlich angezeigte Array wird aufgebaut
  } 
  
  poti_wert =analogRead(POTI_PIN);  //Der aktuelle Stand des Potis wird eingelesen

  Serial.print("Poti-Wert nochmal: ");Serial.println(poti_wert);
  /*
   * Steht der Poti im Bereich des OFFSET, bleibt das Licht aus.
   */
  if ((poti_wert < OFFSET) || (poti_wert > (1024-OFFSET)))
  {
    setLEDsOff();             //schaltet alle LEDs aus
  }  
  else  
  {
    /*
     * Hier werden die LEDs zur Anzeige der Uhrzeit geschaltet, und zwar
     * Abhängig vom Wert, den der Poti liefert.
     * Die Anzeigefläche unterteilt sich wie folgt:
     * [Offset][H][Space][H][Delimiter][M][Space][M][Offset]
     * Der Bereich zwischen den Offsets wird in so viele Teile unterteilt,
     * wie die Darstellung der Anzeige benötigt. Die Anzahl dieser Teile
     * ergibt sich, wenn die für die Anzeige zur Verfügung stehende Poti-Strecke
     * durch die in der Konstante DISPLAY_WIDTH gespeichert Zeichenzahl geteilt wird.
     * Also: ((1024 - (2 * OFFSET)) / DISPLAY_WIDTH)
     * Hier wird jetzt zunächst berechnet, in welchem dieser Teile der Poti
     * grade steht. Wenn die Anzeige 31 Zeichen breit ist, dann muss hier eine Zahl
     * zwischen 1 und 31 herauskommen. Sollte das nicht der Fall sein, werden
     * die LEDs alle deaktiviert.
     * Sollte der Wert zwischen 1 und 31 liegen, dann wird die entsprechende "Spalte" im
     * Display-Array verwendet, um die LEDs an- bzw. auszuschalten.
     */
    
    int x = (poti_wert - OFFSET) / ((1024 - (2 * OFFSET)) / DISPLAY_WIDTH);

    if ((x < 0) || (x > DISPLAY_WIDTH))
    {
      setLEDsOff();             //schaltet alle LEDs aus
    }
    else
    {  
      if(arrDisplay[0][x] == 1){ digitalWrite(LED_1, LOW); } else { digitalWrite(LED_1, HIGH); }
      if(arrDisplay[1][x] == 1){ digitalWrite(LED_2, LOW); } else { digitalWrite(LED_2, HIGH); }
      if(arrDisplay[2][x] == 1){ digitalWrite(LED_3, LOW); } else { digitalWrite(LED_3, HIGH); }
      if(arrDisplay[3][x] == 1){ digitalWrite(LED_4, LOW); } else { digitalWrite(LED_4, HIGH); }
      if(arrDisplay[4][x] == 1){ digitalWrite(LED_5, LOW); } else { digitalWrite(LED_5, HIGH); }
      if(arrDisplay[5][x] == 1){ digitalWrite(LED_6, LOW); } else { digitalWrite(LED_6, HIGH); }
      if(arrDisplay[6][x] == 1){ digitalWrite(LED_7, LOW); } else { digitalWrite(LED_7, HIGH); }
      digitalWrite(LED_8, HIGH); // if(arrDisplay[7][x]){ digitalWrite(LED_8, HIGH); } else { digitalWrite(LED_8, LOW); }
    }
  }
}

/****************************************************************************
 * Hier wird ein zweidimensionales Array so befüllt, dass sich damit
 * die LEDs für die Anzeige sinnvoll ein - und ausschalten lassen.
 ****************************************************************************/
void setDisplay()
{

  displayIndex = 0;

  //Zeit-String in Array umwandeln
  strTime.toCharArray(arrCharTime, strTime.length() + 1);

  //Zusammensetzen der Anzeige nach dem Muster
  //[H1][Space][H2][Delimiter][M1][Space][M2]
  addDigitToDisplay(arrCharTime[0]);
  addSpace();
  addDigitToDisplay(arrCharTime[1]);
  addDelimiter();
  addDigitToDisplay(arrCharTime[3]);
  addSpace();
  addDigitToDisplay(arrCharTime[4]);
}

/****************************************************************************
 * Die richtige Ziffer, also das richtige Array, muss
 * noch ausgewählt werden.
 ****************************************************************************/
void addDigitToDisplay(char i)
{
  
  if (i == '0') {
    addDigit(DIGIT_0);
  }
  else if (i == '1') {
    addDigit(DIGIT_1);
  }
  else if (i == '2') {
    addDigit(DIGIT_2);
  }
  else if (i == '3') {
    addDigit(DIGIT_3);
  }
  else if (i == '4') {
    addDigit(DIGIT_4);
  }
  else if (i == '5') {
    addDigit(DIGIT_5);
  }
  else if (i == '6') {
    addDigit(DIGIT_6);
  }
  else if (i == '7') {
    addDigit(DIGIT_7);
  }
  else if (i == '8') {
    addDigit(DIGIT_8);
  }
  else if (i == '9') {
    addDigit(DIGIT_9);
  }
  else
  {
    //Serial.println("Schwerer Fehler");
    exit(0);
  }
}

/****************************************************************************
 * Hier wird das Array einer Ziffer in das Dispay-Array geschrieben.
 * Gleichzeitig wird ein Zähler mitgeführt, der die Position der aktuell 
 * zu schreibenden SPALTE im Display Array enthält.
 ****************************************************************************/
void addDigit(const byte DIGIT[DIGIT_HEIGHT][DIGIT_WIDTH])
{
  for(int j=0; j < DIGIT_WIDTH; ++j)
  {
    for(int i=0; i < DIGIT_HEIGHT; ++i)
    {
      arrDisplay[i][displayIndex] = DIGIT[i][j];
    }
    ++displayIndex;
  }
}

/****************************************************************************
 * Fügt dem Display-Array einen Leerraum hinzu.
 * Auch hier wird der Zähler geführt, der die Position
 * der aktuell zu schreibenden SPALTE im Display enthält.
 ****************************************************************************/
void addSpace()
{
  for(int j=0; j < sizeof(SPACE[0]); ++j)
  {
    for(int i=0; i < DIGIT_HEIGHT; ++i)
    {
      arrDisplay[i][displayIndex] = SPACE[i][j];
    }
    ++displayIndex;
  }
}

/****************************************************************************
 * Fügt dem Display-Array das Trennzeichen (:) hinzu.
 * Auch hier wird der Zähler geführt, der die Position
 * der aktuell zu schreibenden SPALTE im Display enthält.
 ****************************************************************************/
void addDelimiter()
{
  for(int j=0; j < sizeof(DELIMITER[0]); ++j)
  {
    for(int i=0; i < DIGIT_HEIGHT; ++i)
    {
      arrDisplay[i][displayIndex] = DELIMITER[i][j];
    }
    ++displayIndex;
  }
}

/****************************************************************************
 * Deaktiviert alle LEDs
 ****************************************************************************/
void setLEDsOff()
{
    digitalWrite(LED_1, HIGH);
    digitalWrite(LED_2, HIGH);
    digitalWrite(LED_3, HIGH);
    digitalWrite(LED_4, HIGH);
    digitalWrite(LED_5, HIGH);
    digitalWrite(LED_6, HIGH);
    digitalWrite(LED_7, HIGH);
    digitalWrite(LED_8, HIGH);
}

