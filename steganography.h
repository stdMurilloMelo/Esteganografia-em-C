#ifndef STEGANOGRAPHY_H
#define STEGANOGRAPHY_H

#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura para armazenar a mensagem decodificada
typedef struct {
    char *message;
    unsigned int length;
} DecodedMessage;

// Função para ler o arquivo de texto
char *read_text_file(const char *filename);

// Função para escrever o arquivo de texto
void write_text_file(const char *filename, const char *content);

// Função para ler o arquivo PNG
void read_png_file(char *file_name, png_structp png, png_infop info,
                   png_bytep **row_pointers);

// Função para escrever o arquivo PNG
void write_png_file(char *file_name, png_structp png, png_infop info,
                    png_bytep *row_pointers);

// Função para codificar a mensagem
void encode_message(png_bytep *byte, char *message, unsigned int message_length,
                    unsigned int width, unsigned int heigth);

// Função para decodificar a mensagem
DecodedMessage decode_message(png_bytep *byte, char *message,
                              unsigned int width, unsigned int height);

#endif
