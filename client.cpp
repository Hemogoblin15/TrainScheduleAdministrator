/* cliTCPIt.c - Exemplu de client TCP
   Trimite un numar la server; primeste de la server numarul incrementat.
         
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
/* codul de eroare returnat de anumite apeluri */

/* portul de conectare la server*/
int port;

int main (int argc, char *argv[])
{
  int sd;			// descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare 
  		// mesajul trimis
  char buf[40];
  char msj[40];
  bool pornit = true;

  /* exista toate argumentele in linia de comanda? */
  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  /* stabilim portul */
  port = atoi (argv[2]);

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    } 

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server.sin_port = htons (port);

  printf ("\n[client] *** MENIU DE COMENZI ***\n");
  printf ("[client] 1. SendDelay\n");
  printf ("[client] 2. GetTodayRoutes\n");
  printf ("[client] 3. GetHourDeparts\n");
  printf ("[client] 4. GetHourArrivals\n");
  printf ("[client] 5. Quit\n");
 // printf ("[client]Introduceti o comanda din meniu: \n");

  while(pornit)
  {  /* citirea mesajului */

    if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
        perror ("[client]Eroare la connect().\n");
        return errno;
    }
    int n;

    while(pornit)
    { 
      printf ("\n[client]Introduceti o comanda din meniu: ");
      fflush (stdout);

      scanf("%s", msj);

      printf("\n");
      if (write (sd,&msj,sizeof(msj)) <= 0)
      {
        perror ("[client]Eroare la write() spre server.\n");
        return errno;
      }

      if(strcasecmp(msj, "gettodayroutes")==0)
      {
        char station[100];
        bzero(station, sizeof(station));

        printf ("Alegeti statia pentru care doriti sa vedeti rutele de astazi: ");
        scanf("%s", station);
        fflush(stdout);
        printf("\n");

        if (write (sd,&station,sizeof(station)) <= 0)
        {
          perror ("[client]Eroare la write() spre server.\n");
          return errno;
        }

        char response[1024];
        bzero(response, sizeof(response));

        /*if (read (sd, &n, sizeof(int)) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }
      */
        if (read (sd, &response, sizeof(response)) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }
      
        printf("%s", response);
      }

      else if(strcasecmp(msj, "gethourdeparts")==0)
      {
        char station[100];
        bzero(station, sizeof(station));

        printf ("Alegeti statia pentru care vreti sa vedeti plecarile din urmatoarea ora: ");
        scanf("%s", station);
        fflush(stdout);
        printf("\n");

        if (write (sd,&station,sizeof(station)) <= 0)
        {
          perror ("[client]Eroare la write() spre server.\n");
          return errno;
        }

        char response[1024];
        bzero(response, sizeof(response));

        if (read (sd, &response, 1024) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }
      
        printf("%s", response);
      }

      else if(strcasecmp(msj, "gethourarrivals")==0)
      {
        char station[100];
        bzero(station, sizeof(station));

        printf ("Alegeti statia pentru care vreti sa vedeti sosirile din urmatoarea ora: ");
        scanf("%s", station);
        fflush(stdout);
        printf("\n");

        if (write (sd,&station,sizeof(station)) <= 0)
        {
          perror ("[client]Eroare la write() spre server.\n");
          return errno;
        }

        char response[1024];
        bzero(response, sizeof(response));

        if (read (sd, &response, 1024) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }
      
        printf("%s\n", response);
      }

      else if(strcasecmp(msj, "senddelay")==0)
      {
        char tren[100];
        char delay[100];
        bzero(tren, sizeof(tren));
        bzero(delay, sizeof(delay));

        printf("Alegeti trenul care va avea intarziere: ");
        scanf("%s", tren);

        printf("Specificati durata intarzierii: ");
        scanf("%s", delay);
        fflush(stdout);
        printf("\n");

        if (write (sd,&tren,sizeof(tren)) <= 0)
        {
          perror ("[client]Eroare la write() spre server.\n");
          return errno;
        }

        if (write (sd,&delay,sizeof(delay)) <= 0)
        {
          perror ("[client]Eroare la write() spre server.\n");
          return errno;
        }
        

        char response[1024];
        bzero(response, sizeof(response));

        /*if (read (sd, &response, 1024) < 0)
        {
          perror ("[client]Eroare la read() de la server.\n");
          return errno;
        }*/

        printf("Ati trimis intarzierea trenului %s\n", tren);
      }

      else if(strcasecmp(msj, "quit")==0)
      {
        pornit = false;
      }

      bzero(msj, sizeof(msj));

    }
    close(sd);
  }

  exit(0);

}
