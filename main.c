#include <stdio.h>
#include <stdlib.h>

#define MAX 40  //Define o Tamanho Máximo da Matriz

//Struct q agrupa os dados do labirinto
typedef struct{
    char mapa [MAX][MAX]; //matriz onde é armazenado dados como as paredes, armadilhas e tesouros
    int linhas; //dimensões lidas no arquivo .txt
    int colunas;
    int ppx, ppy; //posição inicial do personagem
}Labirinto;

//função que le o arquivo .txt e carrega o mapa.
void carregaLabirinto(Labirinto *l, const char *nomeArquivo){
    
    FILE *arquivo = fopen(nomeArquivo, "r"); //abre o arquivo para a leitura

    if(arquivo == NULL){ //tratamento de erro no caso do arquivo não existir ou n poder ser aberto
        printf("Erro ao abrir arquivo %s\n", nomeArquivo); //mensagem do erro
        exit(1);
    }
    
        //le a primeira linha do arquivo .txt para configurar as dimensões do mapa
        fscanf(arquivo, "%dx%d\n", &l->linhas, &l->colunas); 

        //loop que percorre cada linha do labirinto
        for(int i = 0; i < l->linhas; i++){
            fgets(l->mapa[i], MAX + 2, arquivo); //le toda a linha preservando espaços vazios que são os corredores
        
        //loop que analisa cada coluna da linha
        for(int j = 0; j < l->colunas; j++){

            //se encontrar P, mapeia o inicio
            if(l->mapa[i][j] == 'P'){
                l->ppx = i;
                l->ppy = j;
            }

            //remove quebra de linhas para não danificar a exibição visual
            if(l->mapa[i][j] == '\n' || l->mapa[i][j] == '\r'){
                l->mapa[i][j] = '\0';
            } 
        }
    }

    fclose(arquivo);
}

int main() {
    Labirinto labi; //cria a struct

    carregaLabirinto(&labi, "labirinto.txt"); //carrega a função carregaLabirinto na main

    printf("Posicao Inicial do Personagem: %d, %d\n", labi.ppx, labi.ppy);

    //loop que imprime o mapa carregado na tela
    for (int i = 0; i < labi.linhas; i++) {
        printf("%s\n", labi.mapa[i]);
    }

    return 0; 
}
