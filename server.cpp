/* servTCPConcTh2.c - Exemplu de server TCP concurent care deserveste clientii
   prin crearea unui thread pentru fiecare client.
   Asteapta un numar de la clienti si intoarce clientilor numarul incrementat.
	Intoarce corect identificatorul din program al thread-ului.
  
   
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <iostream>
#include "pugixml.hpp"
#include <time.h>
#define PORT 2908

using namespace std;

typedef struct thData{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl; //descriptorul intors de accept
}thData;

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void sendDelay(void *);
void getTodayRoutes(void *);
void getHourDeparts(void *);
void getHourArrivals(void *);

int main ()
{
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file("TrenuriDefault.xml");
  doc.save_file("Trenuri.xml");
  
  if (!result) {
        std::cout << "Failed to parse XML: " << result.description() << std::endl;
        return 1;
    }

  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in from;	
  int sd;		//descriptorul de socket 
  pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
	int i=0;
  char msj[1024];

  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }
  /* utilizarea optiunii SO_REUSEADDR */
  int on=1;
  setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
  
  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));
  
  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;	
  /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);
  
  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (server)) < 0)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 5) < 0)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }

  /* servim in mod concurent clientii...folosind thread-uri */
  while (1)
    {
      int client;
      thData * td; //parametru functia executata de thread     
      socklen_t length = sizeof (from);

      printf ("[server]Asteptam la portul %d...\n",PORT);
      fflush (stdout);

      // client= malloc(sizeof(int));
      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
      if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0)
      {
        perror ("[server]Eroare la accept().\n");
        continue;
      }
      bzero(msj, sizeof(msj));
	
        /* s-a realizat conexiunea, se astepta mesajul */
    
	// int idThread; //id-ul threadului
	// int cl; //descriptorul intors de accept

	td=(struct thData*)malloc(sizeof(struct thData));	
	td->idThread=i++;
	td->cl=client;

  printf("\nClient id = %d\n", td->cl);
	pthread_create(&th[i], NULL, &treat, td);	      
	
	}//while    
};				
static void *treat(void * arg)
{		
    int getout=1;
		struct thData tdL; 
		tdL= *((struct thData*)arg);	
		printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);		 
    char msj[40];
    fflush (stdout);
    while(getout)
  {
    tdL= *((struct thData*)arg);	
    if (read (tdL.cl, &msj, sizeof(msj)) <= 0)
        {
          printf("[Thread %d]\n",tdL.idThread);
          perror ("Eroare la read() de la client.\n");
        }
    // Am citit comanda de la client.
    // *********************************
    // Tratam cazurile in functie de comanda primita.

    printf ("[Thread %d]Mesajul a fost receptionat...%s\n", tdL.idThread, msj);

      if(strcasecmp(msj, "sendDelay")==0)
      {
        sendDelay((struct thData *)arg);
        printf("[Thread %d]Ati trimis informatii despre intarzierea trenului.\n", tdL.idThread);
      }
      
      else if(strcasecmp(msj, "sendArriveEarly")==0)
      {
        //sArrEarlyFunc((struct thData *)arg);
        printf("[Thread %d]Ati trimis informatii despre sosirea mai rapida.\n", tdL.idThread);
      }
      
      else if(strcasecmp(msj, "sendDepartLate")==0)
      { 
        //sDepLateFunc((struct thData *)arg);
        printf("[Thread %d]Ati trimis informatii despre plecarea cu intarziere.\n", tdL.idThread);
      }
      
      else if(strcasecmp(msj, "getTodayRoutes")==0)
      {
        printf("[Thread %d]Acestea sunt rutele trenurilor de astazi:\n", tdL.idThread);
        getTodayRoutes((struct thData *)arg);
      }
      
      else if(strcasecmp(msj, "getHourDeparts")==0)
      {
        getHourDeparts((struct thData *)arg);
        printf("[Thread %d]Acestea sunt plecarile din urmatoarea ora:\n", tdL.idThread);
      }
      
      else if(strcasecmp(msj, "getHourArrivals")==0)
      {
        getHourArrivals((struct thData *)arg);
        printf("[Thread %d]Acestea sunt sosirile din urmatoarea ora:\n", tdL.idThread);
      }
      
      else if(strcasecmp(msj, "quit")==0)
      {
        printf("[Thread %d]La revedere!\n", tdL.idThread);
       // shutdown(tdL.cl, SHUT_RDWR);
        //close((intptr_t)arg);
        close((intptr_t)arg);
        return (NULL);
      }
      else 
        printf("[Thread %d]Comanda nerecunoscuta.\n", tdL.idThread);        
        
        /* returnam mesajul clientului */
    /*if (write (tdL.cl, &msj, sizeof(msj)) <= 0)
      {
      printf("[Thread %d] ",tdL.idThread);
      perror ("[Thread]Eroare la write() catre client.\n");
      }

    else
    {
      printf ("[Thread %d]Mesajul a fost transmis cu succes.\n",tdL.idThread);
    } 
    */
    bzero(msj, sizeof(msj));
  }
      /* am terminat cu acest client, inchidem conexiunea */
      close ((intptr_t)arg);
      return (NULL);
  
}

void getTodayRoutes(void *arg) //functia care ne arata ce trenuri vor trece prin 
                               // statie in ziua respectiva
{
  struct thData tdL;
  tdL= *((struct thData*)arg);
  
  char delay[200];
  bool isDelay = false;
  char station[100];
  int count=0;
  time_t raw;
  time(&raw);

  struct tm *time_ptr;
  time_ptr = localtime(&raw);

  char current_time [20];
  strftime(current_time, sizeof(current_time), "%H:%M", time_ptr);

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file("Trenuri.xml");

  pugi::xml_node trenuri = doc.child("Trenuri");
  pugi::xml_attribute attr = trenuri.attribute("time");
  
  attr.set_value(current_time);
  doc.save_file("Trenuri.xml");

  if (!result) 
  {
    std::cout << "Failed to parse XML: " << result.description() << std::endl;
    perror ("Eroare la deschidere XML\n");
  }

  if (read (tdL.cl, &station, sizeof(station)) <= 0)
  {
    printf("[Thread %d]\n",tdL.idThread);
    perror ("Eroare la read() de la client.\n");
  }

  char msj[1024];
  strcat(msj, "\0");
  bool gasit = false;
  char trimit[1024];
  trimit[0] = '\0';

  if(result)
  {
    for(pugi::xml_node tren = doc.child("Trenuri").child("Tren"); tren; tren = tren.next_sibling("Tren")) 
    {
      gasit = false;
      isDelay = false;
      //printf("%s hopa\n", msj);

      for(pugi::xml_node statie = tren.child("Statii").child("Statie"); statie; statie = statie.next_sibling("Statie"))
      {
        //printf("%s\n", statie.attribute("loc").value());
        if (strcasecmp(statie.attribute("loc").value(), station) == 0) 
          {
            gasit = true;
            strcat(msj, "*Prin statia ");
            strcat(msj, station);
            strcat(msj, " va trece trenul ");
            strcat(msj, tren.attribute("nume").value());
            strcat(msj, " cu traseul:\n");
          }
      }

      if(gasit==true)
      {
          for(pugi::xml_node statie = tren.child("Statii").child("Statie"); statie; statie = statie.next_sibling("Statie"))
          {
            strcat(msj, statie.attribute("loc").value());
            strcat(msj, "\n");
            strcpy(delay, statie.attribute("tIntarziere").value());
            if (strcmp(statie.attribute("ifIntarziere").value(), "true")==0)
            {
                isDelay=true;
            }
            //strcat(msj, "\n);
          }
      }
      count++;
      strcat(trimit, msj);
      strcat(trimit, "\n");

      if(gasit == true)
      { 
        //printf("%s\n", msj);
        msj[0]='\0';
      }
      if(isDelay)
      {
        strcat(trimit, "Trenul are intarziere de: ");
        strcat(trimit, delay);
        strcat(trimit, "min\n");
      }
    }
  }
  else
  {
    printf("XML parsed with errors, attr value: [%s]\n", doc.child("node").attribute("attr").value());
    printf("Error description: %s\n", result.description());
  }

  if(count==0)
    strcpy(trimit, "\n No match\n");

  /*int n = strlen(trimit);

  if(write(tdL.cl, &n, sizeof(int)) <= 0)
  {
    printf("[Thread %d] ", tdL.idThread);
    perror ("[Thread]Eroare la write() catre client.\n");
  }
  */
  if(write(tdL.cl, &trimit, sizeof(trimit)) <= 0)
  {
    printf("[Thread %d] ", tdL.idThread);
    perror ("[Thread]Eroare la write() catre client.\n");
  } 

}

void getHourDeparts(void *arg)
{
  struct thData tdL;
  tdL= *((struct thData*)arg);
  
  char delay[200];
  bool isDelay = false;
  char station[100];

  time_t raw;
  time(&raw);

  struct tm *time_ptr;
  time_ptr = localtime(&raw);

  char current_time [20];
  strftime(current_time, sizeof(current_time), "%H:%M", time_ptr);

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file("Trenuri.xml");

  pugi::xml_node trenuri = doc.child("Trenuri");
  pugi::xml_attribute attr = trenuri.attribute("time");
  
  attr.set_value(current_time);
  doc.save_file("Trenuri.xml");

  int ore = atoi(current_time);
  char minuteString[5];
  strcpy(minuteString, current_time+3);
  //int minute = atoi(minuteString);

  int oreFunc;
  //int minuteFunc;

  if (!result) 
  {
    std::cout << "Failed to parse XML: " << result.description() << std::endl;
    perror ("Eroare la deschidere XML\n");
  }

  if (read (tdL.cl, &station, sizeof(station)) <= 0)
  {
    printf("[Thread %d]\n",tdL.idThread);
    perror ("Eroare la read() de la client.\n");
  }

  char msj[1024];
  strcat(msj, "\0");
  bool gasit = false;
  char trimit[1024];
  trimit[0] = '\0';
  bool plec = false;
  int count = 0;

  if(result)
  {
    for(pugi::xml_node tren = doc.child("Trenuri").child("Tren"); tren; tren = tren.next_sibling("Tren")) 
    {
      gasit = false;
      isDelay = false;
      //printf("%s hopa\n", msj);


      for(pugi::xml_node statie = tren.child("Statii").child("Statie"); statie; statie = statie.next_sibling("Statie"))
      {
        //printf("%s\n", statie.attribute("loc").value());
        if (strcasecmp(statie.attribute("loc").value(), station) == 0) 
          {
            oreFunc = atoi(statie.attribute("pOra").value());
            char minuteStringFunc[5];
            strcpy(minuteStringFunc, current_time+3);
            //minuteFunc = atoi(minuteString);

            if(oreFunc == ore+1 || oreFunc == ore || (ore==23 && oreFunc == 0))
            {
              gasit = true;
              if(statie.attribute("pOra"))
              {
                strcat(msj, "*Din statia ");
                strcat(msj, station);
                strcat(msj, " la ora ");
                strcat(msj, statie.attribute("pOra").value());
                strcat(msj, ", va pleca trenul ");
                strcat(msj, tren.attribute("nume").value());
                strcat(msj, " cu traseul:\n");
                plec=true;
              }
              /*strcat(msj, "Din statia ");
              strcat(msj, station);
              strcat(msj, ", va pleca trenul ");
              strcat(msj, tren.attribute("nume").value());
              strcat(msj, " cu traseul:\n");*/
              //plec=true;
            }
          }
      }

      if(gasit==true)
      {
          for(pugi::xml_node statie = tren.child("Statii").child("Statie"); statie; statie = statie.next_sibling("Statie"))
          {
            if(plec==true)
            {   
              strcat(msj, statie.attribute("loc").value());
              strcat(msj, "\n");
              strcpy(delay, statie.attribute("tIntarziere").value());
              if (strcmp(statie.attribute("ifIntarziere").value(), "true")==0)
              {
                isDelay = true;
              }
            }  
          }
        }
      plec=false;
      count++;
      strcat(trimit, msj);

      if(gasit == true)
      { 
        //printf("%s\n", msj);
        msj[0]='\0';
      }

      if(isDelay)
      {
        strcat(trimit, "Trenul are intarziere de: ");
        strcat(trimit, delay);
        strcat(trimit, "min\n");
      }
    }
  }
  else
  {
    printf("XML parsed with errors, attr value: [%s]\n", doc.child("node").attribute("attr").value());
    printf("Error description: %s\n", result.description());
  }

  if(count==0)
  {
    strcat(trimit, "Nu exista plecari in urmatoarea ora.\n");
  }

  if(write(tdL.cl, &trimit, sizeof(trimit)) <= 0)
  {
    printf("[Thread %d] ", tdL.idThread);
    perror ("[Thread]Eroare la write() catre client.\n");
  } 

}

void getHourArrivals(void *arg)
{
  struct thData tdL;
  tdL= *((struct thData*)arg);
  
  char delay[200];
  char station[100];

  time_t raw;
  time(&raw);

  struct tm *time_ptr;
  time_ptr = localtime(&raw);

  char current_time [20];
  strftime(current_time, sizeof(current_time), "%H:%M", time_ptr);

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file("Trenuri.xml");

  pugi::xml_node trenuri = doc.child("Trenuri");
  pugi::xml_attribute attr = trenuri.attribute("time");
  
  attr.set_value(current_time);
  doc.save_file("Trenuri.xml");

  int ore = atoi(current_time);
  char minuteString[5];
  strcpy(minuteString, current_time+3);
  //int minute = atoi(minuteString);

  int oreFunc;
  //int minuteFunc;

  if (!result) 
  {
    std::cout << "Failed to parse XML: " << result.description() << std::endl;
    perror ("Eroare la deschidere XML\n");
  }

  if (read (tdL.cl, &station, sizeof(station)) <= 0)
  {
    printf("[Thread %d]\n",tdL.idThread);
    perror ("Eroare la read() de la client.\n");
  }

  char msj[1024];
  strcat(msj, "\0");
  bool gasit = false;
  char trimit[1024];
  trimit[0] = '\0';
  bool sosit = false;
  int count=0;
  bool isDelay = false;

  if(result)
  {
    for(pugi::xml_node tren = doc.child("Trenuri").child("Tren"); tren; tren = tren.next_sibling("Tren")) 
    {
      gasit = false;
      isDelay = false;
      //printf("%s hopa\n", msj);


      for(pugi::xml_node statie = tren.child("Statii").child("Statie"); statie; statie = statie.next_sibling("Statie"))
      {
        //printf("%s\n", statie.attribute("loc").value());
        if (strcasecmp(statie.attribute("loc").value(), station) == 0) 
          {
            oreFunc = atoi(statie.attribute("sOra").value());
            char minuteStringFunc[5];
            strcpy(minuteStringFunc, current_time+3);
            //minuteFunc = atoi(minuteString);

            if(oreFunc == ore+1 || oreFunc == ore || (ore==23 && oreFunc == 0))
            {
              gasit = true;
              if(statie.attribute("sOra"))
              {
                strcat(msj, "In statia ");
                strcat(msj, station);
                strcat(msj, ", la ora ");
                strcat(msj, statie.attribute("sOra").value());
                strcat(msj, ", va ajunge trenul ");
                strcat(msj, tren.attribute("nume").value());
                strcat(msj, " cu traseul:\n");
                sosit=true;
              }
              /*strcat(msj, "Din statia ");
              strcat(msj, station);
              strcat(msj, ", va pleca trenul ");
              strcat(msj, tren.attribute("nume").value());
              strcat(msj, " cu traseul:\n");*/
              //plec=true;
            }
          }
      }

      if(gasit==true)
      {
          for(pugi::xml_node statie = tren.child("Statii").child("Statie"); statie; statie = statie.next_sibling("Statie"))
          {
            if(sosit==true)
            {   
              strcat(msj, statie.attribute("loc").value());
              strcat(msj, "\n");
              strcpy(delay, statie.attribute("tIntarziere").value());
              if (strcmp(statie.attribute("ifIntarziere").value(), "true")==0)
              {
                  isDelay = true;
              }
            }  
          }
        }
      sosit=false;
      count++;
      strcat(trimit, msj);

      if(gasit == true)
      { 
        //printf("%s\n", msj);
        msj[0]='\0';
      }
      if(isDelay)
      {
        strcat(trimit, " Trenul are intarziere de: ");
        strcat(trimit, delay);
        strcat(trimit, "min\n");
      }
    }
  }
  else
  {
    printf("XML parsed with errors, attr value: [%s]\n", doc.child("node").attribute("attr").value());
    printf("Error description: %s\n", result.description());
  }

  if(count==0)
  {
    strcat(trimit, "Nu exista sosiri in urmatoarea ora.\n");
  }

  if(write(tdL.cl, &trimit, sizeof(trimit)) <= 0)
  {
    printf("[Thread %d] ", tdL.idThread);
    perror ("[Thread]Eroare la write() catre client.\n");
  } 

}

void sendDelay(void *arg)
{
  struct thData tdL;
  tdL= *((struct thData*)arg);
  
  char delay[200];
  //bool isDelay = false;
  char trainRec[100];

  time_t raw;
  time(&raw);

  struct tm *time_ptr;
  time_ptr = localtime(&raw);

  char current_time [20];
  strftime(current_time, sizeof(current_time), "%H:%M", time_ptr);

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file("Trenuri.xml");

  pugi::xml_node trenuri = doc.child("Trenuri");
  pugi::xml_attribute attr = trenuri.attribute("time");
  
  attr.set_value(current_time);
  doc.save_file("Trenuri.xml");

  if (!result) 
  {
    std::cout << "Failed to parse XML: " << result.description() << std::endl;
    perror ("Eroare la deschidere XML\n");
  }

  if (read (tdL.cl, &trainRec, sizeof(trainRec)) <= 0)
  {
    printf("[Thread %d]\n",tdL.idThread);
    perror ("Eroare la read() de la client.\n");
  }

  if (read (tdL.cl, &delay, sizeof(delay)) <= 0)
  {
    printf("[Thread %d]\n",tdL.idThread);
    perror ("Eroare la read() de la client.\n");
  }

  if(result)
  {
    for(pugi::xml_node tren = doc.child("Trenuri").child("Tren"); tren; tren = tren.next_sibling("Tren")) 
    {
      if(strcasecmp(tren.attribute("nume").value(), trainRec)==0)
      {
        for(pugi::xml_node statie = tren.child("Statii").child("Statie"); statie; statie = statie.next_sibling("Statie"))
        {
          pugi::xml_attribute attr = statie.attribute("tIntarziere");
          attr.set_value(delay);
          pugi::xml_attribute attr1 = statie.attribute("ifIntarziere");
          attr1.set_value("true");
          doc.save_file("Trenuri.xml");
        }
      }
    }
  }
  else
  {
    printf("XML parsed with errors, attr value: [%s]\n", doc.child("node").attribute("attr").value());
    printf("Error description: %s\n", result.description());
  }

}



