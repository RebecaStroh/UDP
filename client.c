/*
** client.c => arquivo que requisita a conexão deste cliente com o servidor,
*        trata das entradas, transformando-as em mensagens a serem compreendidas pelo servidos
*        e trata das mensagens recebidas do servidor para dar um feedback par ao cliente
*        segundo o protocolo UDP
*   Autor: Rebeca Portes Stroh, 186407
*/

// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>

#define PORT	 8080
#define MAXDATASIZE 150 // Número máximo de bytes transferidos na mensagem
#define MAXTITLE 40     // Número máximo de bytes em um título
#define MAXDIRECTOR 40  // Número máximo de bytes para um diretor
#define MAXGENDER 20    // Número máximo de bytes em um genero
#define MAXYEAR 5       // Número máximo de bytes de um ano
#define MAXID 5         // Número máximo de bytes de um id
#define MAXNGENDER 3    // Número máximo de generos por filme

int sockfd, n;
char buffer[MAXDATASIZE];
struct sockaddr_in servaddr;
socklen_t len;

int sendServer(char * message) {
 return sendto(sockfd, message, strlen(message), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr)) == -1;
}

// Opção 1: Solicita a adição de um filme
int newMovie() {
    char title[MAXTITLE];
    char gender[MAXGENDER*MAXNGENDER];
    char director[MAXDIRECTOR];
    char year[MAXYEAR];
    char c;

    // Adiciona a função desejada à mensagem
    char msg[MAXDATASIZE] = "1|";
    // Adiciona o título à mensagem, lendo-o
    printf("Escreva um título: ");
    fflush (stdin);
    fgets(title, MAXTITLE, stdin);
    title[strcspn(title, "\n")] = 0;
    strcat(msg, title);
    strcat(msg,"|");
    // Adiciona o diretor à mensagem, lendo-o
    printf("Escreva o nome do diretor: ");
    fflush (stdin);
    fgets(director, MAXDIRECTOR, stdin);
    director[strcspn(director, "\n")] = 0;
    strcat(msg, director);
    strcat(msg,"|");
    // Adiciona o ano à mensagem, lendo-o
    printf("Escreva um ano: ");
    fflush (stdin);
    fgets(year, MAXYEAR, stdin);
    year[strcspn(year, "\n")] = 0;
    strcat(msg, year);
    strcat(msg,"|");
    // Adiciona o(s) genero(s) à mensagem, lendo-o(s)
    do {c = getchar();} while (c != EOF && c != '\n');
    printf("Escreva os generos: ");
    fflush (stdin);
    fgets(gender, MAXGENDER*MAXNGENDER, stdin);
    gender[strcspn(gender, "\n")] = 0;
    for (int i = 0; i < strlen(gender); i++) {
        gender[i] = tolower((unsigned char) gender[i]);
    }
    strcat(msg, gender);

    printf("It should send: %s", msg);

  // Envia a mensagem requisitada
	if (sendServer(msg)) {
    perror("failed on sendto");
    exit(1);
  }

    // // Espera a resposta do servidor
    // if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    //     perror("recv");
    //     return 1;
    // }
    // if (numbytes == 0) {
    //     printf("\nA conexão foi perdida com o servidor\n");
    //     return 1;
    // }

    // if (buf[0] == '1') { // Caso o filme tenha sido adicionado com sucesso
    //     printf("\nFilme adicionado com sucesso!\n");
    // } else { // Caso houveram problemas ao adicionar o filme
    //     printf("\nHouve algum problema ao adicionar o seu filme...\n");
    // }

    return 0;
}

// Opção 2: Solicita a adição de um novo genero a um filme dado
int newGenderInMovie() {
    char gender[MAXGENDER];
    char id[MAXID];
    char c;

    // Adiciona a função desejada à mensagem
    char msg[MAXDATASIZE] = "2|";
    // Adiciona o id à mensagem, lendo-o
    printf("Escreva o id do filme: ");
    fflush (stdin);
    fgets(id, MAXID, stdin);
    id[strcspn(id, "\n")] = 0;
    strcat(msg, id);
    strcat(msg,"|");
    // Adiciona o genero à mensagem, lendo-o
    printf("Escreva o genero desejado: ");
    fflush (stdin);
    fgets(gender, MAXGENDER, stdin);
    gender[strcspn(gender, "\n")] = 0;
    for (int i = 0; i < strlen(gender); i++) {
        gender[i] = tolower((unsigned char) gender[i]);
    }
    strcat(msg, gender);

    printf("It should send: %s", msg);

  // Envia a mensagem requisitada
	if (sendServer(msg)) {
    perror("failed on sendto");
    exit(1);
  }

    // // Espera a resposta do servidor
    // if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    //     perror("recv");
    //     return 1;
    // }
    // if (numbytes == 0) {
    //     printf("\nA conexão foi perdida com o servidor\n");
    //     return 1;
    // }

    // if (buf[0] == '1') { // Se a adição foi feita com sucesso
    //     printf("\nGenero adicionado com sucesso!\n");
    // } else { // Caso encontrou-se problemas na adição
    //     printf("\nFilme com id inexistente ou genero já existe neste filme\n");
    // }

    return 0;
}

// Opção 3: Solicita todos os filmes e seus respectivos identificadores
int getMoviesTitleId() {
    // Envia a mensagem requisitada
    printf("It should send: 3");

  char * msg = "3";

  // Envia a mensagem requisitada
	if (sendServer(msg)) {
    perror("failed on sendto");
    exit(1);
  }

    // // Espera a resposta do servidor com o numero de filmes a serem enviados
    // if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    //     perror("recv");
    //     return 1;
    // }
    // if (numbytes == 0) {
    //     printf("\nA conexão foi perdida com o servidor\n");
    //     return 1;
    // }

    // // Indica o fim da string
    // buf[numbytes] = '\0';

    // // converte pata int o número de filmes a ser enviado
    // int n = atoi(buf);

    // if (n == 0) { // Caso ainda não existam filmes cadastrados
    //     printf("\nNão existem filmes adicionados ainda! \n");
    // } else { // Le e mostra os filmes cadatrados
    //     printf("\nExistem %i filmes cadastrados: \n", n);

    //     for (int i=0; i<n; i++) {
    //         // Espera a resposta do servidor com um filme
    //         if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    //             perror("recv");
    //             return 1;
    //         }
    //         if (numbytes == 0) {
    //             printf("\nA conexão foi perdida com o servidor\n");
    //             return 1;
    //         }

    //         // Indica o fim da string
    //         buf[numbytes] = '\0';

    //         // Printa o filme
    //         printf("  => ");
    //         char * item = strtok(buf, "|"); // id
    //         printf("id: %s, título: ", item);
    //         item = strtok(NULL, "\n"); // title
    //         printf("%s\n", item);
    //     }
    // }
    return 0;
}

// Opção 4: Solicita dados (título, diretor e ano) de filmes de um dado genero
int getMoviesFromGender() {
    char gender[MAXGENDER];

    // Adiciona a função desejada à mensagem
    char msg[MAXDATASIZE] = "4|";
    // Adiciona o genero à mensagem, lendo-o
    printf("Escreva o genero desejado: ");
    fflush (stdin);
    fgets(gender, MAXGENDER, stdin);
    gender[strcspn(gender, "\n")] = 0;
    for (int i = 0; i < strlen(gender); i++) {
        gender[i] = tolower((unsigned char) gender[i]);
    }
    strcat(msg, gender);

    printf("It should send: %s", msg);

  // Envia a mensagem requisitada
	if (sendServer(msg)) {
    perror("failed on sendto");
    exit(1);
  }

    // // Espera uma mensagem, que pode ser um outro filme ou indicio que não tem filme com tal genero (/end)
    // if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    //     perror("recv");
    //     return 1;
    // }
    // if (numbytes == 0) {
    //     printf("\nA conexão foi perdida com o servidor\n");
    //     return 1;
    // }

    // // Indica o fim da string
    // buf[numbytes] = '\0';

    // if (strcmp(buf, "/end") == 0) {
    //     printf("\nNão existem filmes adicionados nesse genero! \n");
    // } else {
    //     printf("\nOs filmes com o genero %s são: \n", gender);

    //     while (strcmp(buf, "/end") != 0) { // Enquanto não for o fim da listagem
    //         // Printa dados do filme
    //         printf("  => ");
    //         char * item = strtok(buf, "|"); // título
    //         printf("título: %s, ", item);
    //         item = strtok(NULL, "|");       // diretor
    //         printf("diretor: %s, ", item);
    //         item = strtok(NULL, "\n");      // ano
    //         printf("ano: %s\n", item);

    //         // Espera próxima mensagem, que pode ser um outro filme ou o fim da listagem (/end)
    //         if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    //             perror("recv");
    //             return 1;
    //         }
    //         if (numbytes == 0) {
    //             printf("\nA conexão foi perdida com o servidor\n");
    //             return 1;
    //         }

    //         // Indica o fim da string
    //         buf[numbytes] = '\0';
    //     }
    // }
    return 0;
}

// Opção 5: Solicita os dados de todos os filmes
int getAllMovies() {
  printf("It should send: 5");

  char * msg = "5";

  // Envia a mensagem requisitada
	if (sendServer(msg)) {
    perror("failed on sendto");
    exit(1);
  }

  // // Espera a resposta do servidor com o numero de filmes a serem enviados
  // if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
  //     perror("recv");
  //     return 1;
  // }
  // if (numbytes == 0) {
  //     printf("\nA conexão foi perdida com o servidor\n");
  //     return 1;
  // }

  // // Indica o fim da string
  // buf[numbytes] = '\0';
  
  // // converte pata int o número de filmes a ser enviado
  // int n = atoi(buf);

  // if (n == 0) { // Caso ainda não existam filmes cadastrados
  //     printf("\nNão existem filmes adicionados ainda! \n");
  // } else { // Le e mostra os filmes cadatrados
  //     printf("\nExistem %i filmes cadastrados: \n", n);

  //     for (int i=0; i<n; i++) {
  //         // Espera a resposta do servidor com um filme
  //         if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
  //             perror("recv");
  //             return 1;
  //         }
  //         if (numbytes == 0) {
  //             printf("\nA conexão foi perdida com o servidor\n");
  //             return 1;
  //         }

  //         // Indica o fim da string
  //         buf[numbytes] = '\0';

  //         // Printa o filme
  //         printf("  => ");
  //         char * item = strtok(buf, "|"); // id
  //         printf("id: %s, ", item);
  //         item = strtok(NULL, "|");       // título
  //         printf("título: %s, ", item);
  //         item = strtok(NULL, "|");       // diretor
  //         printf("diretor: %s, ", item);
  //         item = strtok(NULL, "|");       // ano
  //         printf("ano: %s, ", item);
  //         item = strtok(NULL, "\n");      // genero
  //         printf("generos: %s\n", item);
  //     }
  // }
  return 0;
}

// Opção 6: Solicita as informações de um filme
int getMovie() {
    char id[MAXID];

    // Adiciona a função desejada à mensagem
    char msg[MAXDATASIZE] = "6|";
    // Adiciona o id à mensagem, lendo-o
    printf("Escreva o id do filme: ");
    fflush (stdin);
    fgets(id, MAXID, stdin);
    id[strcspn(id, "\n")] = 0;
    strcat(msg, id);

    printf("It should send: %s", msg);

    // Envia a mensagem requisitada
  	if (sendServer(msg)) {
      perror("failed on sendto");
      exit(1);
    }

    // // Espera a resposta do servidor
    // if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    //     perror("recv");
    //     return 1;
    // }
    // if (numbytes == 0) {
    //     printf("\nA conexão foi perdida com o servidor\n");
    //     return 1;
    // }

    // // Indica o fim da string
    // buf[numbytes] = '\0';

    // if (strcmp(buf, "-1") == 0) { // Se não achou
    //     printf("\nNão existe nenhum filme com esse id!\n");
    // } else { // Se achou, printa as informações
    //     printf("\nSegue as informações do filme desejado:\n");
    //     printf("  => ");
    //     char * item = strtok(buf, "|"); // id
    //     printf("id: %s, ", item);
    //     item = strtok(NULL, "|");       // título
    //     printf("título: %s, ", item);
    //     item = strtok(NULL, "|");       // diretor
    //     printf("diretor: %s, ", item);
    //     item = strtok(NULL, "|");       // ano
    //     printf("ano: %s, ", item);
    //     item = strtok(NULL, "\n");      // generos
    //     printf("generos: %s\n", item);
    // }

    return 0;
}

// Opção 7: Solicita a remoção de um filme a partir de um identificador
int removeMovie() {
    char id[MAXID];

    // Adiciona a função desejada à mensagem
    char msg[MAXDATASIZE] = "7|";
    // Adiciona o id à mensagem, lendo-o
    printf("Escreva o id do filme: ");
    fflush (stdin);
    fgets(id, MAXID, stdin);
    id[strcspn(id, "\n")] = 0;
    strcat(msg, id);

    printf("It should send: %s", msg);

    // Envia a mensagem requisitada
  	if (sendServer(msg)) {
      perror("failed on sendto");
      exit(1);
    }

    // // Espera a resposta do servidor
    // if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    //     perror("recv");
    //     return 1;
    // }
    // if (numbytes == 0) {
    //     printf("\nA conexão foi perdida com o servidor\n");
    //     return 1;
    // }

    // // Indica o fim da string
    // buf[numbytes] = '\0';

    // if (buf[0] == '0') { // Se não deu certo
    //     printf("\nNão existe um filme com tal id\n");
    // } else { // Se deu certo
    //     printf("\nFilme de id %s deletado com sucesso\n", id);
    // }
    return 0;
}

// Função que cuida de tratar a escolha do usuário e printar as opções
int menu() {
  while (1) {
    // Imprime opções possíveis
    printf("------------------------------------------------------------ \n");
    printf("Escolha uma das ações para realizar: \n");
    printf("    1 - Cadastrar um novo filme, determinando um identificador numérico no cadastro \n");
    printf("    2 - Acrescentar um novo gênero em um filme \n");
    printf("    3 - Listar todos os títulos, junto a seus respectivos identificadores \n");
    printf("    4 - Listar informações (título, diretor(a) e ano) de todos os filmes de um determinado gênero \n");
    printf("    5 - Listar todas as informações de todos os filmes \n");
    printf("    6 - Listar todas as informações de um filme a partir de seu identificador \n");
    printf("    7 - Remover um filme a partir de seu identificador \n");
    printf("    Qualquer Outra Tecla - Sair do programa \n");
    printf("------------------------------------------------------------ \n \n");

    // Le opção escolhida
    fflush (stdin);
    char option, c;
    printf("Opção: ");
    int success = scanf("%c", &option);
    do {c = getchar();} while (c != EOF && c != '\n');

    // Se a opção escolhida não for válida, encerra a sessão, avisando o servidor
    if ((option != '1' && option != '2' && option != '3' && option != '4' && option != '5' && option != '6' && option != '7') || !success){
      break;
    }

    // Verifica que opção foi escolhida
    switch (option) {
      case '1': newMovie(); break;
      case '2': newGenderInMovie(); break;
      case '3': getMoviesTitleId(); break;
      case '4': getMoviesFromGender(); break;
      case '5': getAllMovies(); break;
      case '6': getMovie(); break;
      case '7': removeMovie(); break;
      default: return 0;
    }

  	if ((n = recvfrom(sockfd, (char *)buffer, MAXDATASIZE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len)) == -1) {
      perror("failed on recvfrom");
      exit(1);
    }
  	buffer[n] = '\0';
  	printf("Received from server : %s\n", buffer);

    printf("\n");
    system("read -p 'Pressione alguma tecla para continuar...' var");
  }
  return 0;
}

// Driver code
int main() {
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = INADDR_ANY;

  menu();
	
	close(sockfd);
	return 0;
}