/*
Created by: Carlos E. Ramírez <carlose.ramirezsantiago@gmail.com>
Last update: 05 de agosto, 2017
-----
Control firmware of a 3 degrees of freedom, stepper motorized robot
GDL 1: Rotational motion on horizontal plane
GDL 2: Rotational motion on vertical plane (2 motors)
GDL 3: Translational motion on power screw mechanism
-----
            PIN      FUNCIÓN
Entradas
            21       Interruption
Salidas
            2-5      PWM signals
            22-27    Button signals
            28-31    DIR signals
*/

const int miInt = 21;
const int pwm1 = 2, pwm2 = 3, pwm3 = 4, pwm4 = 5;
const int s1 = 28, s2 = 29, s3 = 30, s4 = 31;
volatile int tecla = 0;

class Frecuencia
{
  int pwmPin; // el numero del PIN (digital) que se usará
  long OnTime; // milisegundos del tiempo encendido
  long OffTime; // millisegundos del tiempo apagado
  
  int State; //Se usara para escribir con digitalWrite en pwmPin
  unsigned long previousMillis; // Variable que registra en que ms fue la ultima vez que hubo un cambio
  
  public:
  Frecuencia(int pin, long on, long off) //Esto es un constructor
    {
      pwmPin = pin;
      pinMode(pwmPin, OUTPUT);
      OnTime = on;
      OffTime = off;
      State = LOW;
      previousMillis = 0;
    }
    
    void Update()
    {
      unsigned long currentMillis = millis();
      if((State == LOW) && (currentMillis - previousMillis >= OnTime))
      {
        State = HIGH;
        previousMillis = currentMillis;
        digitalWrite(pwmPin, State);
      }
      else if ((State == HIGH) && (currentMillis - previousMillis >= OffTime))
      {
      State = LOW;
      previousMillis = currentMillis;
      digitalWrite(pwmPin, State);
      }
    }
};

//Objetos motor creados
Frecuencia motor1(pwm1, 100, 100); //usara el pin pwm1, estara prendido 5 ms y apagado por 5 ms. Freq = 100Hz
Frecuencia motor2(pwm2, 100, 100); //4Hz
Frecuencia motor3(pwm3, 100, 100);
Frecuencia motor4(pwm4, 400, 400);


void setup() {
  //Pines de Botones y DIR como salidas
  for (int i = 22; i <= 31; i++) {
    pinMode(i, OUTPUT);
  }
  //Pines PWM como salidas ya se hizo en la clase
  //for (int i = 2; i <= 7; i++) {
    //pinMode(i, OUTPUT);
  //}
  
  //Pines Botones en Alto
  for (int i = 22; i <= 27; i++) {
    digitalWrite(i, HIGH);
  }
  //Pines DIR en Bajo
  for (int i = 28; i <= 31; i++) {
    digitalWrite(i, LOW);
  }
  //Pines PWM en Bajo
  for (int i = 2; i <= 5; i++) {
    digitalWrite(i, LOW);
  }

  attachInterrupt(digitalPinToInterrupt(miInt), btnPress, RISING);
}

void loop() {
  while (tecla != 0) {
    switch (tecla) {
      case 32:
        digitalWrite(s1, HIGH);
        motor1.Update();
        break;
      case 16:
        digitalWrite(s1, LOW);
        motor1.Update();
        break;
      case 8:
        digitalWrite(s2, HIGH);
        digitalWrite(s3, HIGH);
        motor2.Update();
        motor3.Update();
        break;
      case 4:
        digitalWrite(s2, LOW);
        digitalWrite(s3, LOW);
        motor2.Update();
        motor3.Update();
        break;
      case 2:
        digitalWrite(s4, HIGH);
        motor4.Update();
        break;
      case 1:
        digitalWrite(s4, LOW);
        motor4.Update();
        break;
      default:
        break;

    }
    //Actualizamos condicional para saber
    //si permanecer o salir de ciclo
    tecla = btnread();
    
  }
  //Si salimos del ciclo
  //ES MUY NECESARIO Reestablecer salidas a valores setup iniciales
  //Pines DIR en Bajo
  for (int i = 28; i <= 31; i++) {
    digitalWrite(i, LOW);
  }
  //Pines PWM en Bajo
  for (int i = 2; i <= 5; i++) {
    digitalWrite(i, LOW);
  }
  
  attachInterrupt(digitalPinToInterrupt(miInt), btnPress, RISING);
}

void btnPress() {
  detachInterrupt(digitalPinToInterrupt(miInt));
  tecla = btnread();
}

byte btnread() {
  unsigned char count = 0;
  volatile byte aLeer = 22;
  volatile byte queBoton = 0;

  if (digitalRead(miInt) == HIGH) {
    do {
      //Apagamos todas las filas
      for (int i = 22; i <= 27; i++) {
        digitalWrite(i, LOW);
      }
      //Seleccionamos sólo una como salida
      //Y colocamos ese pin en estado alto
      digitalWrite(aLeer, HIGH);
      //Leemos la columna y
      //la guardamos en la variable
      queBoton = queBoton + digitalRead(miInt);
      //Si ya leímos las 6 filas las sig instrucc no son necesarias
      //por lo que salimos del ciclo
      if (count == 5) break;
      queBoton = 2 * queBoton;
      //Recorremos la fila a leer
      aLeer += 1;
      //Incrementar el contador de filas
      count++;
    } while (1);
    //Reestablecemos salidas a valores setup iniciales:
    //Pines Botones en Alto
    for (int i = 22; i <= 27; i++) {
      digitalWrite(i, HIGH);
    }
    //Variable de retorno para posterior manipulacion
    return queBoton;
  }
  return 0;
}
