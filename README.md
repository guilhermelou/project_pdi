# project_pdi

Para utilizar as funções do programa estou utilizando a execução por promnpt com argc, argv**.
Cada função de PDI implementada pode ser chamada pelo nome dela no primeiro argumento de argv**.
Os parâmetros dela serão os próximos argumentos de argv**.
O resultado do processamento será colocado automaticament em um arquivo com nome result + extenção do resultado(pgm, ppm, csv)

Ex: 
  Chamada: ./mypdi PGMHistogramToFile lena.pgm
  Resultado em: result.csv
  
Todas as funções estão comentadas na função main.c com um exemplo de como executar a chamada de cada. O funcionamento também
está comentado nos protótipos e na implementação de cada.
Os comentários do código estão em inglês devido a tentativa de melhorar o prório conhecimento em inglês, podem conter erros...


Estou utilizando uma struct com uma matrix de inteiros para armazenar os arquivos PGM e PPM.
A leitura dos arquivos é feita caracter a caracter guargando o número mágico, largura, altura, escala de cor e pixels.
As funções CONSTFromFile e CONSTFromBuffer são as que leem o arquivo e processam seu buffer.

typedef struct PGM {
    char magic_nmb[3];
    int width;
    int height;
    int color_range;
    int **matrix;
} PGM;

typedef struct PPM {
    char magic_nmb[3];
    int width;
    int height;
    int color_range;
    int ***matrix;
} PPM;
  

