#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#define TAILLE_BUFFER 100
#define VERSION_RAFFINEMENT 1
#define LECTURE 0
static void erreur_si(int test,char *chaineErreur)
{
  if(test)
    {
      perror(chaineErreur);
      exit(1);
    }
  
}
static void copierBufferDansResultat(char *resultat,char *buffer,int curseurResultat,int tailleBuffer)
{
  
  int i;
  for(i=0;i<tailleBuffer;i++){
 
      resultat[i+curseurResultat]=buffer[i];
      
  }
      
    
  
}

static void lectureV2(int socket)
{
/*
    Utilisation d'un tableau de taille TAILLE_BUFFER s'agrandissant dans le cas 
    ou il y a plus que TAILLE_BUFFER caractères à lire . Pour diminuer le nombre d'appels système utilisation d'un buffer dans read .

*/  
  char *buffer=malloc(sizeof(char)*TAILLE_BUFFER);
  char *resultat=malloc(sizeof(char)*TAILLE_BUFFER);
  int tailleResultatMax=TAILLE_BUFFER;
  int curseurResultat=0;

  int sizeRead=0;
  while((sizeRead=read(socket,buffer,sizeof(char)*TAILLE_BUFFER))>0)
    {
      
      if(tailleResultatMax<curseurResultat+sizeRead+1)
	{
	  resultat=realloc(resultat,tailleResultatMax*2*sizeof(char));
	  tailleResultatMax=tailleResultatMax*2;
	}
      copierBufferDansResultat(resultat,buffer,curseurResultat,sizeRead);

      curseurResultat+=sizeRead;
    }
  free(buffer);
  resultat[curseurResultat-1]=0; 

  if(curseurResultat>0)
    printf("Caractere lus : %d\nChaine :\n\n%s\n\n",curseurResultat,resultat);

 
  free(resultat);

  


}
static void lecture(int socket )
{
  char *buffer=malloc(sizeof(char)*TAILLE_BUFFER);
  int sizeRead;
  while((sizeRead=read(socket,buffer,sizeof(char)*TAILLE_BUFFER))>0)
      write(1,buffer,sizeRead*sizeof(char));
  free(buffer);



}

static bool usage(argc)
{
  if(argc<2)
    {
      fprintf(stderr,"\nUsage : ./httpget [adresse web (www.*.*)]\n\n");
      return false;
    }
  return true;
}
int main(int argc,char **argv)
{

  
  if(!usage(argc))
    return EXIT_FAILURE;

  /* Récupération de l'adresse ip à partir de l'adresse web */
  struct hostent *host1=gethostbyname(argv[1]);
  
  if(host1==NULL){
    fprintf(stderr,"Erreur , adresse introuvable\n");
    return EXIT_FAILURE;
  }
  
  if(host1->h_addrtype!=AF_INET){
    fprintf(stderr,"Erreur , adresse non AF_INET");
    return EXIT_FAILURE;
  }

  /*  Récupération de l'adresse ip obtenue par gethostbyname dans in*/
  struct in_addr *in=(struct in_addr*)host1->h_addr;
  fprintf(stderr,"\n\nAdresse OK : %x\n",in->s_addr);

  /* Creation du socket */
  int socket1=socket(AF_INET,SOCK_STREAM,0);
  erreur_si(socket1<0,"Descripteur socket KO");
  fprintf(stderr,"Descripteur socket OK : %d \n",socket1);

  /* Connection entre le socket et l'adresse in sur le port web (80)*/
  struct sockaddr_in sockin={AF_INET,htons(80),*in};
  int er=connect(socket1,(struct sockaddr*)&sockin,sizeof(sockin));
  erreur_si(er<0,"Connect KO");
  fprintf(stderr,"Connect OK\n");

  /*Ecriture sur le socket (2 versions disponibles )*/

  char *get="GET /\n\n";

  #if VERSION_RAFFINEMENT == 0

  er=write(socket1,get,sizeof(char)*strlen(get));

  #else

  /* Le dup2 est utilisé pour pouvoir faire un fclose sans fermé le socket1*/
  int socket1Bis=4;
  dup2(socket1,socket1Bis);
  FILE *fichierEcriture=fdopen(socket1Bis,"w");
  er=fprintf(fichierEcriture,get);
  fflush(fichierEcriture);
  fclose(fichierEcriture);
  #endif

  erreur_si(er!=sizeof(char)*strlen(get),"Ecriture KO");
  
  
  fprintf(stderr,"Ecriture OK : %s\n",get);

  
  /* Lecture sur le socket (2 versions disponibles)*/
  #if LECTURE == 0  
  lecture(socket1);
  #else 
  lectureV2(socket1); /*Version avec tableau dynamique s'agrandissant */
  #endif

  fprintf(stderr,"\n\nLecture OK\n\n");

  return EXIT_SUCCESS;
  

}

