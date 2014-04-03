/*
 * Copyright (c) 2014 Fabio Olive Leite <fabio.olive@gmail.com>
 *
 * See the LICENSE file for details.
 *
 * Este eh um programa "monitor serial" interativo para o Arduino.
 * Ele envia um prompt pela serial e fica esperando que seja digitada
 * uma linha de comando, entao interpreta essa linha e a executa, agindo
 * sobre os pinos de I/O do Arduino. Tambem se pode agir sobre a SRAM.
 *
 * Pode ser usado com um programa de terminal serial, como o minicom,
 * ou mesmo por um programa desenvolvido pelo usuario, como por exemplo
 * um programa em python usando pyserial para interagir com o Arduino
 * e comandar os pinos de I/O.
 *
 * Este programa nao foi pensado para ser otimizado ou apresentar um
 * parser sofisticado. Na verdade ele eh bastante limitado para ser de
 * facil compreensao e adaptacao para outros projetos. O principal uso
 * deste programa na forma como se apresenta eh plugar o Arduino no
 * computador e usa-lo interativamente, ligando e desligando pinos para
 * interagir com outros circuitos manualmente.
 */

/* Buffer global que armazena a linha digitada. */
char linha[80];

void setup() {
  Serial.begin(9600);
  Serial.println("Monitor Arduino pronto.");
}

void ajuda() {
  Serial.println("Comandos reconhecidos:");
  Serial.println("o dX Y : output digital pino X valor Y");
  Serial.println("o pX Y : output PWM pino X valor Y");
  Serial.println("o tX Y : output tone pino X freq Y (freq 0 desliga)");
  Serial.println("i dX   : input digital pino X (imprime o valor)");
  Serial.println("i aX   : input analog pino X (imprime o valor)");
  Serial.println("d ENDh [LINHAS] : dump da SRAM em hexa, quantidade de LINHAS opcional");
  Serial.println("e ENDh BYTEh [BYTEh ...] : insere bytes na SRAM");
}

void prompt() {
  Serial.print("- ");
}

void le_linha() {
  register byte pos = 0;
  register char tecla = 0;

  /* Le e armazena os caracteres ate que seja digitado Enter. */
  while (tecla != '\r') {
    if (Serial.available() > 0) {
      tecla = Serial.read();
      switch(tecla) {
      case '\r':
      case '\n': /* Interpreta LF como CR tambem. */
        linha[pos] = '\0';
        Serial.println();
        tecla = '\r';
        break;
      case '\b': /* Backspace */
        if (pos) {
          pos--;
          Serial.print("\b "); /* "um back e um space" ;-) */
        } else
          tecla = '\a'; /* Beep se estava no inicio. */
        break;
      default:
        if (pos < 79)
          linha[pos++] = tecla;
        else
          tecla = '\a'; /* Beep se chegou no fim do buffer. */
      }
      Serial.write(tecla); /* Echo para o terminal serial. */
    }
  }
}

void output(char *param) {
  register byte pino;
  register int val;

  switch (*param++) {
  case 'd':
  case 'D': /* Digital out */
    pino = strtol(param, &param, 10);
    val = atoi(param);
    pinMode(pino, OUTPUT);
    digitalWrite(pino, val);
    break;

  case 'p':
  case 'P': /* PWM out */
    pino = strtol(param, &param, 10);
    val = atoi(param);
    pinMode(pino, OUTPUT);
    analogWrite(pino, val);
    break;

  case 't':
  case 'T': /* Tone out */
    pino = strtol(param, &param, 10);
    val = atoi(param);
    if (val)
      tone(pino, val);
    else
      noTone(pino);
    break;

  default: /* Desconhecido */
    ajuda();
  }
}

void input(char *param) {
  register byte pino;

  switch (*param++) {
  case 'd':
  case 'D': /* Digital in */
    pino = strtol(param, NULL, 10);
    pinMode(pino, INPUT);
    Serial.println(digitalRead(pino));
    break;

  case 'a':
  case 'A': /* Analog in */
    pino = strtol(param, NULL, 10);
    Serial.println(analogRead(pino));
    break;

  default: /* Desconhecido */
    ajuda();
  }
}

void print_hex_byte(byte b) {
  register byte lownib = b & 0x0f;
  b >>= 4;
  if (b > 9)
    Serial.write('A' + b - 10);
  else
    Serial.write('0' + b);
  if (lownib > 9)
    Serial.write('A' + lownib - 10);
  else
    Serial.write('0' + lownib);
}

void dump(char *param) {
  register byte *endereco;
  register byte i, b, linhas;

  /* Endereço em hexadecimal */
  endereco = (byte *)strtol(param, &param, 16);
  /* numero de linhas em decimal */
  linhas = atoi(param);
  if (linhas == 0)
    linhas = 1;

  while (linhas--) {
    print_hex_byte(highByte((int)endereco));
    print_hex_byte(lowByte((int)endereco));
    Serial.print(' ');

    /* 16 bytes de dados em hexa */
    for (i = 0; i < 8; i++) {
      Serial.print(' ');
      print_hex_byte(*endereco++);
    }
    Serial.print(':');
    for (i = 0; i < 8; i++) {
      print_hex_byte(*endereco++);
      Serial.print(' ');
    }
    Serial.print(' ');

    /* volta e imprime 16 bytes de dados em ASCII */
    endereco -= 16;
    for (i = 0; i < 16; i++) {
      b = (byte)*endereco++;
      if (b >= 32 && b <= 126)
        Serial.write(b);
      else
        Serial.write('.');
    }

    Serial.println();
  }
}

void enter(char *param) {
  register byte *endereco;
  register byte b;

  /* Endereço em hexadecimal */
  endereco = (byte *)strtol(param, &param, 16);
  /* Comeca a interpretar os bytes na linha e armazenar */
  while (*param) {
    b = (byte)strtol(param, &param, 16);
    *endereco++ = b;
    while (*param == ' ') param++; /* consome espaços extras */
    /* Tem horas que da vontade de reimplementar tudo com strtok :-) */
  }
}

void executa() {
  switch (linha[0]) {
  case 'o':
  case 'O':
    if (linha[1] == ' ')
      output(linha + 2);
    break;

  case 'i':
  case 'I':
    if (linha[1] == ' ')
      input(linha + 2);
    break;

  case 'd':
  case 'D':
    if (linha[1] == ' ')
      dump(linha + 2);
    break;

  case 'e':
  case 'E':
    if (linha[1] == ' ')
      enter(linha + 2);
    break;

  default: /* Desconhecido */
    ajuda();
  }
}

void loop() {
  prompt();
  le_linha();
  executa();
}

