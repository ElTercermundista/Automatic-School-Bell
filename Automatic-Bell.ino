/*
Codigo Timbre Escuela con conexion NTP
Elementos utilizados:
-Arduino NANO
-ENC28J60
-DS1302
-Pantalla de 16X2
-I2C (TL PCF8574AT)
-Modulo de Rele 5V (Dos unidades)
*/

#include <EtherCard.h>  // https://github.com/njh/EtherCard
#include <TimeLib.h>
#include <virtuabotixRTC.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//RTC Pinout
// SCLK -> 6, I/O -> 7, CE -> 8
// CLK -> 6 , DAT -> 7, Reset -> 8
virtuabotixRTC myRTC(6, 7, 8);

//Crear el objeto lcd  dirección  0x27 y 16 columnas x 2 filas
LiquidCrystal_I2C lcd(0x27,16,2);  //

// Ethernet mac address - must be unique on your network
const byte myMac[] PROGMEM = { 0x70, 0x69, 0x69, 0x2D, 0x30, 0x31 };
//const char NTP_REMOTEHOST[] PROGMEM = "ntp.bit.nl";  // NTP server name
const char NTP_REMOTEHOST[] PROGMEM = "pool.ntp.org"; 

const unsigned int NTP_REMOTEPORT = 123;             // NTP requests are to port 123
const unsigned int NTP_LOCALPORT = 8888;             // Local UDP port to use
const unsigned int NTP_PACKET_SIZE = 48;             // NTP time stamp is in the first 48 bytes of the message
byte Ethernet::buffer[350];                          // Buffer must be 350 for DHCP to work


// send an NTP request to the time server at the given address
void sendNTPpacket(const uint8_t* remoteAddress) {
  // buffer to hold outgoing packet
  byte packetBuffer[ NTP_PACKET_SIZE];
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;            // Stratum, or type of clock
  packetBuffer[2] = 6;            // Polling Interval
  packetBuffer[3] = 0xEC;         // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

 // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  ether.sendUdp(packetBuffer, NTP_PACKET_SIZE, NTP_LOCALPORT, remoteAddress, NTP_REMOTEPORT );
  //Serial.println("NTP request sent.");
  delay(4000);
}

void udpReceiveNtpPacket(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port, const char *packetBuffer, uint16_t len) {
  //Serial.println();
  // Serial.println("NTP response received.");
  lcd.setCursor(0,0);
  lcd.print("                "); 
  lcd.setCursor(0,0);
  lcd.print("DATOS RECIBIDOS");
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("                "); 
  

  // the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, extract the two words:
  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;
  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
  // subtract seventy years:
  unsigned long epoch = secsSince1900 - seventyYears;
  
 // print Unix time:
  Serial.print("Unix time = ");
  Serial.println(epoch);
  

  sincro(epoch);
  pantalla();
  setSyncInterval(10);   

  
}

  

int r_diaSemana;                  // almacena el resultado del dia de la semana calculado
const int pinRELE1 = 4; //Aulas           
const int pinRELE2 = 5; //Talleres
int segundo;
int minuto;
int hora;
int tiempo_timbre_ingreso_salida = 3000; //3seg
int tiempo_timbre_medio_modulo = 1500; //1,5seg
int tiempo_timbre_recreo = 1000; //1seg
int tiempo_timbre_limpieza = 500; //.5seg

// Horarios Escuela Mañana Aulas
int h1_c1=7;   int m1_c1=25;   int s1_c1=0;
//int h2_c1=7;   int m2_c1=35;  int s2_c1=0;
int h3_c1=8;   int m3_c1=05;  int s3_c1=0;
//int h4_c1=8;   int m4_c1=15;  int s4_c1=0;
int h5_c1=8;   int m5_c1=45;  int s5_c1=0;
//int h6_c1=8;  int m6_c1=55;  int s6_c1=0;
int h7_c1=8;  int m7_c1=55;  int s7_c1=0;
//int h8_c1=9;  int m8_c1=05;  int s8_c1=0;
int h9_c1=9;  int m9_c1=35;  int s9_c1=0;
//int h10_c1=9; int m10_c1=45;  int s10_c1=0;
//int h11_c1=10; int m11_c1=10;  int s11_c1=0;
int h12_c1=10; int m12_c1=15;  int s12_c1=0;
int h13_c1=10; int m13_c1=25;  int s13_c1=0;
//int h14_c1=10; int m14_c1=30;  int s14_c1=0;
//int h15_c1=10; int m15_c1=45;  int s15_c1=0;
int h16_c1=11; int m16_c1=05;  int s16_c1=0;
//int h17_cl=11; int m17_cl=30;  int s17_cl=0;
int h18_cl=11; int m18_cl=45;  int s18_cl=0;
//Horarios Escuela Tarde Aulas-
int h19_c1=14;   int m19_c1=0;   int s19_c1=0;
//int h20_c1=14;   int m20_c1=10;  int s20_c1=0;
int h21_c1=14;   int m21_c1=40;  int s21_c1=0;
//int h22_c1=14;   int m22_c1=50;  int s22_c1=0;
int h23_c1=15;   int m23_c1=20;  int s23_c1=0;
//int h24_c1=15;  int m24_c1=25;  int s24_c1=0;
int h25_c1=15;  int m25_c1=30;  int s25_c1=0;
//int h26_c1=15;  int m26_c1=40;  int s26_c1=0;
int h27_c1=16;  int m27_c1=10;  int s27_c1=0;
//int h28_c1=16; int m28_c1=20;  int s28_c1=0;
//int h29_c1=16; int m29_c1=45;  int s29_c1=0;
int h30_c1=16; int m30_c1=50;  int s30_c1=0;
int h31_c1=17; int m31_c1=00;  int s31_c1=0;
//int h32_c1=17; int m32_c1=05;  int s32_c1=0;
//int h33_c1=17; int m33_c1=30;  int s33_c1=0;
int h34_c1=17; int m34_c1=40;  int s34_c1=0;
//int h35_cl=17; int m35_cl=50;  int s35_cl=0;
int h36_cl=18; int m36_cl=15;  int s36_cl=0;
//Horarios Taller Mañana
int h37_c1=7;   int m37_c1=25;   int s37_c1=30;
int h38_c1=9;    int m38_c1=30;   int s38_c1=0;
int h39_c1=9;    int m39_c1=45;   int s39_c1=0;
int h40_c1=11;   int m40_c1=25;  int s40_c1=0;
int h41_c1=11;   int m41_c1=44;  int s41_c1=45;
//Horrios Taller Tarde
int h42_c1=14;  int m42_c1=0;   int s42_c1=30;
int h43_c1=16;  int m43_c1=0;   int s43_c1=0;
int h44_c1=16;  int m44_c1=15;  int s44_c1=0;
int h45_c1=18;  int m45_c1=00;   int s45_c1=0;
int h46_c1=18;  int m46_c1=14;  int s46_c1=45;



void setup() {
  Serial.begin(9600);
  //lcd.begin (16,2);  // inicializar lcd 
  Serial.println(F("\n[EtherCard NTP Client]"));

    //LCD inicio e encendidio de fondo
   // Inicializar el LCD
  lcd.init();
  
  //Encender la luz de fondo.
  lcd.backlight();
  
 //Limpiamos la pantalla de previos mensajes
  lcd.clear();
  
  // Escribimos el Mensaje en el LCD.
  lcd.setCursor(3,0),
  lcd.print("INICIANDO");
  delay(3000); 
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("TIMBRE ESCUELA");
  delay(2000);
   
  pinMode(pinRELE1, OUTPUT);                    // Configura como salida el pin 
  pinMode(pinRELE2, OUTPUT);                    // Configura como salida el pin 
 
  // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  if (ether.begin(sizeof Ethernet::buffer, myMac, SS) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

  if (!ether.dnsLookup(NTP_REMOTEHOST))
    Serial.println("DNS failed");

  uint8_t ntpIp[IP_LEN];
  ether.copyIp(ntpIp, ether.hisip);
  ether.printIp("NTP: ", ntpIp);

  ether.udpServerListenOnPort(&udpReceiveNtpPacket, NTP_LOCALPORT);
  //Serial.println("Started listening for response.");

  sendNTPpacket(ntpIp);

 }

void loop() {
        
    // this must be called for ethercard functions to work.
    ether.packetLoop(ether.packetReceive());

    // Control para que entre las 1 de la madrugada y las 5
       if(myRTC.hours>14 && myRTC.hours<=12){ 
         // control y ejecucion de timbres
         horario_escuela_aulas();
         horario_escuela_taller();
       } else {
         //Si el horario es entre las 1 y 5 de la madrugada se realiza una actualizacion de reloj
         if(myRTC.hours==13 && myRTC.minutes==00 && myRTC.seconds==00) actualizarServidor();
        }
    
    /*
    //Peticion Turno Mañana
    if(myRTC.hours==7 && myRTC.minutes==0 && myRTC.seconds==0)actualizarServidor();
    //Peticion Turno Tarde        
    if(myRTC.hours==13 && myRTC.minutes ==0 && myRTC.seconds== 0) actualizarServidor();
    */
 
  myRTC.updateTime();
       /* 
       Serial.println();
       Serial.print("Fecha/Horario: ");
       Serial.print(myRTC.dayofmonth);
       Serial.print("/");
       Serial.print(myRTC.month);
       Serial.print("/");
       Serial.print(myRTC.year);
       Serial.print(" ");
       Serial.print(MostrarDigito(myRTC.hours));
       Serial.print(":");
       Serial.print(MostrarDigito(myRTC.minutes));
       Serial.print(":");
       Serial.print(MostrarDigito(myRTC.seconds));
                */
         horario_escuela_aulas();
         horario_escuela_taller();
         
         delay(500);
           
             
         
         //HOra en la pantalla LCD
        // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
         //lcd.setCursor(0,0);
        // Escribimos el número de segundos trascurridos
        //lcd.print(millis()/1000);
        lcd.clear();
        lcd.setCursor(1,0);
        lcd.print("HORA:");
        lcd.setCursor(6, 0);
        lcd.print(MostrarDigito(myRTC.hours));
        //lcd.setCursor(7, 1);
        lcd.print(":");
        lcd.setCursor(9, 0);
        lcd.print(MostrarDigito(myRTC.minutes));
        lcd.print(":");
        lcd.setCursor(12,0);
        lcd.print(MostrarDigito(myRTC.seconds));
        delay(100);
       
          
 }



void sincro(unsigned long epoch ) {
 
      //unsigned long pctime = Serial.parseInt();
      if (epoch >= 1357041600) {       //Verifica valides de fecha-hora (mayor a 1-ENE-2013)
         setTime(epoch);               //Sincroniza reloj Arduino con fecha-hora recivida por puerto serie
         adjustTime(-10800);
         
      //Asignacion de el tiempo del NTP al RTC
      myRTC.setDS1302Time(second(), minute(), hour(), 1, day(), month(), year()); //Configuración del tiempo actual

      myRTC.updateTime();
             /*
       Serial.println ("Horario RTC: ");
       Serial.print(myRTC.dayofmonth);
       Serial.print("/");
       Serial.print(myRTC.month);
       Serial.print("/");
       Serial.print(myRTC.year);
       Serial.print(" ");
       Serial.print(MostrarDigito(myRTC.hours));
       Serial.print(":");
       Serial.print(MostrarDigito(myRTC.minutes));
       Serial.print(":");
       Serial.print(MostrarDigito(myRTC.seconds));
       Serial.println ();
      */
      }
   
}

void pantalla(){

    lcd.setCursor(0, 0),
    lcd.print("                "); 
    lcd.setCursor(2, 0);
    //lcd.print("CONEXION NTP");
    

   /*
   Serial.println ("Horario NTP: " );
   Serial.print(year()); 
   Serial.print('-');
   Serial.print(month());
   Serial.print('-');
   Serial.print(day());
   Serial.print(' ');
   Serial.print(hour());
   printDigito(minute());
   printDigito(second());
   Serial.println();
   setTime(pantalla);
   */
   
}

//Actualizar Servidor NTP cada cieto tiempo

void actualizarServidor() {
   
  // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  if (ether.begin(sizeof Ethernet::buffer, myMac, SS) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
     
     
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));
   
     
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

  if (!ether.dnsLookup(NTP_REMOTEHOST))
    Serial.println("DNS failed");
    

  uint8_t ntpIp[IP_LEN];
  ether.copyIp(ntpIp, ether.hisip);
  ether.printIp("NTP: ", ntpIp);

  ether.udpServerListenOnPort(&udpReceiveNtpPacket, NTP_LOCALPORT);
  //Serial.println("Started listening for response.");
  
  
  sendNTPpacket(ntpIp);

}


void printDigito(int dig){
   Serial.print(':');
   if (dig < 10){
      Serial.print('0');
   }
   Serial.print(dig);
}

 String MostrarDigito(int dig1){
   String dig = String(dig1);
   if (dig1 < 10){
     dig = '0'+dig;
   }
   return dig;
}


// Funcion que activa el Timbre 
void activar_timbre_ingreso_salida_aula(){ //Ingreso/Salida Aulas
    digitalWrite(pinRELE1, HIGH);
    lcd.setCursor(0, 0),
    lcd.print("                "); 
    lcd.setCursor(2, 0);
    lcd.print("TIMBRE AULA");
    //Serial.println("Timbre Ingreso/Salida Aula Activo");
    //Serial.println("Timbre Activo");
    delay(tiempo_timbre_ingreso_salida); 
    digitalWrite(pinRELE1,LOW);
    lcd.setCursor(0, 0);
    lcd.print("                "); 
    
}
void activar_timbre_ingreso_salida_taller(){ //Ingreso/Salida Taller
   digitalWrite (pinRELE2, HIGH);
   lcd.setCursor(0, 0);
   lcd.print("                "); 
   lcd.setCursor(1,0);
   lcd.print("TIMBRE TALLER");
   //Serial.println("Timbre Ingreso/Salida Taller Activo"); 
   // Serial.println("Timbre Activo");
   delay (tiempo_timbre_ingreso_salida);  
   digitalWrite(pinRELE2,LOW),
   lcd.setCursor(0, 0);
   lcd.print("                "); 
}

void activar_timbre_medio_modulo(){
  digitalWrite (pinRELE1, HIGH); //listo
  lcd.setCursor(0, 0);
  lcd.print("                "); 
  lcd.setCursor(2, 0);
  lcd.print("TIMBRE AULA");
  //Serial.println("Timbre Medio Modulo Activo");
  //Serial.println("Timbre Activo");
  delay(tiempo_timbre_medio_modulo);
  digitalWrite(pinRELE1,LOW); 
  lcd.setCursor(0, 0);
  lcd.print("                "); 
}
void activar_timbre_recreo_aula(){
  digitalWrite (pinRELE1, HIGH);
  lcd.setCursor(0, 0);
  lcd.print("                "); 
  lcd.setCursor(2,0);
  lcd.print("TIMBRE AULA");
  //Serial.println("Timbre Recreo Aula Activo");
  //Serial.println("Timbre Activo");
  delay(tiempo_timbre_recreo);
  digitalWrite(pinRELE1,LOW);
  lcd.setCursor(0, 0);
  lcd.print("                ");  
}
void activar_timbre_recreo_taller(){
  digitalWrite (pinRELE2,HIGH);
  lcd.setCursor(0, 0);
  lcd.print("                "); 
  lcd.setCursor(1,0);
  lcd.print("TIMBRE TALLER");
  //Serial.println ("Timbre Recreo Taller Activo");
  //Serial.println ("Timbre Activo");
  delay(tiempo_timbre_recreo);
  digitalWrite(pinRELE2,LOW);
  lcd.setCursor(0,0);
  lcd.print("                "); 
}
void activar_timbre_limpieza(){
  digitalWrite (pinRELE2,HIGH);
  lcd.setCursor(0,0);
  lcd.print("                "); 
  lcd.setCursor(1,0);
  lcd.print("TIMBRE TALLER");
  //Serial.println("Timbre Limpieza Taller Activo");
  //Serial.println("Timbre Activo");
  delay(tiempo_timbre_limpieza);
  digitalWrite(pinRELE2,LOW);
  lcd.setCursor(0, 0);
  lcd.print("                "); 
}


//Condicional Horario Clases
void horario_escuela_aulas(){ 
   //lcd.setCursor(13,0);
  // lcd.print("1");
   //Serial.print("Horario Aulas");

   myRTC.updateTime();
   hora=myRTC.hours;
   minuto=myRTC.minutes;
   segundo=myRTC.seconds;
   
   //Inico Aula Turno Mañana
   if ((hora==h1_c1) && (minuto==m1_c1) && (segundo==s1_c1))  activar_timbre_ingreso_salida_aula();
   //if ((hora==h2_c1) && (minuto==m2_c1) && (segundo==s2_c1))  activar_timbre_ingreso_salida_aula();
   if ((hora==h3_c1) && (minuto==m3_c1) && (segundo==s3_c1))  activar_timbre_medio_modulo();
   //if ((hora==h4_c1) && (minuto==m4_c1) && (segundo==s4_c1))  activar_timbre_medio_modulo();
   if ((hora==h5_c1) && (minuto==m5_c1) && (segundo==s5_c1))  activar_timbre_recreo_aula();
   //if ((hora==h6_c1) && (minuto==m6_c1) && (segundo==s6_c1))  activar_timbre_recreo_aula();
   if ((hora==h7_c1) && (minuto==m7_c1) && (segundo==s7_c1))  activar_timbre_recreo_aula();
   //if ((hora==h8_c1) && (minuto==m8_c1) && (segundo==s8_c1))  activar_timbre_recreo_aula();
   if ((hora==h9_c1) && (minuto==m9_c1) && (segundo==s9_c1))  activar_timbre_medio_modulo();
   //if ((hora==h10_c1) && (minuto==m10_c1) && (segundo==s10_c1))  activar_timbre_medio_modulo();
   //if ((hora==h11_c1) && (minuto==m11_c1) && (segundo==s11_c1))  activar_timbre_recreo_aula();
   if ((hora==h12_c1) && (minuto==m12_c1) && (segundo==s12_c1))  activar_timbre_recreo_aula();
   if ((hora==h13_c1) && (minuto==m13_c1) && (segundo==s13_c1))  activar_timbre_recreo_aula();
   //if ((hora==h14_c1) && (minuto==m14_c1) && (segundo==s14_c1))  activar_timbre_recreo_aula();
   //if ((hora==h15_c1) && (minuto==m15_c1) && (segundo==s15_c1))  activar_timbre_medio_modulo();
   if ((hora==h16_c1) && (minuto==m16_c1) && (segundo==s16_c1))  activar_timbre_medio_modulo();   
   //if ((hora==h17_cl) && (minuto==m17_cl) && (segundo==s17_cl))  activar_timbre_ingreso_salida_aula();
   if ((hora==h18_cl) && (minuto==m18_cl) && (segundo==s18_cl))  activar_timbre_ingreso_salida_aula();
   //Inicio Aula Turno Tarde
   if ((hora==h19_c1) && (minuto==m19_c1) && (segundo==s19_c1))  activar_timbre_ingreso_salida_aula();
   //if ((hora==h20_c1) && (minuto==m20_c1) && (segundo==s20_c1))  activar_timbre_ingreso_salida_aula();
   if ((hora==h21_c1) && (minuto==m21_c1) && (segundo==s21_c1))  activar_timbre_medio_modulo();
   //if ((hora==h22_c1) && (minuto==m22_c1) && (segundo==s22_c1))  activar_timbre_medio_modulo();
   if ((hora==h23_c1) && (minuto==m23_c1) && (segundo==s23_c1))  activar_timbre_recreo_aula();
   //if ((hora==h24_c1) && (minuto==m24_c1) && (segundo==s24_c1))  activar_timbre_recreo_aula();
   if ((hora==h25_c1) && (minuto==m25_c1) && (segundo==s25_c1))  activar_timbre_recreo_aula();
   //if ((hora==h26_c1) && (minuto==m26_c1) && (segundo==s26_c1))  activar_timbre_recreo_aula();
   if ((hora==h27_c1) && (minuto==m27_c1) && (segundo==s27_c1))  activar_timbre_medio_modulo();
   //if ((hora==h28_c1) && (minuto==m28_c1) && (segundo==s28_c1))  activar_timbre_medio_modulo();
   //if ((hora==h29_c1) && (minuto==m29_c1) && (segundo==s29_c1))  activar_timbre_recreo_aula();
   if ((hora==h30_c1) && (minuto==m30_c1) && (segundo==s30_c1))  activar_timbre_recreo_aula();
   if ((hora==h31_c1) && (minuto==m31_c1) && (segundo==s31_c1))  activar_timbre_recreo_aula();
   //if ((hora==h32_c1) && (minuto==m32_c1) && (segundo==s32_c1))  activar_timbre_recreo_aula();
   //if ((hora==h33_c1) && (minuto==m33_c1) && (segundo==s33_c1))  activar_timbre_medio_modulo();
   if ((hora==h34_c1) && (minuto==m34_c1) && (segundo==s34_c1))  activar_timbre_medio_modulo();   
   //if ((hora==h35_cl) && (minuto==m35_cl) && (segundo==s35_cl))  activar_timbre_ingreso_salida_aula();
   if ((hora==h36_cl) && (minuto==m36_cl) && (segundo==s36_cl))  activar_timbre_ingreso_salida_aula();
}
void horario_escuela_taller(){ 
   //lcd.setCursor(12,0);
   //lcd.print("1");
   //Serial.print("Horario Taller");

   myRTC.updateTime();
   hora=myRTC.hours;
   minuto=myRTC.minutes;
   segundo=myRTC.seconds;
   
//Condicional Horario Taller Mañana
   if ((hora==h37_c1) && (minuto==m37_c1) && (segundo==s37_c1))  activar_timbre_ingreso_salida_taller();
   if ((hora==h38_c1) && (minuto==m38_c1) && (segundo==s38_c1))  activar_timbre_recreo_taller();
   if ((hora==h39_c1) && (minuto==m39_c1) && (segundo==s39_c1))  activar_timbre_recreo_taller();
   if ((hora==h40_c1) && (minuto==m40_c1) && (segundo==s40_c1))  activar_timbre_limpieza();
   if ((hora==h41_c1) && (minuto==m41_c1) && (segundo==s41_c1))  activar_timbre_ingreso_salida_taller();
//Condicional Horario Taller Tarde
   if ((hora==h42_c1) && (minuto==m42_c1) && (segundo==s42_c1))  activar_timbre_ingreso_salida_taller();
   if ((hora==h43_c1) && (minuto==m43_c1) && (segundo==s43_c1))  activar_timbre_recreo_taller();
   if ((hora==h44_c1) && (minuto==m44_c1) && (segundo==s44_c1))  activar_timbre_recreo_taller();
   if ((hora==h45_c1) && (minuto==m45_c1) && (segundo==s45_c1))  activar_timbre_limpieza();   
   if ((hora==h46_c1) && (minuto==m46_c1) && (segundo==s46_c1))  activar_timbre_ingreso_salida_taller();
}

        
