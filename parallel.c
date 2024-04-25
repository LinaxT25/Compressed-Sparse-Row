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
	 interactionst, // Número de interações realizadas por cada thread
	 *vetIn,	// Vetor de entrada com n dados esparsos
	 *valor,	// Vetor de saída com valores dos m dados diferentes de 0
	 *posicao,	// Vetor de saída com posição no vetor de entrada dos m dados diferentes de 0
	 *vetaux;   // Vetor auxilar para pegar a posição do vetor de entrada

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

/* 
	Observações: A professora deu algumas dicas basicamente, em resumo temos o seguinte:
 * Modificar o for do conta_elementos_dif0(não deve ter outra coisa dentro da função além do for acredito eu)
 * Vamos ter uma variável global(talvez até mais, porém definitivamente uma variável auxiliar)
 * Ela não descartou a ideia de um vetor, porém pelo que foi dito deve ser menor que m e n(então deve ser possível fazer com ou sem)

   Com base no que ela disse eu acredito que a ideia de preencher o vetor de posição nessa função
deve estar errada, o foco dela foi em explicar que as threads vão distribuir as interações entre elas
dessa forma a possibilidade que vem na minha mente é de realizar a contagem das interações que cada thread
realiza e utilizar isso de alguma forma, teoricamente eu acredito que seja possível pegar essas interações com base
no omp_get_thread_num() e omp_get_num_threads(), a dificuldade vai ser em utilizar posteriormente esse elemento na
função compacta_vetor.

 */
void conta_elementos_dif0()
{
	m = 0;
	int test = 0;
	#pragma omp parallel reduction(+:m)
	for (int i = 0; i < n; i++)
	{
		// // Pega o valor de interação correspondente a cada thread
		// interactionst = (n / omp_get_num_threads()) * omp_get_thread_num();
		// printf("Thread %d: interactionst = %d\n", omp_get_thread_num(), interactionst);

		// Uma ideia mais interessante pode ser realizar a contagem dentro do próprio for
		// se cria um vetor auxiliar com tamanho das x threads e incrementamos a posição com base na
		// thread que está realizando a interação, com isso vamos ter a contagem de interações de cada thread.
		// vetor = malloc(omp_get_num_threads() * sizeof(int));
		// vetor[opm_get_thread_num()] = vetor[omp_get_thread_num()] + 1;
		// com isso acredito que seja possível obter a contagem de interações de cada thread
		// que somadas devem dar o intervalo n, mas não sei se é a forma correta de pensar e como incluir na outra função

		if (vetIn[i] != 0)
			m++;

	}
	// printf("test = %d\n", test);

	// #pragma omp parallel
	// for(int i = 0; i < 1; i++)
	// {
	// 	//Pega o valor de interação correspondente a cada thread
	// 	interactionst = (n / omp_get_num_threads());
	// 	//printf("Thread %d: interactionst = %d\n", omp_get_thread_num(), interactionst);
	// }
}

// ----------------------------------------------------------------------------
void compacta_vetor()
{ 
    int j = 0;
	int i = 0;
	for (i = 0; (i < n) && (j < m); i++)
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

	// Libera vetor auxiliar
	free(vetaux); 

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
