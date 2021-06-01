# TurtleSitter

## Introducere
Proiectul este un sistem de control al mediului in care traieste o broasca testoasa. 
Este util deoarece, in cazul in care nu ne aflam acasa, broasca va fi hranita automat, la un interval bine stabilit.
Contine un dozator de mancare activat de un timer (automat) sau de pe telefonul mobil, folosind un modul bluetooth. Mai contine si un senzor de temperatura, ce trimite datele catre telefon si alerteaza daca temperatura nu este conforma. Informatiile vor fi afisate si pe un modul lcd.

## Descriere Generala
Servomotorul va fi conectat la placa. La un interval stabilit, sau la o comanda primita din exterior, se va activa si va roti un tub gaurit in care se afla mancarea, dupa care va reveni in pozitia initiala. In paralel, senzorul de temperatura waterproof va masura temperatura apei la un interval stabilit si o va transmite prin modul.

## Hardware Design
**Lista de componente**

  * Arduino UNO
  * Modul bluetooth HC-05
  * Senzor Temperatură Rezistent la Apă DS18B20
  * Servomotor de Metal 12g
  * Ecran OLED 0.9"
  * Breadboard
  * Fire mama-tata + tata-tata
  * Sursa de alimentare
  * Tub de plastic in care se depoziteaza mancarea

## Software Design

   * Mediu de dezvoltare: Arduino IDE
   * Biblioteci folosite: SoftwareSerial, Servo, OneWire, DallasTemperature, U8glib
   * Functionarea programului:
     * Se calculeaza nr de milisecunde trecute de la pornire
     * Daca s-a depasit timerul, va porni servomotorul si timerul se va reseta
     * Servomotorul face o miscare de 180 de grade inainte si inapoi, cu viteza de un grad/0.015s.
     * La apasarea butonului de hranire, se va porni servomotorul si se va reseta timerul. Se va transmite mesajul si la ecranul LCD.
     * La apasarea butonului de schimbare de timer, se va astepta dupa un numar de 2 cifre, reprezentand ore. Se va transmite mesajul si la ecranul LCD.
     * In fiecare secunda se masoara temperatura si se transmite prin bluetooth. Aceasta se afiseaza pe telefon si pe LCD. Daca nu se afla in intervalul optim, se alerteaza folosind unul din cele 2 leduri.
