#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(){
    int serveur_fd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in adresse_serveur;
    serveur_fd = socket(AF_INET,SOCK_STREAM,0);
    inet_pton(AF_INET,"127.0.0.1",&adresse_serveur.sin_addr);
    adresse_serveur.sin_family = AF_INET;
    adresse_serveur.sin_port = htons(PORT);
    int conn = connect(serveur_fd,(struct sockaddr*)&adresse_serveur,sizeof(adresse_serveur));
    if(conn == -1){
        perror("Erreur de connexion");
        return -1;
    }
    printf("Connexion reussi\n");
    printf("Bienvenue\nQue voulez vous ?\n-AJOUTER <nom> <age> <groupe_sanguin> <telephone>\n-CONSULTER <nom>\n-LISTER\n-GROUPE <nom du groupe sanguin>\nSUPPRIMER <nom>\nListe des groupes sanguins\nA+,A-,B+,B-,AB+,AB-,O+,O-\n-Tapez exit (Pour quitter)\n");
    while(1){
        memset(buffer,0,BUFFER_SIZE);
        printf(">> ");
        fgets(buffer,BUFFER_SIZE,stdin);

        ssize_t snd = send(serveur_fd,buffer,strlen(buffer),0);
        if(snd < 0){perror("Erreur");break;}
        if(strncmp(buffer,"exit",4) == 0){
            printf("Arret en cours...\n");
            return -1;
        }
        printf("\n");
        memset(buffer,0,BUFFER_SIZE);
        ssize_t receive = recv(serveur_fd,buffer,BUFFER_SIZE-1,0);
        if(receive < 0){perror("Erreur");break;}
        buffer[receive] = '\0';
        printf(">>Serveur: %s\n",buffer);
    }
    printf("Serveur inaccessible\n");
    close(serveur_fd);
    return 0;
}