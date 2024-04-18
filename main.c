// Programa sequencial
// Representa um vetor esparso em um formato compacto
// Compilar com: gcc esparso_seq.c -fopenmp -o esparso_seq -Wall
// Executar por linha de comando: ./esparso_seq arquivo_entrada arquivo_saída

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// ----------------------------------------------------------------------------
// Variáveis globais
int n,	// Número de elementos do vetor de entrada
	 m,	// Número de elementos diferentes de 0 do vetor de entrada e tamanho dos vetores de saída
	 *vetIn,	// Vetor de entrada com n dados esparsos
	 *valor,	// Vetor de saída com valores dos m dados diferentes de 0
	 *posicao;	// Vetor de saída com posição no vetor de entrada dos m dados diferentes de 0

// ----------------------------------------------------------------------------
void inicializa(char* nome_arq_entrada)
{
	FILE *arq_entrada;	// Arquivo texto de entrada

	arq_entrada = fopen(nome_arq_entrada, "rt");

	if (arq_entrada == NULL)
	{
		printf("\nArquivo texto de entrada não encontrado\n");
		exit(1);
	}

	// Lê n do arquivo de entrada
	fscanf(arq_entrada, "%d", &n);

	// Aloca vetor de entrada (*** checar se conseguiu alocar)
	vetIn = malloc(n * sizeof(int));

	// Lê vetor de entrada do arquivo de entrada
	for (int i = 0; i < n; i++)
		fscanf(arq_entrada, "%d", &(vetIn[i]));

	fclose(arq_entrada);
}

// ----------------------------------------------------------------------------
void aloca_vetores_saida()
{
	// Aloca vetores de saída (*** checar se conseguiu alocar)
	valor   = malloc(m * sizeof(int));
	posicao = malloc(m * sizeof(int));
}

// ----------------------------------------------------------------------------
void conta_elementos_dif0()
{
	m = 0;
	for (int i = 0; i < n; i++)
		if (vetIn[i] != 0)
			m++;
}

// ----------------------------------------------------------------------------
void compacta_vetor()
{
	int j = 0;
	for (int i = 0; (i < n) && (j < m); i++)
		if (vetIn[i] != 0)
		{
			valor[j]   = vetIn[i];
			posicao[j] = i;
			j++;
		}
}

// ----------------------------------------------------------------------------
void finaliza(char* nome_arq_saida)
{
	FILE *arq_saida;	// Arquivo texto de saída

	arq_saida = fopen(nome_arq_saida, "wt");

	// Escreve m no arquivo de saída
	fprintf(arq_saida, "%d\n", m);

	// Escreve vetores de saída no arquivo de saída
	for (int j = 0; j < m; j++)
		fprintf(arq_saida, "%d %d\n", valor[j], posicao[j]);

	fclose(arq_saida);

	// Libera vetores de saída
	free(valor);
	free(posicao);

	// Libera vetor de entrada
	free(vetIn);
}

// ----------------------------------------------------------------------------
int main(int argc, char** argv)
{
	char nome_arq_entrada[100],
		  nome_arq_saida[100] ;

	if(argc != 3)
	{
		printf("O programa foi executado com parâmetros incorretos.\n");
		printf("Uso: ./esparso_seq arquivo_entrada arquivo_saída\n");
		exit(1);
	}

	// Obtém nome dos arquivos de entrada e saída
	strcpy(nome_arq_entrada, argv[1]) ;
	strcpy(nome_arq_saida, argv[2]) ;

	// Lê arquivo de entrada e inicializa estruturas de dados
	inicializa(nome_arq_entrada);

	double tini = omp_get_wtime(); // Medição de tempo exclui entrada, saída, alocação e liberação

	// Determina número m de elementos diferentes de 0 do vetor de entrada
	conta_elementos_dif0();

	double tfin = omp_get_wtime();
	double tempo1 = tfin - tini;

	// Aloca vetores de saída
	aloca_vetores_saida();

	tini = omp_get_wtime(); // Medição de tempo exclui entrada, saída, alocação e liberação

	// Preenche vetores de saída com valores e posições, a partir do vetor de entrada
	compacta_vetor();

	tfin = omp_get_wtime();
	double tempo2 = tfin - tini;
	double tempo = tempo1 +tempo2;
	printf("%s n=%12d: tempo= %f + %f = %f s\n", argv[0], n, tempo1, tempo2, tempo);

	// Escreve arquivo de saída e finaliza estruturas de dados
	finaliza(nome_arq_saida);

	return 0 ;
}
