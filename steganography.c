#include "steganography.h"

// Função para ler o arquivo de texto
char *read_text_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Erro ao abrir o arquivo de texto.\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *content = (char *)malloc((file_size + 1) * sizeof(char));
    if (!content) {
        fprintf(stderr, "Erro ao alocar memória para o conteúdo do arquivo.\n");
        fclose(file);
        return NULL;
    }

    fread(content, sizeof(char), file_size, file);
    content[file_size] = '\0';

    fclose(file);
    return content;
}

// Função para escrever o arquivo de texto
void write_text_file(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Erro ao abrir o arquivo de saída.\n");
        return;
    }

    fprintf(file, "%s", content);
    fclose(file);
}

// Função para ler o arquivo PNG
void read_png_file(char *file_name, png_structp png, png_infop info,
                   png_bytep **row_pointers) {
    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        fprintf(stderr, "Falha ao abrir o arquivo %s para leitura.\n",
                file_name);
        abort();
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    png_byte color_type = png_get_color_type(png, info);
    if (color_type == PNG_COLOR_TYPE_PALETTE ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_palette_to_rgb(png);
        png_read_update_info(png, info);
    }

    unsigned int height = png_get_image_height(png, info);
    *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
    if (!*row_pointers) {
        fprintf(stderr, "Falha ao alocar memória para os ponteiros para as "
                        "linhas do arquivo.\n");
        abort();
    }

    for (unsigned int y = 0; y < height; y++) {
        (*row_pointers)[y] = (png_byte *)malloc(png_get_rowbytes(png, info));
        if (!(*row_pointers)[y]) {
            fprintf(stderr, "Falha ao alocar memória para a linha %d.\n", y);
            abort();
        }
    }

    png_read_image(png, *row_pointers);
    fclose(fp);
}

// Função para escrever o arquivo PNG
void write_png_file(char *file_name, png_structp png, png_infop info,
                    png_bytep *row_pointers) {
    FILE *fp = fopen(file_name, "wb");
    if (!fp) {
        fprintf(stderr, "Falha ao abrir o arquivo %s para escrita.\n",
                file_name);
        abort();
    }

    png_init_io(png, fp);
    png_write_info(png, info);
    png_write_image(png, row_pointers);
    png_write_end(png, NULL);

    if (png != NULL)
        png_destroy_write_struct(&png, (png_infopp)NULL);
    if (info != NULL)
        png_free_data(png, info, PNG_FREE_ALL, -1);
    if (fp != NULL)
        fclose(fp);
}

// Função para codificar a mensagem
void encode_message(png_bytep *byte, char *message, unsigned int message_length,
                    unsigned int width, unsigned int height) {
    if (message_length > (height * width - 32)) {
        fprintf(stderr,
                "A mensagem é muito grande para ser codificada na imagem.\n");
        abort();
    }
    unsigned int i = 0, j = 0, num_bits_msg_len = 32;
    while (num_bits_msg_len--) {
        byte[i][j] =
            (byte[i][j] & 0xFE) | ((message_length >> num_bits_msg_len) & 0x01);
        j++;
        if (j == width) {
            i++;
            j = 0;
        }
    }
    unsigned int num_bits_msg = message_length * 8;
    while (num_bits_msg--) {
        byte[i][j] = (byte[i][j] & 0xFE) |
                     ((message[num_bits_msg / 8] >> (num_bits_msg % 8)) & 0x01);
        j++;
        if (j == width) {
            i++;
            j = 0;
        }
    }
}

// Função para decodificar a mensagem
DecodedMessage decode_message(png_bytep *byte, char *message,
                              unsigned int width, unsigned int height) {
    unsigned int message_length = 0;
    unsigned int i = 0, j = 0, num_bits_msg_len = 32;
    while (num_bits_msg_len--) {
        message_length = (message_length << 1) | (byte[i][j] & 0x01);
        j++;
        if (j == width) {
            i++;
            j = 0;
        }
    }

    message = (char *)malloc(sizeof(char) * (message_length + 1));
    if (!message) {
        fprintf(stderr, "Falha ao alocar memória para a mensagem.\n");
        abort();
    }
    message[message_length] = '\0';

    unsigned int num_bits_msg = message_length * 8;
    while (num_bits_msg--) {
        message[num_bits_msg / 8] =
            (message[num_bits_msg / 8] << 1) | (byte[i][j] & 0x01);
        j++;
        if (j == width) {
            i++;
            j = 0;
        }
    }

    DecodedMessage result = {message, message_length};
    return result;
}
