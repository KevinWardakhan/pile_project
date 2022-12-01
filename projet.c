#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define N 1000
#define nbinstr 15 //le nombre d'instructions
char* valeur[]={"push","push#","ipush","pop","ipop","dup","op","jmp","jpz","rnd","read","write","call","ret","halt"};


int nombre_ligne_fichier(char* nom_fichier){
    /*--------Renvoie le nombre de fois où on trouve dans un certain fichier le caractère saut de ligne--------*/
    /* ----------------Attention ne renvoie pas le nombre de ligne visible par l'utilisateur-------------------*/
    /*Renvoie le nombre de fois où on a appuyer sur entrée pour sauter une ligne lors de la création du fichier*/
    /*------------On verifiera si le fichier est bien ouvert--------------*/
    /*Si problème d'ouverture, la fonction va renvoyer -1 pour le signaler*/
    FILE* fichier=NULL;
    fichier=fopen(nom_fichier,"r");
    int cpt=0; /*Pour compter le nombre de ligne*/
    int caractere_courant; /*Pour stocker le carcrère qu'on est en train de lire*/
    int c2='\0';  //ce caractère va nous permet de compter aussi la dernière ligne du fichier

    if(fichier!=NULL){
    while( (caractere_courant=fgetc(fichier) ) != EOF ){
        if(caractere_courant == '\n'){
            cpt++;
            }
        c2=caractere_courant;
        }
    printf("%c",c2);
    if(c2 !='\n'){
        cpt++;
     }
    }

    else{
    printf("Erreur Ouverture fichier.");
    cpt=-2;
    }

    fclose(fichier);
    return cpt;
}




void traduction_pur(char* nom_fichier){
    FILE* fichier=NULL;
    fichier=fopen(nom_fichier,"r");
    int nbligne=nombre_ligne_fichier(nom_fichier);
    char lignes[N];
    for(int i=0;i<(nbligne);i++){
        fgets(lignes,N,fichier); //on prend la i-eme ligne 
        for (int j = 0; j<nbinstr ;j++){ //on va rechercher chaque instruction possible dans la ligne
            char* resultat=strstr(lignes,valeur[j]); //resultat pointe sur la première occurence de la sous chaine dans lignes si elle y est et renvoie NULL si l'instruction n'est pas sur la ligne                                     
            if (resultat!=NULL){    // on a trouvé une instruction sur la ligne, et c'est la seule par hypothese : valeur[j]
                printf("%s\n\n",resultat);  //on arrive bien a isole les instructions
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    char* nom_fichier=NULL;
    nom_fichier=argv[1];
    traduction_pur(nom_fichier);
    return 0;
}
