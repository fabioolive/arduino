/*
 * Copyright (c) 2014 Fabio Olive Leite <fabio.olive@gmail.com>
 *
 * See the LICENSE file for details.
 *
 * Este e um pequeno exercicio de configurar e manipular os pinos
 * do ATmega328P sem utilizar as funcoes normais do Arduino, como
 * pinMode() e digitalWrite(), escrevendo direto nos registradores
 * de I/O mapeados em RAM.
 *
 * Ligue LEDs nos pinos 8, 9, 10, 11, 12 e 13, com o anodo no pino
 * e o catodo indo pro terra atraves de um resistor de 1k. Eles vao
 * acender conforme a contagem em binario de 0 a 63 (6 bits).
 *
 * Usando pinMode() e digitalWrite() o programa fica com 1104 bytes,
 * e escrevendo direto nos registradores cai pra 620 bytes. :-)
 */

/* O endereco 0x24 eh o DDRB (Port B Data Direction Register) */
#define DDRB  (*((byte *)0x24))
/* O endereco 0x25 eh o PORTB (Port B Data Register) */
#define PORTB (*((byte *)0x25))

void setup() {
/*pinMode( 8, OUTPUT);
  pinMode( 9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);*/
  DDRB = 0x3f;
}

void loop() {
  for (byte b = 0; b < 64; b++) {
  /*digitalWrite( 8, b &  1);
    digitalWrite( 9, b &  2);
    digitalWrite(10, b &  4);
    digitalWrite(11, b &  8);
    digitalWrite(12, b & 16);
    digitalWrite(13, b & 32);*/
    PORTB = b;
    delay(500);
  }
}

