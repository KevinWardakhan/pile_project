#include<stdio.h>
#include<stdlib.h>
#include<string.h>

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
  char caractere_courant; /*Pour stocker le carcrère qu'on est en train de lire*/

  if(fichier!=NULL){
    while( (caractere_courant=fgetc(fichier) ) != EOF ){
      if(caractere_courant == '\n'){
        cpt++;
      }
    }
  }
    
  else{
    printf("Erreur Ouverture fichier.");
    cpt=-2;
  }
  
  fclose(fichier);
  return cpt;
}




void traduction(char* nom_fichier){
    FILE* fichier=NULL;
    fichier=fopen(nom_fichier,'r');
    int longueur=0
    while ()
    {
        /* code */
    }
    
    

}
int main(int argc, char const *argv[])
{
    char* nom_fichier=NULL;
    nom_fichier=argv[1]

    return 0;
}
