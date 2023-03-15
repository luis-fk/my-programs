#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*comentario sobre as funcoes estao detalhadas mais abaixo*/
void colocar_nas_linhas (int i, int j);
void colocar_nas_colunas (int i, int j);
void contar_nas_linhas (int i, int j, int n);
void contar_nas_colunas (int i, int j, int m);
void achar_palavra (int p);
int ja_no_tabuleiro_linhas(int i, int j, int p);
int ja_no_tabuleiro_colunas(int i, int j, int p);
void verificar_nas_linhas (int i, int j, int p);
void verificar_nas_colunas (int i, int j, int p);
void remover ();
void marcar_matriz(int **matriz, int m, int n);

/*foi decidido declararar praticamente todas as variaveis globalmente por ser mais simples atualizar os valores
quando os mesmos forem alterandos nas suas respectivas funcoes*/

struct tabuleiro {
    int *pos_linha; /*armazena as coordenadas das linhas onde as ultimas letras foram colocadas*/
    int *pos_coluna; /*armazena as coordenadas das colunas onde as ultimas letras foram colocadas*/
    int *colocadas; /*vetor que armazena quais palavras ja estao no tabuleiro*/
    int topo; /*topo dos vetores pos_linhas e pos_coluna*/
    int tamanho; /*aramzena o tamanho da palavra que pode ser colocada na linha ou coluna*/
    int letras; /*armazena o numero de letras que pode ser colocadas numa linha ou coluna*/
    int contador; /*contador do numero de letras restantes para serem colocada no tabuleiro*/
} tab;

struct pilha {
    int *tamanho_inseridas; /*armazena o tamanho das palavras inseridas no tabuleiro*/
    int *ordem_inseridas; /*armazena a ordem (indice) das palavras inseridas no tabuleiro*/
    int *ult_linha; /*vetor que armazena a posicao inicial das linhas das palavras colocadas*/
    int *ult_coluna; /*vetor que armazena a posicao inicial das colunas das palavras colocadas*/
    int topo; /*topo dos quatro vetores acima*/
} pilha;

/*marca eh usado para evitar que palavras que foram recem retiradas do tabuleiro sejam colocadas de volta
  verificar eh usado em varias funcoes para diversas verificacoes
  pos armazena a posicao de um respectiva palavra da lista de palvras
  num_palavras armazena o numero minimo necessario de palavras para que o tabuleiro tenha solucao
  tabuleiro eh o tabuleiro final e palavras armazena a lista de palavras*/
int marca = -1, verificar = 0, pos = 0, num_palavras = 0;
char **tabuleiro, *palavras[20];

int main (int argc, char **argv)
{
    /*"s" e "t" sao variaveis temporarias
      "m" sao as linhas, "n" as colunas e "p" o numero de palavras*/
    int m, n, i, j, p;
    int **matriz;
    char x[10];
        
    tab.topo = -1;
    tab.tamanho = 0;
    tab.letras = 0;
    pilha.topo = -1;
    FILE *file = fopen("crossword.txt", "r");

    fscanf(file, "%d", &m);
    fscanf(file, "%d", &n);

    matriz = calloc(m, sizeof(int*));
    for (i = 0; i < m; i++)
        matriz[i] = calloc(n, sizeof(int));

    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            fscanf(file, "%d", &matriz[i][j]);

    fscanf(file, "%d", &p);
    for (i = 0; i < p; i++)
        palavras[i] = calloc(p, sizeof(char));

    for (i = 0; i < p; i++)
        fscanf(file, "%s", palavras[i]);

    fclose(file);

    tabuleiro = calloc(m, sizeof(char*));
    for (i = 0; i < m; i++)
        tabuleiro[i] = calloc(n, sizeof(char));

    tab.pos_linha = calloc(m*n, sizeof(int));
    tab.pos_coluna = calloc(m*n, sizeof(int));
    tab.colocadas = calloc(p, sizeof(int));
    pilha.ult_linha = calloc(p, sizeof(int));
    pilha.ult_coluna = calloc(p, sizeof(int));
    pilha.ordem_inseridas = calloc(p, sizeof(int));
    pilha.tamanho_inseridas = calloc(p, sizeof(int));

    /*loop que ajeita o tabuleiro para uso nas funcoes com '*' quando a matriz eh "-1" e
        '0' quando a matriz eh "0"*/
    tab.contador = 0;
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++){
            if (matriz[i][j] == -1)
                tabuleiro[i][j] = '*';
            else{
                tabuleiro[i][j] = '0';
                tab.contador++;
            }
        }

    /*funcao que marca a matriz para palavras serem colocadas em linhas ou colunas e que tambem conta
        o numero minimo de palavras necessaria para o tabuleiro ter solucao*/
    marcar_matriz(matriz, m, n);

    file = fopen("crossword.txt", "w");

    /*se num_palavras for maior que "p" nem precisamos verificar o tabuleiro
        num_palavras eh calculado em marcar_matriz*/
    if (num_palavras > p){
        fprintf(file, "There is no solution.");
        return 0;
    }

    while (tab.contador != 0){
        if (pilha.topo == -1 && marca == p)
            break;

        /*loop geral que vai passando no tabuleiro, a matriz eh dividida em 3 partes, nos espacos onde os
        valores de matriz[i][j] (que foram definidas pela funcao "marcar_matriz") forem iguais e 3, significa
        que podemos colocar uma palavra horizontalmente e outra verticalemente partindo do mesmo quadrado inicial
        se o valor for igual a 1, podemos colocar apenas uma palvra verticalmente e se for igual a 2 podemos
        apenas colocar uma palavra horizontalmente. Se for igual a 1 ou 0 podemos pular porque nao tem
        nenhuma palavra que pode ser colocada ali*/
        for (i = 0; i < m; i++)
            for (j = 0; j < n; j++){
                if (matriz[i][j] == -1 || matriz[i][j] == 0)
                    continue;

                if (tab.contador == 0)
                    break;

                if (matriz[i][j] == 3){
                    contar_nas_linhas(i, j, n);

                    /*condicao que verificar se a palavra ja esta no tabuleiro, em caso afirmativo
                        podemos pular os passos abaixo*/
                    if (ja_no_tabuleiro_linhas(i, j, p) == 0){
                        pos = -1;
                        verificar = -1;
                        /*verificar toda a lista de palavra ate achar uma que possa ser colocada
                            no tabuleiro*/
                        while (verificar != 0 && pos != p){
                            achar_palavra(p);

                            verificar_nas_linhas(i, j, p);
                        }

                        /*caso geral onde nao tem solucao, se chegarmos a este ponto, significa
                            que nao temos mais opcoes de palavras para serem colocadas no tabuleiro*/
                        if (pilha.topo == -1 && marca == p)
                            break;

                        /*se o while loop acima nao achar nenhuma palavra pra ser colocada no tabuleiro
                            iremos remover a ultima palavra colocada e mudar os valores de "i" e "j" para
                            os espacos onde esta palavra que ira ser removida estava*/
                        if (pos == p){
                            remover();
                            i = pilha.ult_linha[pilha.topo];
                            j = pilha.ult_coluna[pilha.topo] - 1;
                            pilha.topo--;
                            continue;
                        }

                        colocar_nas_linhas(i, j);
                    }

                    /*abaixo repetimos o mesmo processo acima, so que nas colunas*/
                    contar_nas_colunas(i, j, m);

                    if (ja_no_tabuleiro_colunas(i, j, p) == 0){
                        pos = -1;
                        verificar = -1;
                        while (verificar != 0 && pos != p){
                            achar_palavra(p);

                            verificar_nas_colunas(i, j, p);
                        }

                        if (pilha.topo == -1 && marca == p)
                            break;

                        if (pos == p){
                            remover();
                            i = pilha.ult_linha[pilha.topo];
                            j = pilha.ult_coluna[pilha.topo] - 1;
                            pilha.topo--;
                            continue;
                        }

                        colocar_nas_colunas(i, j);
                    }

                    continue;
                }

                if (tab.contador == 0)
                    break;

                /*caso onde podemos colocar uma unica palavra na vertical, o processo eh
                    identico ao visto acima*/
                if (matriz[i][j] == 1){
                    contar_nas_colunas(i, j, m);

                    if (ja_no_tabuleiro_colunas(i, j, p) == 0){
                        pos = -1;
                        verificar = -1;
                        while (verificar != 0 && pos != p){
                            achar_palavra(p);

                            verificar_nas_colunas(i, j, p);
                        }

                        if (pilha.topo == -1 && marca == p)
                            break;

                        if (pos == p){
                            remover();
                            i = pilha.ult_linha[pilha.topo];
                            j = pilha.ult_coluna[pilha.topo] - 1;
                            pilha.topo--;
                            continue;
                        }

                        colocar_nas_colunas(i, j);
                    }

                    continue;
                }

                if (tab.contador == 0)
                    break;

                /*caso onde podemos colocar uma unica palavra horizontalmente, processo identico
                    ao visto acima*/
                if (matriz[i][j] == 2){
                    contar_nas_linhas(i, j, n);

                    if (ja_no_tabuleiro_linhas(i, j, p) == 0)
                    {
                        pos = -1;
                        verificar = -1;
                        while (verificar != 0 && pos != p){
                            achar_palavra(p);

                            verificar_nas_linhas(i, j, p);
                        }

                        if (pilha.topo == -1 && marca == p)
                            break;

                        if (pos == p){
                            remover();
                            i = pilha.ult_linha[pilha.topo];
                            j = pilha.ult_coluna[pilha.topo] - 1;
                            pilha.topo--;
                            continue;
                        }

                        colocar_nas_linhas(i, j);
                    }

                    continue;
                }
            }
    }

    /*caso especial visto acima, onde nao tem solucao para o tabuleiro dado, ou seja, eh o caso final
        antes do programa entrar em loop*/
    if (pilha.topo == -1 && marca == p){
        fprintf(file, "There is no solution");
        return 0;
    }

    /*caso onde tem solucao e imprimimos a resposta*/
    if (tab.contador == 0){
        for (i = 0; i < m; i++){
            for (j = 0; j < n; j++)
                fprintf(file, "%c", tabuleiro[i][j]);

            fprintf(file, "\n");
        }
    }

    /*chamando free para todos os vetores mallocados/callocados*/
    free (tab.pos_linha);
    free (tab.pos_coluna);
    free (tab.colocadas);
    free (pilha.ult_linha);
    free (pilha.ult_coluna);
    free (pilha.ordem_inseridas);
    free (pilha.tamanho_inseridas);

    for (i = 0; i < m; i++)
        free(matriz[i]);
    free(matriz);

    for (i = 0; i < p; i++)
        free(palavras[i]);

    for (i = 0; i < m; i++)
        free(tabuleiro[i]);
    free(tabuleiro);

    return 0;
}

/*funcao que coloca as palavras nas linhas. Na verdade colocamos letra por letra, que sao armazenadas
  nos vetores "tab.posicao_linha" e "tab.posicao_coluna"*/
void colocar_nas_linhas (int i, int j){
    int l;

    /*atualizamos os vetores com o indice da palavra que vai ser colocada no tabuleiro,
      o numero de suas letras que foram colocadas e as coordenadas iniciais da sua linha e coluna*/
    pilha.topo++;
    pilha.ordem_inseridas[pilha.topo] = pos;
    pilha.tamanho_inseridas[pilha.topo] = tab.letras;
    pilha.ult_linha[pilha.topo] = i;
    pilha.ult_coluna[pilha.topo] = j;

    /*aqui colocamos letra por letra no tabuleiro, mas apenas as letras da palavra que nao esteja no
      tabuleiro e tais letras sao armazenadas nos vetores*/
    for (l = 0; l < tab.tamanho; l++, j++){
        if (tabuleiro[i][j] == palavras[pos][l])
            continue;

        tab.topo++;
        tab.pos_linha[tab.topo] = i;
        tab.pos_coluna[tab.topo] = j;

        tabuleiro[i][j] = palavras[pos][l];
        tab.contador--;
    }

    /*marca eh resetada caso tenha sido alterada anteriormente e o indice da palavra eh adicionado no vetor
      "tab.colocadas" para evitar que sejam colocadas novamente no tabuleiro*/
    marca = -1;
    tab.colocadas[pos] = 1;
}

/*processo identico ao da funcao de colocar nas linhas, mas dessa vez nas colunas*/
void colocar_nas_colunas (int i, int j){
    int c;

    pilha.topo++;
    pilha.ordem_inseridas[pilha.topo] = pos;
    pilha.tamanho_inseridas[pilha.topo] = tab.letras;
    pilha.ult_linha[pilha.topo] = i;
    pilha.ult_coluna[pilha.topo] = j;

    for (c = 0; c < tab.tamanho; c++, i++){
        if (tabuleiro[i][j] == palavras[pos][c])
            continue;

        tab.topo++;
        tab.pos_linha[tab.topo] = i;
        tab.pos_coluna[tab.topo] = j;

        tabuleiro[i][j] = palavras[pos][c];
        tab.contador--;
    }

    marca = -1;
    tab.colocadas[pos] = 1;
}

/*essa funcao conta o tamanho da palavra que pode ser colocada nas linhas e o numero de letras que estao
  disponiveis a serem colocadas nessa mesma linha. Essa diferenca foi necessaria para que depois nao retiremos letras de
  palavras que nao deveriam estar sendo removidas do tabuleiro*/
void contar_nas_linhas (int i, int j, int n){
    for (tab.tamanho = 0, tab.letras = 0; j < n; j++){
        if (tabuleiro[i][j] == '*')
            return;
        else
            tab.tamanho++;

        if (tabuleiro [i][j] == '0')
            tab.letras++;
    }
}

/*mesmo processo de contar_nas_linhas so que agora nas colunas*/
void contar_nas_colunas (int i, int j, int m){
    for (tab.tamanho = 0, tab.letras = 0; i < m; i++){
        if (tabuleiro[i][j] == '*')
            return;
        else
            tab.tamanho++;

        if (tabuleiro [i][j] == '0')
            tab.letras++;
    }
}

/*funcao que acha uma palavra da lista de palavras para ser colocada no tabuleiro. Essa funcao
  usa a variavel tamanho que foi achada nas funcoes "contar_nas_..." acima para achar uma palavra que encaixe no seu
  devido local*/
void achar_palavra(int p){
    pos++;

    /*se isso for verdade, nao tem necessida de verificar a lista, pos chegamos ao fim dela*/
    if (pos == p)
        return;

    /*se essa condicao eh verdade verificamos palavras novas que ainda nao foram colocadas no
      tabuleiro e que ainda nao estao no tabuleiro*/
    if (marca != -1){
        marca++;
        for ( ; marca < p; marca++)
            if (strlen(palavras[marca]) ==  tab.tamanho && tab.colocadas[marca] == 0){
                pos = marca;
                return;
            }
    }

    /*essa condicao so eh verdade se nao ouver mais nenhuma palavra para ser adicionada no tabuleiro*/
    if (marca == p){
        pos = marca;
        return;
    }

    /*condicao geral de verificacao de palavras que ainda nao estao no tabuleiro parem serem adicionadas*/
    if(marca == -1)
        for ( ; pos < p; pos++)
            if (strlen(palavras[pos]) ==  tab.tamanho && tab.colocadas[pos] == 0)
                return;
}

/*funcao que verifica se uma palavra ja foi colocada na linha em questao no tabuleiro. Esta funcao foi
  necessaria devido ao backtracking de voltar a ultima posicao da palavra que foi recem removida e da
  maneira em que o programa organiza os espacos onde palavras podem ser colocadas com "1", "2" e "3"*/
int ja_no_tabuleiro_linhas(int i, int j, int p){
    int l, s;
    pos = 0;
    verificar = 0;
    s = j;

    /*essa funcao eh uma mistura de "achar_palavra" e de "verificar_nas_colunas", mas de modo mais geral, em
      que ela verifica todas as palavras da lista, independente de estarem no tabuleiro ate achar uma que ja
      esteja no tabuleiro, caso exista.*/
    for (pos = 0 ; pos < p; pos++)
         if (strlen(palavras[pos]) ==  tab.tamanho && tab.colocadas[pos] == 1)
            for (l = 0, verificar = 0, j = s; l < tab.tamanho; l++, j++){
                if (tabuleiro[i][j] == palavras[pos][l] && tabuleiro[i][j] > 64)
                    verificar++;

                if (verificar == tab.tamanho)
                    return 1;
            }

    return 0;
}

/*funciona da mesma maneiro do que a "ja_no_tabuleiro_linhas", mas para as colunas*/
int ja_no_tabuleiro_colunas(int i, int j, int p){
    int c, s;
    pos = 0;
    verificar = 0;
    s = i;

    for (pos = 0 ; pos < p; pos++)
         if (strlen(palavras[pos]) ==  tab.tamanho && tab.colocadas[pos] == 1)
            for (c = 0, verificar = 0, i = s; c < tab.tamanho; c++, i++){
                if (tabuleiro[i][j] == palavras[pos][c] && tabuleiro[i][j] > 64)
                    verificar++;

                if (verificar == tab.tamanho)
                    return 1;
            }

    return 0;
}

/*funcao que verfica se a palavra escolida por "achar_palavras" pode de fato, ser colocada na linha, caso
  contrario (se "verificar >= 1") a funcao main ira chamar "achar_palavra" novamente com o valor de
  "pos" atualizado para procurar outra palavra, que sera novamente verificada nessa funcao*/
void verificar_nas_linhas (int i, int j, int p){
    int l;
    verificar = 0;

    if (pos == p){
        verificar++;
        return;
    }

    for (l = 0; l < tab.tamanho; l++, j++)
        if (tabuleiro[i][j] != palavras[pos][l] && tabuleiro[i][j] > 64)
            verificar++;
}

/*funciona da mesma maneira do que "verificar_nas_linhas" so que nas colunas*/
void verificar_nas_colunas (int i, int j, int p){
    int c;
    verificar = 0;

    if (pos == p){
        verificar++;
        return;
    }

    for (c = 0; c < tab.tamanho; c++, i++)
        if (tabuleiro[i][j] != palavras[pos][c] && tabuleiro[i][j] > 64)
            verificar++;
}

/*funcao que remove as palavras, usando praticamente todos os vetores declarados neste programa*/
void remover (){
    int i;
    verificar = 0;

    /*aqui adicionamos o valor para "marca" com o indice da palavra que esta sendo removida
      e ao mesmo tempo atualizamos a "tab.colocadas" que indica que a palavra nao esta mais no tabuleiro*/
    marca = pilha.ordem_inseridas[pilha.topo];
    tab.colocadas[pilha.ordem_inseridas[pilha.topo]] = 0;

    for (i = 0; i < pilha.tamanho_inseridas[pilha.topo]; i++){
        /*aqui retornamos os valores do "tabuleiro" de volta para '0', mas apenas nos espacos onde tem
        letras da palavra que esta sendo removida, sem mexer nas letras de outras palavras, caso existam*/
        tabuleiro[tab.pos_linha[tab.topo]][tab.pos_coluna[tab.topo]] = '0';
        tab.topo--;
        tab.contador++;
    }
}

/*funcao que marca a matriz dada pelo usuario com os valores "1", "2" e "3" para serem usados na funcao main
  e chamar as funcoes apropriadas. Ao mesmo tempo, essa funcao conta o numero de palavras que serao necessarias
  para completar este tabuleiro*/
void marcar_matriz(int **matriz, int m, int n){
    int i, j;
    num_palavras = 0;

    /*este loop passa pela linha 0 verificando todos os lugares que podemos colocar palavras
      no sentido vertical e isso so acontece se o espaco atual estiver livre o espaco abaixo
      nao tiver um '*' */
    for (i = 0, j = 0; j < n; j++){
            if (matriz[i][j] == -1)
                continue;

            if (matriz[i][j] == 0 && matriz[i + 1][j] != -1){
                matriz[i][j] = 1;
                num_palavras++;
            }
        }

        /*loop que varre a coluna 0 e funciona do mesmo jeito do loop acima, exceto que desta vez
          estamos liadando na horizontal e nao na vertical*/
        for (i = 0, j = 0; i < m; i++){
            if (matriz[i][j] == -1)
                continue;

            if (matriz[i][j] == 0 && matriz[i][j + 1] != -1){
                matriz[i][j] = 2;
                num_palavras++;
            }
        }

    /*este eh um loop geral que verifica todos os espacos onde temos uma '*'. Esse loop verifica as posicoes
      ao redor das '*' para verificar se alguma palavra pode ser colocada ali*/
    for (i = 0; i < m; i++)
            for (j = 0; j < n; j++){
                if (i + 2 < m)
                    /*verifica a parte debaixo de um lugar com uma estrela*/
                    if (matriz[i][j] == -1 && matriz[i + 1][j] != -1 && matriz[i + 2][j] != -1){
                        if (matriz[i + 1][j] == 2){
                            matriz[i + 1][j] = 3;
                            num_palavras++;
                        }

                        else{
                            matriz[i + 1][j] = 1;
                            num_palavras++;
                        }
                    }

                if (j + 2 < n)
                    /*verifica a parte da direita de um lugar com uma estrela*/
                    if (matriz[i][j] == -1 && matriz[i][j + 1] != -1 && matriz[i][j + 2] != -1){
                        if (matriz[i][j + 1] == 1){
                            matriz[i][j + 1] = 3;
                            num_palavras++;
                        }

                        else{
                            matriz[i][j + 1] = 2;
                            num_palavras++;
                        }
                    }
            }
}
