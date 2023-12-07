#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <dirent.h>

#define MAX_FILENAME_LEN 256
#define DIRECTORY_PATH "."
#define CACHE_SIZE 3  // Nombre de fichiers à mettre en cache


typedef struct User
{
    char filename[MAX_FILENAME_LEN];
} User;
//cette fonction envoie le fichier au client et prend en paramètre le nom du fichier et la socketclient
void send_file(int socket, const char *filename)
{


    // j'ouvre le fichier et je l'atribue à un pointeur
    FILE *file = fopen(filename, "rb");
    if (!file) //je verivie si le fichier a bien été ouvert
    {
        char error_msg[] = "Erreur lors de l'ouverture du fichier.";
        send(socket, error_msg, strlen(error_msg) + 1, 0);
        return;
    }
    //je me positionne à la fin du fichier
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file); //je déterminer la taille du dontenu
    fseek(file, 0, SEEK_SET); //je me replace au debut du fichier
    
    char *file_buffer = malloc(file_size); //j'attribue un espace mémoire de taille du fichier
    fread(file_buffer, 1, file_size, file);
    
    //des print que je fais pour des vérifications
    printf("le buffer %s \n", file_buffer);
    printf("la taile %ld \n", file_size);

    // Envoyer le fichier au client
    printf("je commence l'envoie \n");
    
    send(socket, file_buffer, file_size, 0);
    
    free(file_buffer); // je désalloue l'espace mémoire du file buffer
}

void *function(void *arg)
{
    int socket = *(int *)arg;
    User user;
    recv(socket, &user, sizeof(user), 0);

    // Recherche du fichier dans le répertoire
    DIR *dir;
    struct dirent *ent;
    int file_found = 0;

    if ((dir = opendir(DIRECTORY_PATH)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (strcmp(ent->d_name, user.filename) == 0)
            {
                file_found = 1;
                break;
            }
        }
        closedir(dir);
    }

    // Envoyer le fichier si trouvé, sinon envoyer un message d'erreur
    if (file_found)
    {
        send(socket, "Fichier trouvé", strlen("Fichier trouvé") + 1, 0);
        send_file(socket, user.filename);
    }
    else
    {
        send(socket, "Aucun fichier trouvé", strlen("Aucun fichier trouvé") + 1, 0);
    }
//desallocation du la mémoire allouée à arg
    free(arg);
    pthread_exit(NULL);
}
int main()
{
    pthread_t threads[3]; //les thread attendu par le serveur
    //définition de la socket serveur
    int socketServer = socket(AF_INET, SOCK_STREAM, 0); // ipv4, en mode connecté
    struct sockaddr_in addrServer;
    addrServer.sin_addr.s_addr = inet_addr("127.0.0.1"); // communication via localhost
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(30000);

    //construction de la socket serveur
    bind(socketServer, (const struct sockaddr *)&addrServer, sizeof(addrServer));
    printf("Bind: %d\n", socketServer);

    // Écoute
    listen(socketServer, 5);
    printf("Listen\n");

    for (int i = 0; i < 3; i++)
    {
        //définit la socket client
        struct sockaddr_in addrClient;
        socklen_t csize = sizeof(addrClient);
        //accepter la connexion
        int socketClient = accept(socketServer, (struct sockaddr *)&addrClient, &csize);
        printf("Accept: %d\n", socketClient);

        int *arg = malloc(sizeof(int));  //aloue l'espace mémoire
        *arg = socketClient;
        pthread_create(&threads[i], NULL, function, arg); //création du thread
    }

    // mettre les threadfs en attente
    for (int i = 0; i < 3; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Fermer la socket du serveur
    close(socketServer);

    printf("Close\n");


    return 0;
}
