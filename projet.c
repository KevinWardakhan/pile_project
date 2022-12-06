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
      if(strstr(ligne,":")==NULL){  /*Si il ne l'est pas on a pas d'étiquette, et on met dans tab_etiquette la valeur NULL.*/
        tab_etiquette[i]=NULL;
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

int recuperation_donnee(char* nom_fichier,int nombre_ligne,int *tab_instruction_courante_decimale,char* tab_etiquette[],int *tab_donnee){
  /*
  Dans cette fonction on va recuperer dans le tableau l'ensemble des donnees necessaire pour pouvoir ecrire le fichier en langage machine.
  Pour cela on dispose des instruction sous format decimal dans le tableau tab_instruction_courante_decimale et des etiquettes dans le tableau tab_etiquette.
  L'indice i correspond à la ligne i. Ainsi, a la ligne i on trouve l'instruction tab_instruction_courante_decimale[i] et l'etiquette tab_etiquette[i].
  */

  /*On renvoie 0 si la recuperation c'est bien passé, -1 sinon.*/

  /*On suppose que le fichier ne contient pas d'erreur de syntaxe.*/


  FILE* fichier=NULL;
  fichier=fopen(nom_fichier,"r");

  if(fichier!=NULL){

    char ligne[nombre_caractere_max_ligne]; /*On initialise un tableau de char ligne qui recuperera chaque ligne.*/

    for(int i=0;i<nombre_ligne;i++){ /*On boucle donc sur chaque ligne.*/

      /*
      Methode de recherche : Dans notre cas il y a 4 cas.

      Cas 1:
      Dans la ligne, il y a une instruction, qui ne necessite pas de donnee : on mettra la valeur 0 dans le tableau tab_donnee.

      Cas2 :
      Dans la ligne, il y a une instruction, qui necessite une donnee : on recupere la donnee et on la met dans le tableau tab_donnee.

      Cas 3:
      Dans la ligne, il y a une instruction, qui necessite le traitement des etiquettes : on recupere la valeur qu'il faut et on la met dans le tableau tab_donnee.

      Cas 4:
      On traite ici le cas des lignes supplementaire en fin de fichier.


      On utilisera un if pour voir a chaque fois dans quel cas nous sommes.

      */

      /*Cas 1 :*/
      if( ((tab_instruction_courante_decimale[i]==2) || (tab_instruction_courante_decimale[i]==4) || (tab_instruction_courante_decimale[i]==5) || (tab_instruction_courante_decimale[i]==13) || (tab_instruction_courante_decimale[i]==14)) ){
        fgets(ligne,nombre_caractere_max_ligne,fichier); //ESSENTIEL MEME SI ON UTILISE PAS. On doit pouvoir a chaque fois, decaler le curseur virtuel du fichier à la ligne associee.
        tab_donnee[i]=0;
        continue;
      }


      /*Cas 2 :*/
      if( (tab_instruction_courante_decimale[i]==0) || (tab_instruction_courante_decimale[i]==3) || (tab_instruction_courante_decimale[i]==10) || (tab_instruction_courante_decimale[i]==11) || (tab_instruction_courante_decimale[i]==6) || (tab_instruction_courante_decimale[i]==1) || (tab_instruction_courante_decimale[i]==9) ){
        fgets(ligne,nombre_caractere_max_ligne,fichier);

        /*On boucle sur les caracteres de la ligne. On cherche la premiere occurence correspondant a un chiffre (donc 0 1 2 3 4 5 6 7 8 9), ce qui nous indique le debut de la donnee a recuperer.*/
        for(int j=0;j<strlen(ligne);j++){

          if(ligne[j]=='0' || ligne[j]=='1' || ligne[j]=='2' || ligne[j]=='3' || ligne[j]=='4' || ligne[j]=='5' || ligne[j]=='6' || ligne[j]=='7' || ligne[j]=='8' || ligne[j]=='9'){
            tab_donnee[i]=strtol(&ligne[j],NULL,10); /*On recupere la donnee sous forme entiere !*/
            break; /*NECESSAIRE : strtol va renvoyer toute la donnee à partir de l'adresse de ligne[j]. On recupere donc en une fois notre donee.*/
          }
        }
        continue;
      }



      /*Cas 3 :*/
      if((tab_instruction_courante_decimale[i]==7) || (tab_instruction_courante_decimale[i]==8) || (tab_instruction_courante_decimale[i]==10)){
        fgets(ligne,nombre_caractere_max_ligne,fichier);

        /*On boucle ici, sur le nombre de ligne. ON cherche ici a savoir quel etiquette est presente apres l'instruction.*/
        for(int j=0;j<nombre_ligne;j++){
          if(tab_etiquette[j]!=NULL && strstr(ligne,tab_etiquette[j])!=NULL){
            tab_donnee[i]=j-i-1;  // ligne ou etiquette est presente=j  ligne ou l'on est = i
            break;
          }
        }
        continue;
      }



      /*Cas 4 :*/
      if(tab_instruction_courante_decimale[i]==100){
        /*On sait que par defaut, on met la valeur 100 si il n'y a pas d'instruction à la ligne considerer.*/
        fgets(ligne,nombre_caractere_max_ligne,fichier);
        tab_donnee[i]=0; /*Valeur par defaut. On ne veut pas que notre tableau de donnee soit vide.*/
        continue;
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

int creation_fichier_langage_assembleur(int *tab_instruction_courante_decimale,int *tab_donnee){

  FILE* fichier=NULL;
  fichier=fopen("hexa.txt","w");

  if(fichier!=NULL){
    for(int i=0;tab_instruction_courante_decimale[i]!=100;i++){
      fprintf(fichier,"0%x %08x\n",tab_instruction_courante_decimale[i],tab_donnee[i]); fflush(fichier);
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
  int tab_instruction_courante_decimale[nombre_ligne];
  char* tab_etiquette[nombre_ligne];
  int tab_donnee[nombre_ligne];

  traduction_instruction_octect_poids_fort(argv[1],nombre_ligne,&tab_instruction_courante_decimale[0]);
  recuperation_etiquette(argv[1],nombre_ligne,tab_etiquette);
  recuperation_donnee(argv[1],nombre_ligne,tab_instruction_courante_decimale,tab_etiquette,tab_donnee);
  creation_fichier_langage_assembleur(tab_instruction_courante_decimale,tab_donnee);

  /*for(int i=0;i<nombre_ligne;i++){
    if(tab_etiquette[i]!=NULL){
      printf("Ligne %d : Instruction numero 0%x, l'etiquette est %s, et la donnee est %d.\n",i,tab_instruction_courante_decimale[i],tab_etiquette[i],tab_donnee[i]);
    }
    else{
      printf("Ligne %d : Instruction numero 0%x, pas d'etiquette et la donnee est %d.\n",i,tab_instruction_courante_decimale[i],tab_donnee[i]);
    }
  }*/

  /*for(int i=0;i<nombre_ligne;i++){
      printf("0%x %08x\n",tab_instruction_courante_decimale[i],tab_donnee[i]);
  }
  return 0;*/

  for(int i=0;i<nombre_ligne;i++){
    if(tab_etiquette[i]!=NULL){
      free(tab_etiquette[i]);
    }
  }
}
