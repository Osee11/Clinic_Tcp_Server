#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


#define PORT 8080
#define BUFFER_SIZE 1024

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


typedef struct ArgTh{
    int client_fd;
}ArgTh;

/*typedef struct Patient{
    char nom[50];
    char age[20];
    char telephone[50];
    char groupe[10];
    struct Patient *gauche;
    struct Patient *droite;
}Patient;*/

typedef struct Patient {
    char nom[50];
    int  age;
    char groupe_sanguin[5];
    char telephone[20];
    char date_ajout[30];  // rempli automatiquement avec time.h
    struct Patient *gauche;
    struct Patient *droite;
} Patient;

Patient *creerptt(char *nom,int age,char *telephone,char *groupe,char *date_creation){
    Patient *pt = malloc(sizeof(Patient));
    if(pt == NULL)return NULL;
    strcpy(pt->nom,nom);
    pt->age = age;
    strcpy(pt->telephone,telephone);
    strcpy(pt->groupe_sanguin,groupe);
    strcpy(pt->date_ajout,date_creation);
    pt->gauche = NULL;
    pt->droite = NULL;
    return pt;
}
Patient *inserer(Patient *racine,char *nom,int age,char *telephone,char *groupe,char *date_creation){
    if(racine == NULL){
        racine = creerptt(nom,age,telephone,groupe,date_creation);
        return racine;
    }else{
        int strc = strcmp(nom,racine->nom);
        if(strc < 0){
            racine->gauche = inserer(racine->gauche,nom,age,telephone,groupe,date_creation);
        }else{
            racine->droite = inserer(racine->droite,nom,age,telephone,groupe,date_creation);
        }
        return racine;
    }
}

Patient *chercher(Patient *racine,char *nom){
    if(racine == NULL)return NULL;
    int strc = strcmp(nom,racine->nom);
    if(strc < 0) return chercher(racine->gauche,nom);
    else if(strc > 0) return chercher(racine->droite,nom);
    else if(strc == 0)return racine;
    return NULL;
}

Patient *minimum(Patient *racine){
    if(racine == NULL)return NULL;
    Patient *next = racine;
    while(next->gauche != NULL){
        //racine = next;
        next = next->gauche;
    }
    return next;
}

Patient *supprimer(Patient* racine,char *nom){
    if(racine == NULL)return NULL;
    //Patient *sup = racine;
    
        if(strcmp(nom,racine->nom) < 0)racine->gauche = supprimer(racine->gauche,nom);
        else if(strcmp(nom,racine->nom) > 0) racine->droite = supprimer(racine->droite,nom);
        else if(strcmp(nom,racine->nom) == 0){
            Patient *s = NULL;
            if(racine->droite == NULL){
               s = racine->gauche;
               free(racine);
               return s;
            }else if(racine->gauche == NULL){
               s = racine->droite;
               free(racine);
               return s;
           }else {
               Patient *minimu = minimum(racine->droite);
               strcpy(racine->nom,minimu->nom);
               strcpy(racine->telephone,minimu->telephone);
               racine->age = minimu->age;
               strcpy(racine->groupe_sanguin,minimu->groupe_sanguin);
               strcpy(racine->date_ajout,minimu->date_ajout);
               racine->droite = supprimer(racine->droite,minimu->nom);
               
            }
            return racine;
        }
        return racine;
}


void rechercher_groupe(Patient *racine,char *grp,char *reponse){
    if(racine == NULL){return;}
    rechercher_groupe(racine->gauche,grp,reponse);
    if(strcmp(grp,racine->groupe_sanguin) == 0){
        char ligne[256];
        
        snprintf(ligne,sizeof(ligne),"%s %d %s [%s]\n",racine->nom,racine->age,
        racine->telephone,racine->groupe_sanguin);

         if(strlen(ligne) + strlen(reponse) < BUFFER_SIZE){
            strcat(reponse,ligne);
         }
    }
    rechercher_groupe(racine->droite,grp,reponse);
}
void Lister(Patient *racine,char *reponse){
    char ligne[256];
    if(racine == NULL){return;}
    Lister(racine->gauche,reponse);
    snprintf(ligne,sizeof(ligne),"(Ajoutee le %s) %s %d %s [%s]\n",racine->date_ajout,racine->nom,
    racine->age,racine->telephone,racine->groupe_sanguin);

    if(strlen(ligne) + strlen(reponse) < BUFFER_SIZE){
        strcat(reponse,ligne);
    }
    Lister(racine->droite,reponse);

}

//___________________________________________________________
Patient *racine = NULL;
void Sauvegarder(Patient *racine,FILE *fichier){
    if(fichier == NULL){
        printf("Sauvegarde impossible pour le moment\nVeuillez initialiser f\n");
        return;
    }
    char buffer[200];
    if(racine == NULL)return;
    Sauvegarder(racine->gauche,fichier);
    snprintf(buffer,sizeof(buffer),"%s|%d|%s|%s|%s\n",racine->nom,racine->age,
    racine->groupe_sanguin,racine->telephone,racine->date_ajout);
    fprintf(fichier,"%s",buffer);
    Sauvegarder(racine->droite,fichier);
}

void sauvegarder_fichier(char *chemin){
    FILE *f = fopen(chemin,"w");
    if(f == NULL){
        printf("Erreur d'allocation\n");
        return;
    }
        Sauvegarder(racine,f);
        fclose(f);
    }


void charger(char *fichier){
    FILE *f = fopen(fichier,"r");
    if(f == NULL){
        printf("F est nul\n");
        return;
    }
    
    char buffer[200],nom[50],groupe[5],telephone[20],ajout[30];
    int age = -1;
    memset(buffer,0,sizeof(buffer));
    while(fgets(buffer,sizeof(buffer),f) != NULL){
        sscanf(buffer,"%[^|]|%d|%[^|]|%[^|]|%[^|]",nom,&age,groupe,telephone,ajout);
        racine = inserer(racine,nom,age,telephone,groupe,ajout);
        memset(buffer,0,sizeof(buffer));
    }
    fclose(f);
    
}
//___________________________________________________________
//FILE *f;

void traiter_cmd(char *buffer,char *reponse){
char cmd[50],nom[50],groupe[5],telephone[20],date_ajout[30];
int  age = -1;
memset(cmd,0,50);
memset(nom,0,50);
memset(groupe,0,5);
memset(telephone,0,20);
memset(date_ajout,0,sizeof(date_ajout));
int nblu = sscanf(buffer,"%49s %49s %d %4s %19s",cmd,nom,&age,groupe,telephone);

if(nblu > 5){
    sprintf(reponse,"Nombre d'argument requis depassee");
}else{
    if(strcmp(cmd,"AJOUTER") == 0){
    if(strlen(nom) < 1 || age == -1 || strlen(groupe) < 1 || strlen(telephone) < 1){
        sprintf(reponse,"format incorrect\nAJOUTER   <nom> <age> <groupe_sanguin> <telephone>");
    }else{
        // Vérifie si une chaîne contient une autre
        // Retourne un pointeur si trouvé, NULL sinon
        char *groupes_valides[] = {"A+","A-","B+","B-","AB+","AB-","O+","O-"};

        // Pour vérifier si le groupe entré est valide :
        int resul = -1;
        int nb = sizeof(groupes_valides) / sizeof(groupes_valides[0]);
        //printf("%lu et %d\n",sizeof(groupes_valides),nb);
        for(int i = 0;i< nb;i++){
            resul = strcmp(groupes_valides[i], groupe);
            if(resul == 0)break;
        }
        if(resul == 0){
        time_t tim = time(NULL);
        struct tm *lcltme = localtime(&tim);
        strftime(date_ajout,sizeof(date_ajout),"%d/%m/%Y %H:%M",lcltme);
        //snprintf(date_ajout,sizeof(date_ajout),"%.2d/%.2d");
        racine = inserer(racine,nom,age,telephone,groupe,date_ajout);
        sauvegarder_fichier("registre.txt");
        sprintf(reponse,"%s ajoutee",nom);
        }else{
            sprintf(reponse,"Groupe sanguin inconnu\nIl doit correspondre au groupe ci dessous\nA+,A-,B+,B-,AB+,AB-,O+,O-");
            return;
        }
    }
}else if(strcmp(cmd,"CONSULTER") == 0){
    if(strlen(nom) < 1){
        sprintf(reponse,"Format incorrect\nCONSULTER <nom>");
    }else{
        Patient *search = chercher(racine,nom);
        if(search == NULL){sprintf(reponse,"%s introuvable",nom);}
        else{sprintf(reponse,"(Ajoutee le %s) %s %d %s [%s]\n",search->date_ajout,
            search->nom,search->age,search->telephone,search->groupe_sanguin);}
    }
}else if(strcmp(cmd,"LISTER") == 0){
    Lister(racine,reponse);
    if(strlen(reponse) < 1){sprintf(reponse,"Liste vide");}
}else if(strcmp(cmd,"SUPPRIMER") == 0){
    if(strlen(nom) < 1){
        sprintf(reponse,"Format incorrect\nSUPPRIMER <nom>");
    }else{
        Patient * search = chercher(racine,nom);
        if(search == NULL){sprintf(reponse,"Suppression Impossible car inexistant");}
        else{
            racine = supprimer(racine,nom);
            sauvegarder_fichier("registre.txt");
            sprintf(reponse,"%s supprimee",nom);
        }
    }
}else if(strcmp(cmd,"GROUPE") == 0){
    
    if(strlen(nom) < 1){
        sprintf(reponse,"Format incorrect\nGROUPE <nom du groupe>");
    }else{
        /*integer *i = malloc(sizeof(integer));
        if(i == NULL)return;
        i->i = 0;*/
        //int y = i->i;
        rechercher_groupe(racine,nom,reponse);
            /*if(i->i == 0){memset(reponse,0,BUFFER_SIZE);;}
        free(i);*/
        if(strlen(reponse) < 1){
            sprintf(reponse,"Aucune personne avec un groupe sanguin de ce type");}
    }

    }
else {
    sprintf(reponse,"Choix invalide\n");
}
}
}

void* gerer_Client(void *arg){
    int client_fd = ((ArgTh*)arg)->client_fd;
    char reponse[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    free(arg);
    while(1){
        memset(buffer,0,BUFFER_SIZE);
        memset(reponse,0,BUFFER_SIZE);

        printf(">>");
    ssize_t receive = recv(client_fd,buffer,BUFFER_SIZE-1,0);
    if(receive <= 0){
        break;
    }
    buffer[receive] = '\0';
    
    printf("Client %i: %s\n ",client_fd,buffer);
    if(strncmp(buffer,"exit",4)== 0){
        printf("Client %i deconnectee\n",client_fd);
        break;
    }
    printf(">>");
    pthread_mutex_lock(&mutex);
    traiter_cmd(buffer,reponse);
    pthread_mutex_unlock(&mutex);

    ssize_t envoi = send(client_fd,reponse,strlen(reponse),0);
    printf("[Pour client %i]: %s\n",client_fd,reponse);
    if(envoi == -1){
        printf("Client %i inaccessible\n",client_fd);
        break;
    }
    }
    
    close(client_fd);
    
    return NULL;
}


int main(){
    int serveur_fd;
    struct sockaddr_in adresse;

    serveur_fd = socket(AF_INET,SOCK_STREAM,0);
    if(serveur_fd == -1){
        perror("Erreur");
        return -1;
    }
    adresse.sin_family = AF_INET;
    adresse.sin_addr.s_addr = INADDR_ANY;
    adresse.sin_port = htons(PORT);

    if(bind(serveur_fd,(struct sockaddr*)&adresse,sizeof(adresse)) < 0){
        perror("Erreur");
        return -1;
    }
    //f = fopen("registre.txt","w");
    
    charger("registre.txt");
    listen(serveur_fd,20);
    printf("Serveur pret en ecoute\n");
    
    while(1){
        int client_fd = accept(serveur_fd,NULL,NULL);
        printf("Nouveau client connectee\n");
        pthread_t thread;
        ArgTh *arg = malloc(sizeof(ArgTh));
        if(arg == NULL){close(client_fd);close(serveur_fd);return -1;}
        arg->client_fd = client_fd;
        pthread_create(&thread,NULL,gerer_Client,arg);
        pthread_detach(thread);
    }
    //sauvegarder_fichier("registre.txt");
    close(serveur_fd);

}

