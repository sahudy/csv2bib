//Evelin Soares

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 350

typedef struct article {
	char label[MAX/2];
	char author[MAX];
	char title[MAX];
	char journal[MAX];
	char year[5];
} Article;

typedef struct {
        char nome[MAX];
        int label;
} Autor;

int GerarBibTex (char *Arq, Article ar); // Retorna 1 se houve problema pra criar ou abrir o arquivo Bib
char *primeiroAutor(char *autores);

void limparMemoria(FILE *arq, Autor *autor, char *aux);
void Liberar(void *ptr);

int main (int argc, char *argv[]) {
	char c, delim, nome_bib[256], string[MAX];
	char aux[MAX] = "", label[MAX/2], *p, *p_autor = NULL;
	int cont = 0, tab_cont = 0, numArtigos = 0, i;
	
	Article ar;
	Autor *autores = NULL;
	FILE *Arq;
	
	if ( argc >= 4 ) {		
		if ( ( Arq = fopen(argv[1], "r") ) == NULL ) {
			printf("\nO arquivo .csv nao foi encontrado!\nEncerrando o programa...\n");
			return 1;
		}
		
		//Conta o número de linhas antes para saber quantos artigos serão e assim alocar posições suficientes para autores[]
		while ( ! feof(Arq) ) {
			c = fgetc(Arq);
			if ( c == '\n' ) {
				numArtigos++;
			}
		}
		rewind(Arq);
		
		if ( ( autores = calloc(numArtigos, sizeof(Autor) ) ) == NULL ) {
			printf("\nMemoria insuficiente para continuar a execucao do programa!\n");
			limparMemoria(Arq, autores, aux);
			exit(4);
		}
		numArtigos = 0;
        		
		strcpy(nome_bib, argv[2]);
		
		if ( ! ( strcmp(argv[3], "\t") ) )
			delim = '\t';
		else
			delim = argv[3][0];
	} else {
		printf("\nErro!\nUso do programa: %s \"arq_origem.csv\" \"arq_destino.bib\" delimitador(\";\" \"\\t\")\n", argv[0]);
		return 1;
	}
	    
	while ( ! feof(Arq) ) {
		c = fgetc( Arq );
		if ( c != delim && c != '\n' ) {
			string[cont++] = c;
		} else {
			string[cont] = '\0';
			tab_cont++;
			cont = 0;
			
			switch (tab_cont) {
				case 1:
					strcpy(ar.title, string);
					break;
				case 2: 
					strcpy(ar.author, string);
					break;
				case 3:
					strcpy(ar.year, string);
					break;
				case 4:
					strcpy(ar.journal, string);
					tab_cont = 0;
					p = NULL;
					numArtigos++;
					
					if ( strlen(ar.author) > 0 ) {
						p_autor = primeiroAutor(ar.author);
						
						int achou = 0;
						for ( i = 0; i < numArtigos - 1; i++ ) {
							if ( strstr(autores[i].nome, p_autor) != NULL ) {
								achou = 1;
								break;
							}
						}
						if ( achou ) {
							autores[i].label++;
						} else {
							i = numArtigos - 1;
							strcpy(autores[i].nome, p_autor);
							autores[i].label = 1;	
						}
						
						strcpy(aux, autores[i].nome);
						p = strtok(aux, ", "); // p = NULL antes de começar o novo artigo, evita problemas com endereço para onde aponta
						sprintf(label, "%s%d", p, autores[i].label); // printf em string
						strcpy(ar.label, label);
						
						printf("\nTitulo: %s\nAutores: %s\nAno: %s\nPublicacao: %s", ar.title, ar.author, ar.year, ar.journal);

						//GerarBibTex(nome do arquivo bib, struct article com dados do artigo processado);
						if ( GerarBibTex(nome_bib, ar) ) { // retorna 1 se houve erro!
							printf("\nNao foi possivel criar o arquivo .bib! Encerrando o programa...\n");
							limparMemoria(Arq, autores, aux);
							Liberar(p_autor);
							return 1;
						}
						Liberar(p_autor); // p_autor é alocado dinamicamente em char *primeiroAutor
					}
					// Depois de identificar todos os campos do artigo, busca ler um ponto-e-vírgula ou um espaço ( formato .csv )
					// e sai do loop quando acha um caractere diferente.
					// O primeiro caracter diferente encontrado será o "\n", então sai do while e na próxima leitura
					//na 1ª linha depois do while principal será um caracter para gravar no título do próximo artigo.
					while ( ! feof(Arq) && ( ( c = fgetc(Arq) ) == delim || c == ' ') );
					break;
			}
		}
	}
	
	printf("\nForam analisados %d artigos\n", numArtigos);
	
	system ("pause");
	
	limparMemoria(Arq, autores, aux);
	
	return 0;
}

int GerarBibTex (char *Arq, Article ar ) {
	FILE *a;
	char inproceedings = 0;
	/* Sera INPROCEEDINGS quando encontrar na leitura da string: proceedings, 
	acta, conference, symposium. Caso contrário, sera ARTICLE.
    */
     
	if ( ( strstr(ar.journal, "Inproceedings") != NULL ) || ( strstr(ar.journal, "Conference") != NULL ) || ( strstr(ar.journal, "Symposium") != NULL ) || ( strstr(ar.journal, " Acta ") != NULL ) ) { 
		inproceedings = 1;
	}
          
	if ( ( a = fopen(Arq, "a+") ) != NULL ) {
		if ( inproceedings ) 
			fprintf(a, "@inproceedings {%s\n", ar.label);
		else 
			fprintf(a, "@article {%s\n", ar.label);
		fprintf(a, "author = {\"%s\"},\n", ar.author);
		fprintf(a, "title = {\"%s\"},\n", ar.title);
		fprintf(a, "journal = {\"%s\"},\n", ar.journal);
		fprintf(a, "year = {\"%s\"}\n}\n\n", ar.year);
	} else {
		fclose(a);
		return 1;
	}
     
	fclose(a); 
	return 0;
}

char *primeiroAutor(char *autores) {
	char *s, *nome;
	int tam;
	
	if ( ( s = strstr(autores, " and") ) != NULL ) {
		tam = (strlen(autores) - strlen(s)) + 1;
		nome = (char *) malloc(tam * sizeof(char));
		strncpy(nome, autores, s - autores);
		nome[tam-1] = '\0';
		return nome;
	} else {
		if ( ( tam = strlen(autores) ) > 0 ) {
			tam++;
			nome = (char *) malloc(tam * sizeof(char) );
			strcpy(nome, autores);
			return nome;
		}
	}
	
	return NULL;
}

void limparMemoria(FILE *arq, Autor *autor, char *aux) {
	fclose(arq);
	Liberar(autor);
	Liberar(aux);
}

void Liberar(void *ptr) {
     free(ptr);
     ptr = NULL; // evitar "dangling pointers"
}
