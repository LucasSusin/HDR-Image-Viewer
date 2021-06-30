// Lucas Simon Susin e Guilherme Paris

#include <math.h>
#include <string.h>		// para usar strings

// Rotinas para acesso da OpenGL
#include "opengl.h"

// Protótipos
void process();
void carregaHeader(FILE* fp);
void carregaImagem(FILE* fp, int largura, int altura);
void criaImagensTeste();

// Variáveis globais a serem utilizadas (NÃO ALTERAR!)

int minhaLargura, minhaAltura;

// Dimensões da imagem de entrada
int sizeX, sizeY;

// Header da imagem de entrada
unsigned char header[11];

// Pixels da imagem de ENTRADA (em formato RGBE)
unsigned char* image;

// Pixels da imagem de SAÍDA (em formato RGB)
unsigned char* image8;

// Fator de exposição
float exposure;

// Histogramas
float histogram[HISTSIZE];
float adjusted[HISTSIZE];

// Flag para exibir/ocultar histogramas
unsigned char showhist = 0;

// Níveis mínimo/máximo (preto/branco)
int minLevel = 0;
int maxLevel = 255;

// Função principal de processamento: ela deve chamar outras funções
// quando for necessário (ex: algoritmo de tone mapping, etc)
void process()
{

    printf("Exposure: %.3f\n", exposure);
    float expos = pow(2,exposure);

    float *fpixels = malloc(sizeX * sizeY * 3 * sizeof(float));
    float *copia = fpixels;
    float *copiaDois = fpixels;
    float *copiaTres = fpixels;
    float *copiaQuatro = fpixels;
    float *copiaCinco = fpixels;
    int posicaoVet = 0;
    int posHist = 0;

    /*
        Conversão pixel
    */

    unsigned char* ptr = image;
    ptr -= 1;
    unsigned char* tmp = image;

    for(int pos=0; pos<sizeX * sizeY * 4; pos+=4) { // sizeX * sizeY * 4
        ptr+=4;
        float exp = *ptr - 136;
        // printf("%d\n", *ptr); // ver se está pegando a mantissa
        float m = pow(2,exp);
        // printf("%f\n", m);  // ver se está calculando correto

        // printf("%d\n", *tmp); // número antes
        float um = *tmp * m;
        // printf("%f\n", um); // número depois
        tmp++;
        fpixels[posicaoVet] = um;
        posicaoVet++;
        
        float dois = *tmp * m;
        tmp++;
        fpixels[posicaoVet] = dois;
        posicaoVet++;
        
        float tres = *tmp * m;
        tmp++;
        fpixels[posicaoVet] = tres;
        posicaoVet++;

        tmp++;
    }

    // visualizar vetor novo de floats
    // for(int pos=0; pos<20; pos++) {
    //     printf("%f \n", fpixels[pos]);
    // }
    
    /*
        Fator exposição
    */

    posicaoVet = 0;
    for(int pos=0; pos<sizeX * sizeY * 3; pos+=3) { // sizeX * sizeY * 3

        float um = expos * *copiaDois;
        copiaDois++;
        fpixels[posicaoVet] = um;
        posicaoVet++;
        
        float dois = expos * *copiaDois;
        copiaDois++;
        fpixels[posicaoVet] = dois;
        posicaoVet++;

        float tres = expos * *copiaDois;
        copiaDois++;
        fpixels[posicaoVet] = tres;
        posicaoVet++;
    }

    /*
        Tone mapping
    */

    posicaoVet = 0;
    for (int i = 0; i < sizeX * sizeY * 3; i+=3) { // sizeX * sizeY * 3

        float um = *copiaTres * 0.6;
        float ump = (um * ((2.51 * um) + 0.03)) / (((um * ((2.43 * um) + 0.59)) + 0.14));
        if(ump > 1) {
            ump = 1;
        }
        if(ump < 0) {
            ump = 0;
        }
        copiaTres++;
        fpixels[posicaoVet] = ump;
        posicaoVet++;
        
        float dois = *copiaTres * 0.6;
        float doisp = (dois * ((2.51 * dois) + 0.03)) / (((dois * ((2.43 * dois) + 0.59)) + 0.14));
        if(doisp > 1) {
            doisp = 1;
        }
        if(doisp < 0) {
            doisp = 0;
        }
        copiaTres++;
        fpixels[posicaoVet] = doisp;
        posicaoVet++;

        float tres = *copiaTres * 0.6;
        float tresp = (tres * ((2.51 * tres) + 0.03)) / (((tres * ((2.43 * tres) + 0.59)) + 0.14));
        if(tresp > 1) {
            tresp = 1;
        }
        if(tresp < 0) {
            tresp = 0;
        }
        copiaTres++;
        fpixels[posicaoVet] = tresp;
        posicaoVet++;
    }

    /*
        Correção gama
    */

    float y = 1 / 2.2;
    posicaoVet = 0;
    for (int i = 0; i < sizeX * sizeY * 3; i+=3) { // sizeX * sizeY * 3
        // *x = pow(*x, y);
        // x++;

        float um = pow(*copiaQuatro, y);
        copiaQuatro++;
        fpixels[posicaoVet] = um;
        posicaoVet++;
        
        float dois = pow(*copiaQuatro, y);
        copiaQuatro++;
        fpixels[posicaoVet] = dois;
        posicaoVet++;

        float tres = pow(*copiaQuatro, y);
        copiaQuatro++;
        fpixels[posicaoVet] = tres;
        posicaoVet++;
    }

    /*
        Conversão 24 bits
    */

    posicaoVet = 0;
    unsigned char* img = image8;
    for (int i = 0; i < sizeX * sizeY * 3; i+=3) { // sizeX * sizeY * 3
        
        int um = *copiaCinco * 255;
        copiaCinco++;
        *img = um;
        img++;
        
        int dois = *copiaCinco * 255;
        copiaCinco++;
        *img = dois;
        img++;

        int tres = *copiaCinco * 255;
        copiaCinco++;
        *img = tres;
        img++;
    }

    /*
        Histograma
    */

    // inicializar o histograma com zeros
    // for (int i = 0; i < HISTSIZE; i++) {
    //     histogram[i] = 0.0f;
    // }

    // visualizar histograma
    // for (int i = 0; i < HISTSIZE; i++) {
    //     printf("%f \n",histogram[i]);
    // }

    unsigned char* imagem = image8;
    for (int i = 0; i < sizeX * sizeY * 3; i+=3) { // sizeX * sizeY * 3
        
        float r = (*imagem * 0.299);
        imagem++;
        float g = r + (*imagem * 0.587);
        imagem++;
        float b = g + (*imagem * 0.114);
        int rr = (int) b;
        imagem++;
        // printf("%d\n", rr);

        for (int j = 0; j < HISTSIZE; j++) {
            if(rr == j) {
                histogram[j] = histogram[j] + 1;
            }
        }
    }

    // normalizar
    float max = histogram[0];
    for (int i = 0; i < HISTSIZE; i++) {
        if (histogram[i] > max) {
            max = histogram[i];
        }
    }
    for (int i = 0; i < HISTSIZE; i++) {
        // printf("maximo: %f\n", max);
        // printf("antes: %f \n", histogram[i]);
        float r = histogram[i] / max;
        // printf("depois: %f \n", r);
        histogram[i] = r;
    }

    // for (int i = 0; i < HISTSIZE; i++) {
    //     printf("%f \n",histogram[i]);
    // }

    /*
        Ajuste dos níveis de preto e branco
    */

    // inicializar o histograma com zeros
    // for (int i = 0; i < HISTSIZE; i++) {
    //     adjusted[i] = 0.0f;
    // }

    unsigned char* imagemAntigo = image8;
    unsigned char* imagemTroca = image8;
    for (int i = 0; i < sizeX * sizeY * 3; i+=3) { // sizeX * sizeY * 3
        
        int antigoR = *imagemAntigo;
        imagemAntigo++;
        int antigoG = *imagemAntigo;
        imagemAntigo++;
        int antigoB = *imagemAntigo;
        imagemAntigo++;

        float r = (antigoR * 0.299);
        float g = r + (antigoG * 0.587);
        float b = g + (antigoB * 0.114);
        int intensidadeAntiga = b;

        float max = intensidadeAntiga - minLevel;
        if(max < 0.0f) {
            max = 0.0f;
        }

        float nova = max / (maxLevel - minLevel);
        if(nova > 1.0f) {
            nova = 1.0f;
        }

        int novaFinal = (int) (nova * 255.0f);

        int recebe = (int) ((antigoR * novaFinal) / intensidadeAntiga);
        *imagemTroca = recebe;
        imagemTroca++;

        recebe = (int) ((antigoG * novaFinal) / intensidadeAntiga);
        *imagemTroca = recebe;
        imagemTroca++;

        recebe = (int) ((antigoB * novaFinal) / intensidadeAntiga);
        *imagemTroca = recebe;
        imagemTroca++;
        

        for (int j = 0; j < HISTSIZE; j++) {
            if(novaFinal == j) {
                adjusted[j] = adjusted[j] + 1;
            }
        }
    }

    // normalizar
    float maxNovo = adjusted[0];
    for (int i = 0; i < HISTSIZE; i++) {
        
        if (adjusted[i] > maxNovo) {
            maxNovo = adjusted[i];
        }
    }
    for (int i = 0; i < HISTSIZE; i++) {
        float r = adjusted[i] / maxNovo;
        adjusted[i] = r;
    }

    // for (int i = 0; i < HISTSIZE; i++) {
    //     printf("%f \n",adjusted[i]);
    // }

    // NÃO ALTERAR A PARTIR DAQUI!!!!
    free(fpixels);
    buildTex();
}

int main(int argc, char** argv)
{
    if(argc==1) {
        printf("hdrvis [image file.hdf]\n");
        exit(1);
    }

    // Inicialização da janela gráfica
    init(argc,argv);

    //
    // PASSO 1: Leitura da imagem
    // A leitura do header já foi feita abaixo
    // 
    FILE* arq = fopen(argv[1], "rb");
    carregaHeader(arq);

    // Printando o header para visualizá-lo
    // printf("-----\n");
    // for (int i = 0; i < 11; i++)
    // {
    //     printf(":%d ", header[i]);
    // }
    // printf("-----\n");

    int minhaLargura = ((pow(256, 0) * header[3]) + (pow(256, 1) * header[4]) + (pow(256, 2) * header[5]) + (pow(256, 3) * header[6])); 
    int minhaAltura = ((pow(256, 0) * header[7]) + (pow(256, 1) * header[8]) + (pow(256, 2) * header[9]) + (pow(256, 3) * header[10]));
    
    // Printando largura e altura
    // printf("%d \n",minhaLargura);
    // printf("%d \n",minhaAltura);

    // IMPLEMENTE AQUI o código necessário para
    // extrair as informações de largura e altura do header
    // Descomente a linha abaixo APENAS quando isso estiver funcionando!
    carregaImagem(arq, minhaLargura, minhaAltura);

    // Fecha o arquivo
    fclose(arq);


    // COMENTE a linha abaixo quando a leitura estiver funcionando!
    // (caso contrário, ele irá sobrepor a imagem carregada com a imagem de teste)
    // criaImagensTeste();

    exposure = 0.0f; // exposição inicial

    // Aplica processamento inicial
    process();

    // Não retorna... a partir daqui, interação via teclado e mouse apenas, na janela gráfica

    // Mouse wheel é usada para aproximar/afastar
    // Setas esquerda/direita: reduzir/aumentar o fator de exposição
    // A/S: reduzir/aumentar o nível mínimo (black point)
    // K/L: reduzir/aumentar o nível máximo (white point)
    // H: exibir/ocultar o histograma
    // ESC: finalizar o programa
   
    glutMainLoop();

    return 0;
}

// Esta função deverá ser utilizada para ler o conteúdo do header
// para a variável header (depois você precisa extrair a largura e altura da imagem desse vetor)
void carregaHeader(FILE* fp)
{
    // Lê 11 bytes do início do arquivo
    fread(header, 11, 1, fp);
    // Exibe os 3 primeiros caracteres, para verificar se a leitura ocorreu corretamente
    printf("Id: %c%c%c\n", header[0], header[1], header[2]);
}

// Esta função deverá ser utilizada para carregar o restante
// da imagem (após ler o header e extrair a largura e altura corretamente)
void carregaImagem(FILE* fp, int largura, int altura)
{
    sizeX = largura; // 512
    sizeY = altura; // 768

    // Aloca imagem de entrada (32 bits RGBE)
    image = (unsigned char*) malloc(sizeof(unsigned char) * sizeX * sizeY * 4);

    // Aloca memória para imagem de saída (24 bits RGB)
    image8 = (unsigned char*) malloc(sizeof(unsigned char) * sizeX * sizeY * 3);

    // Lê o restante da imagem de entrada
    fread(image, sizeX * sizeY * 4, 1, fp);
    // Exibe primeiros 3 pixels, para verificação
    for(int i=0; i<12; i+=4) {
        printf("%02X %02X %02X %02X\n", image[i], image[i+1], image[i+2], image[i+3]);
    }
}

// Função apenas para a criação de uma imagem em memória, com o objetivo
// de testar a funcionalidade de exibição e controle de exposição do programa
void criaImagensTeste()
{
    // TESTE: cria uma imagem de 800x600
    sizeX = 800;
    sizeY = 600;

    printf("%d x %d\n", sizeX, sizeY);

    // Aloca imagem de entrada (32 bits RGBE)
    image = (unsigned char*) malloc(sizeof(unsigned char) * sizeX * sizeY * 4);

    // Aloca memória para imagem de saída (24 bits RGB)
    image8 = (unsigned char*) malloc(sizeof(unsigned char) * sizeX * sizeY * 3);
}