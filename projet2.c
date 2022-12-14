#include<stdlib.h>
#include<stdio.h>

#define nombre_caractere_max_ligne 1000

char* Instruction[]={"push","push#","ipush","pop","ipop","dup","op","jmp","jpz","rnd","read","write","call","ret","halt"};


typedef struct{
    int SP;
    int PC;
    long long int tableau_pile[5000];
}Machine;

int nombre_ligne_fichier(char* nom_fichier){
/*--------Renvoie le nombre de fois où on trouve dans un certain fichier le caractère saut de ligne--------*/
/* ----------------Attention ne renvoie pas le nombre de ligne visible par l'utilisateur-------------------*/
/*Renvoie le nombre de fois où on a appuyer sur entrée pour sauter une ligne lors de la création du fichier*/
/*------------On verifiera si le fichier est bien ouvert--------------*/
/*Si problème d'ouverture, la fonction va renvoyer -1 pour le signaler*/
FILE* fichier=NULL;
fichier=fopen(nom_fichier,"r");
int cpt=0; /*Pour compter le nombre de ligne*/
char caractere_courant; /*Pour stocker le caractère qu'on est en train de lire*/
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

int instruction_courante[100];
int donnee_courante[100];

int stockage_hexa(char* nom_fichier){
    FILE* fichier=NULL;
    fichier= fopen(nom_fichier,"r");
    int n=nombre_ligne_fichier(nom_fichier);
    if(fichier!=NULL){
        for(int i=0;i<n;i++){   //on parcoure toutes les lignes
        fscanf(fichier,"%x %x",&instruction_courante[i],&donnee_courante[i]);
        }
        fclose(fichier);
        return 0;
    }
    else{
        fclose(fichier);
        return -1;     //fichier vide
    }
}


void initiliation_pile(Machine* m){
    m->PC=0;
    m->SP=0;
    for(int j=0;j<5000;j++){
        m->tableau_pile[j]=3000000000;  //on remplit avec des valeurs inatégnables en fonctionnement (>2*10^9)
    }
}

void AfficherPile(const Machine* m){
    int i=0;
    printf("PC: %d\nSP: %d\n",m->PC,m->SP);
    while (i<(m->SP)){   
        printf("Tableau_pile[%d]=%lld    \n",i,m->tableau_pile[i]); 
        i++;
    }
    //printf("valeur indice 231: %d\n",m->tableau_pile[231]);
    for(int j=4998;j<5000;j++){
        if((m->tableau_pile[j])!=0){
            printf("Valeur au %d-eme indice de la pile : %lld\n",j,m->tableau_pile[j]);
        }
    }
}

void push(Machine* m,int x){
    m->tableau_pile[m->SP]=(m->tableau_pile)[x];
    m->SP++;
}

void push_constante(Machine* m, int entier){
    m->tableau_pile[m->SP]=entier;
    m->SP++;
}

void ipush(Machine* m){
    int n=m->tableau_pile[(m->SP)-1];
    m->tableau_pile[(m->SP)-1]=m->tableau_pile[n]; 
}

void pop(Machine* m,int x){
    (m->SP)--;
    (m->tableau_pile[x]=(m->tableau_pile[m->SP]));
}

void ipop(Machine* m){
    int n=m->tableau_pile[(m->SP)-1];
    m->tableau_pile[n]=m->tableau_pile[(m->SP)-2];
    m->SP-=2;
}

void dup(Machine* m){
    m->tableau_pile[m->SP]=m->tableau_pile[(m->SP)-1];
    m->SP++;
}

void op(Machine* m,int i){
    if (i<0 || i>15) return;
    else{
        if(i==0){
            m->SP--;
            m->tableau_pile[(m->SP)-1]+=m->tableau_pile[m->SP];
        }
        if(i==1){
            m->SP--;
            m->tableau_pile[(m->SP)-1]-=m->tableau_pile[m->SP];
        }
        if(i==2){
            m->SP--;
            m->tableau_pile[(m->SP)-1]*=m->tableau_pile[m->SP];
        }
        if(i==3){
            m->SP--;
            m->tableau_pile[(m->SP)-1]/=m->tableau_pile[m->SP];
        }
        if(i==4){
            m->SP--;
            m->tableau_pile[(m->SP)-1]%=m->tableau_pile[m->SP];
        }
        if(i==5){
            m->tableau_pile[(m->SP)-1]=-m->tableau_pile[(m->SP)-1];
        }
        if(i==6){
            m->SP--;
            if((m->tableau_pile[(m->SP)-1])==(m->tableau_pile[m->SP])){
                m->tableau_pile[(m->SP)-1]=0;
            }
            else{
                m->tableau_pile[(m->SP)-1]=1;
            }
        }
        if(i==7){
            m->SP--;
            if((m->tableau_pile[(m->SP)-1])!=(m->tableau_pile[m->SP])){
                m->tableau_pile[(m->SP)-1]=0;
            }
            else{
                m->tableau_pile[(m->SP)-1]=1;
            }
        }
        if(i==8){
            m->SP--;
            if((m->tableau_pile[(m->SP)-1])>(m->tableau_pile[m->SP])){
                m->tableau_pile[(m->SP)-1]=0;
            }
            else{
                m->tableau_pile[(m->SP)-1]=1;
            }
        }
        if(i==9){
            m->SP--;
            if((m->tableau_pile[(m->SP)-1])>=(m->tableau_pile[m->SP])){
                m->tableau_pile[(m->SP)-1]=0;
            }
            else{
                m->tableau_pile[(m->SP)-1]=1;
            }
        }
        if(i==10){
            m->SP--;
            if((m->tableau_pile[(m->SP)-1])<(m->tableau_pile[m->SP])){
                m->tableau_pile[(m->SP)-1]=0;
            }
            else{
                m->tableau_pile[(m->SP)-1]=1;
            }
        }
        if(i==11){
            m->SP--;
            if((m->tableau_pile[(m->SP)-1])<=(m->tableau_pile[m->SP])){
                m->tableau_pile[(m->SP)-1]=0;
            }
            else{
                m->tableau_pile[(m->SP)-1]=1;
            }
        }
        if(i==12){
            m->SP--;
            m->tableau_pile[(m->SP)-1]&=m->tableau_pile[m->SP];
        }
        if(i==13){
            m->SP--;
            m->tableau_pile[(m->SP)-1]|=m->tableau_pile[m->SP];
        }
        if(i==14){
            m->SP--;
            m->tableau_pile[(m->SP)-1]^=m->tableau_pile[m->SP];
        }
        if(i==15){
            m->tableau_pile[(m->SP)-1]=~m->tableau_pile[(m->SP)-1];
        }
    }  
}

void jump(Machine* m,int Adr){
    (m->PC)+=Adr;
}

void jpz(Machine* m,int Adr){
    m->SP--;
    int x=m->tableau_pile[(m->SP)]; //element au sommet de la pile
    if(x==0){
        (m->PC)+=Adr;
    }
}

void rnd(Machine* m,int x){
    int random=rand()%x;  //random est un entier entre 0 et x-1
    m->tableau_pile[m->SP]=random;
    m->SP++;    
}
void read(Machine* m,int x){
    printf("Valeur à rentrer à l'adresse %d:\n",x);
    scanf("%lld",&(m->tableau_pile[x]));  
}

void write(Machine* m,int x){
    printf("Valeur de la variable à l'adresse %d: %lld\n",x,m->tableau_pile[x]);
}

void call(Machine* m,int Adr){
    push_constante(m,(m->PC));  //pas sur
    m->PC+=Adr;
}

void ret(Machine* m){
    pop(m,m->PC);   //pas sur non plus sah
}

void halt(void){
    exit(0); //exit(0) si tout s'est bien passé; exit(1) sinon
}
int main(int argc, char const *argv[]){
    int nbligne=nombre_ligne_fichier("hexa.txt");
    printf("%d\n",stockage_hexa("hexa.txt"));
    for (int i = 0; i < nbligne; i++){
        printf("Instruction courante indice %d: %d\t",i,instruction_courante[i]);
        printf("Donne courante indice %d: %d\n",i,donnee_courante[i]);
    }
    return 0;
}
