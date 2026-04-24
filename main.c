#include <stdio.h>
#include <stdlib.h>

#define MAX 40  //Define o Tamanho Máximo da Matriz

typedef struct{
    char mapa [MAX][MAX];
    int linhas;
    int colunas;
    int ppx, ppy;
}Labirinto;

void carregaLabirinto(Labirinto *l, const char *nomeArquivo){
    
    FILE *arquivo = fopen(nomeArquivo, "r");

    if(arquivo == NULL){
        printf("Erro ao abrir arquivo %s\n", nomeArquivo);
        exit(1);
    }
    else{
        fscanf(arquivo, "%dx%d\n", &l->linhas, &l->colunas);
    }
}
