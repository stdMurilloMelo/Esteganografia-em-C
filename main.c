/*
Ciências da Computação (UFG)
Murillo Pires Teixeira Melo (202300074)
Trabalho sobre Esteganografia (AED2) 2024.1
Prof.: Ronaldo Martins da Costa
-----------------------------------------------------------------------------------------------------------------

Este trabalho consiste em um programa que codifica uma mensagem em um arquivo
PNG utilizando esteganografia.

O programa lê um arquivo PNG e uma mensagem a ser codificada, e então codifica a
mensagem nas linhas da imagem.

Para codificar uma mensagem, execute o programa com a opção -e (encode), seguida
do nome do arquivo PNG e da mensagem a ser codificada.

É possível também decodificar uma mensagem de um arquivo PNG.
Para decodificar uma mensagem, execute o programa com a opção -d (decode),
seguida do nome do arquivo PNG.

Exemplo de uso:
// Codifica a mensagem (arquivo de saída por padrão é output.png)
./<NomeDoPrograma> -e <arquivo_png> <messagem|arquivo_texto>
[<arquivo_saida_png>]
// Decodifica a mensagem
./<NomeDoPrograma> -d <arquivo_png> [<arquivo_saida_texto>]

Este programa utiliza a biblioteca libpng para manipulação de arquivos PNG.
Para instalar a biblioteca no Ubuntu, execute o comando: sudo apt-get install
libpng-dev

Para compilar o programa não esqueça da flag para linkar a libpng:
clang (ou gcc) -o NomeDoPrograma *.c -lpng
*/

#include "steganography.h"

// Função principal
int main(int argc, char **argv) {

    // Cria a estrutura de leitura do arquivo PNG
    png_structp png =
        png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png) {
        fprintf(stderr, "Falha ao criar a estrutura de leitura do PNG.\n");
        return 1;
    }

    // Cria a estrutura de informações do arquivo PNG
    png_infop info = png_create_info_struct(png);

    if (!info) {
        fprintf(stderr, "Falha ao criar a estrutura de informações do PNG.\n");
        png_destroy_read_struct(&png, NULL, NULL);
        return 1;
    }

    // Verifica se houve algum erro durante a leitura do arquivo PNG
    if (setjmp(png_jmpbuf(png))) {
        fprintf(stderr, "Erro durante a criação do PNG.\n");
        png_destroy_read_struct(&png, &info, NULL);
        return 1;
    }

    // Ponteiro para as linhas da imagem
    png_bytep *png_pixel_bytes;

    // Lê o arquivo PNG
    read_png_file(argv[2], png, info, &png_pixel_bytes);
    unsigned int height = png_get_image_height(png, info);
    unsigned int width = png_get_image_width(png, info);

    // Verifica se a opção é de codificação
    if (strcmp(argv[1], "-e") == 0) {
        char *message;
        unsigned int message_length;

        // Verifica se o terceiro argumento é um arquivo de texto
        FILE *test_file = fopen(argv[3], "r");
        if (test_file) {
            fclose(test_file);
            message = read_text_file(argv[3]);
            if (!message) {
                png_destroy_read_struct(&png, &info, NULL);
                return 1;
            }
        } else {
            // Se não for um arquivo de texto, assume que é uma string passada
            // diretamente
            message = argv[3];
        }

        message_length = strlen(message);

        // Codifica a mensagem nas linhas da imagem
        encode_message(png_pixel_bytes, message, message_length, height, width);

        // Escreve o arquivo PNG com a mensagem codificada
        png_structp png_out =
            png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_out) {
            fprintf(stderr, "Falha ao criar a estrutura de escrita do PNG.\n");
            png_destroy_read_struct(&png, &info, NULL);
            if (test_file) {
                free(message);
            }
            return 1;
        }

        char *output_file = argc > 4 ? argv[4] : "output.png";
        write_png_file(output_file, png_out, info, png_pixel_bytes);

        printf("Tamanho da mensagem: %d\nMensagem codificada com sucesso!\n",
               message_length);

        if (test_file) {
            free(message);
        }
    }

    // Verifica se a opção é de decodificação
    else if (strcmp(argv[1], "-d") == 0) {
        unsigned int height = png_get_image_height(png, info);
        char *decoded_message = NULL;
        unsigned int message_length = 0;

        // Decodifica a mensagem das linhas da imagem
        DecodedMessage result =
            decode_message(png_pixel_bytes, decoded_message, width, height);
        decoded_message = result.message;

        // Verifica se a mensagem foi decodificada com sucesso
        if (decoded_message != NULL) {
            printf("Tamanho da mensagem: %d\n", result.length);

            // Verifica se um arquivo de saída foi especificado
            if (argc > 3) {
                write_text_file(argv[3], decoded_message);
                printf("Mensagem escrita no arquivo: %s\n", argv[3]);
            } else {
                printf("Mensagem decodificada:\n %s\n", decoded_message);
            }
        } else {
            printf("Falha ao decodificar a mensagem\n");
        }
    } else {
        fprintf(
            stderr,
            "ERRO\n"
            "Codifica a mensagem (arquivo de saída por padrão é output.png):\n"
            "./<NomeDoPrograma> -e <arquivo_png> <messagem|arquivo_texto> "
            "[<arquivo_saida_png>]\n"
            "Decodifica a mensagem:\n"
            "./<NomeDoPrograma> -d <arquivo_png>\n");
    }

    // Cleanup
    png_destroy_read_struct(&png, &info, NULL);
    return 0;
}
