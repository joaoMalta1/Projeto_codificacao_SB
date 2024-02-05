/* Vinicius Machado da Rocha Viana 2111343 3WA */
/* Joao Ricardo Malta de Oliveira  2112714 3WA */

#include "converte.h"
#include <stdio.h>
#include <stdlib.h>

// verifica o numero de bytes do caractere UTF-8
unsigned int verificaBytesUTF(unsigned char c) {
  if ((c & 0x80) == 0) // verifica se o bit mais significativo do byte eh igual a 0
  {
    return 1; 
  } 
  else if ((c & 0xE0) == 0xC0) //verifica se os 3 bits mais significativos do byte sao igual a 110
  {
    return 2; 
  } 
  else if ((c & 0xF0) == 0xE0) //verifica se os 4 bits mais significativos do byte sao igual a 1110
  {
    return 3;
  } 
  else if ((c & 0xF8) == 0xF0) //verifica se os cinco bits mais significativos do byte sao igual a 11110
  {
    return 4;
  }
  return 0; // Caractere inválido
}


int utf2varint(FILE *arq_entrada, FILE *arq_saida) {
  unsigned char c;
  if (!arq_entrada) {
    fputs("Erro na abertura do arquivo de entrada", stderr);
    return -1;
  }
  if (!arq_saida) {
    fputs("Erro no arquivo que será escrito", stderr);
    return -1;
  }

  int resultado = 1;
  while (resultado) {
    resultado = fread(&c, 1, 1, arq_entrada);

    // verifica o numero de bytes do caracter
    if (verificaBytesUTF(c) == 1) {
      // caracter de 1 byte, diretamente para varin
      fwrite(&c, 1, 1, arq_saida);
    } 
    else if (verificaBytesUTF(c) == 2) {
      unsigned char d;
      resultado = fread(&d, 1, 1, arq_entrada);
      if (!resultado) {
        break; 
      }
      // converte o caracter de 2 bytes para varint
      unsigned char varint[2];

      varint[0] = (c & 0x1F) | 0xC0; // isola os 5 bits menos significativos de c e configura o bit mais significativo para 1 e o segundo para 1

      varint[1] = (d & 0x3F) | 0x80; //isola os 6 bits menos significativos de d e configura o bit mais significativo (MSB) para 1
      fwrite(varint, 1, 2, arq_saida);
    } 
    else if (verificaBytesUTF(c) == 3) 
    {
      unsigned char d[2];
      resultado = fread(d, 1, 2, arq_entrada);
      if (resultado == 0) 
      {
        break; 
      }
      // Converte o caractere de 3 bytes para varint
      unsigned char varint[3];

      varint[0] = (c & 0x0F) | 0xE0; //isola os 4 bits menos significativos de c e configura os 3 bits mais significativos para 1110

      varint[1] = (d[0] & 0x3F) | 0x80; //isola os 6 bits menos significativos de d[0] e  configura o bit mais significativo (MSB) para 1
      
      varint[2] = (d[1] & 0x3F) | 0x80; //(d[1] & 0x3F) isola os 6 bits menos significativos de d[1] e configura o bit mais significativo (MSB) para 1
      
      fwrite(varint, 1, 3, arq_saida);
    } 
    else if (verificaBytesUTF(c) == 4) 
    {
      unsigned char d[3];
      resultado = fread(d, 1, 3, arq_entrada);
      if (resultado == 0) 
      {
        break; 
      }
      // converte o caracter de 4 bytes para varint
      unsigned char varint[4];

      varint[0] = (c & 0x07) | 0xF0; //isola os 3 bits menos significativos de c e configura os quatro bits mais significativos para 1111

      varint[1] = (d[0] & 0x3F) | 0x80; // isola os 6 bits menos significativos de d[0] e configura o bit mais significativo

      varint[2] = (d[1] & 0x3F) | 0x80;  //isola os 6 bits menos significativos de d[1] e configura o bit mais significativo (MSB) para 1
      
      varint[3] = (d[2] & 0x3F) | 0x80; //isola os 6 bits menos significativos de d[2] e configura o bit mais significativo (MSB) para 1
      fwrite(varint, 1, 4, arq_saida);
    }
  }
  return 0;
   // "configurar o bit mais significativo" significa que estamos definindo o valor desse bit 
}


int varint2utf(FILE *arq_entrada, FILE *arq_saida) {
  if (!arq_entrada) {
    fputs("Erro de leitura", stderr);
    return -1;
  }
  if (!arq_saida) {
    fputs("Erro na gravação", stderr);
    return -1;
  }
  char n;
  int bytesLidos = 0;
  while (fread(&n, 1, 1, arq_entrada)) {
    int ch = n & 0x7F; // extrai 7 bits de dados do byte
    bytesLidos = 1;
    while ((n >> 7) & 1) {
      if (!fread(&n, 1, 1, arq_entrada)) {
        fputs("Erro de leitura", stderr);
        return -1;
      }
      
      ch = ch | (n & 0x7F) << (7 * bytesLidos); // combine os bits de dados do byte no valor ch. Shift de 7 * bytesLidos para a esquerda.
      bytesLidos++;
    }

    if (ch < 0x7F) 
    {
      fputc(ch, arq_saida); // escreve o valor como um unico 
    } else if (ch < 0x7FF) 
    {
      fputc(0xC0 | (ch >> 6), arq_saida);// escreve o primeiro byte dos 2 bytes (0xC0 indica inicio de um caracter de 2bytes e 0x80 mascara os 6 bits de dados)
      
      fputc(0x80 | (ch & 0x3F), arq_saida); // escreve o segundo byte dos 2 bytes 
    } 
    else if (ch < 0xFFFF) 
    {
      fputc(0xE0 | (ch >> 12), arq_saida); // escreve o primeiro byte dos 3 bytes (0xE0 indica inicio de um caracter de 3bytes e 0x80mascara os 4 bits de dados)
      
      fputc(0x80 | ((ch >> 6) & 0x3F), arq_saida); // escreve o segundo byte dos 3 bytes 
     
      fputc(0x80 | (ch & 0x3F), arq_saida);  // escreve o terceiro byte dos 3 bytes 
    } else {
      
      fputc(0xF0 | (ch >> 18), arq_saida); // escreve o primeiro byte dos 4 bytes (0xF0 indica inicio de um caracter de 4bytes e 0x80mascara os 3 bits de dados)
      
      fputc(0x80 | ((ch >> 12) & 0x3F), arq_saida);// escreve o segundo byte dos 4 bytes 
      
      fputc(0x80 | ((ch >> 6) & 0x3F), arq_saida);// escreve o terceiro byte dos 4 bytes 
      
      fputc(0x80 | (ch & 0x3F), arq_saida); // escreve o quarto byte dos 4 bytes 
    }
  }
  return 0; // Retorne 0 para indicar que a conversão foi bem-sucedida

  // usamos 0x80  como uma mascara para definir o bit mais significativo do byte de continuacao em UTF-8 esse bit sempre deve ser definido como 1 em bytes de continuacao.

// & com 0x3F  mascara os bits de dados, garantindo que qualquer bit fora desses 6 bits seja zerado.
}



