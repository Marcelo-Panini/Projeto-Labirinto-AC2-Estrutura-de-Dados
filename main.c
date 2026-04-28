#include <stdio.h>
#include <stdlib.h>

#define MAX 40  //Define o Tamanho Máximo da Matriz

//struct para os itens da mochila
typedef struct No{
    int valor; //valor do tesouro
    struct No *prox; //ponteiro para próximo elemento da lista
}No;

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

//função para guardar item na mochila
void guardaMochila(No **topo, int valorNovo){

    //alocação dinamica na memoria para novo tesouro
    No  *novo = (No*) malloc(sizeof(No));
    novo->valor = valorNovo;
    novo->prox = NULL;

    //se a mochila estiver vazia ou o tesouro tiver o menor valor, entra na primera posição
    if(*topo == NULL || valorNovo < (*topo)->valor){
        novo->prox = *topo;
        *topo = novo;
    }

    //caso n seja o menor valor, procura a posição correta na ordem crescente dos valores
    else{
        No *atual = *topo;

        //loop para percorrer a lista enquanto o proximo valor ainda for menor que o novo
        while(atual->prox != NULL && atual->prox->valor < valorNovo){
            atual = atual->prox;
        }

        //faz o encadeamento, fazendo o novo valor apontar para o proximo e o atual apontar para o novo
        novo->prox = atual->prox;
        atual->prox = novo;
    }
}

//função que mostra a mochila
void imprimeMochila(No *topo){
    printf("Moxhila: ");
    No *atual = topo;

    //percorre toda a lista até o fim
    while(atual != NULL){
        printf("%i ", atual->valor);
        atual = atual->prox;
    }
    printf("\n");
}

int main() {
    Labirinto labi; //cria a struct
    No mochila = NULL; //inicia a mochila vazia

    carregaLabirinto(&labi, "labirinto.txt"); //carrega a função carregaLabirinto na main

    printf("Posicao Inicial do Personagem: %d, %d\n", labi.ppx, labi.ppy);

    //loop que imprime o mapa carregado na tela
    for (int i = 0; i < labi.linhas; i++) {
        printf("%s\n", labi.mapa[i]);
    }

    return 0;
}

