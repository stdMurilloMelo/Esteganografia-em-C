# Projeto de Esteganografia

Este projeto implementa uma técnica de esteganografia em imagens PNG utilizando o método Least Significant Bit (LSB) em C. A biblioteca libpng é usada para manipulação e processamento das imagens.

## Sumário

- [Descrição](#descrição)
- [Requisitos](#requisitos)
- [Instalação](#instalação)
- [Uso](#uso)

## Descrição

A esteganografia é uma técnica utilizada para ocultar dados dentro de outros arquivos de forma que a sua presença não seja detectada visualmente. Neste projeto, dados são ocultados em imagens PNG alterando o bit menos significativo (LSB) de cada componente de cor dos pixels da imagem.

## Requisitos

- GCC (ou outro compilador C compatível)
- Biblioteca libpng

## Instalação

1. Clone o repositório para sua máquina local:
   ```sh
   git clone https://github.com/seu-usuario/seu-repositorio.git

2. Navegue até o diretório do projeto:
   ```sh
   cd seu-repositorio

3. Instale a libpng. Em sistemas Debian/Ubuntu, você pode usar:
   ```sh
   sudo apt-get install libpng-dev

4. Compile o projeto:
    ```sh
    gcc -o <NomeDoPrograma> *.c -lpng

## Uso
Para ocultar dados em uma imagem (arquivo de saída é opcional por padrão é gerado um arquivo chamado "output.png"):
```sh
./<NomeDoPrograma> -e <arquivo_png> <messagem|arquivo_texto> [<arquivo_saida_png>]
```

Para extrair dados de uma imagem (arquivo de saída é opcional por padrão a messagem é printada no terminal):
```sh
./<NomeDoPrograma> -d <arquivo_png> [<arquivo_saida_texto>]
```
