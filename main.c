#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

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
    char mapa [MAX * MAX]; //matriz onde é armazenado dados como as paredes, armadilhas e tesouros
    int linhas; //dimensões lidas no arquivo .txt
    int colunas;
    int ppx, ppy; //posição inicial do personagem
}Labirinto;

//função que le o arquivo .txt e carrega o mapa.
void carregaLabirinto(Labirinto *l, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r"); //abre o arquivo em mode de leitura

    //se o arquivo não existir retorna a mensagem de nao encontrado
    if (!arquivo) {
        printf("Arquivo nao encontrado!\n");
        exit(1);
    }

    // Le a primeira linha buscando as dimensoes de linha x coluna e se segue o formato esperado de até 40x40
    if (fscanf(arquivo, "%dx%d", &l->linhas, &l->colunas) != 2) {
        printf("Erro no formato da dimensao!\n");
        exit(1);
    }

    int i = 0, j = 0;
    char c;

    // Pula caracteres ate chegar no \n começando o mapa do primeiro caracter
    while ((c = fgetc(arquivo)) != '\n' && c != EOF);

    //loop para percorrer o mapa
    for (i = 0; i < l->linhas; i++) {
        for (j = 0; j < l->colunas; j++) {
            c = fgetc(arquivo); //le o proximo caracter

            // Se ler uma quebra de linha antes do esperado tenta ler o proximo
            if (c == '\n' || c == '\r') {
                j--;
                continue;
            }

            //calcula a posicao linear no vetor
            int indice = (i * l->colunas) + j;
            l->mapa[indice] = c;

            //encontra a posicao inicial do personagem
            if (c == 'P') {
                l->ppx = i;
                l->ppy = j;
            }
        }
    }
    fclose(arquivo); //fecha o arquivo
}


//função para guardar item na mochila
void guardaMochila(No **topo, int valorNovo){

    //alocação dinamica na memoria para novo tesouro
    No  *novo = (No*) malloc(sizeof(No));
    novo->valor = valorNovo;
    novo->prox = NULL;

    //se a mochila estiver vazia ou o tesouro tiver o menor valor entra na primera posicao
    if(*topo == NULL || valorNovo < (*topo)->valor){
        novo->prox = *topo;
        *topo = novo;
    }

    //caso n seja o menor valor procura a posição correta na ordem crescente dos valores
    else{
        No *atual = *topo;

        //loop para percorrer a lista enquanto o proximo valor ainda for menor que o novo
        while(atual->prox != NULL && atual->prox->valor < valorNovo){
            atual = atual->prox;
        }

        //faz o encadeamento fazendo o novo valor apontar para o proximo e o atual apontar para o novo
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
    if(l->mapa[(x * l->colunas) + y] == '#' || l->mapa[(x * l->colunas) + y] == '.'){
        return 0;
    }

    return 1; //caminho livre
}

//funcao para a movimentacao no labi
void exploracaoLabi(Labirinto *l, No **mochila, Caminho **trilha){
    int x = l->ppx; //coordenadas atuais x e y do personagem
    int y = l->ppy;
    int saida = 0;

    while(!saida){

        //limpa o console a cada iteração e desenha o mapa novamente
        system("cls");
        for (int i = 0; i < l->linhas; i++) {
            for (int j = 0; j < l->colunas; j++) {
                // Se a posição do loop for a posição atual do jogador desenha o P
                if (i == x && j == y) {
                    printf("P");
                } else {
                    printf("%c", l->mapa[(i * l->colunas) + j]);
                }
            }
            printf("\n");
        }
        imprimeMochila(*mochila); //mostra o conteudo atual da mochila

        //carrega o que tem no local atual que o personagem esta
        char sAtual = l->mapa[(x * l->colunas) + y];

        if(sAtual == 'T'){ //verifica se esta em um tesouro
            int valor = (rand() % 100) + 1; //sorteia o valor do tesouro
            guardaMochila(mochila, valor); //guarda o tesouro e valor
            printf("\n[!] TESOURO ENCONTRADO: %i moedas!\n", valor);
            Sleep(1000); // Pausa de 1 segundo para leitura
        }
        else if(sAtual == 'A'){ //verifica se esta em uma armadilha
            printf("\n[!] CUIDADO: Armadilha\n");
            retiraTesouro(mochila); //retira o tesouro de menor valor
            Sleep(1000); // Pausa de 1 segundo para leitura
        }
        else if(sAtual == 'S'){ //verifica se esta na saida
            printf("\n SAIDA ENCONTRADA!\n");
            saida = 1;
            Sleep(2000); // Pausa de 2 segundos para leitura
            break;
        }

        l->mapa[(x * l->colunas) + y] = '.'; //Marca o caminho com .

        //Logica de movimentação
        if(caminhoLivre(l, x, y + 1)){ //tenta mover para direita
            salvaCaminho(trilha, x, y); //empilha a posicao atual antes de avancar
            y++;
        }
        else if (caminhoLivre(l, x + 1, y)) { //tenta mover para baixo
            salvaCaminho(trilha, x, y);
            x++;
        }
        else if (caminhoLivre(l, x, y - 1)) { //tenta mover para a esquerda
            salvaCaminho(trilha, x, y);
            y--;
        }
        else if (caminhoLivre(l, x - 1, y)) { //tenta mover para cima
            salvaCaminho(trilha, x, y);
            x--;
        }
        else{ //verifica se o caminho n tem saida
            if(*trilha == NULL){
                printf("\n Caminho sem Saida!\n");
                Sleep(2000);
                break;
            }

            printf("\n[!] Beco sem saida em (%d, %d)! Retornando...\n", x, y);
            Sleep(400);

            voltaCaminho(trilha, &x, &y); //desempilha  a coordenada anterior
        }

        Sleep(150); // Delay para a caminhada
    }
}

//funcao para calcular os valores dos tesouros
int calculaTesouros(No *topo){
    int total = 0; //variavel que acumula para o valor total
    No *atual = topo; //ponteiro para percorrer a lista sem perder a referencia

    // Enquanto o ponteiro não atingir o fim da lista (NULL)
    while(atual != NULL){
        total += atual->valor; //soma o valor do nó atual ao total acumulado
        atual = atual->prox; //ponteiro atual aponta para o proximo no
    }

    return total; //retorna a soma final
}

//funcao para gravar a solucao do labi em outro arquivo
void gravaCaminhoArquivo(Labirinto l, Caminho *topo, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w"); //abre o arquivo para escrita
    if (!arquivo) return; //tratamento de erro

    fprintf(arquivo, "--- COORDENADAS DO CAMINHO (FIM PARA INICIO) ---\n");
    Caminho *atual = topo; //ponteiro aux para percorrer a trilha

    while (atual != NULL) {
        //grava as coordenadas de cada passo
        fprintf(arquivo, "[%d, %d]\n", atual->x, atual->y);

        //substitui o caractere pelo símbolo '*' mostrando o trajeto final desenhado no arquivo
        l.mapa[(atual->x * l.colunas) + atual->y] = '*';

        atual = atual->prox; //avanca para o prox no
    }


    fprintf(arquivo, "\n--- MAPA DA SOLUCAO (* = Caminho) ---\n");
    for (int i = 0; i < l.linhas; i++) {
        for (int j = 0; j < l.colunas; j++) {
            //faz o desenho do mapa no arquivo
            fprintf(arquivo, "%c", l.mapa[(i * l.colunas) + j]);
        }
        fprintf(arquivo, "\n"); //quebra de linha após completar uma linha de colunas
    }

    fclose(arquivo); //frcha e salva o arquivo
    printf("\nSolucao visual e coordenadas gravadas em '%s'!\n", nomeArquivo);
}

int main() {

    srand(time(NULL));

    Labirinto labi; //cria a struct
    No *mochila = NULL; //inicia a mochila vazia
    Caminho *trilha = NULL;

    carregaLabirinto(&labi, "labirinto.txt"); //carrega a função carregaLabirinto na main
    exploracaoLabi(&labi, &mochila, &trilha);

    int total = calculaTesouros(mochila);

    printf("\n--Resultado--\n");

    imprimeMochila(mochila);
    printf("VALOR TOTAL ACUMULADO: %d moedas\n", total);

    //loop que imprime o mapa carregado na tela
    for (int i = 0; i < labi.linhas; i++) {
        for (int j = 0; j < labi.colunas; j++) {
            printf("%c", labi.mapa[(i * labi.colunas) + j]);
        }
        printf("\n");
    }

    gravaCaminhoArquivo(labi, trilha, "caminho_saida.txt");

    return 0;

}
