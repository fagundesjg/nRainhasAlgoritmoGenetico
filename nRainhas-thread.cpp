#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <thread>
#include <algorithm>
#include <chrono>
#include <mutex>

using namespace std;

// 40,50,50,3000,0.2,0.7

#define TAM_GENES 10 // TAMANHO DO TABULEIRO DA N RAINHAS... NxN
#define TAM_POPULACAO 200
#define TAM_TORNEIO 500
#define GERACOES 10000
#define PROB_MUTACAO 0.2
#define PROB_CRUZAMENTO 0.7

#define N_THREADS 8

typedef vector<vector<int>> Matriz;

void iniciarMatriz(Matriz &matriz);
void inicializarPopulacao();
void exibir(Matriz matriz);
int checarLinha(Matriz matriz, int linha);
int checarColuna(Matriz matriz, int coluna);
int checarDiagonalPrincipal(Matriz matriz, int linha, int coluna);
int checarDiagonalSecundaria(Matriz matriz, int linha, int coluna);
int obterPontuacao(Matriz matriz);
void mutacao(Matriz &matriz);
void cruzamento(vector<Matriz> populacao, int indice_pai1, int indice_pai2, Matriz &filho);

vector<Matriz> populacao;

void iniciarMatriz(Matriz &matriz)
{
    int pos;

    for (size_t i = 0; i < TAM_GENES; i++)
    {
        vector<int> linha(TAM_GENES);
        pos = rand() % TAM_GENES;
        linha[pos] = 1;
        matriz.push_back(linha);
    }
}

void inicializarPopulacao()
{
    for (size_t i = 0; i < TAM_POPULACAO; i++)
    {
        Matriz matriz;
        iniciarMatriz(matriz);
        populacao.push_back(matriz);
    }
}

void exibir(Matriz matriz)
{
    for (size_t i = 0; i < TAM_GENES; i++)
    {
        for (size_t j = 0; j < TAM_GENES; j++)
        {
            cout << matriz[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl
         << endl;
}

int checarLinha(Matriz matriz, int linha)
{
    int conflitos = 0;
    for (size_t j = 0; j < TAM_GENES; j++)
    {
        conflitos += matriz[linha][j];
    }
    return (conflitos > 1) ? conflitos - 1 : 0;
}

int checarColuna(Matriz matriz, int coluna)
{
    int conflitos = 0;
    for (size_t i = 0; i < TAM_GENES; i++)
    {
        conflitos += matriz[i][coluna];
    }
    return (conflitos > 1) ? conflitos - 1 : 0;
}

int checarDiagonalPrincipal(Matriz matriz, int linha, int coluna)
{
    int conflitos = 0;
    size_t i, j;

    i = linha + 1;
    j = coluna + 1;

    while (i >= 0 && i < TAM_GENES && j >= 0 && j < TAM_GENES)
    {
        conflitos += matriz[i][j];
        i++;
        j++;
    }

    i = linha - 1;
    j = coluna - 1;
    while (i >= 0 && i < TAM_GENES && j >= 0 && j < TAM_GENES)
    {
        conflitos += matriz[i][j];
        i--;
        j--;
    }

    return conflitos;
}

int checarDiagonalSecundaria(Matriz matriz, int linha, int coluna)
{
    int conflitos = 0;
    size_t i, j;

    i = linha - 1;
    j = coluna + 1;

    while (i >= 0 && i < TAM_GENES && j >= 0 && j < TAM_GENES)
    {
        conflitos += matriz[i][j];
        i--;
        j++;
    }

    i = linha + 1;
    j = coluna - 1;
    while (i >= 0 && i < TAM_GENES && j >= 0 && j < TAM_GENES)
    {
        conflitos += matriz[i][j];
        i++;
        j--;
    }

    return conflitos;
}

int obterPontuacao(Matriz matriz)
{
    int conflitos = 0;

    for (size_t i = 0; i < TAM_GENES; i++)
    {
        for (size_t j = 0; j < TAM_GENES; j++)
        {
            if (matriz[i][j] == 1) // se achar uma rainha, checar se ela da conflito com outra
            {
                conflitos += checarLinha(matriz, i);
                conflitos += checarColuna(matriz, j);
                conflitos += checarDiagonalPrincipal(matriz, i, j);
                conflitos += checarDiagonalSecundaria(matriz, i, j);
            }
        }
    }

    return conflitos;
}

void mutacao(Matriz &matriz)
{
    vector<int> individuo(TAM_GENES);
    int pos, linha;

    pos = rand() % TAM_GENES;
    do
    {
        linha = rand() % TAM_GENES;
    } while (linha == pos);

    individuo[pos] = 1;
    matriz[linha] = individuo;
}

void cruzamento(int indice_pai1, int indice_pai2, Matriz &filho)
{
    size_t ponto = rand() % TAM_GENES;

    for (size_t i = 0; i <= ponto; i++)
        filho.push_back(populacao[indice_pai1][i]);
    for (size_t i = ponto + 1; i < TAM_GENES; i++)
        filho.push_back(populacao[indice_pai2][i]);
}

int obterMelhor()
{
    int indice_melhor = 0;
    int score_melhor = obterPontuacao(populacao[0]);
    int score;

    for (size_t i = 1; i < TAM_POPULACAO; i++)
    {
        score = obterPontuacao(populacao[i]);
        if (score < score_melhor)
        {
            indice_melhor = i;
            score_melhor = score;
        }
    }
    return indice_melhor;
}

void threadFn(size_t &i)
{
    double prob;
    int indice_pai1, indice_pai2, score_pai1, score_filho;
    size_t j;
    //clock_t start = clock();
    for (j = 0; j < TAM_TORNEIO; j++)
    {
        prob = ((double)rand() / ((double)RAND_MAX + 1));

        if (prob < PROB_CRUZAMENTO)
        {
            indice_pai1 = rand() % TAM_POPULACAO;

            do
            {
                indice_pai2 = rand() % TAM_POPULACAO;
            } while (indice_pai1 == indice_pai2);

            Matriz filho;
            cruzamento(indice_pai1, indice_pai2, filho);
            prob = ((double)rand() / ((double)RAND_MAX + 1));

            if (prob < PROB_MUTACAO)
                mutacao(filho);

            score_pai1 = obterPontuacao(populacao[indice_pai1]);
            score_filho = obterPontuacao(filho);

            if (score_filho < score_pai1)
            {
                
                populacao[indice_pai1] = filho;
                
            }
        }
    }
    i++;
    //clock_t end = clock();
    //cout << "Tempo de Thread " << i << ": " << (end - start) / (double)CLOCKS_PER_SEC << " segundos" << endl;
}

int main()
{
    srand(unsigned(time(NULL)));
    int score_melhor, indice_melhor;
    size_t i, contador = 0;
    vector<thread> threads;
    inicializarPopulacao();
    auto iniciar = [](thread &t) { t.join(); };
    cout << "Procurando solucao por algoritmo genetico..." << endl;
    auto start = chrono::steady_clock::now();
    for (i = 0; i < GERACOES;)
    {
        if (contador < N_THREADS)
        {
            threads.push_back(thread(threadFn, ref(i)));
            contador++;
            continue;
        }
        else
        {
            for_each(threads.begin(), threads.end(), iniciar);
            threads.clear();
            contador = 0;
        }
        indice_melhor = obterMelhor();
        score_melhor = obterPontuacao(populacao[indice_melhor]);
        // esse if é pra fazer imprimir na tela a pontuação apenas a cada 50 gerações.
        if (i % (N_THREADS * 5) == 0)
        {
            cout << "Geracao: " << i << endl;
            cout << "Pontuacao: " << score_melhor << endl
                 << endl;
        }
        // se o score chegou a zero, significa que encontrou a solução
        if (score_melhor == 0)
            break;
    }

    auto end = chrono::steady_clock::now();
    // se chegou aqui é porq encontrou a solução ou acabou as gerações
    cout << "Geracao: " << i + 1 << endl;
    exibir(populacao[indice_melhor]);

    cout << endl;
    cout << "Pontuacao: " << score_melhor << "\n\n";

    cout << "Tempo total gasto: " << chrono::duration_cast<chrono::seconds>(end - start).count() << " segundos" << endl;

    return 0;
}
