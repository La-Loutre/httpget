#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <wait.h>
#include <arpa/inet.h>
#include <signal.h>
#define TAILLE_BUFFER 100
static void erreur_si(int test,char *chaineErreur)
{
  if(test)
    {
      perror(chaineErreur);
      exit(1);
    }
  
}
static void lecture(int socket )
{
  char *buffer=malloc(sizeof(char)*TAILLE_BUFFER);
  int sizeRead;
  while((sizeRead=read(socket,buffer,sizeof(char)*TAILLE_BUFFER))>0){
      write(1,buffer,sizeRead*sizeof(char));
      if(buffer[sizeRead-2]=='\n'||buffer[sizeRead-1]=='\n')
	break;
  }
  close(socket);
  free(buffer);



}
static int attend(int fd1)
{
  fd_set set;
  FD_ZERO(&set);
  FD_SET(fd1,&set);
  int max=fd1;
  int selectR=select(max+1,&set,NULL,NULL,NULL);
  if(selectR==-1)
    return -1;
  else if(FD_ISSET(fd1,&set))
    return 1;
  
  return 0;

}
int main(int argc,char **argv)
{
  int socket1=socket(AF_INET,SOCK_STREAM,0);
  int socketfd;
  erreur_si(socket1<0,"Descripteur socket KO");
  struct in_addr in;
  erreur_si(inet_aton("0.0.0.0",&in)==0,"Inet_aton");
  fprintf(stderr,"\n\nAdresse : %x\n",in.s_addr);
  struct sockaddr_in sockin={AF_INET,htons(80),in};
  int err=bind(socket1,(struct sockaddr*)&sockin,sizeof(sockin));
  erreur_si(err==-1,"bind");

  listen(socket1,1);
  struct sockaddr_in sockin2;
  socklen_t taillesockin2;
  int attendResult;
  char *message="<HTML><head>\n<title>Test</title>\n</head><body>\n<h1><A HREF=\"intro.txt\">TEST CLIQUE</A>Hello World !</h1></body></html>";
  //signal(SIGPIPE, SIG_IGN);
  while(true){
    attendResult=attend(socket1);
    if(attendResult==1){
      socketfd=accept(socket1,(struct sockaddr*)&sockin2,&taillesockin2);
      erreur_si(socketfd<0,"Socket2");
      fprintf(stderr,"\nConnection établie avec %x\n",sockin2.sin_addr.s_addr);
      pid_t p=fork();
      if(p==0){
      lecture(socketfd);
      return 0;
      }
      
    
      if(sendto(socketfd,message,sizeof(char)*strlen(message),0,(struct sockaddr*)&sockin2,taillesockin2)==-1)
	fprintf(stderr,"Erreur lors de l'envoie\n");
     
      close(socketfd);
      wait(NULL);
      fprintf(stderr,"Connection terminé avec %x\n",sockin2.sin_addr.s_addr);
     
    }
    else if(attendResult==-1)
      {
	break;
      }
      
  }

    
  
  return EXIT_SUCCESS;
}
