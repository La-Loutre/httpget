/*static int lectureV1(int socket,int argc,char **argv)
{
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
  fprintf(stderr,"Read OK\n\n");
  printf("Caractere lus : %d\nChaine :\n\n%s\n\n",curseurResultat,resultat);
  if(argc==3)
    {
      int dFichier = open(argv[2],O_CREAT|O_WRONLY|O_TRUNC,0666);
      erreur_si(dFichier==-1,argv[2]);
      dup2(dFichier,1);
      printf("1");
      
    }
  free(resultat);
  return EXIT_SUCCESS;
  


  }*/
static void copierBufferDansResultat(char *resultat,char *buffer,int curseurResultat,int tailleBuffer)
{
  
  int i;
  for(i=0;i<tailleBuffer;i++){
 
      resultat[i+curseurResultat]=buffer[i];
      
  }
      
    
  
}
