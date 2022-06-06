/*
** server.c => arquivo que trata das conexões com clientes e lida com o "banco de dados" e processamentos
*           lidando com o envio e tratamento de mensagens segundo o protocolo UDP
*   Autor: Rebeca Portes Stroh, 186407
*/

// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT	 8080
#define MAXLINE 1024
#define BACKLOG 10      // Quantas conexões pendentes podem existir
#define MAXDATASIZE 10000 // Número máximo de bytes transferidos na mensagem
#define FILENAME "data.txt" // Nome do arquivo que salva todos os dados
#define TEMPFILENAME "delete.tmp" // Nome do arquivo que salva todos os dados

FILE* file; // O Arquivo que usaremos para salvar os dados
int numbytes, result;
int sockfd, n;
char buffer[MAXLINE];
struct sockaddr_in servaddr;
socklen_t len;

// handling non blocking functions
struct timeval tv;
fd_set rset;
int nready, maxfdp;

int sendClient(char * message, struct sockaddr_in cliaddr) {
  return sendto(sockfd, message, strlen(message), 0, (const struct sockaddr *) &cliaddr, len) == -1;
}

// Garante que o arquivo tem mais o primeiro item que conta a quantidade de filmes e o header
void garanteeNumberAndHeaderLinesOnFile() {
    char buf[MAXDATASIZE];
    int count = 1;

    if( access( FILENAME, F_OK ) == 0 ) { // Se o arquivo existir, pesquisa se tem as primeiras linhas
        file = fopen(FILENAME, "r"); // Abre o arquivo como leitura
        while ((fgets(buf, MAXDATASIZE, file)) != NULL) { // Le cada linha do arquivo
            count++;
        }
        fclose(file); // fecha arquivo
    }

    if ((count == 1) || (count == 2)) {
        file = fopen(FILENAME, "w"); // Abre o arquivo como escrita
        fputs("0", file);
        fputs("\nid|title|director|year|genders", file);
        fclose(file); // fecha arquivo
    }
}

// Deleta uma dada linha do arquivo
void deleteLine(const int line) {
    char buf[MAXDATASIZE];
    char movie[MAXDATASIZE];
    int count = 1;
    int n;

    file = fopen(FILENAME, "r"); // Abre o arquivo para leitura
    FILE *temp = fopen(TEMPFILENAME, "w"); // Cria um arquivo auxiliar e o abre como escrita
    while ((fgets(buf, MAXDATASIZE, file)) != NULL) { // Le cada linha do arquivo
        buf[strcspn(buf, "\n")] = 0; // Indica o fim da string
        if (count == 1) { // Incrementa quantidade de filmes no arquivo
            n = atoi(buf) - 1;
            sprintf(buf, "%i", n);
            fputs(buf, temp);
        } else if (line != count) { // Caso não seja a linha a ser deletada
            strcpy(movie,"\n");
            strcat(movie, buf);
            fputs(movie, temp); // Insere no arquivo
        }
        count++;
    }

    fclose(file); // fecha arquivo
    fclose(temp); // fecha arquivo
    remove(FILENAME); // apaga o arquivo original
    rename(TEMPFILENAME, FILENAME); // renomeia o arquivo temporário pro nome original
}

// Opção 1: Adiciona um novo filme à lista do arquivo
int newMovie(char* code, struct sockaddr_in cliaddr, char* movie) {
  char buf[MAXDATASIZE];
  int count = 1;
  int n;

  file = fopen(FILENAME, "r"); // Abre o arquivo como leitura
  FILE *temp = fopen(TEMPFILENAME, "w"); // Cria um arquivo auxiliar e o abre como escrita
  while ((fgets(buf, MAXDATASIZE, file)) != NULL) { // Le cada linha do arquivo
      if (count == 1) { // Aumenta a quantidade de filmes no arquivo
          n = atoi(buf) + 1;
          sprintf(buf, "%i\n", n);
      }
      fputs(buf, temp); // Insere no arquivo temporário
      count++;
  }
  // Adiciona novo filme
  char * lastId = strtok(buf, "|"); // le qual é o ultimo id
  n = atoi(lastId) + 1; // acrescenta ao número
  sprintf(buf, "\n%i", n);
  strcat(buf, "|"); // concatena com separados
  strcat(buf, movie); // concatena com dados do filme
  fputs(buf, temp); // Insere no arquivo temporário

  fclose(file); // fecha arquivo
  fclose(temp); // fecha arquivo
  remove(FILENAME); // apaga o arquivo original
  rename(TEMPFILENAME, FILENAME); // renomeia o arquivo temporário pro nome original

  char all[MAXDATASIZE] = "";
  strcat(all, code);
  strcat(all, "_/ok");
  if (sendClient(all, cliaddr) == -1) {
    perror("failed on sendto");
    exit(1);
  }

  return 0;
}

// Opção 2: Adiciona um novo genero ao filme indicado
int newGenderInMovie(char* code, struct sockaddr_in cliaddr, char* args) {
  char buf[MAXDATASIZE];
  char movie[MAXDATASIZE];
  char* id = strtok(args, "|"); // primeiro argumento é o id
  char* gender = strtok(NULL, "\n"); // segundo argumento é o genero
  int movieExists = 0;
  int genderExists = 0;
  int count = 0;

  file = fopen(FILENAME, "r"); // Abre o arquivo como leitura
  FILE *temp = fopen(TEMPFILENAME, "w"); // Cria um arquivo auxiliar e o abre como escrita

  fgets(buf, MAXDATASIZE, file); // Le quantos filmes existem no arquivo
  int movieQuantity = atoi(buf);
  fputs(buf, temp); // insere a linha no arquivo temporário
  fgets(buf, MAXDATASIZE, file); // Le o header do arquivo
  fputs(buf, temp); // insere a linha no arquivo temporário

  while ((fgets(buf, MAXDATASIZE, file)) != NULL) { // Le cada linha do arquivo
    count++;
    strcpy(movie, buf); // copia o buffer
    char * item = strtok(buf, "|"); // id

    if (strcmp(id, item) == 0) { // Se é esse o filme que queremos editar
      movieExists = 1;
      item = strtok(NULL, "|"); // título
      item = strtok(NULL, "|"); // diretor
      item = strtok(NULL, "|"); // ano
      item = strtok(NULL, "|"); // genero
      char * currGender = strtok(item, ",");
      while (currGender) { // Para cada genero da lista
        currGender[strcspn(currGender, "\n")] = 0; // Indica o fim da string
        if (strcmp(currGender, gender) == 0) { // Se o genero já existe
          genderExists = 1;
          break;
        }
        currGender = strtok(NULL, ",");
      }
      if (!genderExists) { // Se o genero não existe, adiciona o genero no filme
        movie[strcspn(movie, "\n")] = 0; // Indica o fim da string
        strcat(movie, ",");
        strcat(movie, gender);
        if (movieQuantity != count) {
          strcat(movie, "\n");
        }
      }
    }

    fputs(movie, temp); // insere a linha no arquivo temporário
  }

  fclose(file); // fecha arquivo
  fclose(temp); // fecha arquivo
  remove(FILENAME); // apaga o arquivo original
  rename(TEMPFILENAME, FILENAME); // renomeia o arquivo temporário pro nome original

  char all[MAXDATASIZE] = "";
  strcat(all, code);
  strcat(all, "_");

  if (genderExists || !movieExists) {
    strcat(all, "/failed");
  } else {
    strcat(all, "/ok");
  }

  if (sendClient(all, cliaddr) == -1) { // Se o genero já existe ou o filme não foi encontrado, envia que houve um erro
    perror("failed on sendto");
    fclose(file); // fecha o arquivo
    exit(1);
  }

  return 0;
}

// Opção 3: Retorna todos os filmes e seus respectivos identificadores
int getMoviesTitleId(char* code, struct sockaddr_in cliaddr) { 
  char buf[MAXDATASIZE], movie[MAXDATASIZE], all[MAXDATASIZE] = "";

  strcat(all, code);
  strcat(all, "_");

  file = fopen(FILENAME, "r"); // Abre o arquivo como leitura
  fgets(buf, MAXDATASIZE, file); // Le quantos filmes existem no arquivo
  fgets(buf, MAXDATASIZE, file); // Le o header do arquivo

  while (fgets(buf, MAXDATASIZE, file) != NULL) { // Le cada filme do arquivo
    char * item = strtok(buf, "|"); // id
    strcpy(movie, item);            // adiciona o id à mensagem de retorno
    item = strtok(NULL, "|");       // título
    strcat(movie, "|");             // adiciona o separador à mensagem de retorno
    strcat(movie, item);            // adiciona o título à mensagem de retorno

    movie[strcspn(movie, "\n")] = 0;
    strcat(all, movie);
    strcat(all, "_");
  }
  strcat(all, "end");

  if (sendClient(all, cliaddr) == -1) {
    perror("failed on sendto");
    fclose(file); // fecha o arquivo
    exit(1);
  }

  fclose(file); // fecha o arquivo
  return 0;
}

// Opção 4: Retorna todos os filmes (título, diretor e ano) de um determinado genero
int getMoviesFromGender(char* code, struct sockaddr_in cliaddr, char* gender) {
  char buf[MAXDATASIZE], movie[MAXDATASIZE], all[MAXDATASIZE] = "";

  strcat(all, code);
  strcat(all, "_");

  file = fopen(FILENAME, "r"); // Abre o arquivo como leitura
  fgets(buf, MAXDATASIZE, file); // Le quantos filmes existem no arquivo
  fgets(buf, MAXDATASIZE, file); // Le o header do arquivo

  while (fgets(buf, MAXDATASIZE, file) != NULL) { // Le cada filme do arquivo
    char * item = strtok(buf, "|"); // id
    item = strtok(NULL, "|");       // título
    strcpy(movie, item);
    item = strtok(NULL, "|");       // diretor
    strcat(movie, "|");
    strcat(movie, item);
    item = strtok(NULL, "|");       // ano
    strcat(movie, "|");
    strcat(movie, item);
    item = strtok(NULL, "|");       // genero
    char * currGender = strtok(item, ",");
    while (currGender) { // Enquanto tiver mais generos para serem lidos
      currGender[strcspn(currGender, "\n")] = 0; // Finaliza a string caso tenha um \n
      if (strcmp(currGender, gender) == 0) { // Se tiver o genero no filme
        movie[strcspn(movie, "\n")] = 0;
        strcat(all, movie);
        strcat(all, "_");
        break;
      }
      currGender = strtok(NULL, ",");
    }
  }
  strcat(all, "end");

  if (sendClient(all, cliaddr) == -1) {
    perror("failed on sendto");
    fclose(file); // fecha o arquivo
    exit(1);
  }

  fclose(file); // fecha o arquivo
  return 0;
}

// Opção 5: Retorna todos os filmes
int getAllMovies(char* code, struct sockaddr_in cliaddr) {   
  char buf[MAXDATASIZE], all[MAXDATASIZE] = "";

  strcat(all, code);
  strcat(all, "_");

  file = fopen(FILENAME, "r"); // Abre o arquivo como leitura
  fgets(buf, MAXDATASIZE, file); // Le quantos filmes existem no arquivo
  fgets(buf, MAXDATASIZE, file); // Le o header do arquivo

  while (fgets(buf, MAXDATASIZE, file) != NULL) { // Le cada filme do arquivo
    buf[strcspn(buf, "\n")] = 0;
    strcat(all, buf);
    strcat(all, "_");
  }
  strcat(all, "end");

  if (sendClient(all, cliaddr) == -1) {
    perror("failed on sendto");
    fclose(file); // fecha o arquivo
    exit(1);
  }

  fclose(file); // fecha o arquivo
  return 0;
}

// Opção 6: Retorna o filme de um determinado identificador
int getMovie(char* code, struct sockaddr_in cliaddr, char* id) {
  char buf[MAXDATASIZE];
  char movie[MAXDATASIZE];
  char all[MAXDATASIZE] = "";

  strcat(all, code);
  strcat(all, "_");

  file = fopen(FILENAME, "r"); // Abre o arquivo como leitura
  fgets(buf, MAXDATASIZE, file); // Le quantos filmes existem no arquivo
  fgets(buf, MAXDATASIZE, file); // Le o header do arquivo

  while (fgets(buf, MAXDATASIZE, file) != NULL) { // Le cada filme do arquivo
    strcpy(movie, buf); // Coloca o ponteiro do movie no atual do buffer
    char * currId = strtok(buf, "|"); // Pega o primeiro item, que é o id
    if (strcmp(currId, id) == 0) { // Caso seja o id desejado
      strcat(all, movie);
      if (sendClient(all, cliaddr) == -1) { // Envia o filme
        perror("failed on sendto");
        fclose(file); // fecha o arquivo
        exit(1);
      }
      fclose(file); // fecha o arquivo
      return 0;
    }
  }
  strcat(all, "/notfound");

  if (sendClient(all, cliaddr) == -1) { // Envia que houve um erro
    perror("failed on sendto");
    fclose(file); // fecha o arquivo
    exit(1);
  }
  
  fclose(file); // fecha o arquivo
  return 0;
}

// Opção 7: Remove o filme de um determinado identificador
int removeMovie(char* code, struct sockaddr_in cliaddr, char* id) {
  char buf[MAXDATASIZE];
  int line = 3;
  char all[MAXDATASIZE] = "";

  strcat(all, code);
  strcat(all, "_");

  file = fopen(FILENAME, "r"); // Abre o arquivo como leitura
  fgets(buf, MAXDATASIZE, file); // Le quantos filmes existem no arquivo
  fgets(buf, MAXDATASIZE, file); // Le o header do arquivo

  while (fgets(buf, MAXDATASIZE, file) != NULL) { // Le cada filme do arquivo
    char * currId = strtok(buf, "|"); // Pega o primeiro item, que é o id
    if (strcmp(currId, id) == 0) { // Se for o id desejado
      deleteLine(line); // Apaga a linha daquele filme
      strcat(all, "/ok");
      if (sendClient(all, cliaddr) == -1) { // Envia que deu certo
        perror("failed on sendto");
        fclose(file); // fecha o arquivo
        exit(1);
      }
      fclose(file); // fecha o arquivo
      return 0;
    }
    line++;
  }

  strcat(all, "/failed");
  if (sendClient(all, cliaddr) == -1) {
    perror("failed on sendto");
    fclose(file); // fecha o arquivo
    exit(1);
  }

  fclose(file); // fecha o arquivo
  return 1;
}

// Lida com cada cliente conectado, entendendo cada mensagem recebida e chamando a ação a ser feita
int handleOptions(char * buffer, struct sockaddr_in cliaddr) {
  // Pega o primeiro item na mensagem que é a função desejada
  char * item = strtok(buffer, "|");
  char * code;
  strcpy(code,item);
  item = strtok(NULL, "|"); // le a opção

  // Estuda qual é a opção desejada
  switch (item[0]) {
    case '1':
      item = strtok(NULL, "\n"); // le demais argumentos
      newMovie(code, cliaddr, item);
      break;
    case '2':
      item = strtok(NULL, "\n"); // le demais argumentos
      newGenderInMovie(code, cliaddr, item);
      break;
    case '3':
      getMoviesTitleId(code, cliaddr);
      break;
    case '4':
      item = strtok(NULL, "\n"); // le demais argumentos
      getMoviesFromGender(code, cliaddr, item);
      break;
    case '5':
      getAllMovies(code, cliaddr);
      break;
    case '6':
      item = strtok(NULL, "\n"); // le demais argumentos
      getMovie(code, cliaddr, item);
      break;
    case '7':
      item = strtok(NULL, "\n"); // le demais argumentos
      removeMovie(code, cliaddr, item);
      break;
    default: // qualquer outra opção, ele sai
      return 0;
  }

  return 0;
}

// Driver code
int main() {
  struct sockaddr_in cliaddr;
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
		
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);
		
	// Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	
	len = sizeof(cliaddr); //len is value/result

  garanteeNumberAndHeaderLinesOnFile();
  
  // handling non blocking
  FD_ZERO(&rset); // clear the set ahead of time
  maxfdp = sockfd +1;
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  while (1) {
    FD_SET (sockfd, &rset); // add our descriptors to the set

    nready = select (maxfdp, &rset, NULL, NULL, &tv);

    if (nready == -1) {
      perror("select"); // error occurred in select()
      exit(1);
    } else if (nready == 0) {
      printf("Tempo limite excedido. Não foi recebido nenhum dado depois de 5 segundos.\n");
      continue;
    } else if (!FD_ISSET (sockfd, &rset)) {
      exit(1);
    }

  	if ((n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len)) == -1) {
      perror("failed on recvfrom");
      exit(1);
    }
  	buffer[n] = '\0';

    handleOptions(buffer, cliaddr); // Trata dos dados recebidos pelo cliente
  }
	
	return 0;
}
