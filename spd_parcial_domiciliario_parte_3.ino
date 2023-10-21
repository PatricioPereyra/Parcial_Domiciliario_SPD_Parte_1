/*
En esta tercera parte del proyecto integramos un FOTODIODO,
que es un dispositivo sensible a la incidencia de la luz.
La magnitud de sus valores están definidos en un rango decimal
de cero a uno. Cuando el sensor llega a uno, se resetean los displays.

*/

/* Define los segmentos de cada display con el pin correspondiente*/
#define A 10
#define B 11
#define C 5
#define D 6
#define E 7
#define F 9
#define G 8

/* Define el positivo y negativo del motor.*/
#define ADELANTE 13
#define ATRAS 12


/*Se definen los displays UNIDADES y DECENAS
con los pines 3 y 2 respectivamente*/
#define UNIDADES 3
#define DECENAS 2

/*Se definen los pulsadores SUBIR y BAJAR
con los pines A5 y A4 respectivamente*/
#define SUBIR A5
#define BAJAR A4

/*Se define cada lado del Slideswitch a sus pines */
#define D_IZQUIERDA A2
#define D_DERECHA A3

/*Define el sensor de fuerza y el fotodiodo a sus pines */
int FUERZA = A0;
int FOTO = A1;

void setup()
{
/*Configura pines correspondientes como salida*/
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(UNIDADES, OUTPUT);
  pinMode(DECENAS, OUTPUT);
  pinMode(ADELANTE, OUTPUT);
  pinMode(ATRAS, OUTPUT);
  
 /*Configura los pulsadores, slideswitch y sensor 
 como input PULLUP para que estén conectados a 
 la resistencia interna de la placa*/
  pinMode(SUBIR, INPUT_PULLUP);
  pinMode(BAJAR, INPUT_PULLUP);
  pinMode(D_IZQUIERDA, INPUT_PULLUP);
  pinMode(D_DERECHA, INPUT_PULLUP);
  pinMode(FUERZA, INPUT_PULLUP);
  pinMode(FOTO, INPUT_PULLUP);
  Serial.begin(9600);
}


/*Define como NO PRESIONADOS a los valores iniciales
de estas variables*/
int subirAnterior = 1;
int bajarAnterior = 1;

/*Inicializa las variables unidades y decenas en 0*/
int unidades = 0;
int decenas = 0;

/*Inicializa el modo de números primos y 
el del estado deslizante en falso*/
bool modoPrimo = false;
bool estadoDeslizante = false;

/*Inicializa las variables lectura y estandar 
(para el sensor de fuerza) como int */
int lectura;
int estandar;
int lecturaFoto;
int estandarFoto;

/*Enciende y apaga de manera alternativa cada display, 
con un delay de 50 milisegundos entre un display y el otro,
mostrando los números que deseamos como si ambos estuviesen encendidos
simultaneamente.*/
void loop()
{
  leeSensor(FUERZA);
  leeFoto(FOTO);
  ManipularBotones();
  PrenderDisplay(UNIDADES, unidades);
  delay(50);
  PrenderDisplay(DECENAS, decenas);
  delay(50);
}

/*Lee el valor del sensor de fuerza y
si llega a 10N, resetea el contador*/
void leeSensor(int sensor)
{
  lectura = analogRead(FUERZA);
  estandar = map(lectura,1023,24,0,10);
  estandar = estandar / 10;
  //Serial.println(estandar);
  if (estandar == 1)
  {
    Resetear();
  }
    
}

/*Función que lee el valor del FOTODIODO y si llega al
valor de 1, se resetean los displays*/
void leeFoto(int foto)
{
  lecturaFoto = analogRead(FOTO);
  float magnitud = map(lecturaFoto, 85, 88, 0, 10);
  magnitud = magnitud / 10;
  Serial.println(magnitud);
  if (magnitud == 1.0)
  {
    Resetear();
  }
}

/*Crea la función resetear para que reinicie el contador en 00*/
void Resetear()
{
  unidades = 0;
  decenas = 0;
}


/*Chequea el estado de los pulsadores utilizando 
las funciones Subir(), Bajar(). 
Toma la función deslizante() con dos parámetros dependiendo 
su estado. Actualiza los valores de las variables 
subirAnterior, bajarAnterior*/
void ManipularBotones()
{
  subirAnterior = Subir(SUBIR, subirAnterior, modoPrimo);
  bajarAnterior = Bajar(BAJAR, bajarAnterior, modoPrimo);
  deslizante(D_IZQUIERDA, D_DERECHA);
}

void deslizante(int izquierdo, int derecho)
{//lee el estado del deslizante del switch
  int estadoIzq = digitalRead(izquierdo);
  int estadoDer = digitalRead(derecho);
  /*si el estado izq está prendido el contador va a 
  ir a través de numeros primos y el motor 
  se va a mover hacia atras*/
  if (estadoIzq == 0)
  {
    modoPrimo = true;
    digitalWrite(ATRAS, HIGH);
    digitalWrite(ADELANTE, LOW);
  }
  else
  {
   /*de lo contrario, el contador irá de uno en uno 
   y el motor se moverá hacia adelante*/
    modoPrimo = false;
    digitalWrite(ADELANTE, HIGH);
    digitalWrite(ATRAS, LOW);
  }
}


/*Aumenta el valor de las unidades o decenas de 1 en 1
cuando el pulsador es presionado. Si el valor de las unidades
alcanza el valor de 10, unidades vuelve a valer cero
y decenas aumenta un valor. Toma el parámetro modoPrimo 
para ver si se mueve a través de los primos o a través 
de todos los números del 0 al 99. La función retorna 
el estado del botón.*/
int Subir(int pulsador, int estadoAntBoton, bool modoPrimo)
{
  int estadoBoton = digitalRead(pulsador);
  if (estadoBoton == 0 && estadoAntBoton == 1)
  {
    if (modoPrimo) {
      // Si el modoPrimo está activado, busca el siguiente número primo
      int num = decenas * 10 + unidades + 1; // Combinamos decenas y unidades para formar el número completo
      while (num <= 99) { // Aseguramos que el número no sea mayor de 99
        if (EsPrimo(num)) {
          unidades = num % 10; // Unidades del número primo
          decenas = num / 10; // Decenas del número primo
          break;
        }
        num++;
      }
    } 
    else {
      unidades++;
      if (unidades == 10)
      {
        unidades = 0;
        decenas++;
        if (decenas == 10)
        {
          decenas = 0;
        }
      }
    }
  }
  return estadoBoton;
}


/*Función que toma un número y devuelve true 
si es primo, false si no lo es*/
bool EsPrimo(int num)
{
  if (num <= 1)
  {
    return false;
  } 
  else
  {
    int cont = 0;
    for (int i = 1; i <= num; i++)
  	{
      if (num % i == 0)
      {
        cont++;
      }   
  	}
    if (cont == 2)
    {
      return true;
    }
    else
    {
      return false;
    }
 
  }  
  
}

/*Disminuye el valor de las unidades o decenas de 1 en 1 
cuando el pulsador es presionado. Si el valor de las unidades
alcanza el valor de 0, unidades pasa a valer 9 y 
decenas disminuye un valor. Toma el parámetro modoPrimo 
para ver si se mueve a través de los primos o 
a través de todos los números del 0 al 99. 
La función retorna el estado del botón*/
int Bajar(int pulsador, int estadoAntBoton, bool modoPrimo)
{
  int estadoBoton = digitalRead(pulsador);
  if (estadoBoton == 0 && estadoAntBoton == 1)
  { //// Si el modoPrimo está activado, busca el número primo anterior
    if (modoPrimo) {
      // Obtener el número actual en pantalla
      int num = decenas * 10 + unidades;

      // Encontrar el número primo anterior al número actual
      int numAnterior = num;
      while (numAnterior > 2) {
        numAnterior--;
        if (EsPrimo(numAnterior)) 
        {
          break;
        }
      }

      // Establecer el número actual en el número primo anterior
      unidades = numAnterior % 10;
      decenas = numAnterior / 10;
    } 
    else {
      unidades--;
      if (unidades == -1)
      {
        unidades = 9;
        decenas--;
        if (decenas == -1)
        {
          decenas = 9;
        }
      }
    }
  }
  return estadoBoton;
}


/*Enciende los segmentos del display pasado por parámetro.
Utiliza la función MostrarNumero() para encender los
segmentos correspondientes al número ingresado por parámetro*/
void PrenderDisplay(int display, int numero)
{
  if (display == UNIDADES)
  {
    digitalWrite(DECENAS, HIGH);
    digitalWrite(UNIDADES, LOW);
  }
  else if (display == DECENAS)
  {
    digitalWrite(DECENAS, LOW);
    digitalWrite(UNIDADES, HIGH);
  }
  else
  {
    digitalWrite(DECENAS, LOW);
    digitalWrite(UNIDADES, LOW);
  }
  MostrarNumero(numero);
}


/*Enciende los segmentos del display según el número pasado por parámetro.
Utiliza la función ApagarDisplay() para apagar todos los segmentos
antes de encender los segmentos correspondientes.*/
void MostrarNumero(int numero)
{
  ApagarDisplay();
  switch (numero)
  {
    case 0:
    digitalWrite(A, HIGH);
    digitalWrite(B, HIGH);
    digitalWrite(C, HIGH);
    digitalWrite(D, HIGH);
    digitalWrite(E, HIGH);
    digitalWrite(F, HIGH);
    break;
    case 1:
    digitalWrite(B, HIGH);
    digitalWrite(C, HIGH);
    break;
    case 2:
    digitalWrite(A, HIGH);
    digitalWrite(B, HIGH);
    digitalWrite(G, HIGH);
    digitalWrite(E, HIGH);
    digitalWrite(D, HIGH);
    break;
    case 3:
    digitalWrite(A, HIGH);
    digitalWrite(B, HIGH);
    digitalWrite(C, HIGH);
    digitalWrite(D, HIGH);
    digitalWrite(G, HIGH);
    break;
    case 4:
    digitalWrite(B, HIGH);
    digitalWrite(C, HIGH);
    digitalWrite(F, HIGH);
    digitalWrite(G, HIGH);
    break;
    case 5:
    digitalWrite(A, HIGH);
    digitalWrite(C, HIGH);
    digitalWrite(D, HIGH);
    digitalWrite(F, HIGH);
    digitalWrite(G, HIGH);
    break;
    case 6:
    digitalWrite(A, HIGH);
    digitalWrite(C, HIGH);
    digitalWrite(D, HIGH);
    digitalWrite(E, HIGH);
    digitalWrite(F, HIGH);
    digitalWrite(G, HIGH);
    break;
    case 7:
    digitalWrite(A, HIGH);
    digitalWrite(B, HIGH);
    digitalWrite(C, HIGH);
    break;
    case 8:
    digitalWrite(A, HIGH);
    digitalWrite(B, HIGH);
    digitalWrite(C, HIGH);
    digitalWrite(D, HIGH);
    digitalWrite(E, HIGH);
    digitalWrite(F, HIGH);
    digitalWrite(G, HIGH);
    break;
    case 9:
    digitalWrite(A, HIGH);
    digitalWrite(B, HIGH);
    digitalWrite(C, HIGH);
    digitalWrite(D, HIGH);
    digitalWrite(F, HIGH);
    digitalWrite(G, HIGH);
    break;
  }
}


/*Apaga todos los segmentos del display*/
void ApagarDisplay()
{
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, LOW);	
}


