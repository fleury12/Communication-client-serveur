#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>

#define MAX_FILENAME_LEN 256
#define BUFFER_SIZE 4096


char filename[MAX_FILENAME_LEN];

/*
aant je recevais et affichais les 
void receive_and_print_file(int socket)
{
    
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    bytes_received = recv(socket, buffer, sizeof(buffer), 0);
    printf("est ce que ya buff  %s", buffer);
    if (bytes_received == 0)
    {
        // La connexion a été fermée du côté distant
        printf("La connexion a été fermée par le serveur.\n");
    }
    else if (bytes_received == -1)
    {
        // Erreur lors de la réception
        perror("Erreur lors de la réception des données du serveur");
        exit(EXIT_FAILURE);
    }else if (bytes_received > 0)
    {
        printf(" \nbit de la boucle %ld \n",bytes_received);
        // Traitez les données reçues ici, par exemple, en les écrivant sur la sortie standard
        if (write(STDOUT_FILENO, buffer, bytes_received) == -1)
        {
            perror("Erreur lors de l'écriture sur la sortie standard");
            exit(EXIT_FAILURE);
        }
        printf("\nFichier reçu avec succès.\n");
    }

    

    // Le reste de votre code...
    printf("\nFin de l'ecriture.\n");
}
*/

int main()
{
    //je définis ma socket client.
    int socketClient = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrClient;
    addrClient.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrClient.sin_family = AF_INET;
    addrClient.sin_port = htons(30000);

    // Connexion
    if (connect(socketClient, (const struct sockaddr *)&addrClient, sizeof(addrClient)) == -1)
    {
        perror("Erreur de connexion");
        exit(EXIT_FAILURE);
    }
    printf("Connecté\n");

    printf("Entrez le nom du fichier à rechercher : ");
    scanf("%255s", filename);

    // Envoi du nom du fichier au serveur
    send(socketClient, &filename, sizeof(filename), 0);

    // Réception de la réponse du serveur
    char response[BUFFER_SIZE];
    ssize_t bytes_received = recv(socketClient, response, sizeof(response) - 1, 0);

    if (bytes_received == -1)
    {
        perror("Erreur lors de la réception de la réponse du serveur");
        exit(EXIT_FAILURE);
    }
    else if (bytes_received == 0)
    {
        printf("La connexion a été fermée par le serveur. dans le main\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        //
        response[bytes_received] = '\0';
        printf("Réponse du serveur : %s\n", response);

        // Si le serveur indique que le fichier a été trouvé, recevoir et afficher le fichier
        if (strcmp(response, "Fichier trouvé") == 0)
        {
            printf("Attente de la réception du fichier...\n");

            char contenu[BUFFER_SIZE];
            ssize_t bytes_received = recv(socketClient, contenu, sizeof(contenu) - 1, 0); //je reçoit
            contenu[bytes_received] = '\0';
            printf("contenu du serveur : %s\n", contenu);


            //receive_and_print_file(socketClient); 
            //j'appelle la fonction qui reçoit le fichier
            printf("fin.")
            
            ;}
        else
            {
            printf("Le serveur n'a pas trouvé le fichier.\n");
            }
    }

    close(socketClient);
    return 0;
}
