#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 40  //Define o Tamanho Máximo da Matriz

//struct para os itens da mochila
typedef struct No{
    int valor; //valor do tesouro
    struct No *prox; //ponteiro para próximo elemento da lista
}No;

//struct para cada passo da trilha
typedef struct Caminho{
    int x; //local na linha
    int y; //local na coluna
    struct Caminho *prox; //local aterior
}Caminho;

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
    printf("Mochila: ");
    No *atual = topo;

    //percorre toda a lista até o fim
    while(atual != NULL){
        printf("%i ", atual->valor);
        atual = atual->prox;
    }
    printf("\n");
}

//função para remover o primeiro item da mochila
void retiraTesouro(No **topo){

    //verifica se a mochila esta vazia
    if(*topo == NULL){
        printf("A mochila esta vazia!\n");
        return;
    }

    //ponteiro temporario para armazenar o item que vai ser removido
    No *temp = *topo;
    *topo = (*topo)->prox; //topo aponta para o proximo item
    free(temp); //libera a memoria do item removido

    printf("Voce perdeu um tesouro!\n");
}

//função que registra os passos no labirinto
void salvaCaminho(Caminho **topo, int x, int y){
    //aloca memoria para armazenar o local atual
    Caminho *novo = (Caminho*) malloc(sizeof(Caminho));
    if(novo == NULL) 
    exit(1);

    //dados das cordenadas na linha e coluna
    novo->x = x;
    novo->y = y;
    novo->prox = *topo;
    *topo = novo;
}

//função de pop que recupera o passo anterior
void voltaCaminho(Caminho **topo, int *x, int *y){
    if(*topo == NULL){
        return;
    }

    Caminho *temp = *topo; //ponteiro temporario para n oerder o no
   
    //devolve os valores para as suas variaveis
    *x = temp->x;
    *y = temp->y;
    *topo = (*topo)->prox;
    free(temp); //libera a memoria
}

//função que verifica se o caminho esta livre para andar
int caminhoLivre(Labirinto *l, int x, int y){

    //verifica se esta nos limites da matriz
    if(x < 0 || x >= l->linhas || y < 0 || y >= l->colunas){
        return 0;
    }

    //verifica se não é uma parede ou um lugar que já passou
    if(l->mapa[x][y] == '#' || l->mapa[x][y] == '.'){
        return 0;
    }

    return 1; //caminho livre
}

void exploracaoLabi(Labirinto *l, No **mochila, Caminho **trilha){
    int x = l->ppx;
    int y = l->ppy;
    int saida = 0;

    printf("\n--Labirinto--\n");

    while(!saida){
        char sAtual = l->mapa[x][y];

        if(sAtual == 'T'){
            int valor = (rand() % 100) + 1;
            guardaMochila(mochila, valor);
            printf("\nEnconrou um Tesouro! \n Valor: %i\n", valor);
        }

        else if(sAtual == 'A'){
            retiraTesouro(mochila);
        }

        else if(sAtual == 'S'){
            printf("\nEncontrou a Saída!");
            saida = 1;
            break;
        }

        l->mapa[x][y] = '.';

        if(caminhoLivre(l, x, y + 1)){
            salvaCaminho(trilha, x, y);
            y++;
        }

        else if (caminhoLivre(l, x + 1, y)) {
            salvaCaminho(trilha, x, y);
            x++;
        } 

        else if (caminhoLivre(l, x, y - 1)) {
            salvaCaminho(trilha, x, y);
            y--;
        } 

        else if (caminhoLivre(l, x - 1, y)) {
            salvaCaminho(trilha, x, y);
            x--;
        }

        else{
            if(*trilha == NULL){
                printf("\nCaminho sem Saída\n");
                break;
            }

            voltaCaminho(trilha, &x, &y);
            printf("Beco sem saida! Retornando\n");
        }
    }
}

int main() {

    srand(time(NULL));

    Labirinto labi; //cria a struct
    No *mochila = NULL; //inicia a mochila vazia
    Caminho *trilha = NULL;

    carregaLabirinto(&labi, "labirinto.txt"); //carrega a função carregaLabirinto na main
    exploracaoLabi(&labi, &mochila, &trilha);

    printf("\n--Resultado--\n");

    imprimeMochila(mochila);


    //loop que imprime o mapa carregado na tela
    for (int i = 0; i < labi.linhas; i++) {
        printf("%s\n", labi.mapa[i]);
    }

    return 0; 

}
