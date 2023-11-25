
/*
 * Протокол: @ байт_команды данные_1 данные_2 ... данные_[BUFFER_SIZE-1]
 * Стартовый байт @ (значение 64)
 * Конечный байт ; (значение 59)
 * ВНИМАНИЕ! Если отправляет print (отправка символа) - ASCII_CONVERT должен быть '0'
 * (переводит символы даты в цифры)
 * Если отправляет write (отправка чистого байта) - ASCII_CONVERT должен быть просто 0 
 */

#define SOFTWARE_BT

#ifdef SOFTWARE_BT
#include <SoftwareSerial.h>
#endif

#define LEDPIN 13 
#define ASCII_CONVERT '0'
#define BUFFER_SIZE 7

#define LEFT_SPEED_PULSE 2
#define RIGHT_SPEED_PULSE 3
#define LEFT_AN 5 // PWM pin
#define RIGHT_AN 6 // PWM pin
#define LEFT_CW 7 // HIGH - forward
#define RIGHT_CW 8 // LOW - forward

typedef enum {
    CMD_STOP = 0,
    CMD_GO_FWD,
    CMD_GO_BWD,
    CMD_ROTATE_LEFT,
    CMD_ROTATE_RIGHT,
    CMD_INVALID  
} command_t;

typedef enum {
    FWD = 0,
    BWD
} direction_t;

#ifdef SOFTWARE_BT
#define BT_RX 9
#define BT_TX 10
SoftwareSerial btSerial(BT_RX, BT_TX);
#endif

uint8_t buffer[BUFFER_SIZE];
uint8_t gHasFirstCommand = 0;

command_t gCurrentCmd = CMD_STOP;
direction_t gLeftDirection = FWD;
direction_t gRightDirection = FWD;

int gLeftTask = 0;
int gRightTask = 0;
int gLeftCounter = 0;
int gRightCounter = 0;

int parsing() 
{
    static bool parseStart = false;
    static byte counter = 0;
    static bool useBt = false;

#ifdef SOFTWARE_BT
    if (useBt = btSerial.available() || Serial.available())
#else
    if (Serial.available())
#endif
    {    
#ifdef SOFTWARE_BT
        byte in = useBt ? btSerial.read() : Serial.read();  
//        Serial.print(in);
#else
        byte in = Serial.read();        
#endif
        if (in == '\n' || in == '\r') return 0; // игнорируем перевод строки    
        if (in == ';') // завершение пакета
        {        
            parseStart = false;
            return counter;
        }
        if (in == '@') // начало пакета
        {        
            parseStart = true;
            if (!gHasFirstCommand)
            {
                gHasFirstCommand = 1;
            }
                
            counter = 0;
            return 0;
        } 
        if (parseStart) // чтение пакета
        {       
            // - '0' это перевод в число (если отправитель print)
            buffer[counter] = in - ASCII_CONVERT;
            counter++;

            if (counter == BUFFER_SIZE) 
            {
                parseStart = false;
                return counter;
            }
        }
    }
    return 0;
}

// Get distance in marks
int getTravelDistance()
{
    return (buffer[1] * 1000 + buffer[2] * 100 + buffer[3] * 10 + buffer[4]);
}

void processCommand()
{
    if (parsing()) 
    {
        uint8_t cmd = buffer[0];
        switch (cmd) // согласно коду команды
        {
            case CMD_STOP:
            {
                Serial.println("Got CMD_STOP");
#ifdef SOFTWARE_BT
                btSerial.println("Got CMD_STOP");
#endif
                gCurrentCmd = cmd;
                gLeftTask = gRightTask = 0;
                gLeftCounter = gRightCounter = 0;
                gLeftDirection = FWD;
                gRightDirection = FWD;
                break;
            }
            case CMD_GO_FWD:
            {
                Serial.println("Got CMD_GO_FWD");
#ifdef SOFTWARE_BT
                btSerial.println("Got CMD_GO_FWD");
#endif
                gCurrentCmd = cmd;
                gLeftTask = gRightTask = getTravelDistance();
                gLeftCounter = gRightCounter = 0;
                gLeftDirection = FWD;
                gRightDirection = FWD;
                Serial.print("Task = ");
                Serial.println(gLeftTask);
#ifdef SOFTWARE_BT
                btSerial.print("Task = ");
                btSerial.println(gLeftTask);
#endif
                break;
            }
            case CMD_GO_BWD:
            {
                Serial.println("Got CMD_GO_BWD");
#ifdef SOFTWARE_BT
                btSerial.println("Got CMD_GO_BWD");
#endif
                gCurrentCmd = cmd;
                int travelDistance = getTravelDistance();
                gLeftTask = -travelDistance;
                gRightTask = -travelDistance;
                gLeftCounter = gRightCounter = 0;
                gLeftDirection = BWD;
                gRightDirection = BWD;
                Serial.print("Task = ");
                Serial.println(gLeftTask);
#ifdef SOFTWARE_BT
                btSerial.print("Task = ");
                btSerial.println(gLeftTask);
#endif
                break;
            }
            case CMD_ROTATE_LEFT:
            {
                Serial.println("Got CMD_ROTATE_LEFT");
#ifdef SOFTWARE_BT
                btSerial.println("Got CMD_ROTATE_LEFT");
#endif
                gCurrentCmd = cmd;
                int travelDistance = getTravelDistance();
                gLeftTask = travelDistance;
                gRightTask = -travelDistance;
                gLeftCounter = gRightCounter = 0;
                gLeftDirection = BWD;
                gRightDirection = FWD;
                Serial.print("Left = ");
                Serial.println(gLeftTask);
#ifdef SOFTWARE_BT
                btSerial.print("Left = ");
                btSerial.println(gLeftTask);
#endif
                break;
            }
            case CMD_ROTATE_RIGHT:
            {
                Serial.println("Got CMD_ROTATE_RIGHT");
#ifdef SOFTWARE_BT
                btSerial.println("Got CMD_ROTATE_RIGHT");
#endif
                gCurrentCmd = cmd;
                int travelDistance = getTravelDistance();
                gLeftTask = -travelDistance;
                gRightTask = travelDistance;
                gLeftCounter = gRightCounter = 0;
                gLeftDirection = FWD;
                gRightDirection = BWD;
                Serial.print("Right = ");
                Serial.println(gLeftTask);
#ifdef SOFTWARE_BT
                btSerial.print("Right = ");
                btSerial.println(gLeftTask);
#endif
                break;
            }
            case CMD_INVALID:
            default:
            {
                Serial.println("Got CMD_INVALID");
#ifdef SOFTWARE_BT
                btSerial.println("Got CMD_INVALID");
#endif
                gCurrentCmd = CMD_STOP;
                gLeftTask = gRightTask = 0;
                gLeftCounter = gRightCounter = 0;
                gLeftDirection = FWD;
                gRightDirection = FWD;
                break;              
            }
        } // end switch()

        digitalWrite(LEDPIN, !digitalRead(LEDPIN));
        
    } else { // end parsing()
        if (gCurrentCmd == CMD_STOP && !gHasFirstCommand) {
            Serial.println("Waiting for CMD");
#ifdef SOFTWARE_BT
            btSerial.println("Waiting for CMD");
#endif
            delay(1000);
        }
    }
}

void setup() {
  // Serial over usb or radio
  Serial.begin(9600);
  
  // Serial over bluetooth
#ifdef SOFTWARE_BT
  pinMode(BT_RX, INPUT);
  pinMode(BT_TX, OUTPUT);
  btSerial.begin(9600);
#endif

  pinMode(LEFT_SPEED_PULSE, INPUT);
  pinMode(RIGHT_SPEED_PULSE, INPUT);
  attachInterrupt(digitalPinToInterrupt(LEFT_SPEED_PULSE), leftPulse, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RIGHT_SPEED_PULSE), rightPulse, CHANGE);

  pinMode(LEFT_AN, OUTPUT);
  pinMode(RIGHT_AN, OUTPUT);

  pinMode(LEFT_CW, OUTPUT);
  pinMode(RIGHT_CW, OUTPUT);

  // Лампочка для индикации
  pinMode(LEDPIN, OUTPUT);
}

// TODO: get direction
void leftPulse() {
  gLeftCounter++;  
}

void rightPulse() {
  gRightCounter++;
}

void doMove()
{
    if (gCurrentCmd = CMD_STOP)
    {
        analogWrite(LEFT_AN, 0); 
        analogWrite(RIGHT_AN, 0); 
        gLeftCounter = 0;
        gLeftTask = 0; 
        gRightCounter = 0;
        gRightTask = 0; 
        return;
    }

    if (gLeftTask < 0)
    {
        gLeftTask = -gLeftTask;
    }

    if (gLeftCounter >= gLeftTask)
    {
        analogWrite(LEFT_AN, 0);
    }
    else
    {
        if (gLeftDirection == FWD)
        {
            digitalWrite(LEFT_CW, HIGH);
        }
        else
        {
            digitalWrite(LEFT_CW, LOW);          
        }

        Serial.print("LEFT POS = ");
        Serial.println(gLeftCounter);
#ifdef SOFTWARE_BT
        btSerial.print("LEFT POS = ");
        btSerial.println(gLeftCounter);
#endif               
        analogWrite(LEFT_AN, 100); 
    }

    if (gRightTask < 0)
    {
        gRightTask = -gRightTask;
    }

    if (gRightCounter >= gRightTask)
    {
        analogWrite(RIGHT_AN, 0);
    }
    else
    {
        if (gRightDirection == FWD)
        {
            digitalWrite(RIGHT_CW, LOW);
        }
        else
        {
            digitalWrite(RIGHT_CW, HIGH);          
        }

        Serial.print("RIGHT POS = ");
        Serial.println(gRightCounter);
#ifdef SOFTWARE_BT
        btSerial.print("RIGHT POS = ");
        btSerial.println(gRightCounter);
#endif
        analogWrite(RIGHT_AN, 100);
    }
}

void loop() 
{
    processCommand();
    doMove();
}
