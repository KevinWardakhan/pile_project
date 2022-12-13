#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define nombre_caractere_max_ligne 1000
#define nombre_instruction 15
#define nombre_caratere_max_etiquette 100

char* Instruction[]={"push","push#","ipush","pop","ipop","dup","op","jmp","jpz","rnd","read","write","call","ret","halt"};



/*-------------------------------Prototypes-----------------------------------*/
int verif_fichier_vide(char*);
int actualisation_fichier_code_assembleur(char*);
int nombre_ligne_fichier(char*);
int recuperation_etiquette(char*,int,char**);
int verif_etiquette(int,char**);
void liberation_espace_memoire_tab_etiquette(int,char**);
int verif_instruction_donnee(char*,int,char**);
int traduction_instruction_octect_poids_fort(char*,int,int*);
int recuperation_donnee(char*,int,int*,char**,int*);
int creation_fichier_langage_machine(int*,int*);

/*--------------------------------Fonctions-----------------------------------*/

int verif_fichier_vide(char* nom_fichier){
  /*On verifie ici, si le fichier qu'on ouvre est vide ou pas.*/
  /*On renvoie 1 si le fichier est vide.*/
  /*On renvoie 0 si le fichier n'est pas vide.*/
  /*On renvoie -1 si on arrive pas ouvrir le fichier.*/

  FILE* fichier=NULL;
  fichier=fopen(nom_fichier,"r");
  if(fichier!=NULL){
    if(fgetc(fichier)==EOF){
      fclose(fichier);
      return 1;
    }
    else{
      fclose(fichier);
      return 0;
    }
  }
  else{
    fclose(fichier);
    return -1;
  }
}

int actualisation_fichier_code_assembleur(char* nom_fichier){

/*On rajoute une ligne en fin de fichier, sinon la dernière ligne ne pourra pas être lu dans les fonctions prochaines...
...si l'utilisateur n'appuie pas sur entrée à la fin de son fichier. Cette ligne supplémentaire nous assure...
...donc que la dernière ligne puisse être, plus tard lu si on utilise fgets.*/

/*Attention dans une premiere partie on ne rajoutera pas directement la ligne : cela creera un nouveau fichier, s'il n'est pas existant.*/
/*On cherche donc d'abord à le lire, pour verifier en même temps son existence.*/

  FILE* fichier=NULL;                 /*On verifie ici l'existence du fichier...*/
  fichier=fopen(nom_fichier,"r");
  if(fichier==NULL){
    fclose(fichier);
    return -1;
  }
  fclose(fichier);



  fichier=NULL;                /*On passe, a l'ajout si le fichier existe...*/
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

int verif_etiquette(int nombre_ligne,char* tab_etiquette[]){
  /*Le tableau tab_etiquette est deja remplie. On verifiera la coherence des etiquettes.*/
  /*Renvoie -2 si pas d'erreur.*/
  /*Renvoie -1 si doublons*/
  /*Renvoie le numero de la ligne si erreur : espace ou tabulation au sein de l'etiquette.*/


  /*On verifie d'abord que chaque etiquette ne possede pas d'espace et ne porte pas le nom d'une instruction.*/
  int ligne_erreur=-2;
  for(int i=0;i<nombre_ligne;i++){
    if(tab_etiquette[i]!=NULL){
      if( (strstr(tab_etiquette[i]," ")!=NULL) || (strstr(tab_etiquette[i],"\t")!=NULL) ){
        ligne_erreur=i;
        break;
      }
    }
  }

  if(ligne_erreur!=-2){
    return ligne_erreur;
  }


  /*On verifie maintenant qu'il n'y a pas de doublons dans les etiquettes.*/
  int doublon=0;
  for(int i=0;i<nombre_ligne;i++){

    if(tab_etiquette[i]!=NULL){
      for(int j=i+1;j<nombre_ligne;j++){
        if(tab_etiquette[j]!=NULL){
          if(strcmp(tab_etiquette[i],tab_etiquette[j])==0){
            doublon=-1;
            break;
          }
        }
      }
    }

    if(doublon==-1){
      break;
    }
  }

  if(doublon==-1){
    return -1;
  }
  else{
    return -2;
  }
}

void liberation_espace_memoire_tab_etiquette(int nombre_ligne,char* tab_etiquette[]){
  for(int i=0;i<nombre_ligne;i++){
    if(tab_etiquette[i]!=NULL){
      free(tab_etiquette[i]);
    }
  }
}

int verif_instruction_donnee(char* nom_fichier,int nombre_ligne,char* tab_etiquette[]){


  /*-----------------------------Fonctionnement-------------------------------*/

  /*On sait que pour chaque ligne, la suite de caractere est precise, il y a un certain ordre logique.*/
  /*Cette ordre logique est il respecte ?*/

  /*Lorsqu'il n'y a pas d'etiquette, on a dans l'ordre :
  1: des espaces et/ou des tabulations
  2: une instruction

  3_1: un saut de ligne, si l'instruction ne prend pas de parametre
  3_1bis: un saut de ligne, si l'instruction prend un parametre et que ce parametre vaut 0. (Abscence de parametre = 0 par defaut. Attention a la coherence... Par ex jmp ne peut pas prendre 0 en parametre...)
  3_2: un espace, si l'instruction prend un parametre

  4: Si parametre il y a, il faut que celui ci soit coherent. On finit par un saut de ligne.*/


  /*Lorsqu'il y a une etiquette, on a dans l'ordre :

  1:Tous ce qui est avant le caractere ':', correspond a l'etiquette
  2: des espaces et/ou des tabulations
  3: une instruction

  4_1: un saut de ligne, si l'instruction ne prend pas de parametre
  4_1bis: un saut de ligne, si l'instruction prend un parametre et que ce parametre vaut 0. (Abscence de parametre = 0 par defaut. Attention a la coherence... Par ex jmp ne peut pas prendre 0 en parametre...)
  4_2: un espace, si l'instruction prend un parametre

  5: Si parametre il y a, il faut que celui ci soit coherent. On finit par un saut de ligne.*/



  /*Comment fonctionne notre verification : On verifie que toute ces etapes sont respecter, avec des points de controle.
  On passera par une serie de test, verifiant les differents ordres logiques evoques.
  Si l'un d'entre eux n'est pas verifié, on renvoie le code erreur associe.
  Sinon, si tous les points de controle se sont deroules avec succes, on renvoie -1 (Donc pas d'erreur).*/

  /*-------------------------------Code Erreur--------------------------------*/

  /*Renvoie -1 si aucune erreur*/
  /*Renvoie [|0;nombre de ligne-1|] si instruction inexistante.*/
  /*Renvoie [|nombre de ligne ; 2*nombre de ligne-1|] si erreur dans un parametre d'une instruction.*/
  /*Renvoie [|4*nombre de ligne ; 4*nombre de ligne-1|] si qu'une etiquette sur la ligne,sans instruction (donc ligne incomplete).*/
  /*Renvoie [|3*nombre de ligne ; 3*nombre de ligne-1|] si tabulation entre instruction et le parametre.*/
  /*Renvoie [|5*nombre de ligne ; 5*nombre de ligne-1|] ligne vide ou saut de ligne*/

  /*Renvoie -2 si ouverture du fichier impossible.*/

/*----------------------------------------------------------------------------*/





  FILE* fichier=NULL;
  fichier=fopen(nom_fichier,"r");


  if(fichier!=NULL){

    /*On parcourt notre fichier en entier, ligne par ligne et on fait les verifications, necessaires.*/
    for(int i=0;i<nombre_ligne;i++){
      char ligne[nombre_caractere_max_ligne]; /*Contiendra notre ligne.*/


      /*On initialise deux tableaux vides qui nous aiderons pour les verifications.*/
      char ligne_aide_verif[nombre_caractere_max_ligne]; /*Le premier pour les instructions.*/
      char donnee_aide_verif[nombre_caractere_max_ligne];  /*Le second pour les parametres/donnees associe aux instructions.*/

      fgets(ligne,nombre_caractere_max_ligne,fichier);/*On recupere notre ligne.*/


      if( strlen(ligne)==1 && ligne[0]=='\n' && i==nombre_ligne-1){ /*J'ai pu parcourir toute la boucle sans probleme jusqu'a la fin de mon fichier.*/
        return -1;
      }


      if(tab_etiquette[i]==NULL){

        int j=0; /*Compteur que l'on utilisera comme indice pour parcourir la ligne.*/

        if(ligne[j]=='\n'){
          return i+4*nombre_ligne; /*On a un saut de ligne a la ligne i. Renvoie une erreur.*/
        }


        while( ligne[j]==' ' || ligne[j]=='\t' ){ /*On a pas d'etiquette, donc on a d'abord des espaces et/ou des tabulations. On les ignore.*/
          j++;
        }

         /*Une fois avoir ignore les espaces et tabulation, on est cense tomber sur une instruction...*/

        if(ligne[j]=='\n'){
          return i+4*nombre_ligne; /*ligne vide renvoyer une erreur*/
        }

        else{
          int k=0; /*Compteur que l'on utilisera comme indice pour remplir le premier tableau qui nous aidera a la verification des instructions.*/
          /*On recupere tout ce qui est cense etre une instruction. Donc on recupere tous jusqu'a un espace,un saut de ligne ou une tabulation.*/
          while( ligne[j]!=' ' && ligne[j]!='\n' && ligne[j]!='\t'){
            ligne_aide_verif[k]=ligne[j];
            j++;
            k++;
          }
          ligne_aide_verif[k]='\0'; /*On oublie pas l'octet nul a la fin. */

          if(ligne[j]=='\t'){
            return i+2*nombre_ligne; /*On a une tabulation au lieu d'un espace apres l'instruction. Renvoyer une erreur*/
          }

          /*On a a present recuperer la potentielle instruction. Est-elle existante ? Si oui, la donnee/parametre associe est il coherent ?*/
          /*C'est l'objet de cette seconde partie.*/
          /*On verifie si l'instruction est existante. Si oui, on verifie juste apres les parametres. Sinon on renvoie une erreur.*/


          if(strcmp(ligne_aide_verif,"ipush")==0 || strcmp(ligne_aide_verif,"ipop")==0 || strcmp(ligne_aide_verif,"dup")==0 || strcmp(ligne_aide_verif,"ret")==0 || strcmp(ligne_aide_verif,"halt")==0 ){

            if(ligne[j]!='\n'){
              return i+nombre_ligne; /*Renvoyer une erreur. On a une donnee apres l'instruction. Ce n'est pas possible.*/
            }
          }

          else{
            if(strcmp(ligne_aide_verif,"jmp")==0 || strcmp(ligne_aide_verif,"jpz")==0 || strcmp(ligne_aide_verif,"call")==0 ){


              if(ligne[j]=='\n'){
                return i+nombre_ligne; /*Renvoyer une erreur. On a rien apres pas possible.Forcement il y a une etiquette*/
              }
              j++; /*On saute l'espace.*/
              int l=0; /*Compteur que l'on utilise comme un indice pour pouvoir remplir le tableau de donnee/parametre*/
              while(ligne[j]!='\n'){
                donnee_aide_verif[l]=ligne[j];
                l++;
                j++;
              }
              donnee_aide_verif[l]='\0';

              int verif_etiquette_existante=1; /*Drapeau qui nous dira si l'etiquette est une etiquette existante ou pas.*/

              for(int indice_tab_etiquette=0; indice_tab_etiquette<nombre_ligne ; indice_tab_etiquette++){
                /*Rappel : il y a NULL dans le tableau d'etiquette si pas d'etiquette a la ligne indice_tab_etiquette, l'etiquette correspondante sinon.*/
                if( (tab_etiquette[indice_tab_etiquette]!=NULL) && (strcmp(donnee_aide_verif,tab_etiquette[indice_tab_etiquette])==0) ){
                  verif_etiquette_existante=0;
                }
              }



              if(verif_etiquette_existante==1){
                return i+nombre_ligne; /*Donnee erronee : pas d'etiquette correpondante.*/
              }


            }
            else{
              if(strcmp(ligne_aide_verif,"push")==0 || strcmp(ligne_aide_verif,"pop")==0 || strcmp(ligne_aide_verif,"read")==0 || strcmp(ligne_aide_verif,"write")==0 ){
                /*Apres ces instructions on a une adresse memoire. Donc un nombre allant de 0 à 5000 compris.*/


                /*Si on a un saut de ligne, on considere que le parametre vaut 0 par defaut.*/
                if(ligne[j]==' '){

                  j++;
                  int verif_chiffre_uniquement=0; /*Cette variable aura de fonction. Dans un premier temps servira de drapeau, puis d'aide pour verifier si la donnee est correct.*/
                  int l=0; /*On utilisera cette variable comme indice pour recuperer les donnees numeriques.*/


                  /*On recupere tous ce qui est cense etre une donnee. (Donc tous après l'instruction, jusqu'au prochain saut de ligne.) On doit avoir uniquement des chiffres positifs. (Pas de signe moins) Si on recupere autre choses qu'un chiffre, on arrete et on renvoie une erreur.*/
                  while( ligne[j]!='\n'){
                    if( !(ligne[j]=='1' || ligne[j]=='2' || ligne[j]=='3' || ligne[j]=='4' || ligne[j]=='5' || ligne[j]=='6' || ligne[j]=='7' || ligne[j]=='8' || ligne[j]=='9' || ligne[j]=='0') ){
                      verif_chiffre_uniquement=1;
                      break;
                    }
                    donnee_aide_verif[l]=ligne[j];
                    l++;
                    j++;
                  }

                  if(verif_chiffre_uniquement==1){
                    return i+nombre_ligne; /*On retourne une erreur. On ne peut pas avoir que des chiffres dans notre donnee/parametre */
                  }
                  donnee_aide_verif[l]='\0';

                  /*On verifie la longueur du nombre recolte.*/
                  if(strlen(donnee_aide_verif)>=5){
                    return i+nombre_ligne; /*Retourner une erreur : donnee trop grande*/
                  }

                  /*On affine notre verification...*/
                  verif_chiffre_uniquement=strtol(donnee_aide_verif,NULL,10);
                  if(verif_chiffre_uniquement>5000){
                    return i+nombre_ligne; /*Retourner une erreur : donnee trop grande*/
                  }
                }




              }
              else{
                if(strcmp(ligne_aide_verif,"op")==0){
                    /*Apres cette instruction on a un nombre entre 0 et 15 compris.*/

                  if(ligne[j]==' '){

                    j++;
                    int verif_chiffre_uniquement=0;
                    int l=0;

                    /*On recupere tous ce qui est cense etre une donnee. (Donc tous après l'instruction, jusqu'au prochain saut de ligne.) On doit avoir uniquement des chiffres positifs. (Pas de signe moins) Si on recupere autre choses qu'un chiffre, on arrete et on renvoie une erreur.*/
                    while( ligne[j]!='\n'){
                      if( !(ligne[j]=='1' || ligne[j]=='2' || ligne[j]=='3' || ligne[j]=='4' || ligne[j]=='5' || ligne[j]=='6' || ligne[j]=='7' || ligne[j]=='8' || ligne[j]=='9' || ligne[j]=='0') ){
                        verif_chiffre_uniquement=1;
                        break;
                      }
                      donnee_aide_verif[l]=ligne[j];
                      l++;
                      j++;
                    }

                    if(verif_chiffre_uniquement==1){
                      return i+nombre_ligne;/*On retourne une erreur. On ne peut pas avoir que des chiffres dans notre donnee/parametre */
                    }
                    donnee_aide_verif[l]='\0';

                    /*On verifie la longueur du nombre recolte.*/
                    if(strlen(donnee_aide_verif)>=3){
                      return i+nombre_ligne;/*Retourner une erreur : donnee trop grande*/
                    }
                    /*On affine notre verification...*/
                    verif_chiffre_uniquement=strtol(donnee_aide_verif,NULL,10);
                    if(verif_chiffre_uniquement>15){
                      return i+nombre_ligne;/*Retourner une erreur : donnee trop grande*/
                    }
                  }



                }
                else{
                  if(strcmp(ligne_aide_verif,"push#")==0){
                    /*Apres cette instruction on a un nombre SIGNE sur 4 octets.*/

                    if(ligne[j]==' '){

                      j++;
                      long long verif_chiffre_uniquement=0; /*On utilisera cette variable comme drapeau, puis comme aide a la averification du parametre. Type long long necessaire.*/
                      int l=0;

                      /*Avec cette instruction, on peut avoir un signe negatif. On ne l'oublie pas de traiter ce cas.*/
                      if(ligne[j]=='-'){
                        donnee_aide_verif[l]='-';
                        l++;
                        j++;
                        if(ligne[j]=='\n'){
                          return i+nombre_ligne;/*Retourner une erreur, on a que un moins ici*/
                        }
                      }

                      /*Apres avoir traiter le cas du signe negatif, on recupere ce qui est cense etre des chiffres...*/

                      while( ligne[j]!='\n'){
                        if( !(ligne[j]=='1' || ligne[j]=='2' || ligne[j]=='3' || ligne[j]=='4' || ligne[j]=='5' || ligne[j]=='6' || ligne[j]=='7' || ligne[j]=='8' || ligne[j]=='9' || ligne[j]=='0') ){
                          verif_chiffre_uniquement=1;
                          break;
                        }
                        donnee_aide_verif[l]=ligne[j];
                        l++;
                        j++;
                      }

                      if(verif_chiffre_uniquement==1){
                        return i+nombre_ligne;/*Retourner une erreur : on ne peut pas avoir d'autre donee que des chiffres*/
                      }
                      donnee_aide_verif[l]='\0';

                      if(strlen(donnee_aide_verif)>=12){
                        return i+nombre_ligne; /*Retourner une erreur : donnee trop grande*/
                      }
                      /*On affine notre verification...*/
                      verif_chiffre_uniquement=strtol(donnee_aide_verif,NULL,10);
                      if(verif_chiffre_uniquement>2147483647 || verif_chiffre_uniquement<-2147483648){ /*D'ou le type long long pour verif_chiffre_uniquement!*/
                        return i+nombre_ligne; /*Retourner une erreur : donnee trop grande*/
                      }
                    }




                  }
                  else{
                    if(strcmp(ligne_aide_verif,"rnd")==0){

                      /*Meme commentaire que plus haut, pour les donnee/parametre entiers.*/


                      if(ligne[j]==' '){

                        j++;
                        unsigned long verif_chiffre_uniquement=0;
                        int l=0;

                        while( ligne[j]!='\n'){
                          if( !(ligne[j]=='1' || ligne[j]=='2' || ligne[j]=='3' || ligne[j]=='4' || ligne[j]=='5' || ligne[j]=='6' || ligne[j]=='7' || ligne[j]=='8' || ligne[j]=='9' || ligne[j]=='0') ){
                            verif_chiffre_uniquement=1;
                            break;
                          }
                          donnee_aide_verif[l]=ligne[j];
                          l++;
                          j++;
                        }

                        if(verif_chiffre_uniquement==1){
                          return i+nombre_ligne; /*Retourner une erreur : On ne peut pas avoir d'autre donee que des chiffres*/
                        }
                        donnee_aide_verif[l]='\0';

                        if(strlen(donnee_aide_verif)>=11){
                          return i+nombre_ligne; //Retourner une erreur : donnee trop grande
                        }
                        verif_chiffre_uniquement=strtol(donnee_aide_verif,NULL,10);
                        if(verif_chiffre_uniquement>2147483648){ /*rnd x renvoie un nombre aleatoire entre 0 et x-1.... */
                          return i+nombre_ligne; //Retourner erreur par possible donnee trop grande
                        }
                      }

                    }
                    else{
                      return i;  /*On renvoie une erreur. L'instruction recolte n'est pas existante.*/
                    }
                  }
                }
              }

            }

          }
        }
      }
      else{


        int j=0; /*Compteur que l'on utilisera comme indice pour parcourir la ligne.*/

        if(ligne[j]=='\n'){
          return i+4*nombre_ligne; /*On a un saut de ligne a la ligne i. Renvoie une erreur.*/
        }


        /*On ignore tous les caracteres correpondant a l'etiquette.*/

        while( ligne[j]!=':'){
          j++;
        }
        j++;
        if(ligne[j]=='\n'){
          return i+3*nombre_ligne; /*Rien apres l'etiquette renvoie erreur*/
        }



        /*On ignore tous les espaces et tabulations apres l'etiquette, pour acceder à la potentielle instructions.*/
        while( ligne[j]==' ' || ligne[j]=='\t' ){
          j++;
        }

        if(ligne[j]=='\n'){
          return i+3*nombre_ligne; /*Rien apres l'etiquette. Renvoyer Erreur.*/
        }

        /*Comme a partir du deuxieme if de la fonction. Comme lorsqu'on a pas d'etiquette.*/
        else{
          int k=0;
          while( (ligne[j]!=' ') && (ligne[j]!='\n') && (ligne[j]!='\t')){
            ligne_aide_verif[k]=ligne[j];
            j++;
            k++;
          }

          ligne_aide_verif[k]='\0';
          if(ligne[j]=='\t'){
            return i+2*nombre_ligne; //tabulation au lieu de espace renvoyer une erreur
          }



          if(strcmp(ligne_aide_verif,"ipush")==0 || strcmp(ligne_aide_verif,"ipop")==0 || strcmp(ligne_aide_verif,"dup")==0 || strcmp(ligne_aide_verif,"ret")==0 || strcmp(ligne_aide_verif,"halt")==0 ){

            if(ligne[j]!='\n'){
              return i+nombre_ligne; //renvoyer erreur. On a qql chose apres c'est pas possible pas de donnee normalement
            }

          }
          else{
            if(strcmp(ligne_aide_verif,"jmp")==0 || strcmp(ligne_aide_verif,"jpz")==0 || strcmp(ligne_aide_verif,"call")==0 ){



              if(ligne[j]=='\n'){
                return i+nombre_ligne; //renvoyer erreur. On a rien apres pas possible. Forcement il y a une etiquette
              }
              j++;
              int l=0;
              while(ligne[j]!='\n'){
                donnee_aide_verif[l]=ligne[j];
                l++;
                j++;
              }
              donnee_aide_verif[l]='\0';

              int verif_etiquette_existante=1;

              for(int indice_tab_etiquette=0; indice_tab_etiquette<nombre_ligne ; indice_tab_etiquette++){

                if( (tab_etiquette[indice_tab_etiquette]!=NULL) && (strcmp(donnee_aide_verif,tab_etiquette[indice_tab_etiquette])==0) ){
                  verif_etiquette_existante=0;
                }

              }


              if(verif_etiquette_existante==1){
                return i+nombre_ligne;//retourner erreur donnee mauvaise
              }


            }
            else{
              if(strcmp(ligne_aide_verif,"push")==0 || strcmp(ligne_aide_verif,"pop")==0 || strcmp(ligne_aide_verif,"read")==0 || strcmp(ligne_aide_verif,"write")==0 ){




                if(ligne[j]==' '){

                  j++;
                  int verif_chiffre_uniquement=0;
                  int l=0;


                  while( ligne[j]!='\n'){
                    if( !(ligne[j]=='1' || ligne[j]=='2' || ligne[j]=='3' || ligne[j]=='4' || ligne[j]=='5' || ligne[j]=='6' || ligne[j]=='7' || ligne[j]=='8' || ligne[j]=='9' || ligne[j]=='0')){
                      verif_chiffre_uniquement=1;
                      break;
                    }
                    donnee_aide_verif[l]=ligne[j];
                    l++;
                    j++;
                  }
                  donnee_aide_verif[l]='\0';


                  if(verif_chiffre_uniquement==1){
                    return i+nombre_ligne; //retourner une erreur peut pas avoir d'autre donee que des chiffres
                  }

                  if(strlen(donnee_aide_verif)>=5){
                    return i+nombre_ligne; //retourner une erreur : donnee trop grande
                  }
                  verif_chiffre_uniquement=strtol(donnee_aide_verif,NULL,10);
                  if(verif_chiffre_uniquement>5000){
                    return i+nombre_ligne; //retourner erreur pas possible donnee trop grande
                  }
                }




              }
              else{
                if(strcmp(ligne_aide_verif,"op")==0){


                  if(ligne[j]==' '){

                    j++;
                    int verif_chiffre_uniquement=0;
                    int l=0;

                    while( ligne[j]!='\n'){
                      if( !(ligne[j]=='1' || ligne[j]=='2' || ligne[j]=='3' || ligne[j]=='4' || ligne[j]=='5' || ligne[j]=='6' || ligne[j]=='7' || ligne[j]=='8' || ligne[j]=='9' || ligne[j]=='0') ){
                        verif_chiffre_uniquement=1;
                        break;
                      }
                      donnee_aide_verif[l]=ligne[j];
                      l++;
                      j++;
                    }

                    if(verif_chiffre_uniquement==1){
                      return i+nombre_ligne; //retourner une erreur peut pas avoir d'autre donee que des chiffres
                    }
                    donnee_aide_verif[l]='\0';

                    if(strlen(donnee_aide_verif)>=3){
                      return i+nombre_ligne; //retourner une erreur : donnee trop grande
                    }
                    verif_chiffre_uniquement=strtol(donnee_aide_verif,NULL,10);
                    if(verif_chiffre_uniquement>15){
                      return i+nombre_ligne; //retourner erreur par possible donnee trop grande
                    }
                  }



                }
                else{
                  if(strcmp(ligne_aide_verif,"push#")==0){


                    if(ligne[j]==' '){

                      j++;
                      long long verif_chiffre_uniquement=0;
                      int l=0;

                      if(ligne[j]=='-'){
                        donnee_aide_verif[l]='-';
                        l++;
                        j++;
                        if(ligne[j]=='\n'){
                          return i+nombre_ligne; //retourner une erreur on a que un moins
                        }
                      }


                      while( ligne[j]!='\n'){
                        if( !(ligne[j]=='1' || ligne[j]=='2' || ligne[j]=='3' || ligne[j]=='4' || ligne[j]=='5' || ligne[j]=='6' || ligne[j]=='7' || ligne[j]=='8' || ligne[j]=='9' || ligne[j]=='0') ){
                          verif_chiffre_uniquement=1;
                          break;
                        }
                        donnee_aide_verif[l]=ligne[j];
                        l++;
                        j++;
                      }

                      if(verif_chiffre_uniquement==1){
                        return i+nombre_ligne; //retourner une erreur, on ne peut pas avoir d'autre donee que des chiffres
                      }
                      donnee_aide_verif[l]='\0';

                      if(strlen(donnee_aide_verif)>=12){
                        return i+nombre_ligne; //retourner une erreur : donnee trop grande
                      }
                      verif_chiffre_uniquement=strtol(donnee_aide_verif,NULL,10);
                      if(verif_chiffre_uniquement>2147483647 || verif_chiffre_uniquement<-2147483649){
                        return i+nombre_ligne; //retourner erreur par possible donnee trop grande
                      }

                    }




                  }
                  else{
                    if(strcmp(ligne_aide_verif,"rnd")==0){




                      if(ligne[j]==' '){

                        j++;
                        unsigned long verif_chiffre_uniquement=0;
                        int l=0;

                        while( ligne[j]!='\n'){
                          if( !(ligne[j]=='1' || ligne[j]=='2' || ligne[j]=='3' || ligne[j]=='4' || ligne[j]=='5' || ligne[j]=='6' || ligne[j]=='7' || ligne[j]=='8' || ligne[j]=='9' || ligne[j]=='0') ){
                            verif_chiffre_uniquement=1;
                            break;
                          }
                          donnee_aide_verif[l]=ligne[j];
                          l++;
                          j++;
                        }

                        if(verif_chiffre_uniquement==1){
                          return i+nombre_ligne; //retourner une erreur. On ne peut pas avoir d'autre donee que des chiffres
                        }
                        donnee_aide_verif[l]='\0';

                        if(strlen(donnee_aide_verif)>=11){
                          return i+nombre_ligne; //retourner une erreur : donnee trop grande
                        }
                        verif_chiffre_uniquement=strtol(donnee_aide_verif,NULL,10);
                        if(verif_chiffre_uniquement>2147483648){
                          return i+nombre_ligne; //retourner erreur par possible donnee trop grande
                        }
                      }



                    }
                    else{
                      return i; //renvoyer une erreur pas d'instruction correpondantes
                    }
                  }
                }
              }

            }

          }
        }


      }
    }

    fclose(fichier);
    return -1;
  }

  else{
    fclose(fichier);
    return -2;
  }

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
        /*Attention, une etiquette peut avoir des chiffres. SI et seulement si il y a une etiquette, on commence notre recherche apres avoir croise la carctere ':' */
        /*On s'aidera pour cela d'un drapeau.*/


        int flag=1;
        if(tab_etiquette[i]==NULL){
          flag=0;
        }


        for(int j=0;j<strlen(ligne);j++){

          if(ligne[j]==':'){  /*Si on croise ':', on met flag a 0. Si il n'y a pas d'etiquette, on deja flag a 0. Donc pas de probleme.*/
            flag=0;
          }

          if( (ligne[j]=='0' || ligne[j]=='1' || ligne[j]=='2' || ligne[j]=='3' || ligne[j]=='4' || ligne[j]=='5' || ligne[j]=='6' || ligne[j]=='7' || ligne[j]=='8' || ligne[j]=='9' || ligne[j]=='-') && flag==0 ) {
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

int creation_fichier_langage_machine(int *tab_instruction_courante_decimale,int *tab_donnee){
  /*Apres avoir recupere les instruction dans un tableau et les donnee dans un tableau, on peut a present creer le fichier contenant le langage machine.*/

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


/*---------------------------Programme Principale-----------------------------*/

int main(int argc,char* argv[]){

  //On verifie si le fichier est vide ou pas.
  int verif=verif_fichier_vide(argv[1]);
  if(verif==-1){
    printf("Ouverture du fichier impossible. Fichier probablement inexistant.\n");
    return 0;
  }
  if(verif==1){
    printf("Le fichier contenant le langage assembleur est vide ! Traduction en langage machine impossible.\n");
    return 0;
  }


  //On rajoute un saut de ligne a la fin, pour s'assurer de lire toute les lignes avec la fonction fgets, plus tard.
  verif=actualisation_fichier_code_assembleur(argv[1]);
  if(verif==-1){
    printf("Ouverture du fichier impossible. Traduction en langage machine non effectuee.\n");
    return 0;
  }


  //On recupere maintenant le nombre de ligne dans le fichier.
  //Pour etre exact le nombre de fois ou on a clique sur la touche entree pour faire un saut de ligne. Detail tres tres important.
  //On recuperera donc surement des lignes inutiles. A nous de faire attention plus tard, pour bien les reperer.
  int nombre_ligne=nombre_ligne_fichier(argv[1]);
  if(nombre_ligne==-1){
    printf("Erreur lors du calcul du nombre de ligne. Ouverture du fichier impossible. Traduction en langage machine impossible.\n"); //Le programme s'arrete si erreur, il y a.
    return 0;
  }



  //Avant de proceder a la verification de la syntaxe du fichier contenant le langage assembleur, on recupere les potentielles etiquettes.
  //Attention le tableau d'etiquette est un tableau de char*.
  //Dans chaque case du tableau, on a utiliser la fonction malloc pour pouvoir ecrire notre chaine de caractere.
  //Ne pas oublier de liberer l'espace memoire a chaque fin potentiel du programme.
  char* tab_etiquette[nombre_ligne];
  verif=recuperation_etiquette(argv[1],nombre_ligne,tab_etiquette);
  if(verif==-1){
    printf("Erreur lors de la recuperation des etiquettes. Ouverture du fichier impossible. Traduction en langage machine impossible.\n");
    return 0;
  }

  //On a donc un tableau contenant les etiquettes. On verifie ici, que tout est bien coherent.
  verif=verif_etiquette(nombre_ligne,tab_etiquette);
  if(verif==-1){
    printf("Erreur de syntaxe : doublon(s) dans les etiquettes. Traduction en langage machine impossible.\n");
    liberation_espace_memoire_tab_etiquette(nombre_ligne,tab_etiquette);
    return 0;
  }
  if(verif!=-2){
    printf("Erreur de syntaxe à la ligne %d : espace et/ou tabulation presente dans les etiquettes. Traduction en langage machine impossible.\n",verif);
    liberation_espace_memoire_tab_etiquette(nombre_ligne,tab_etiquette);
    return 0;
  }

  //Sachant que les etiquettes sont corrects on peut maintenant verifier, la coherence syntaxique du fichier contenant le code en langage assembleur.
  verif=verif_instruction_donnee(argv[1],nombre_ligne,tab_etiquette);

  if(verif==-2){
    printf("Erreur lors de la verification des instructions et donnees. Ouverture du fichier impossible. Traduction en langage machine impossible.\n");
    liberation_espace_memoire_tab_etiquette(nombre_ligne,tab_etiquette);
    return 0;
  }

  if(verif==-1){
    printf("Il n'y a eucune erreur dans le fichier! On peut passer a la traduction du code en langage machine!\n");
  }
  else{
    if(verif>=0 && verif<=nombre_ligne-1){
      printf("Erreur de syntaxe a la ligne %d : instruction non existante.\n",verif);
      liberation_espace_memoire_tab_etiquette(nombre_ligne,tab_etiquette);
      return 0;
    }
    else{
      if(verif>=nombre_ligne && verif<=2*nombre_ligne-1){
        printf("Erreur de syntaxe a la ligne %d : donnee non conforme.\n",verif-nombre_ligne);
        liberation_espace_memoire_tab_etiquette(nombre_ligne,tab_etiquette);
        return 0;
      }
      else{
        if(verif>=2*nombre_ligne && verif<=3*nombre_ligne-1){
          printf("Erreur de syntaxe a la ligne %d : tabulation au lieu d' un espace entre l'instruction et la donnee.\n",verif-2*nombre_ligne);
          liberation_espace_memoire_tab_etiquette(nombre_ligne,tab_etiquette);
          return 0;
        }
        else{
          if(verif>=3*nombre_ligne && verif<=4*nombre_ligne-1){
            printf("Erreur de syntaxe a la ligne %d : il n'y a qu'une etiquette. Manque une instruction et/ou une donnee.",verif-3*nombre_ligne);
            liberation_espace_memoire_tab_etiquette(nombre_ligne,tab_etiquette);
            return 0;
          }
          else{
            if(verif>=4*nombre_ligne && verif<=5*nombre_ligne-1){
              printf("Erreur de syntaxe a la ligne %d : ligne vide ou saut de la ligne.",verif-4*nombre_ligne);
              liberation_espace_memoire_tab_etiquette(nombre_ligne,tab_etiquette);
              return 0;
            }
          }
        }
      }
    }
  }


  //On sait maintenant qu'il n'y a (probablement) pas d'erreur de syntaxe. On peut passer a la traduction en langage machine.
  //On initialise, deux autres tableaux qui recupereront nos differents outils pour creer le fichier en langage machine.
  //Il y a peut etre des cases en trop (Nombre de ligne peut etre plus grand que prevu.)
  //On reglera ce probleme en remplissant le tableau tab_instruction_courante_decimale par l'entier 100 lorsque c'est la cas. (Aucune instruction n'est associe a 100)

  int tab_instruction_courante_decimale[nombre_ligne];
  int tab_donnee[nombre_ligne];

  //On recupere les instructions.
  verif=traduction_instruction_octect_poids_fort(argv[1],nombre_ligne,&tab_instruction_courante_decimale[0]);
  if(verif==-1){
    printf("Erreur lors de la recuperation des instructions assembleurs. Ouverture du fichier impossible. Traduction en langage machine impossible.\n");
    liberation_espace_memoire_tab_etiquette(nombre_ligne,tab_etiquette);
    return 0;
  }

  //Enfin, on recupere les donnees.
  verif=recuperation_donnee(argv[1],nombre_ligne,tab_instruction_courante_decimale,tab_etiquette,tab_donnee);
  if(verif==-1){
    printf("Erreur lors de la recuperation des donnees. Ouverture du fichier impossible. Traduction en langage machine impossible.\n");
    liberation_espace_memoire_tab_etiquette(nombre_ligne,tab_etiquette);
    return 0;
  }

  //On cree maintenant le fichier contenant le langage machine.
  verif=creation_fichier_langage_machine(tab_instruction_courante_decimale,tab_donnee);
  if(verif==-1){
    printf("Erreur lors de la creation du fichier en langage machine. Ouverture du fichier impossible. Traduction en langage machine impossible.\n");
    liberation_espace_memoire_tab_etiquette(nombre_ligne,tab_etiquette);
    return 0;
  }

  printf("Le fichier hexa.txt a ete cree avec succes!");
  liberation_espace_memoire_tab_etiquette(nombre_ligne,tab_etiquette);
  return 0;
}
