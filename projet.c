#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define nombre_caractere_max_ligne 1000
#define nombre_instruction 15
#define nombre_caratere_max_etiquette 100

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

int traduction_instruction_octect_poids_fort(char* nom_fichier,int nombre_ligne,int *tab_instruction_courante_decimale){
  /*On va dans cette fonction transformer les code instructions (octet de poid forts pour l'instant) en code numérique.*/
  /*On va mettre les diffèrentes valeurs du code numérique (octet de poids fort seulement) dans un tableau de int (entier base 10).*/
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
        tab_instruction_courante_decimale[indice]=1; /*Si push# est dans ma ligne, je rajoute 1 dans mon tableau*/
        indice++; /*On incrémente notre indice, pour la prochaine instruction*/
        verif=0; /*On change la valeur de notre drapeau pour indiqué qu'on a trouver l'instrcution : pas besoin d'aller plus loin.*/
      }
      else{
        if(strstr(ligne,"ipush")!=NULL){
          tab_instruction_courante_decimale[indice]=2;
          indice++;
          verif=0;
        }
        else{
          if(strstr(ligne,"push")!=NULL){
            tab_instruction_courante_decimale[indice]=0;
            indice++;
            verif=0;
          }
        }
      }

      if(verif==1){
        if(strstr(ligne,"ipop")!=NULL){
          tab_instruction_courante_decimale[indice]=4;
          indice++;
          verif=0;
        }
        else{
          if(strstr(ligne,"pop")!=NULL){
            tab_instruction_courante_decimale[indice]=3;
            indice++;
            verif=0;
          }
        }
      }

      /*On peut ici effectuer une boucle for car aucune instruction n'est maintenant une sous chaine d'une autre instruction.*/
      if(verif==1){
        for(int j=5;j<nombre_instruction;j++){
          if(strstr(ligne,Instruction[j])!=NULL){
            tab_instruction_courante_decimale[indice]=j;
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
          tab_instruction_courante_decimale[indice]=100;
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

int recuperation_etiquette(char* nom_fichier,int nombre_ligne,char* tab_etiquette[]){
/*Dans cette fonction on va recuperer les eventuelles etiquettes existante dans un tableau.*/
/*Ce tableau est de taille du nombre de ligne du fichier.*/
/*S'il y a une étiquette à la ligne numero i, à l'indice i du tableau on la trouvera*/
/*Dans le cas contraire, il y aura une chaine vide*/

/*Si la recuperation c'est bien passé on renvoie 0, sinon -1*/

  FILE* fichier=NULL;
  fichier=fopen(nom_fichier,"r"); /*On ouvre le fichier contenant le langage assembleur.*/

  if(fichier!=NULL){ /*On verifie que le fichier c'est bien ouvert.*/

    for(int i=0;i<nombre_ligne;i++){

      /*On recupere chaque ligne dans un tableau de char ligne.*/
      char ligne[nombre_caractere_max_ligne];
      fgets(ligne,nombre_caractere_max_ligne,fichier);


      /*Puis on regarde est ce que le carcatere ':' est présent...*/
      if(strstr(ligne,":")==NULL){  /*Si il ne l'est pas on a pas d'étiquette, et on met dans tab_etiquette une chaine vide.*/
        tab_etiquette[i]="";
      }
      else{ /*Sinon, on essayera de récuperer dans ligne, seulement l'étiquette. (Sachant que tout élèment avant le carctère ':' est un carcatère de l'etiquette.)*/

        /*On compte le nombre de caractère dans la ligne avant ':'*/
        int nombre_occurence_etiquette=0;
        while(ligne[nombre_occurence_etiquette]!=':'){
          nombre_occurence_etiquette++;
        }

        /*Puis on alloue un espace mémoire de taille suffisante pour pouvoir stocker par la suite cette etiquette.*/
        /*Attention de ne pas oublier l'espace necessaire pour l'octet nul. D'où le plus 1.*/
        tab_etiquette[i]=malloc((nombre_occurence_etiquette+1)*sizeof(char));

        /*On verifie au prealable que l'espace memoire necessaire nous a bien été mis a disposition.*/
        if(tab_etiquette[i]==NULL){
          return -1;
        }

        /*Puis on recopie l'etiquette dans cette espace memoire.*/
        for(int j=0;j<nombre_occurence_etiquette;j++){
          tab_etiquette[i][j]=ligne[j];
        }
        tab_etiquette[i][nombre_occurence_etiquette]='\0'; /*Sans oublier l'octect nul.*/
      }
    }

    fclose(fichier);  /*On ferme le fichier.*/
    return 0;
  }

  else{
    fclose(fichier); /*On ferme le fichier.*/
    return -1;
  }
}

int main(int argc,char* argv[]){

  actualisation_fichier_code_assembleur(argv[1]);
  int nombre_ligne=nombre_ligne_fichier(argv[1]); /*On récupère le nombre de ligne dans notre fichier (pour etre exact le nombre de fois ou on a clique sur la touche entree pour faire un saut de ligne.)*/
  int tab_instruction_courante_decimale[nombre_ligne];
  char* tab_etiquette[nombre_ligne];

  traduction_instruction_octect_poids_fort(argv[1],nombre_ligne,&tab_instruction_courante_decimale[0]);
  recuperation_etiquette(argv[1],nombre_ligne,tab_etiquette);

  for(int i=0;i<nombre_ligne;i++){
    if(strcmp(tab_etiquette[i],"")!=0){
      printf("Ligne %d : Instruction numero 0%x et l'etiquette est %s.\n",i,tab_instruction_courante_decimale[i],tab_etiquette[i]);
    }
    else{
      printf("Ligne %d : Instruction numero 0%x et pas d'etiquette.\n",i,tab_instruction_courante_decimale[i]);
    }

  }

  return 0;
}
