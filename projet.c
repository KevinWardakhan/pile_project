#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define nombre_caractere_max_ligne 1000
#define nombre_instruction 15

char* Instruction[]={"push","push#","ipush","pop","ipop","dup","op","jmp","jpz","rnd","read","write","call","ret","halt"};




int actualisation_fichier_code_assembleur(char* nom_fichier){

/*On rajoute une ligne en fin de fichier, sinon la dernière ligne ne pourra pas être lu dans les fonctions prochaines...
...si l'utilisateur n'appuie pas sur entrée à la fin de son fichier. Cette ligne supplémentaire nous assure...
...donc que la dernière ligne puisse être, plus tard lu si on utilise fgets.*/

  FILE* fichier=NULL;
  fichier=fopen(nom_fichier,"a");

  if(fichier!=NULL){
    fputc('\n',fichier); fflush(fichier);
    fclose(fichier);
    return 0;
  }

  else{
    fclose(fichier);
    return -1;
  }
}





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






int traduction_instruction_octect_poids_fort(char* nom_fichier,int nombre_ligne,int *tab_instruction_courante){
  /*On va dans cette fonction transformer les code instructions (octet de poid forts pour l'instant) en code numérique.*/
  /*On va mettre les diffèrentes valeurs du code numérique (octet de poids fort seulement) dans un tableau de int.*/
  /*Cette fonction va renvoyer 0 si la traduction c'est bien passé, -1 si il y a eu un problème dans l'ouverture du fichier*/

  /*On a au prealable rajouter une ligne en fin de fichier, sinon la dernière ligne ne pourra pas être lu, si l'utilisateur n'appuie pas sur entrée à la fin de son fichier.*/
  /*On suppose que le fichier ne comporte pas d'erreur de syntaxe!*/


  FILE* fichier=NULL;
  fichier=fopen(nom_fichier,"r");/*On ouvre notre fichier...*/


  if(fichier!=NULL){ /*... et on teste si cela a ete fait avec succès ou pas.*/

    char ligne[nombre_caractere_max_ligne]; /*On initialise un tableau vide qui contiendra notre ligne. On fait la supposition que la ligne ne dépasse pas 1000 carctères.*/
    int indice=0; /* Il s'agit d'une variable qui va nous aider à mettre les nombres associé dans notre tableau instruction courante*/

    /*On récupère chaque ligne, et pour chaque ligne on regarde si il y a la presence d'une instruction que l'on a decrit dans le tableau globale Instruction*/
    for(int i=0;i<nombre_ligne;i++){
      int verif=1; /*Il s'agit d'un drapeau. Si on a trouvé l'instruction correpodnante dans le tableau, il passe a 0 et on arrête la recherche.*/
      fgets(ligne,nombre_caractere_max_ligne,fichier); /*On récupère la ligne. On récupère forcement toute les lignes car on a ajouter le carcère /n à la fin du fichier.*/

      /*On procede à la recherche de l'instruction. Attention ici on ne peut pas parcourir toute le tableau Instrcution et verifier si l'instruction et présente dans la ligne (avec le fonction strstr())*/
      /*En effet, si on commence par chercher est-ce que push est dans la ligne, on aura une réponse affirmative même si la ligne contient push#...*/
      /*Il y a un ordre à respecter, pour éviter ce conflit*/

      if(strstr(ligne,"push#")!=NULL){
        tab_instruction_courante[indice]=1; /*Si push# est dans ma ligne, je rajoute 1 dans mon tableau*/
        indice++; /*On incrémente notre indice, pour la prochaine instruction*/
        verif=0; /*On change la valeur de notre drapeau pour indiqué qu'on a trouver l'instrcution : pas besoin d'aller plus loin.*/
      }
      else{
        if(strstr(ligne,"ipush")!=NULL){
          tab_instruction_courante[indice]=2;
          indice++;
          verif=0;
        }
        else{
          if(strstr(ligne,"push")!=NULL){
            tab_instruction_courante[indice]=0;
            indice++;
            verif=0;
          }
        }
      }

      if(verif==1){
        if(strstr(ligne,"ipop")!=NULL){
          tab_instruction_courante[indice]=4;
          indice++;
          verif=0;
        }
        else{
          if(strstr(ligne,"pop")!=NULL){
            tab_instruction_courante[indice]=3;
            indice++;
            verif=0;
          }
        }
      }

      /*On peut ici effectuer une boucle for car aucune instruction n'est maintenant une sous chaine d'une autre instruction.*/
      if(verif==1){
        for(int j=5;j<nombre_instruction;j++){
          if(strstr(ligne,Instruction[j])!=NULL){
            tab_instruction_courante[indice]=j;
            indice++;
            verif=0;
            break;
          }
        }
      }

      /*Le fait d'avoir ajouter \n à la fin du fichier contenant le code assembleur...*/
      /*...on peut avoir des cases NON INITIALISE dans le tableau instruction courante...*/
      /*...on remplis donc la fin par la valeur 100 correpondant à aucune instruction.*/

      if(verif==1){
        for(int j=indice;j<nombre_ligne;j++){
          tab_instruction_courante[indice]=100;
          indice++;
        }
      }


    }
    fclose(fichier);
    return 0;
  }

  else{
    fclose(fichier);
    return -1;
  }
}


int main(int argc,char* argv[]){

  actualisation_fichier_code_assembleur(argv[1]);
  int nombre_ligne=nombre_ligne_fichier(argv[1]); /*On récupère le nombre de ligne dans notre fichier (pour etre exact le nombre de fois ou on a clique sur la touche entree pour faire un saut de ligne.)*/
  int tab_instruction_courante[nombre_ligne];
  traduction_instruction_octect_poids_fort(argv[1],nombre_ligne,&tab_instruction_courante[0]);

  for(int i=0;i<nombre_ligne;i++){
    printf("%d\n",tab_instruction_courante[i]);
  }

  return 0;
}
