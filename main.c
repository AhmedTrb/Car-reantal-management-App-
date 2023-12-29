#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define MAX_SIZE 1000

float prixLocationParClasse[] = {35.0,40.5,54.4,60.2};
typedef struct voiture {
    int id;
    char modele[70];
    char marque[70];
    char etat[20]; // peut etre : louer ou disponible ou en panne
    char Matricule[40];
    char classe;
    int age;
}Voiture ;

typedef struct client {
    int id;
    int age;
    char nom[70];
    char prenom[70];
    int numCIN;
    int tel;
    char email[100];
    int PermisNb;
}Client;

typedef struct location {
    int id;
    int id_C;
    int id_V;
    char dateD[10];
    int nbjour;
    float prix;
}Location;


//liste chainé
struct node {
    Voiture val;
    struct node* suiv;
};
typedef struct node *node;


// creer un noeud
node createNode(){
	node tmp ;
	tmp = (node)malloc(sizeof(struct node));
	tmp->suiv = NULL;
	return tmp;
}


// ajouter un noeud a la fin de la liste chainé des voitures
void addNode(node head,Voiture value){
    node temp,p;
    temp = createNode();
    temp->val = value;
    if(head == NULL){
        head = temp;
    }
    else{
        p  = head;
        while(p->suiv != NULL){
            p = p->suiv;
        }
        p->suiv = temp;
    }
}


int nbvoiture=0;
int nbLocation=0;
int nbClient=0;
int nbReparation=0;
node ListeVoiture;
Client TabClients[MAX_SIZE];
Location TabLocations[MAX_SIZE];



void afficherLesVoitures(){
    node p=ListeVoiture;
	if (p==NULL) return ;
	p=p->suiv;
	while(p != NULL){
		printf("%d | %s | %s | %d ans | %s \n",p->val.id,p->val.modele,p->val.marque,p->val.age,p->val.etat);
    	p = p->suiv;
	}
}



// afficher une description de la voiture
void descriptionVoiture (int idv){
    node head=ListeVoiture;
    if (head==NULL) {
        printf("error liste vide \n");
        return;
    }
    node p=head;
    while (p->val.id!=idv) {
        if (p->suiv==NULL) {
            printf("erreur identifiant\n");
            return ;
        }
        p=p->suiv;
    }
    Voiture v = p->val;
    printf("voiture modele %s de la marque %s ,elle a %d ans et elle est %s.\n Matricule %s\n",
        v.modele,v.marque,v.age,v.etat,v.Matricule);
}


// afficher les voitures disponibles
void VoitureDisponible(){
    node headV=ListeVoiture;
    node tmp=headV;
    if (headV==NULL) printf("erreur liste vide");
    bool d=0;
    while (tmp!=NULL){
        if (strcmp("disponible",tmp->val.etat)==0) {
            printf("id : %d :la voiture %s , %d ans de classe %c \n",tmp->val.id,tmp->val.modele,tmp->val.age,tmp->val.classe);
            d=1;
        }
        tmp=tmp->suiv;
    }
    if (!d) printf("pas de voiture disponible pour le moment \n");
}

//calcul du prix de location selon le nbre de jour saisie par l'utilisateur
float calculerPrixLocation(int id_V,int nbj){
    node headV = ListeVoiture;


    if (headV==NULL) {
        printf("pas de voiture dans la liste\n");
        return 0;
    }
    node p=headV;
    char c;
    while (p!=NULL){
            p= p->suiv;
        if(p->val.id==id_V){
            c=p->val.classe;
            break;
        }
    }
    int i=c-'A';
    return nbj*prixLocationParClasse[i];

}

// louer une voiture
void louerVoiture (){
    FILE* locationFile =  fopen("location.txt","a");

    nbLocation++;

    //entrer les données de la voiture
    printf("entrer l'identifiant de la voiture : ");
    scanf("%d",&TabLocations[nbLocation].id_V);
    printf("entrer l'identifiant du client : ");
    scanf("%d",&TabLocations[nbLocation].id_C);
    printf("entrer la date de debut : ");
    scanf("%s",&TabLocations[nbLocation].dateD);
    printf("entrer le nombre de jour :");
    scanf("%d",&TabLocations[nbLocation].nbjour);
    TabLocations[nbLocation].id=nbLocation;

    TabLocations[nbLocation].prix=calculerPrixLocation(TabLocations[nbLocation].id_V,TabLocations[nbLocation].nbjour);

    // ajouter les information dans le fichier location
    fprintf(locationFile,"%d %d %d %s %d %.2f\n",TabLocations[nbLocation].id,TabLocations[nbLocation].id_C,TabLocations[nbLocation].id_V,TabLocations[nbLocation].dateD,
            TabLocations[nbLocation].nbjour,TabLocations[nbLocation].prix);
    fclose(locationFile);

    //modifier l'etat de la voiture dans la liste chainé contenant les voitures
    node p=ListeVoiture;
    while (p->val.id!=TabLocations[nbLocation].id_V) p = p->suiv;
    strcpy(p->val.etat,"loue");

}

// ajouter une voiture dans le systeme
void ajouterVoiture(){
    Voiture v;
    nbvoiture++;

    //entrer les information de la voiture
    printf("entrer les informations sur la voiture suivant la forme suivante : \n marque / modele / matricule / classe / age :");
    strcpy(v.etat,"disponible");
    v.id=nbvoiture;
    scanf("%s %s %s %c %d",&v.marque,&v.modele,&v.Matricule,&v.classe,&v.age);

    //ajouter une voiture dans la liste chainée
    addNode(ListeVoiture,v);
}


void rapportLocation(){
    float revenueLocation=0;

    for (int i=0;i<nbLocation;i++){
        revenueLocation+=TabLocations[i].prix;
    }
    printf("\n----- Rapport -----\n\n");
    printf("1. revenue total : %.3f\n",revenueLocation);

    node p=ListeVoiture;
    printf("2. Pourcentage de location de chaque voiture\n");
    p=p->suiv;
    while(p!=NULL){
        printf("voiture %s:\n",p->val.modele);
        historiqueLocationVoiture(p->val.id);
        p=p->suiv;
    }

}

// supprimer une voiture de la liste chainée
void supprimerVoiture(int id_v){
    node p1=ListeVoiture->suiv,p2=ListeVoiture;
    while (p1->val.id!=id_v) {
        p1=p1->suiv;
        p2=p2->suiv;
    }
    p2->suiv=p1->suiv;
    free(p1);
}

// modifier l'etat d'une voiture
void modifierEtatVoiture(int id_v,char eta[10]){
    node p=ListeVoiture;
    while(p->val.id!=id_v) p=p->suiv;
    strcpy(p->val.etat,eta);
}




void reclamationRetourVoiture(int id_L,char date[10]){
    for (int i=0;i<nbLocation;i++){
        if (id_L==TabLocations[i].id){
            modifierEtatVoiture(TabLocations[i].id_V,"en-panne");
            break;
        }
    }
}

void historiqueLocationVoiture(int idv){
    int count =0;
    for (int i=0;i<nbLocation;i++){
        if (TabLocations[i].id_V==idv) {
            printf("loue le %s pendant %d jours\n",TabLocations[i].dateD,TabLocations[i].nbjour);
            count++;
        }
    }
    char mod[40];
    node p= ListeVoiture;
    p=p->suiv;
    while(p!=NULL){
        if (p->val.id==idv) {
            strcpy(mod,p->val.modele);
            break;
        }
        p=p->suiv;
    }
    if (count==0) {
        printf("voiture pas encore loue \n");
    } else {
       float a = (float)count / nbLocation;
        printf("Pourcentage de location de la voiture %s (ID %d): %.2f%%\n\n", mod, idv, a * 100);
    }

}


void historiqueLocationClient(int idc){
    char nom[40],prenom[40];
    for (int i=0;i<nbClient;i++) {
        if (TabClients[i].id==idc){
            strcpy(nom,TabClients[i].nom);
            strcpy(prenom,TabClients[i].prenom);
            break;
        }
    }
    printf("Historique du client %s %s d'identifiant %d :\n",prenom,nom,idc);
    for (int i=0;i<nbLocation;i++){
        if (TabLocations[i].id_C==idc) {
            node p= ListeVoiture;
            p=p->suiv;
            while(p!=NULL){
                if (p->val.id==TabLocations[i].id_V) {
                    printf("voiture %s loue le  %s pendant %d jours \n",p->val.modele,TabLocations[i].dateD,TabLocations[i].nbjour);
                    break;
                }
            p=p->suiv;
            }
        }
    }
}
int main(){

    FILE* voitureFile =  fopen("voiture.txt","r");
    FILE* clientFile =  fopen("client.txt","r");
    FILE* locationFile =  fopen("location.txt","r");




    Voiture tmpV;

    // creation de la tete de la liste chainé qui va stocker les voitures
    ListeVoiture = createNode();

    // stocker les informations sur les voitures du fichier voiture dans une liste chainé
    while (fscanf(voitureFile,"%d %s %s %s %s %c %d",&tmpV.id,&tmpV.modele,&tmpV.marque,&tmpV.etat,&tmpV.Matricule,&tmpV.classe,&tmpV.age)==7){
        nbvoiture++;
        addNode(ListeVoiture,tmpV);
    }



    // lire les information du fichier Clients et les stocker dans un tableau
    while (fscanf(clientFile,"%d %d %s %s %d %d %s %d",&TabClients[nbClient].id,&TabClients[nbClient].age,&TabClients[nbClient].nom,
                  &TabClients[nbClient].prenom,&TabClients[nbClient].numCIN,&TabClients[nbClient].PermisNb,
                  &TabClients[nbClient].email,&TabClients[nbClient].tel)==8){
                    nbClient++;
                  }

    // lire les information du fichier Locations et les stocker dans un tableau
    while (fscanf(locationFile,"%d %d %d %s %d %f",&TabLocations[nbLocation].id,&TabLocations[nbLocation].id_C,&TabLocations[nbLocation].id_V,
        &TabLocations[nbLocation].dateD,&TabLocations[nbLocation].nbjour,&TabLocations[nbLocation].prix)==6){
            nbLocation++;
    }

    fclose(voitureFile);
    fclose(clientFile);
    fclose(locationFile);

    for (int i=0;i<nbLocation;i++){
        TabLocations[i].prix = calculerPrixLocation(TabLocations[i].id_V,TabLocations[i].nbjour);
    }
    printf("\nbonjour et bienvenue dans note application de gestion de location des voitures\n\n");

    int typeUtilisateur;

    printf("choissir le type d'utilisateur\n");
    printf("1. Client\n");
    printf("2. Admin\n");
    printf("Entrer votre choix :");
    scanf("%d", &typeUtilisateur);

    switch (typeUtilisateur) {
        case 1:
            clientMenu();
            break;
        case 2:
            adminMenu();
            break;
        case 0:
            break;
        default:
            printf("type d'utilisateur invalide...\n");
    }



    FILE* vf = fopen("voiture.txt","w");
    node p =ListeVoiture;
    p=p->suiv;
    while(p!=NULL){
        Voiture v=p->val;
        fprintf(vf,"%d %s %s %s %s %c %d\n",v.id,v.modele,v.marque,v.etat,v.Matricule,v.classe,v.age);
        p=p->suiv;
    }
    fclose(vf);
    free(ListeVoiture);

}

void clientMenu() {
    int choix;
    printf("\n----- Client Menu -----\n");
    printf("1. Description d'une voiture\n");
    printf("2. Les Voitures Disponibles\n");
    printf("3. Louer une voiture\n");
    printf("4. Reclamation Retour Voiture\n");
    printf("5. Espace admin\n");
    printf("0. Quitter\n\n");
    do {
        printf("Entrez votre choix: ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                printf("entrer l'id de la voiture :");
                int idv;
                scanf("%d",&idv);
                descriptionVoiture(idv);
                break;
            case 2:
                VoitureDisponible();
                break;
            case 3:
                louerVoiture();
                break;
            case 4:
                printf("Entrez l'identifiant de la location: ");
                int idLocationReclamation;
                char dateReclamation[10];
                scanf("%d", &idLocationReclamation);
                printf("Entrez la date de réclamation: ");
                scanf("%s", dateReclamation);
                reclamationRetourVoiture(idLocationReclamation, dateReclamation);
                break;
            case 5:
                adminMenu();
                break;
            case 0:
                printf("Deconnexion...\n");
                break;
            default:
                printf("Choix invalide. Veuillez réessayer.\n");
        }

    } while (choix != 0);
}

// Admin Menu
void adminMenu() {
    int choix;

    printf("\n----- Admin Menu -----\n");
    printf("1. Ajouter une voiture\n");
    printf("2. Supprimer une voiture\n");
    printf("3. Modifier l'etat d'une voiture\n");
    printf("4. Afficher tout les voitures\n");
    printf("5. Rapport\n");
    printf("6. Historique de location du client\n");
    printf("0. Quitter\n");
    do {



        printf("Entrez votre choix: ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                ajouterVoiture();
                break;
            case 2:
                printf("Entrez l'identifiant de la voiture à supprimer: ");
                int idVsup;
                scanf("%d", &idVsup);
                supprimerVoiture(idVsup);
                break;
            case 3:
                printf("Entrez l'identifiant de la voiture: ");
                int idV;
                char nEtat[10];
                scanf("%d", &idV);
                printf("Entrez le nouvel etat: ");
                scanf("%s", nEtat);
                modifierEtatVoiture(idV, nEtat);
                break;
            case 4:
                afficherLesVoitures();
                break;
            case 5:
                rapportLocation();
            case 6:
                printf("entrer l'identifiant de client :");
                int idc;
                scanf("%d",&idc);
                historiqueLocationClient(idc);
                break;
            case 0:
                printf("Deconnexion...\n");
                break;
            default:
                printf("Choix invalide. Veuillez réessayer.\n");
        }

    } while (choix != 0);
}
