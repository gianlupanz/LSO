#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#define N 9000


// STRUTTURA NOTIFICA

typedef struct el

{

    char hotel[100];

    char op[100];

    char utente[100];

    int inserito;

}notifica;



// PROTOTIPI

void login (int);

void menu (int,char[]);

/* funzioni handler */

void rottura (int);

void invia (int);




//FUNZIONE PRINCIPALE

int main(int argc, char *argv[]) 

{    

    int i,c_fd,portno,y;

    /* 
       segnali utilizzati.
       SIGPIPE, nel caso il server si disconnette
       SIGINT nel caso di chiusura forzata del client 
    */

    signal (SIGPIPE,rottura);

    signal (SIGINT,invia);

    // struttura indirizzo

    struct sockaddr_in sin;

    struct hostent *hp;

    char buffer[N], buf[N];

    /* Controllo sul numero di argomenti */

    if (argc != 3) 
    
    {

        printf("Illegal number of arguments");

        exit(1);
    
    }

    /* Ottengo l'indirizzo del server tramite nome */

    if ((hp = gethostbyname(argv[1])) == 0) 

    {

        perror("tom: gethostbyname");

        exit(1); 

    }

    /* Creo il socket TCP */

    c_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (c_fd < 0)

    {

        perror("socket");

        return 1;

    }

    portno = atoi(argv[2]); // numero di porta

    sin.sin_family = AF_INET; // assegno la famiglia di protocolli

    memcpy((char *)&sin.sin_addr,(char *)hp->h_addr,hp->h_length); 

    sin.sin_port = htons(portno);

    // Mi connetto al server

    if(connect(c_fd, (struct sockaddr *) &sin, sizeof(sin)) < 0)

    {

        printf("connect() failed\n");

        return 1; 

    }

    // se la connessione è andata a buon fine, allora vado al menù principale

    login (c_fd);

}





// MENU INIZIALE DEL CLIENT

void login (int c_fd)

{

    int val = 1,scelta,err,n=0,y, eta,k;

    char c, stringa[1000],utente[N],nome [20], cognome [20],indirizzo [100],nickname [100],pwd [100], et[3];

    do

    {

        for (k = 0; k < 8; k++)

            printf ("\n\n\n\n\n\n\n\n\n\n");

        printf ("\n\nLOGIN\n\n'0' = Registrazione Utente\n\n'1' = Login Utente\n\n'2' = Uscita dal programma\n\nScelta: ");

        err = scanf ("%d",&scelta);
    
        fflush(stdin);

        if (err == 0)

            printf("valore errato");

        else if (scelta == 0)

        {    

            strcpy(stringa,"");

            strcpy(stringa,"registrazione;");

            printf ("Inserire nome: ");

            scanf ("%s",nome);

            strcat(stringa,nome);

            strcat(stringa,";");

            printf ("Inserire cognome: ");

            scanf ("%s",cognome);

            strcat(stringa,cognome);

            strcat(stringa,";");

            printf ("Inserire eta': ");

            scanf ("%d",&eta);

            sprintf(et,"%d",eta);

            strcat(stringa,et);

            strcat(stringa,";");

            printf ("Inserire indirizzo: ");

            scanf ("%s",indirizzo);

            strcat(stringa,indirizzo);

            strcat(stringa,";");

            printf ("Inserire nickname: ");

            scanf ("%s",nickname);

            strcat(stringa,nickname);

            strcat(stringa,";");

            printf ("Inserire password: ");

            scanf ("%s",pwd);

            strcat(stringa,pwd);

            strcat(stringa,";\0");

            write(c_fd,stringa,strlen(stringa));

            strcpy(stringa,"");

            y = read (c_fd,stringa,N);

            stringa[y] = '\0';

            printf ("*** %s ***\n",stringa);

            sleep(3);

        }

        else if (scelta == 1)

        {

            strcpy(stringa,"");

            strcpy(stringa,"login;");

            printf ("Nome Utente: ");

            scanf ("%s",utente);

            printf ("Password: ");

            scanf ("%s",pwd);

            strcat(stringa,utente);

            strcat(stringa,";");

            strcat(stringa,pwd);

            strcat(stringa,";");

            write (c_fd,stringa,strlen(stringa));

            strcpy(stringa,"");

            y = read (c_fd,stringa,N);

            stringa[y] = '\0';

            y = 0;

            while (stringa[y] != ';')

            {

                nome[y] = stringa[y];

                ++y;

            }

            nome[y] = '\0';

            if (strcmp (nome,"login_corretto") == 0)
    
            {

                printf ("Login corretto\n");

                ++y;

                k = 0;
    
                while (stringa[y] != ';' && stringa[y] != '\0')

                {

                    et[k] = stringa[y];

                    ++y;++k;

                }

                et[k] = '\0';

                menu(c_fd,et);

            }     
    
            else

                printf ("login fallito\n");

        }

        else if (scelta == 2)

        {

            printf ("Arrivederci\n\n");

            strcpy(stringa,"");

            strcpy(stringa,"uscita;");

            write(c_fd,stringa,strlen(stringa));

        }

        else

        {

            printf ("Errore!!!\nPremi INVIO per continuare...");

            scanf ("%c",&c);

        }

    }while (scelta != 2);

}    




//INTERFACCIA MENU' DOPO IL LOGIN

void menu (int c_fd,char utente[])

{

    signal (SIGPIPE,rottura);

    char hotel[5][100]={"Holiday_Inn","Excelsior","Star_Hotel","Royal_Palace","Miramare"};

    int choice,error,scelta,voto,scelta1,i,j,k,nutenti,y,x;

    char stringa[N],commento[120],valore[2],hot[100],ut[200],c;

    notifica not [100];

    for (i = 0; i < 100; i++)

        not[i].inserito = -1;

    do

	{

        for (i = 0; i < 8; i++)

            printf ("\n\n\n\n\n\n\n\n\n\n");

		printf ("  _____________________________________________________________  \n");

		printf (" |                                                             | \n");

		printf (" |               ***   VALUTAZIONE HOTEL  ***                  | \n");

		printf (" |                        BENVENUTO                            | \n");

		printf ("  _____________________________________________________________  \n");

        printf ("\nUtente -----> %s ",utente);

		printf ("\n\n'0' = LOGOUT\n\n'1' = EFFETTUA UNA VALUTAZIONE DELL'HOTEL\n\n");

		printf ("'2' = VISUALIZZA VALUTAZIONE HOTEL\n\n'3' = VISUALIZZA COMMENTI\n\n'4' = VISUALIZZA NOTIFICHE\n\nscelta: ");

		error = scanf ("%d", &choice);

	    if (error == 0)

			printf ("Errore input\n");

        else if (choice < 0 || choice > 4)

        {

            printf ("\n **** SCELTA NON VALIDA **** \n\nPremi INVIO per continuare...");

            scanf ("%c",&c);

        }

        else

        {

            switch (choice)

            {

                // SCELTA '0': LOGOUT UTENTE

                case 0:

                    strcpy(stringa,"");

                    strcpy(stringa,"logout;");

                    strcat(stringa,utente);
    
                    strcat(stringa,";");

                    write(c_fd,stringa,strlen(stringa));

                    strcpy (stringa,"");

                    y = read (c_fd,stringa,N);

                    stringa[y] = '\0';

                    break;

                // SCELTA '1': VALUTAZIONE RISTORANTE CON VOTO O COMMENTO

                case 1:

                    printf ("\n\n*** AVETE SCELTO DI EFFETTUARE UNA VALUTAZIONE DI UN HOTEL ***\n\n");

                    printf("\n\n'0' = Holiday_Inn\n'1' = Excelsior\n'2' = Star_Hotel\n'3' = Royal_Palace\n'4' = Miramare\n\nscelta: ");

                    scanf("%d",&scelta1);

                    while (scelta1 < 0 || scelta1 > 4)

                    {

                        printf ("Errore!!!\nInserire un valore tra 0 e 4\n\nscelta: ");

                        scanf("%d",&scelta1);
    
                    }

                    printf ("Scegliere il tipo di valutazione:\n'1' = voto numerico\n'2' = commento\n\nscelta: ");

                    scanf("%d",&scelta);

                    // INSERIMENTO NUOVO VOTO

                    if (scelta == 1)

                    {

                        printf ("Inserire voto: ");

                        scanf ("%d",&voto);

                        while (voto < -5 || voto > 5)

                        {

                            printf ("Attenzione: voto non valido.\nBisogna rispettare il seguente intervallo: (-5) - (+5)\n\nInserire voto: ");

                            scanf("%d",&voto);

                        }

                        sprintf(valore,"%d",voto);
    
                        strcpy(stringa,"");

                        strcpy(stringa,"voto;");

                        strcat(stringa,utente);

                        strcat(stringa,";");

                        strcat(stringa,hotel[scelta1]);

                        strcat(stringa,";");

                        strcat(stringa,valore);

                        strcat(stringa,";");

                        write (c_fd,stringa,strlen(stringa));

                        strcpy(stringa,"");

                        y = read (c_fd,stringa,N);

                        stringa[y] = '\0';  

                        printf ("%s\n",stringa);      

                    }

                    // INSERIMENTO NUOVO COMMENTO

                    else if (scelta == 2)

                    {

                        printf ("Inserire commento: ");

                        scanf ("%s",commento);

                        strcpy(stringa,"");

                        strcpy(stringa,"commento;");

                        strcat(stringa,utente);

                        strcat(stringa,";");

                        strcat(stringa,hotel[scelta1]);

                        strcat(stringa,";");

                        strcat(stringa,commento);

                        strcat(stringa,";");

                        write (c_fd,stringa,strlen(stringa));

                        y = read (c_fd,stringa,N);

                        stringa[y] = '\0';

                        printf ("%s\n",stringa);

                    }

                    else

                        printf ("\nERRORE SCELTA\n\n");

                    break;

                // SCELTA '2': VISUALIZZAZIONE VOTI HOTEL

                case 2:

                    printf ("\n\n*** AVETE SCELTO DI VISUALIZZARE LE VALUTAZIONI DEI VARI HOTEL ***\n\n");

                    strcpy(stringa,"");

                    strcpy(stringa,"visualizza_voti;");

                    write (c_fd,stringa,strlen(stringa));

                    y = read (c_fd,stringa,N);

                    stringa[y] = '\0';

                    j = 0;
            
                    for (i=0; i < 5; i++)

                    {

                        printf ("%s : ",hotel[i]);

                        k = 0;

                        while (stringa[j] != ';')

                        {

                            valore[k] = stringa[j];

                            ++j; ++k;

                        }

                        valore[k] = '\0';

                        ++j;

                        if (strcmp(valore,"-999") == 0)

                            printf ("nessun voto\n\n");

                        else

                            printf ("%s\n\n",valore);

                    }                    

                    break;

                // SCELTA '3': VISUALIZZAZIONE COMMENTI RISTORANTI

                case 3:

                    printf ("\n\n*** AVETE SCELTO DI VISUALIZZARE DEI COMMENTI ***\n\n");

                    strcpy(stringa,"");

                    strcpy(stringa,"visualizza_commenti;");

                    write (c_fd,stringa,strlen(stringa));

                    strcpy(stringa,"");

                    y = read (c_fd,stringa,N);

                    stringa[y] = '\0';

                    if (strcmp(stringa,"null") == 0)

                        printf ("\nNon sono stati effettuati commenti\n");

                    else

                    {

                        y = 0;

                        k = 0;

                        while (stringa[y] != '\0')

                        {

                            k = 0;

                            while (stringa[y] != ';')
    
                            {

                                ut[k] = stringa[y];

                                ++k;++y;

                            }
    
                            ut[k] = '\0';

                            ++y;

                            k = 0;

                            while (stringa[y] != ';' && stringa[y] != '\0')

                            {

                                hot[k] = stringa[y];

                                ++k;++y;

                            }

                            hot[k] = '\0';

                            ++y;

                            k = 0;

                            while (stringa[y] != ';' && stringa[y] != '\0')

                            {

                                commento[k] = stringa[y];

                                ++k;++y;

                            }

                            commento[k] = '\0';

                            ++y;

                            printf ("\nL'utente %s ha commentato il seguente hotel : %s --> %s\n",ut,hot,commento); 

                       } 

                    }

                    break;

                // CENTRO NOTIFICHE

                case 4:

                    strcpy(stringa,"");

                    strcpy(stringa,"notifica;");

                    write (c_fd,stringa,strlen(stringa));

                    y = read (c_fd,stringa,N);

                    stringa[y] = '\0';

                    if (strcmp(stringa,"null") != 0)

                        printf (" -------> ATTENZIONE: %s <-------\n",stringa);

                    else

                        printf ("NESSUNA NOTIFICA\n\n");

                    break; 

            }                        

            if (choice != 0)

            {

                do

                {

                    printf ("\n\n\nLOGOUT? [0/*]: ");

                    error = scanf ("%d",&choice);

                    if (error == 0)

    			        printf ("errore input\n");

                    else if (choice == 0)

                        printf ("\n\nLOGOUT\n");

                } while (error == 0);

            }  

        }         

	} while (error == 0 || choice != 0);

}


// HANDLER SIGPIPE

void rottura (int segnale)

{

    signal (segnale,SIG_IGN);

    perror ("\nIL SERVER E' STATO CHIUSO INASPETTATAMENTE\n");

    signal (segnale,rottura);

}

// HANDLER SIGINT

void invia (int segnale)

{

    printf ("\nCHIUSURA INASPETTATA DEL CLIENT\n");

    exit(1);

}
    