#include "converte.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *abreArq(char *nome, char *modo) {
  FILE *file = fopen(nome, modo);
  if (file == NULL) {
    printf("erro ao abrir o arquivo para ler");
    exit(1);
  }
  return file;
}

void fecharArquivo(FILE *arquivo) {
  if (arquivo != NULL) {
    fclose(arquivo);
  } else {
    printf("erro ao fechar o arquivo");
    exit(1);
  }
}

int main() {
  // testando se os arquivos abrem corretamente
  printf("começamos\n");
  FILE *utf_leitura = abreArq("utf8_demo.txt", "rb");
  FILE *varint_leitura = abreArq("var_demo", "rb");
  FILE *utf_saida = abreArq("utf8_saida.txt", "wb");
  FILE *varint_saida = abreArq("varint_saida", "wb");
  printf("arquivos abertos, vou começar a conversão\n");
  // criamos o arquivo com as funções
  utf2varint(utf_leitura, varint_saida);
  printf("utf2varint realizado com sucesso\n");
  varint2utf(varint_leitura, utf_saida);
  printf("varint2utf realizado com sucesso\n");
  printf("conversão feita\n");
  // Fechamos os arquivos e reabrimos para voltarmos ao começo
  fecharArquivo(utf_leitura);
  fecharArquivo(varint_leitura);
  fecharArquivo(utf_saida);
  fecharArquivo(varint_saida);
  printf("arquivos fechados\n");

  utf_leitura = abreArq("utf8_demo.txt", "rb");
  varint_leitura = abreArq("var_demo", "rb");
  utf_saida = abreArq("utf8_saida.txt", "rb");
  varint_saida = abreArq("varint_saida", "rb");

  int byteUtfOriginal, byteUtfNovo, byteVarOriginal, byteVarNovo;

  while (1) { // teste para comparar os arquivos UTF 8 original e o convertido
    byteUtfOriginal = fgetc(utf_leitura);
    byteUtfNovo = fgetc(utf_saida);

    if (byteUtfOriginal == EOF && byteUtfNovo == EOF) {
      printf("Os arquivos são iguais.\n");
      break;
    } else if (byteUtfOriginal != byteUtfNovo || byteUtfOriginal == EOF || byteUtfNovo == EOF) {
      printf("Os arquivos não estão iguais.\n");
      break;
    }
  }

  while (1) { // teste para comparar os arquivos Varint original e o convertido
    byteVarOriginal = fgetc(varint_leitura);
    byteVarNovo = fgetc(varint_saida);

    if (byteVarOriginal == EOF && byteVarNovo == EOF) {
      printf("Os arquivos são iguais.\n");
      break;
    } else if (byteVarOriginal != byteVarNovo || byteVarOriginal == EOF || byteVarNovo == EOF) {
      printf("Os arquivos não estão iguais.\n");
      break;
    }
  }

  // testando se os arquivos são fechados corretamente
  fecharArquivo(utf_leitura);
  fecharArquivo(varint_leitura);
  fecharArquivo(utf_saida);
  fecharArquivo(varint_saida);

  return 0;
}
