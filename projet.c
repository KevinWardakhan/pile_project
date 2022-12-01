#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define nombre_caractere_max_ligne 1000
#define nombre_instruction 15

char* Instruction[]={"push","push#","ipush","pop","ipop","dup","op","jmp","jpz","rnd","read","write","call","ret","halt"};

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
  fichier=fopen(nom_fichier,"r");
  int nombre_ligne=nombre_ligne_fichier(nom_fichier);
  char ligne[nombre_caractere_max_ligne];
  for(int i=0;i<nombre_ligne;i++){
    int verif=1;
    fgets(ligne,nombre_caractere_max_ligne,fichier);

    if(strstr(ligne,"push#")!=NULL){
      printf("%d\n",1);
      verif=0;
    }
    else{
      if(strstr(ligne,"ipush")!=NULL){
        printf("%d\n",2);
        verif=0;
      }
      else{
        if(strstr(ligne,"push")!=NULL){
          printf("%d\n",0);
          verif=0;
        }
      }
    }

    if(verif==1){
      if(strstr(ligne,"ipop")!=NULL){
        printf("%d\n",4);
        verif=0;
      }
      else{
        if(strstr(ligne,"pop")!=NULL){
          printf("%d\n",3);
          verif=0;
        }
      }
    }

    if(verif==1){
      for(int j=5;i<nombre_instruction;j++){
        if(strstr(ligne,Instruction[j])!=NULL){
          printf("%d\n",j);
          break;
        }
      }
    }
  }

  fclose(fichier);
}


int main(int argc,char* argv[]){

  FILE* fichier=NULL;
  fichier=fopen(argv[1],"a"); fflush(fichier);
  fputc('\n',fichier);
  fclose(fichier);

  traduction(argv[1]);
  return 0;
}
