#include<stdlib.h>
#include<stdio.h>

typedef struct{
    int SP;
    int PC;
    int tableau_pile[5000];
}Machine;

void AfficherPile(const Machine* m){
    int i=0;
    printf("PC: %d\nSP: %d\n",m->PC,m->SP);
    while (i<=(m->SP)){          
        printf("Tableau_pile[%d]=%d\n",i,m->tableau_pile[i]); 
        i++;
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

void ipush(Machine* m, int n){
    (m->tableau_pile[m->SP])=(m->tableau_pile[n]);  //l'élement au sommet de la pile est remplacé par l'élément à l'adresse n
}

void pop(Machine* m,int x){
    (m->SP)--;
    (m->tableau_pile[x]=(m->tableau_pile[m->SP]));
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
    scanf("%d",&(m->tableau_pile[x]));  
}

void write(Machine* m,int x){
    printf("Valeur de la variable à l'adresse %d: %d\n",x,m->tableau_pile[x]);
}

void halt(void){
    exit(0); //exit(0) si tout s'est bien passé; exit(1) sinon
}
int main(int argc, char const *argv[])
{
    Machine m;
    push_constante(&m,6);
    AfficherPile(&m);
    return 0;
}
