/* 

                            ╔╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╗
                            ║                                            SERVER                                                   ║
                            ║   Autori: Simone Margio, Luca Ioffredo                                                              ║
                            ║   Data: 00/00/0000                                                                                  ║
                            ║   Descrizione: sistema client-server di gioco                                                       ║
                            ║   Ulteriori Info: https://github.com/ahw0                                                           ║
                            ║                                                                                                     ║
                            ╚╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╝
*/

/* LIBRERIE */
#include <sys/socket.h> /* gestione socket */
#include <netinet/in.h> /* conversione indirizzi (htonl/ntohs) */
#include <stdio.h> /* standard input-output header */
#include <stdlib.h> /* exit */
#include <errno.h> /* perror */
#include <unistd.h> /* write, read, close */
#include <sys/types.h> /* open */
#include <sys/stat.h> /* open */
#include <fcntl.h> /* open */
#include <string.h> /* gestione delle stringhe e delle aree di memoria */
#include <ctype.h> /* classificazione dei caratteri */   //NB https://it.wikipedia.org/wiki/Ctype.h  ( lo usiamo per verificare user e passw )
#include <sys/wait.h> /* wait(), waitpid() */
#include <arpa/inet.h> /* conversione indirizzi (inet)*/
#include <pthread.h>
#include <time.h> /* funzione per randomizzare */
#include <signal.h>
#include "errori.c" /* libreria per la gestione degli errori */

/* DEFINE */
#define MAXSIZE 1000

/* Chiavi condivise per i TSD */
static pthread_key_t chiave_globale_fd, username_globale_thread, chiave_globale_pos_riga, chiave_globale_pos_colonna;
static pthread_key_t chiave_globale_ID_Partita; /* CHIAVE GLOBALE PER OGNI THREAD */
/* Mutex per ogni variabile da bloccare */
pthread_mutex_t lock_ID_Partita = PTHREAD_MUTEX_INITIALIZER, sem = PTHREAD_MUTEX_INITIALIZER, lock_lista_pthread = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_lista_utenti = PTHREAD_MUTEX_INITIALIZER, lock_misura_matrice = PTHREAD_MUTEX_INITIALIZER, lock_tempo = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_flag_partita = PTHREAD_MUTEX_INITIALIZER, lock_oggetti_trovati = PTHREAD_MUTEX_INITIALIZER, lock_utenti_in_gioco = PTHREAD_MUTEX_INITIALIZER;
/* MUTEX PER ID_PARTITA, ListaUtenti, Lista_pthread, misura_matrice, tempo, flag_partita, numero_oggetti_trovati, utenti_in_gioco */
/*  */
/* Oggetti: se non ci sono sono < 0. Se è presente > 0. Se era presente, ma ora non più == 0 -- Flag muro è 1 se non è presente 0 se è presente -- pif settato >= 0 altrimenti -1 */
typedef struct info_casella /* Struttura di una casella della mappa */
{
    pthread_t numero_pid;
    int flag_muro; /* se 1 muro altrimenti casella vuota/ogg */
    int indice_ogg;
}casella;
/* STRUTTURA PER UNA LISTA DI OGGETTI */
typedef struct lista_oggetti
{
	char nome_oggetto[MAXSIZE];
	struct lista_oggetti *next;
}oggetti;
/* Struttura per un ABR di utenti -- E' ordinato in base al nickname */
typedef struct lista_utenti_in_gioco
{
	pthread_t numero_tid;
	char nickname[MAXSIZE];
	int in_gioco; /* verifica se l'utente è in gioco o nel menu */
	int numero_oggetti_trovati;
	oggetti *oggetti_trovati;
	struct lista_utenti_in_gioco* dx;
	struct lista_utenti_in_gioco* sx;
}utenti;
/* STRUTTURA PER ABR DI TID */
typedef struct lista_pthread
{
    pthread_t pid;
    struct lista_pthread *dx;
    struct lista_pthread *sx;
}lista_thread;


/* VAR GLOBALI */
int misura_matrice=0;                   //  Dimensione della mappa
int utenti_in_gioco=0;                  //  utenti in gioco nella mappa
int utenti_max=0;                       //  massimo numero di utenti che possono entrare nella mappa
int numero_oggetti_nella_mappa=0;       //  numero di oggetti presenti nella mappa
int numero_oggetti_trovati=0;           //  numero di oggetti trovati dai giocatori
lista_thread *Lista_pthread = NULL;     //  Lista di tid condivisa globalmente
casella** matrice = NULL;               //  Variabile globale della mappa
utenti* ListaUtenti = NULL;             //  Lista di utenti, ordinato in base al nickname
int flag_partita = 0;                   //  0 se partita in corso, 1 se terminata
int ID_PARTITA = 0;                     //  ID della partita, viene incrementato ad ogni mappa ricreata
time_t tempo;                           //  variabile di tempo condivisa globalmente, serve a segnalare il tempo trascorso
int flag_tempo = 0;                     //  0 non avviato, 1 avviato, 2 terminato

/* File descriptor per file log */
int fd_log = 0; char temp[MAXSIZE];

/* PROTOTIPI DI FUNZIONI */
int controllo_porta(int porta);                                             //Controlla la porta inserita
void *funzione_principale(void *arg);                                       //Funzione principale dei thread. Ogni thread parte da questa funzione

int controllo_utente(char user[], int flag, int file);                      //Funzione che controlla se l'utente è presente nel database.txt
int controllo_password(char password[],int file);                           //Controlla se la password ricevuta dal client è corretta
casella **crea_matrice();                                                   //Crea la matrice per la mappa ad ogni partita, setta la dimensione e diverse variabili globali
utenti *InserisciUtente(utenti* top, pthread_t tid, char nickname[], int flag);                                             //Inserisce l'utente con nickname e tid in un ABR ordinato in base al nickname
void StampaListaUtenti(utenti* top, int fd);                                                                                //Invia la lista di utenti al client
utenti *ScollegaUtente(utenti* top, int fd2, char nickname[MAXSIZE], int *pos_riga_giocatore, int *pos_colonna_giocatore); //L'utente chiude il client, viene scollegato.
utenti *RimuoviGiocatore_Dalla_Partita(utenti* top, int fd2, char nickname[MAXSIZE], int *pos_riga_giocatore, int *pos_colonna_giocatore); //L'utente esce dalla partita. Riporta la flag in gioco a 0
void NumeroOggTrovati(int fd);                                                                                                               //Manda il numero di oggetti trovati al client
void NumeroOggRimanenti(int fd);                                                                                                             //Invia il numero di oggetti rimanenti nella mappa al client
void Gioca(int fd2, char username[], int *pos_riga_giocatore, int *pos_colonna_giocatore);                                  //Funzione di gioco, viene richiamata solamente quando l'utente entra in gioco. Non gestisce i movimenti
void StampaMappa();                                                                                                         //Stampa lista utenti a video, non usata direttamente, ma solo per debug dato che stampa su STDOUT
void InvioMappaClient(int fd2, int *pos_riga_giocatore, int *pos_colonna_giocatore);                                        //Invia la mappa, con i relativi ostacoli, giocatori ed oggetti al client
void li_vuoi_quei_kiwi(int fd2, int *pos_riga_giocatore, int *pos_colonna_giocatore, char nickname[]);                      //Funzione adibita allo spostamento
utenti *StaccaMinna(utenti *T, utenti *P);                                                                                  //Stacca il minimo dell'ABR della ListaUtenti
void lista_oggetti(int indice, char Oggetto[]);                                                                             //Inserisce il nome dell'oggetto in Oggetto[]
oggetti *InserisciOgg(oggetti *top, char oggetto[]);                                                                        //Inserisce l'oggetto Oggetto[] in una lista top
void InserisciOggettoUtente(utenti* top, char nickname[], char oggetto[]);                                                  //Inserisce l'oggetto Oggetto[] nella lista di oggetti dell'utente nickname[]
void segnalami_tutta(int num_segnale);                                                                                      //Handler dei segnali
lista_thread *Inserisci_thread(lista_thread *top, pthread_t pid);                                                           //Inserisce in un ABR un Thread
lista_thread *StaccaCappello(lista_thread *T, lista_thread *P);                                                             //Stacca il minimo di un ABR di thread
lista_thread *Rimuovi_thread(lista_thread *top, pthread_t pid);                                                             //Rimuove pid dall'abr Top di thread
void StampaListaUtenti2(utenti* top,int fd);                                                                                //Invia lista utenti al client
int RicercaUtenteLoggato(utenti *top,char nickname[]);                                                                      //Controlla che l'utente non sia già loggato
utenti *ScollegaUtente_tid(utenti *top);                                                                                    //Scollega l'utente ricercandolo per TID
void RimuoviGiocatori_dalla_Partita(utenti* top);                                           //Rimuove i giocatori dalla mappa, ma non effettua il logut.
oggetti *RimuoviOggettiTrovati(oggetti *top);                                               //Rimuove gli oggetti trovati ad ogni creazione della mappa
void Invia_tempo_Client(int fd2);                                                           //Invia il tempo trascorso al client
void cancellaMappa(casella **Mappa);                                                        //Funzione per deallocare una mappa

/*PROGRAMMA PRINCIPALE */
int main(int argc, char **argv)
{
    /* DICHIARAZIONI VARIABILI */
    int fd, fd2,porta=atoi(argv[1]),errore_thread=0,controllo_log_remove;

    /* CONTROLLO NUMERO ARGOMENTI PASSATI */
    if(argc!=2) errori_generali(0);

    /* APERTURA FILE DI LOG */
    controllo_log_remove=remove("log.txt");
    if((fd_log=open("log.txt",O_RDWR | O_CREAT, S_IRWXU))<0) errori_generali(1);
    if(controllo_log_remove==0)
        if( write(fd_log,"Log precedente cancellato! \n",strlen("Log precedente cancellato! \n")) < 0 ) errori_generali(2);
    /* LOGGING */
    if( write(fd_log,"Porta: ",strlen("Porta: ")) < 0 ) errori_generali(2);
    if( write(fd_log,argv[1],strlen(argv[1])) < 0 ) errori_generali(2);
    /* LOGGING */
    snprintf(temp,sizeof temp, "\nTimestamp: %d.",(int)time(NULL));
    if( write(fd_log,temp,strlen(temp)) < 0 )
    {
        perror("Errore write in funzione_principale::4.");
        exit(1);
    }
    /* DA QUI È POSSIBILE CATTURARE I SEGNALI SIGINT, SIGTSTP, SIGPIPE E SIGALRM */
    if( signal(SIGINT,segnalami_tutta) == SIG_ERR || signal(SIGTSTP,segnalami_tutta) == SIG_ERR )
    {
        perror("SIGINT / SIGTSTP Error");
        exit(1);
    }

    if( signal(SIGPIPE,segnalami_tutta) == SIG_ERR || signal(SIGALRM,segnalami_tutta) == SIG_ERR  )
    {
        perror("SIGPIPE o SIGALRM Error::Funzione principale");
        exit(1);
    }

    /* CONTROLLO CORETTEZZA PORTA */
    porta=controllo_porta(porta);

    /* STRUTTRA PER SPECIFICHE DI CREAZIONE SOCKET */
    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;                   /* Specifico il tipo di protocolla da utilizzare */
    my_addr.sin_port = htons(porta);                /* Secifico il tipo di porta presa in input da terminale */
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    fd = socket(PF_INET, SOCK_STREAM,0);            /* Creazione file descriptor per il socket */

    if( bind(fd, (struct sockaddr *) &my_addr, sizeof(my_addr)) != 0 ) errori_generali(3);  /* Lega il socket appena creato con l'indirizzo specificato */

    sleep(1);
    system("clear");
    puts("********************************************");
    puts("* Server avviato con successo e in attesa! *");
    puts("********************************************");
    puts("* Per terminare la sessione inviare CTRL+C *");
    puts("********************************************");
    printf("             * Porta: %d *\n",porta);
    puts("             ***************");
    /* Creazione della mappa */
    matrice=crea_matrice();

    /* Creazione delle variabili globali TSD */
    if( pthread_key_create(&chiave_globale_fd,NULL) != 0 )
    {
        puts("\nErrore key create tsd 1");
        exit(1);
    }
    if( pthread_key_create(&username_globale_thread,NULL) != 0 )
    {
        puts("\nErrore key create tsd 2");
        exit(1);
    }
    if( pthread_key_create(&chiave_globale_pos_colonna,NULL) != 0 )
    {
        puts("\nErrore key create tsd 3");
        exit(1);
    }
    if( pthread_key_create(&chiave_globale_pos_riga,NULL) != 0 )
    {
        puts("\nErrore key create tsd 4");
        exit(1);
    }
    if( pthread_key_create(&chiave_globale_ID_Partita,NULL) != 0 )
    {
        puts("\nErrore key create tsd 5");
        exit(1);
    }

    //Inizializzazione dei mutex
    if(pthread_mutex_init(&lock_ID_Partita,NULL) != 0 || pthread_mutex_init(&sem,NULL) != 0 || pthread_mutex_init(&lock_lista_pthread,NULL) != 0 || pthread_mutex_init(&lock_lista_utenti,NULL) != 0 || pthread_mutex_init(&lock_misura_matrice,NULL) != 0 )
    {
        perror("\nMutex Error Main::Inizializzazione fallita!!!");
        exit(1);
    }
    if(pthread_mutex_init(&lock_flag_partita,NULL) != 0 || pthread_mutex_init(&lock_oggetti_trovati,NULL) != 0 || pthread_mutex_init(&lock_tempo,NULL) != 0 || pthread_mutex_init(&lock_utenti_in_gioco,NULL) != 0)
    {
        perror("\nMutex Error Main::Inizializzazione fallita!!!");
        exit(1);
    }
    /* Inizio attesa */
    while(1==1)
    {
        if( listen(fd,5) < 0 ) errori_generali(4); /* Il server rimane in ascolto */

        /* Accetta la connessione e memorizza la pipe in fd2 */
        fd2 = accept(fd, NULL, NULL);

        /* Crea un nuovo thread e lancia la funzione principale del thread passandole fd2 come argomento */
        pthread_t tid=0;
        if(errore_thread=pthread_create(&tid,NULL,funzione_principale,(void *)&fd2) != 0) /* QUI IL THREAD DI UN CLIENT VIENE LANCIATO, SI AVVIA LA FUNZIONE PRINCIPALE */
        {
            perror("Errore creazione thread::while infinito in funzione main.");
            exit(1);
        }
    }
    close(fd2);
    close(fd);

    /* Distruzione delle chiavi globali TSD */
    if( pthread_key_delete( chiave_globale_fd ) != 0 )
    {
        puts("\nErrore chiusura tsd1");
        exit(1);
    }
    if( pthread_key_delete( chiave_globale_ID_Partita ) != 0 )
    {
        puts("\nErrore chiusura tsd2");
        exit(1);
    }
    if( pthread_key_delete( chiave_globale_pos_colonna ) != 0 )
    {
        puts("\nErrore chiusura tsd2");
        exit(1);
    }
    if( pthread_key_delete( chiave_globale_pos_riga ) != 0 )
    {
        puts("\nErrore chiusura tsd2");
        exit(1);
    }
    if( pthread_key_delete( username_globale_thread ) != 0 )
    {
        puts("\nErrore chiusura tsd2");
        exit(1);
    }

    return 0;
}



/* FUNZIONI */
/* Funzione che controlla la presenza dell'utente nel database.txt */
int controllo_utente(char user[], int flag, int file)
{
    int risultato = -1, i = 0, cont = 1;
    char parola[MAXSIZE];
    if( lseek(file,0,SEEK_SET) < 0 ) errori_generali(5); /* Si reimposta l'indice del seek all'inizio del file */
    while(risultato == -1 && cont > 0)
    {
        i = 0;
        while(read(file,&parola[i],1) > 0)
        {
            if(parola[i] == ' ') break;
            i++;
        }
        parola[i] = '\0';
        if(strcmp(parola,user) == 0)
        {
            risultato = 0; /* Per risparmiare tempo, un return andrebbe bene anche qui */
            break;
        }
        i = 0;
        while(risultato != 0 && (cont=read(file,&parola[i],1)) > 0 && parola[i] != '\n')
        {
            i++;
        }
    }
    return risultato;
}
/* Funzione che controlla la correttezza della password */
int controllo_password(char password[],int file)
{
    int risultato = -1, i = 0;
    char parola[MAXSIZE];
    i = 0;
    while(read(file,&parola[i],1) > 0)
    {
        if(parola[i] == '\n') break;
        i++;
    }
    parola[i] = '\0';
    if(strcmp(parola,password) == 0)
    {
        risultato = 0; /* Per risparmiare tempo, un return andrebbe bene anche qui */
    }
    return risultato;
}
/* Funzione principale di avvio dei thread */
void *funzione_principale(void *arg)
{
        /* Blocco del mutex per la variabile ID_PARTITA */
        if( pthread_mutex_lock(&lock_ID_Partita) < 0 )
        {
            perror("Mutex Lock Error::Funzione principale");
            exit(1);
        }
        int ID_partita_Thread = ID_PARTITA;
        /* Sblocco del mutex per la variabile ID_PARTITA */
        if( pthread_mutex_unlock(&lock_ID_Partita) < 0 )
        {
            perror("Mutex unLock Error::Funzione principale");
            exit(1);
        }
        /* In fd2 salviamo la pipe del collegamento tra Server e Client --- Le altre sono variabili di supporto per le varie funzionalità */
        int fd2=*(int*)arg,controllo_up,file,flag_user=-1, contatore = 0, i=0, k=0, numero_lettere_lette = 0, utenti_in_gioco_tmp = 0;
        char flag_menu = '-',username[MAXSIZE],password,appoggio[MAXSIZE], *app_accesso;
        strcpy(username,"VUOTO");                                   /* Inizializziamo l'arrey dell'username per evitare comportamenti inaspettati */
        char nome_utente[MAXSIZE], password_utente[MAXSIZE];        /* Variabili usate nel case 1 e case 2 */
		char tmp='X', tmp_2 =' ';                                   /* Variabili di supporto */
		int *pos_riga_giocatore = (int *)malloc(sizeof(int));       /* Posizione riga del giocatore */
		int *pos_colonna_giocatore = (int *)malloc(sizeof(int));    /* Posizione colonna del giocatore */
        *pos_riga_giocatore = -5;
        *pos_colonna_giocatore = -5;

        /* Settiamo le variabili globali TSD sulle variabili appena create */
		pthread_setspecific(chiave_globale_fd,&fd2);
        pthread_setspecific(username_globale_thread,username);
        pthread_setspecific(chiave_globale_pos_riga,pos_riga_giocatore);
        pthread_setspecific(chiave_globale_pos_colonna,pos_colonna_giocatore);
        /* Blocco del mutex per la variabile */
        if( pthread_mutex_lock(&lock_ID_Partita) < 0 )
        {
            perror("Mutex Lock Error::Funzione principale");
            exit(1);
        }
        pthread_setspecific(chiave_globale_ID_Partita,&ID_partita_Thread);
        /* Sblocco del mutex per la variabile */
        if( pthread_mutex_unlock(&lock_ID_Partita) < 0 )
        {
            perror("Mutex Lock Error::Funzione principale");
            exit(1);
        }
        Lista_pthread = Inserisci_thread(Lista_pthread,pthread_self());

        /* LOGGING */
        snprintf(temp,sizeof temp, "\nAvvio thread(tid): %d.",(int)pthread_self());
        if( write(fd_log,temp,strlen(temp)) < 0 )
        {
            perror("Errore write in funzione_principale::1.");
            exit(1);
        }
        /* Ciclo per evitare che si tenti il login per tre volte di fila con credenziali errate */
        do
        {
            /* Leggo il carattere della funzionalità dal client */
            if(read(fd2,&flag_menu,1) < 0 ) errori_generali(6);

            switch(flag_menu)
            {
                case '1': /* Case di LOGIN */

                    /* Apertura del file con le credenziali degli utenti già registrati */
                    if((file=open("database.txt",O_RDWR | O_CREAT, S_IRWXU))<0) errori_generali(7);

                    /* Contatore per i tentativi di login */
                    contatore = 0;

                    /* Flag per la presenza o meno dell'utente nel database.txt */
                    flag_user = -1;

                    while( flag_user == -1 && contatore < 3 )
                    {
                        if( (numero_lettere_lette=read(fd2,appoggio,MAXSIZE)) < 0 ) errori_generali(8); /* Lettura dell'username */
                        appoggio[numero_lettere_lette] = '\0';

                        flag_user = controllo_utente(appoggio,1,file);

                        /* Se l'utente non è presente, flag è uguale a -1 */
                        if( flag_user == -1 )
                        {
                            contatore++;
                            if( write(fd2,"e",1) < 0 ) errori_generali(9);


                            /* LOGGING */
                            snprintf(temp,sizeof temp, "\nTid: %d --- Errore Login(nickname errato): %s.",(int)pthread_self(),appoggio);
                            if( write(fd_log,temp,strlen(temp)) < 0 ) errori_generali(10);
                        }
                        else
                        {
                            /* LOGGING */
                            snprintf(temp,sizeof temp, "\nTid: %d --- Login(nickname corretto): %s.",(int)pthread_self(),appoggio);
                            if( write(fd_log,temp,strlen(temp)) < 0 ) errori_generali(11);
							strcpy(username,appoggio);
                            if( write(fd2,"o",1) < 0 ) errori_generali(12);
                            if( (numero_lettere_lette=read(fd2,appoggio,MAXSIZE)) < 0 ) errori_generali(13); //Lettura della password
                            appoggio[numero_lettere_lette] = '\0';
                            flag_user = controllo_password(appoggio,file);
                            if( flag_user == -1 )
                            {   /* SE LA PASS È SBAGLIATA ALLORA DEVE INIZIARE DALL'INIZIO, ALTRIMENTI VA AVANTI E NON TROVA PIU' L'UTENTE */
                                if( lseek(file,0,SEEK_SET) < 0 )
                                {
                                    perror("Errore lseek in funzione_principale::errore su file.");
                                    exit(1);
                                }
                                contatore++;
                                if( write(fd2,"e",1) < 0 )
                                {
                                    perror("Errore write in funzione_principale::ciclo while::Scrittura messaggio di errore 'e' per controllo_password.");
                                    exit(1);
                                }
                                /* LOGGING */
                                snprintf(temp,sizeof temp, "\nTid: %d --- Errore Login(password errata): %s.",(int)pthread_self(),appoggio);
                                if( write(fd_log,temp,strlen(temp)) < 0 )
                                {
                                    perror("Errore write in funzione_principale::4.");
                                    exit(1);
                                }

                            }
                            else
                            {
                                /* Blocchiamo il mutex */
                                if( pthread_mutex_lock(&lock_lista_utenti) < 0 )
                                {
                                    perror("Errore lock lista utenti");
                                    exit(1);
                                }
                                if( (flag_user=RicercaUtenteLoggato(ListaUtenti,username)) != 0 )
                                {
                                    if( lseek(file,0,SEEK_SET) < 0 )
                                    {
                                        perror("Errore lseek in funzione_principale::errore su file.");
                                        exit(1);
                                    }
                                    contatore++;
                                    if( write(fd2,"e",1) < 0 )
                                    {
                                        perror("Errore write in funzione_principale::ciclo while::Scrittura messaggio di errore 'e' per controllo_password.");
                                        exit(1);
                                    }
                                    /* LOGGING */
                                    snprintf(temp,sizeof temp, "\nTid: %d --- Login rifiutato: %s. Utente già loggato.",(int)pthread_self(),appoggio);
                                    if( write(fd_log,temp,strlen(temp)) < 0 )
                                    {
                                        perror("Errore write in funzione_principale::8.");
                                        exit(1);
                                    }
                                }
                                else
                                {
                                    /* Se i controlli riguardo alla password ed al nickname restituiscono una conferma, allora l'utente viene inserito nella lista */
                                    ListaUtenti = InserisciUtente(ListaUtenti, pthread_self(), username, 0);

                                    /* LOGGING */
                                    snprintf(temp,sizeof temp, "\nTid: %d --- Login(password corretta): %s.",(int)pthread_self(),appoggio);
                                    if( write(fd_log,temp,strlen(temp)) < 0 )
                                    {
                                        perror("Errore write in funzione_principale::4.");
                                        exit(1);
                                    }
                                    /* Scrittura di un carattere di conferma */
                                    if( write(fd2,"o",1) < 0 )
                                    {
                                        perror("Errore write in funzione_principale::ciclo while::Scrittura messaggio di ok 'o'.");
                                        exit(1);
                                    }
                                }
                                /* Sblocchiamo il mutex */
                                if( pthread_mutex_unlock(&lock_lista_utenti) < 0 )
                                {
                                    perror("Errore unlock lista utenti");
                                    exit(1);
                                }
                            }
                        }
                    }

                    /* LOGGING */
                    snprintf(temp,sizeof temp, "\nTid: %d --- Tentativi login: %d.",(int)pthread_self(),contatore);
                    if( write(fd_log,temp,strlen(temp)) < 0 )
                    {
                        perror("Errore write in funzione_principale::2.");
                        exit(1);
                    }
                    close(file);
                    numero_lettere_lette = 0;
					break;
                case '2': /* Case di REGISTRAZIONE */

                    /* Apertura del file con le credenziali degli utenti già registrati */
                    if( (file=open("database.txt",O_RDWR | O_APPEND, S_IRWXU)) < 0 )
                    {
                        perror("Errore apertura file database.txt::Registrazione::funzione_principale::case 2");
                        exit(1);
                    }

                    do{
                        /* LETTURA NOME UTENTE / NICKNAME PER LA REGISTRAZIONE */
                        if( (numero_lettere_lette=read(fd2,nome_utente,MAXSIZE)) < 0 ) //Leggo l'username
                        {
                            perror("Errore read in funzione_principale::Registrazione::ciclo while::Lettura username.");
                            exit(1);
                        }
                        nome_utente[numero_lettere_lette] = '\0';

                        /*SE L'UTENTE E' PRESENTE, RESTITUISCE -1, DIVERSO ALTRIMENTI */
                        flag_user = controllo_utente(nome_utente,1,file);

                        if( flag_user != -1 )
                        {
                            /* Scrive un carattere di errore */
                            if( write(fd2,"e",1) < 0 )
                            {
                                perror("Errore write in funzione_principale::ciclo while::Registrazione::Scrittura messaggio di errore 'e' per controllo_utente.");
                                exit(1);
                            }

                            /* LOGGING */
                            snprintf(temp,sizeof temp, "\nTid: %d --- Errore registrazione(nickname gia' presente): %s.",(int)pthread_self(),nome_utente);
                            if( write(fd_log,temp,strlen(temp)) < 0 )
                            {
                                perror("Errore write in funzione_principale::Registrazione::3.");
                                exit(1);
                            }
                        }
                        else
                        {
                            /* Scrive un carattere di conferma */
                            if( write(fd2,"o",1) < 0 )
                            {
                                perror("Errore write in funzione_principale::ciclo while::Registrazione::Scrittura messaggio di conferma 'o' per controllo_utente.");
                                exit(1);
                            }
                            /* LETTURA PASSWORD PER LA REGISTRAZIONE */
                            if( (numero_lettere_lette=read(fd2,password_utente,MAXSIZE)) < 0 ) /* Lettura dell'username */
                            {
                                perror("Errore read in funzione_principale::Registrazione::ciclo while::Lettura password_utente.");
                                exit(1);
                            }
                            password_utente[numero_lettere_lette] = '\0';

                            if( write(file,nome_utente,strlen(nome_utente)) < 0 )
                            {
                                perror("Errore write in funzione_principale::ciclo while::Registrazione::Scrittura nome_utente nel file database.txt.");
                                exit(1);
                            }
                            /* SCRITTURA DI UNO SPAZIO NEL DATABASE.TXT */
                            if( write(file," ",strlen(" ")) < 0 )
                            {
                                perror("Errore write in funzione_principale::ciclo while::Registrazione::Scrittura di uno spazio nel file database.txt.");
                                exit(1);
                            }
                            /* SCRITTURA DELLA PASSWORD NEL DATABASE.TXT */
                            if( write(file,password_utente,strlen(password_utente)) < 0 )
                            {
                                perror("Errore write in funzione_principale::ciclo while::Registrazione::Scrittura password_utente nel file database.txt.");
                                exit(1);
                            }
                            /* SCRITTURA DI UN \n NEL DATABASE.TXT */
                            if( write(file,"\n",strlen("\n")) < 0 )
                            {
                                perror("Errore write in funzione_principale::ciclo while::Registrazione::Scrittura password_utente nel file database.txt.");
                                exit(1);
                            }
                        }
                    }while( flag_user != -1 );

                    /* LOGGING */
                    snprintf(temp,sizeof temp, "\nTid: %d --- Registrazione utente: %s.",(int)pthread_self(),nome_utente);
                    if( write(fd_log,temp,strlen(temp)) < 0 )
                    {
                        perror("Errore write in funzione_principale::Registrazione::4.");
                        exit(1);
                    }

                    close(file);
                    break;
                case 'A': /* Inserimento giocatore e stampa mappa aggiornata */

                    /* Funzione che inserisce il giocatore nella mappa */
                    Gioca(fd2, username,pos_riga_giocatore,pos_colonna_giocatore);

                    /* LOGGING */
                    snprintf(temp,sizeof temp, "\nTid: %d --- Utente: %s --- Avvio gioco.",(int)pthread_self(),username);
                    if( write(fd_log,temp,strlen(temp)) < 0 )
                    {
                        perror("Errore write in funzione_principale::3.");
                        exit(1);
                    }
                    break;
                case 'B': /* Utenti collegati */
                    if( pthread_mutex_lock(&lock_lista_utenti) < 0 )
                    {
                        perror("Errore lock lista utenti");
                        exit(1);
                    }
                    /* Stampa una lista di utenti collegati */
                    StampaListaUtenti(ListaUtenti,fd2);

                    if( pthread_mutex_unlock(&lock_lista_utenti) < 0 )
                    {
                        perror("Errore unlock lista utenti");
                        exit(1);
                    }
                    if( write(fd2,"e",1) < 0 ) /* Scrivo il carattere di terminazione nel file descriptor */
                    {
                        perror("Errore read in funzione_principale::ciclo while::caso B::Scrittura del carattere di terminazione.");
                        exit(1);
                    }

                    /* LOGGING */
                    snprintf(temp,sizeof temp, "\nTid: %d --- Utente: %s --- Avvio stampa lista degli utenti in gioco.",(int)pthread_self(),username);
                    if( write(fd_log,temp,strlen(temp)) < 0 )
                    {
                        perror("Errore write in funzione_principale::4.");
                        exit(1);
                    }
                    break;
                case 'C': /* Manda al client gli oggetti rimanenti nella mappa */

                    NumeroOggRimanenti(fd2);

                    /* LOGGING */
                    snprintf(temp,sizeof temp, "\nTid: %d --- Utente: %s --- Avvio stampa numero degli oggetti rimanenti.",(int)pthread_self(),username);
                    if( write(fd_log,temp,strlen(temp)) < 0 )
                    {
                        perror("Errore write in funzione_principale::5.");
                        exit(1);
                    }
                    break;
                case 'D': /* Manda al client gli oggetti trovati nella mappa */

                    NumeroOggTrovati(fd2);

                    /* LOGGING */
                    snprintf(temp,sizeof temp, "\nTid: %d --- Utente: %s --- Avvio stampa numero degli oggetti trovati.",(int)pthread_self(),username);
                    if( write(fd_log,temp,strlen(temp)) < 0 )
                    {
                        perror("Errore write in funzione_principale::5.");
                        exit(1);
                    }
                    break;
                case 'X': /* Case di spostamento */

                    /* Mandiamo la conferma al client */
                    if( write(fd2,"o",1) < 0 )
                    {
                        perror("Errore read in X::Scrittura della 'o' caso x.");
                        exit(1);
                    }

                    /* Funzione che riceve la casella in cui l'utente deve spostarsi */
                    li_vuoi_quei_kiwi(fd2,pos_riga_giocatore,pos_colonna_giocatore,username); /* Funzione di raccolta della frutta */

                    if( pthread_mutex_lock(&lock_flag_partita) < 0 )
                    {
                        perror("Errore lock flag partita");
                        exit(1);
                    }
                    /* Controlliamo che la partita sia finita o meno */
                    if( flag_partita == 1 )
                    {
                        /* RICREA LA MAPPA */

                        if( pthread_mutex_lock(&lock_lista_utenti) < 0 )
                        {
                            perror("Errore lock lista utenti");
                            exit(1);
                        }
                        /* Rimuove i giocatori dalla partita così non risultano più in gioco */
                        RimuoviGiocatori_dalla_Partita(ListaUtenti);

                        if( pthread_mutex_unlock(&lock_lista_utenti) < 0 )
                        {
                            perror("Errore unlock lista utenti");
                            exit(1);
                        }
                        /* Crea una nuova mappa */
                        matrice=crea_matrice();

                        if( pthread_mutex_lock(&lock_ID_Partita) < 0 )
                        {
                            perror("Errore lock id partita");
                            exit(1);
                        }
                        /* Incrementa l'ID della Partita */
                        ID_PARTITA++;
                        /* Aggiorna l'ID del thread corrente */
                        ID_partita_Thread = ID_PARTITA;

                        if( pthread_mutex_unlock(&lock_ID_Partita) < 0 )
                        {
                            perror("Errore unlock id partita");
                            exit(1);
                        }

                        /* LOGGING */
                        snprintf(temp,sizeof temp, "\nTid: %d --- L'utente %s termina la partita.\nMappa ricreata con ID: %d!\n",(int)pthread_self(),username, ID_PARTITA);
                        if( write(fd_log,temp,strlen(temp)) < 0 )
                        {
                            perror("Errore write in funzione_principale::10.");
                            exit(1);
                        }
                    }
                    if( pthread_mutex_unlock(&lock_flag_partita) < 0 )
                    {
                        perror("Errore unlock flag partita");
                        exit(1);
                    }
                    break;
                case 'L': /* INVIO LA LISTA DI UTENTI CON GLI OGGETTI */
                    if( pthread_mutex_lock(&lock_lista_utenti) < 0 )
                    {
                        perror("Errore lock lista utenti");
                        exit(1);
                    }
                    /* Invia al client solo gli utenti in gioco */
                    StampaListaUtenti2(ListaUtenti,fd2);

                    if( pthread_mutex_unlock(&lock_lista_utenti) < 0 )
                    {
                        perror("Errore unlock lista utenti");
                        exit(1);
                    }
                    if( write(fd2,"e",1) < 0 ) //Scrivo il carattere di terminazione nel file descriptor
                    {
                        perror("Errore read in funzione_principale::ciclo while::caso B::Scrittura del carattere di terminazione.");
                        exit(1);
                    }

                    /* LOGGING */
                    snprintf(temp,sizeof temp, "\nTid: %d --- L'utente %s ha richiesto la lista degli utenti con gli oggetti trovati.",(int)pthread_self(),username);
                    if( write(fd_log,temp,strlen(temp)) < 0 )
                    {
                        perror("Errore write in funzione_principale::9.");
                        exit(1);
                    }
                    break;
                case 'W': /* Case per la stampa della mappa */
                    /*Invia al client la mappa */
                    InvioMappaClient(fd2, pos_riga_giocatore, pos_colonna_giocatore);
                    break;
                case 'T': /* SEGNALE DI TERMINAZIONE LATO CLIENT */
                    if( pthread_mutex_lock(&lock_lista_utenti) < 0 )
                    {
                        perror("Errore lock lista utenti");
                        exit(1);
                    }

                    /*Scollego l'utente */
                    ListaUtenti = ScollegaUtente(ListaUtenti, fd2, username,pos_riga_giocatore,pos_colonna_giocatore);

                    if( pthread_mutex_unlock(&lock_lista_utenti) < 0 )
                    {
                        perror("Errore unlock lista utenti");
                        exit(1);
                    }
                    /* LOGGING */
                    snprintf(temp,sizeof temp, "\nTid: %d --- L'utente %s ha terminato il client con CTRL-C.",(int)pthread_self(),username);
                    if( write(fd_log,temp,strlen(temp)) < 0 )
                    {
                        perror("Errore write in funzione_principale::7.");
                        exit(1);
                    }
                    /* Imposto la flag a 0 così esce dal ciclo e termina il thread */
                    flag_menu='0';
                    break;
                case '0': /* Imposto la flag a 0 così esce dal ciclo e termina il thread */
                    flag_menu='0';
                    break;
                case 'P': /* USCITA DALLA PARTITA IN CORSO, DECREMENTO NUMERO GIOCATORI IN GIOCO */
                    if( pthread_mutex_lock(&lock_lista_utenti) < 0 )
                    {
                        perror("Errore lock lista utenti");
                        exit(1);
                    }

                    /*Rimuove il giocatore dalla mappa */
                    ListaUtenti = RimuoviGiocatore_Dalla_Partita(ListaUtenti, fd2, username, pos_riga_giocatore, pos_colonna_giocatore);

                    if( pthread_mutex_unlock(&lock_lista_utenti) < 0 )
                    {
                        perror("Errore unlock lista utenti");
                        exit(1);
                    }

                    //Mandiamo conferma al client
                    if( write(fd2,"o",1) < 0 )
                    {
                        perror("Errore read in X::Scrittura della 'o' caso p.");
                        exit(1);
                    }

                    /* LOGGING */
                    snprintf(temp,sizeof temp, "\nTid: %d --- L'utente %s esce dalla partita.",(int)pthread_self(),username);
                    if( write(fd_log,temp,strlen(temp)) < 0 )
                    {
                        perror("Errore write in funzione_principale::6.");
                        exit(1);
                    }
                    break;
                case 'Z': //USCITA DOPO IL LOGIN
                    if( pthread_mutex_lock(&lock_lista_utenti) < 0 )
                    {
                        perror("Errore lock lista utenti");
                        exit(1);
                    }
                    ListaUtenti = ScollegaUtente(ListaUtenti, fd2, username, pos_riga_giocatore, pos_colonna_giocatore);
                    if( pthread_mutex_unlock(&lock_lista_utenti) < 0 )
                    {
                        perror("Errore unlock lista utenti");
                        exit(1);
                    }
                    /* LOGGING */
                    snprintf(temp,sizeof temp, "\nTid: %d --- L'utente %s ha effettuato il logout.",(int)pthread_self(),username);
                    if( write(fd_log,temp,strlen(temp)) < 0 )
                    {
                        perror("Errore write in funzione_principale::5.");
                        exit(1);
                    }
                    break;
                case 'V':
                    Invia_tempo_Client(fd2);
                    break;
                case 'H': //Controllo partita
                    if( pthread_mutex_lock(&lock_ID_Partita) < 0 )
                    {
                        perror("Errore lock id partita");
                        exit(1);
                    }
                    if( ID_PARTITA != ID_partita_Thread )
                    {
                        if( write(fd2,"c",1) < 0 )
                        {
                            perror("Errore read in X::Scrittura della 'o' caso h.");
                            exit(1);
                        }
                        ID_partita_Thread = ID_PARTITA;
                    }
                    else
                    {
                        if( pthread_mutex_lock(&lock_flag_partita) < 0 )
                        {
                            perror("Errore lock flag partita");
                            exit(1);
                        }
                        if( flag_partita == 1 )
                        {
                            if( write(fd2,"c",1) < 0 )
                            {
                                perror("Errore read in X::Scrittura della 'o' caso h2.");
                                exit(1);
                            }
                            //RICREA LA MAPPA
                            if( pthread_mutex_lock(&lock_lista_utenti) < 0 )
                            {
                                perror("Errore lock lista utenti");
                                exit(1);
                            }
                            RimuoviGiocatori_dalla_Partita(ListaUtenti);
                            if( pthread_mutex_unlock(&lock_lista_utenti) < 0 )
                            {
                                perror("Errore unlock lista utenti");
                                exit(1);
                            }
                            matrice=crea_matrice(); //DEALLOCO LA VECCHIA MAPPA E NE CREO UNA NUOVA
                            ID_PARTITA++; //INCREMENTO L'ID DELLA PARTITA
                            ID_partita_Thread = ID_PARTITA; //AGGIORNO LA VARIABILE ID PARTITA DEL THREAD ALL'ID PARTITA GLOBALE
                            /* LOGGING */
                            snprintf(temp,sizeof temp, "\nTid: %d --- Tempo scaduto!\nMappa ricreata con ID: %d!\n",(int)pthread_self(),ID_PARTITA);
                            if( write(fd_log,temp,strlen(temp)) < 0 )
                            {
                                perror("Errore write in funzione_principale::10.");
                                exit(1);
                            }
                        }
                        else
                        {
                            if( write(fd2,"o",1) < 0 )
                            {
                                perror("Errore read in X::Scrittura della 'o' caso h3.");
                                exit(1);
                            }
                        }
                        if( pthread_mutex_unlock(&lock_flag_partita) < 0 )
                        {
                            perror("Errore unlock flag partita");
                            exit(1);
                        }
                    }
                    if( pthread_mutex_unlock(&lock_ID_Partita) < 0 )
                    {
                        perror("Errore unlock id partita");
                        exit(1);
                    }
                    break;
                default:
                    //QUALCOSA
                    break;
            }
        }while( flag_menu != '0' && flag_menu != 'e' ); //UN VALORE PER TERMINARE ED UNO PER CAMBIARE COSE

    /* Rimuovo il thread dall'ABR di thread costruito */
    Lista_pthread = Rimuovi_thread(Lista_pthread,pthread_self());

    /* LOGGING */
    snprintf(temp,sizeof temp, "\nTid: %d --- Thread chiuso --- Utente: %s.",(int)pthread_self(),username);
    if( write(fd_log,temp,strlen(temp)) < 0 )
    {
        perror("Errore write in funzione_principale::3.");
        exit(1);
    }
    close(fd2);
    return;
}

/* FUNZIONE CONTROLLO PORTA */
int controllo_porta(int porta)
{
    while(!(porta>=5000) && (porta<=32768)) // La porta specificata non e' presente nel range delle porte utente che possono essere usate
    {
        puts("************************* ERRORE ************************");
        printf("* La porta specificata e' riservata ai processi di root *\n* Si prega di inserire un'altra porta (5000-32768)      *\n");
        puts("*********************************************************\n");
        printf("Nuova porta:");
        scanf("%d",&porta);
        system("clear");
    };
    //puts("************************* SERVER ************************");
    //printf("*           Numero di porta (%d) accettato!           *\n",porta);
    //puts("*********************************************************");
    //puts("\nAvvio del server in corso...");
    return porta;
}

/* Funzione creazione mappa */
casella **crea_matrice()
{
    if( matrice != NULL )                                               /* SE LA MAPPA ESISTE GIÀ ALLORA VIENE DEALLOCATA */
    {
        cancellaMappa(matrice);
        matrice = NULL;
    }
    int i=0,k=0,tipo_casella,cont_muri=0,cont_oggetti=0;                /* Variabili di supporto alla funzione */
    srand(time(NULL));
    if( pthread_mutex_lock(&lock_misura_matrice) < 0 )
    {
        perror("Errore lock");
        exit(1);
    }
    misura_matrice = 15 + rand() % 11 ;                                 /* La dimensione della mappa varia da 15x15 a 25x15 */
    utenti_max = misura_matrice / 3;                                    /* Il numero di utenti max per partita è sempre un terzo della dimensione della mappa */
    utenti_in_gioco = 0;                                                /* Contatore utenti in gioco */
    numero_oggetti_nella_mappa = 0;
    numero_oggetti_trovati = 0;
    flag_partita = 0;
    flag_tempo = 0;
    matrice=(casella**)malloc(misura_matrice * sizeof(casella*));

    for(i=0;i<misura_matrice;i++)
        matrice[i]=(casella*)malloc(misura_matrice * sizeof(casella));

    for(i=0;i<misura_matrice;i++)
        for(k=0;k<misura_matrice;k++)
        {
            if((i>0) && (i<misura_matrice-1) && (k>0) && (k<misura_matrice-1))
                {
                    tipo_casella =  rand() % 5;  /* SE ZERO MURO - SE UNO METTE OGG - ALTRIMENTI NIENTE */
                    switch(tipo_casella)
                    {
                        case 0:
                           if(cont_muri<misura_matrice*20)
                             {
                                cont_muri++;
                                matrice[i][k].flag_muro=1; /* MURO */
                            }
                            matrice[i][k].numero_pid=-1;
                            matrice[i][k].indice_ogg=-1;
                            break;
                        case 1:
                                if(++cont_oggetti<misura_matrice*5) /* il numero di ogg è al massimo la dim della matrice moltiplicata per cinque */
                                {
                                    /* MEMORIZZO IL NUMERO TOTALE DI OGGETTI */
                                    numero_oggetti_nella_mappa++;
                                    matrice[i][k].indice_ogg = 1+ rand() % 50;
                                }
                                else
                                    matrice[i][k].indice_ogg=-1;

                                matrice[i][k].flag_muro=0; /* no muro */
                                matrice[i][k].numero_pid=-1;
                            break;
                        default:
                            matrice[i][k].flag_muro=0; /* no muro */
                            matrice[i][k].numero_pid=-1;
                            matrice[i][k].indice_ogg=-1;
                            break;
                    }
                }
                else
                {
                    tipo_casella =  1 + rand() % 5;  /* SE UNO METTE OGG - ALTRIMENTI NIENTE */
                    switch(tipo_casella)
                    {
                       case 1:
                            if(++cont_oggetti<misura_matrice*5) /* il numero di ogg è al massimo la dim della matrice moltiplicata per cinque */
                            {
                                /* MEMORIZZO IL NUMERO TOTALE DI OGGETTI */
                                numero_oggetti_nella_mappa++;
                                matrice[i][k].indice_ogg= 1+ rand() % 50;
                            }
                            else
                                matrice[i][k].indice_ogg=-1;

                            matrice[i][k].flag_muro=0; /* no muro */
                            matrice[i][k].numero_pid=-1;
                            break;
                        default:
                            matrice[i][k].flag_muro=0; /* no muro */
                            matrice[i][k].numero_pid=-1;
                            matrice[i][k].indice_ogg=-1;
                            break;
                    }
                }
        }

    /* LOGGING */
    char temp[MAXSIZE];
    snprintf(temp,sizeof temp, "\nDimensione mappa: %d x %d\nNumero oggetti presenti: %d\nAnteprima della mappa:\n",misura_matrice,misura_matrice,numero_oggetti_nella_mappa);
    if( write(fd_log,temp,strlen(temp)) < 0 )
    {
        perror("Errore write in crea_matrice::1.");
        exit(1);
    }
    for(i=0;i<misura_matrice;i++)
    {
        //puts("");
        //LOG
        if( write(fd_log,"\n",strlen("\n")) < 0 )
        {
            perror("Errore write in crea_matrice::2.");
            exit(1);
        }
        for(k=0;k<misura_matrice;k++)
            {
                if( matrice[i][k].flag_muro==1) // MURO
                {
                    //printf("\033[01;31m|M|\033[00m");
                    //LOG
                    if( write(fd_log,"|M|",strlen("|M|")) < 0 )
                    {
                        perror("Errore write in crea_matrice::3.");
                        exit(1);
                    }
                }
                else
                    if( matrice[i][k].indice_ogg>=0) // OGGETTO
                    {
                        //printf("\x1b[35m|O|\x1b[0m"); //Colore oggetti magenta
                        //LOG
                        if( write(fd_log,"|O|",strlen("|O|")) < 0 )
                        {
                            perror("Errore write in crea_matrice::4.");
                            exit(1);
                        }
                    }
                    else
                    {
                        //printf("|V|"); // POSIZIONE VUOTA
                        //LOG
                        if( write(fd_log,"|V|",strlen("|V|")) < 0 )
                        {
                            perror("Errore write in crea_matrice::5.");
                            exit(1);
                        }
                    }
            }
    }
    if( pthread_mutex_unlock(&lock_misura_matrice) < 0 )
    {
        perror("Errore unlock");
        exit(1);
    }
    //LOG
    if( write(fd_log,"\n",strlen("\n")) < 0 )
    {
        perror("Errore write in crea_matrice::6.");
        exit(1);
    }
    //puts("\n\nStampa della mappa completata!!!"); //PER DEBUG
return matrice;
}

/* Funzione di inserimento dell'oggetto Oggetto[] nella lista di un utente */
oggetti *InserisciOgg(oggetti *top, char oggetto[])
{
    if( top!= NULL )
    {
        top->next = InserisciOgg(top->next,oggetto);
    }
    else
    {
        top = (oggetti *)malloc(sizeof(oggetti));
        top->next = NULL;
        strcpy(top->nome_oggetto,oggetto);
    }
    return top;
}

/* Funzione di inserimento dell'oggetto Oggetto[] nella lista di un utente */
void InserisciOggettoUtente(utenti* top, char nickname[], char oggetto[])
{
    if( top != NULL )
    {
        if( strcmp(top->nickname,nickname) > 0 )
		{
			InserisciOggettoUtente(top->sx, nickname, oggetto);
		}
		else
		{
            if( strcmp(top->nickname,nickname) < 0 )
            {
                InserisciOggettoUtente(top->dx, nickname, oggetto);
            }
            else
            {
                top->numero_oggetti_trovati++;
                top->oggetti_trovati = InserisciOgg(top->oggetti_trovati,oggetto);
            }
		}
    }
    return;
}

/* Funzione dello spostamento -- Riceve i singoli spostamenti dal client e per risposta invia una conferma */
void li_vuoi_quei_kiwi(int fd2, int *pos_riga_giocatore, int *pos_colonna_giocatore, char nickname[])
{
    /* Variabili di supporto alle funzionalità */
    char tmp = ' ', appoggio[MAXSIZE], temp[MAXSIZE], var_tmp_finzi = 'M';
    if( read(fd2,&tmp,1) < 0 ) /* Lettura dello spostamento */
    {
        perror("Errore read in li_vuoi_quei_kiwi::Lettura spostamento.");
        exit(1);
    }
    /* UN UPPER DEL CARATTERE LETTO CI CONSENTE DI RESTRINGERE I CASI DA CONTROLLARE */
    tmp = toupper(tmp);
    if( pthread_mutex_lock(&lock_misura_matrice) < 0 )
    {
        perror("Errore lock");
        exit(1);
    }
    switch(tmp)
    {
        case 'D':  /* SPOSTAMENTO A DESTRA */
            if( (*pos_colonna_giocatore+1) < misura_matrice )
            {
                if( pthread_mutex_lock(&sem) < 0 ) /* BLOCCO MUTEX */
                {
                    perror("\nErrore mutex lock caso D 1!");
                    exit(1);
                }
                /* Controlliamo che la casella non sia un muro e che non sia già occupata da un giocatore */
                if( matrice[*pos_riga_giocatore][*pos_colonna_giocatore+1].flag_muro == 0 && matrice[*pos_riga_giocatore][*pos_colonna_giocatore+1].numero_pid == -1 )
                {
                    matrice[*pos_riga_giocatore][*pos_colonna_giocatore+1].numero_pid = matrice[*pos_riga_giocatore][*pos_colonna_giocatore].numero_pid;
                    matrice[*pos_riga_giocatore][*pos_colonna_giocatore].numero_pid = -1;
                    *pos_colonna_giocatore = *pos_colonna_giocatore + 1;
                    /* Mandiamo conferma al client */
                    if( matrice[*pos_riga_giocatore][*pos_colonna_giocatore].indice_ogg > 0 )
                    {
                        numero_oggetti_nella_mappa--;
                        lista_oggetti(matrice[*pos_riga_giocatore][*pos_colonna_giocatore].indice_ogg,appoggio);
                        if( write(fd2,"g",1) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'o'.");
                            exit(1);
                        }
                        if( write(fd2,appoggio,sizeof(appoggio)) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'o'.");
                            exit(1);
                        }
                        matrice[*pos_riga_giocatore][*pos_colonna_giocatore].indice_ogg = 0;
                        if( pthread_mutex_lock(&lock_oggetti_trovati) < 0 )
                        {
                            perror("Errore lock ogg");
                            exit(1);
                        }

                        /* Incrementiamo il numero di oggetti trovati nella mappa */
                        numero_oggetti_trovati++;

                        /* LOGGING */
                        snprintf(temp,sizeof temp, "\nTid: %d --- Numero oggetti trovati: %d --- Oggetti rimanenti: %d.",(int)pthread_self(),numero_oggetti_trovati,numero_oggetti_nella_mappa);
                        if( write(fd_log,temp,strlen(temp)) < 0 )
                        {
                            perror("Errore write in funzione_principale::1.");
                            exit(1);
                        }
                        if( pthread_mutex_unlock(&lock_oggetti_trovati) < 0 )
                        {
                            perror("Errore unlock ogg");
                            exit(1);
                        }
                        if( pthread_mutex_lock(&lock_lista_utenti) < 0 )
                        {
                            perror("Errore lock lista utenti");
                            exit(1);
                        }
                        InserisciOggettoUtente(ListaUtenti, nickname, appoggio);
                        if( pthread_mutex_unlock(&lock_lista_utenti) < 0 )
                        {
                            perror("Errore unlock lista utenti");
                            exit(1);
                        }
                        /* LOGGING */
                        snprintf(temp,sizeof temp, "\nTid: %d --- Utente %s spostato in posizione %c%c --- Trovato oggetto: %s.",(int)pthread_self(),nickname,(*pos_riga_giocatore+65),(*pos_colonna_giocatore+65),appoggio);
                        if( write(fd_log,temp,strlen(temp)) < 0 )
                        {
                            perror("Errore write in funzione_principale::1.");
                            exit(1);
                        }
                        /* Controllo oggetti */
                        if( read(fd2,&var_tmp_finzi,1) < 0 && var_tmp_finzi != 'o' ) //Leggo la riga
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Lettura conferma OGGETTI TERMINATI.");
                            exit(1);
                        }
                        if( numero_oggetti_nella_mappa == 0 )
                        {
                            /*  TERMINA PARTITA, RICOMINCIA IL TUTTO CREANDO UNA NUOVA MAPPA
                                INVIAMO UN CONTROLLO AL CLIENT
                            */
                            var_tmp_finzi = 'f';
                            if( pthread_mutex_lock(&lock_flag_partita) < 0 )
                            {
                                perror("Errore lock flag partita");
                                exit(1);
                            }
                            flag_partita = 1;
                            if( pthread_mutex_unlock(&lock_flag_partita) < 0 )
                            {
                                perror("Errore unlock flag partita");
                                exit(1);
                            }
                        }
                        else
                        {
                            var_tmp_finzi = 'o';
                        }
                        if( write(fd2,&var_tmp_finzi,sizeof(var_tmp_finzi)) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura conferma OGGETTI.");
                            exit(1);
                        }
                    }
                    else
                    {
                        if( write(fd2,"o",1) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'o'.");
                            exit(1);
                        }
                        /* LOGGING */
                        snprintf(temp,sizeof temp, "\nTid: %d --- Utente %s spostato in posizione %c%c.",(int)pthread_self(),nickname,(*pos_riga_giocatore+65),(*pos_colonna_giocatore+65));
                        if( write(fd_log,temp,strlen(temp)) < 0 )
                        {
                            perror("Errore write in funzione_principale::1.");
                            exit(1);
                        }
                    }
                }
                else
                {
                    if( matrice[*pos_riga_giocatore][*pos_colonna_giocatore+1].numero_pid == -1 )
                    {   /*
                            ALLORA C'E' UN MURO
                            Mandiamo l'errore al client
                        */
                        if( write(fd2,"e" ,1) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'e'.");
                            exit(1);
                        }
                    }
                    else
                    {   /*
                            C'È UN GIOCATORE
                            Mandiamo l'errore al client
                        */
                        if( write(fd2,"j" ,1) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'e'.");
                            exit(1);
                        }
                    }
                    /* LOGGING */
                    snprintf(temp,sizeof temp, "\nTid: %d --- Utente: %s --- Tentato spostamento non valido.",(int)pthread_self(),nickname);
                    if( write(fd_log,temp,strlen(temp)) < 0 )
                    {
                        perror("Errore write in funzione_principale::1.");
                        exit(1);
                    }
                }
                if( pthread_mutex_unlock(&sem) < 0 ) /* SBLOCCO MUTEX */
                {
                    perror("\nErrore mutex unlock caso D 1!");
                    exit(1);
                }
            }
            else /* NON PUO' ANDARE QUINDI ERRORE */
            {
                /* Mandiamo l'errore al client */
                if( write(fd2,"e" ,1) < 0 )
                {
                    perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'e'.");
                    exit(1);
                }
                /* LOGGING */
                snprintf(temp,sizeof temp, "\nTid: %d --- Utente: %s --- Tentato spostamento non valido.",(int)pthread_self(),nickname);
                if( write(fd_log,temp,strlen(temp)) < 0 )
                {
                    perror("Errore write in funzione_principale::1.");
                    exit(1);
                }
            }
            break;
        case 'A': /* Spostamento a sinistra */
            if( (*pos_colonna_giocatore-1) >= 0 )
            {
                if( pthread_mutex_lock(&sem) < 0 )
                {
                    perror("\nErrore mutex lock caso A 1!");
                    exit(1);
                }
                /* Controlliamo che la casella non sia un muro e che non sia già occupata da un giocatore */
                if( matrice[*pos_riga_giocatore][*pos_colonna_giocatore-1].flag_muro == 0 && matrice[*pos_riga_giocatore][*pos_colonna_giocatore-1].numero_pid == -1 )
                {
                    matrice[*pos_riga_giocatore][*pos_colonna_giocatore-1].numero_pid = matrice[*pos_riga_giocatore][*pos_colonna_giocatore].numero_pid;
                    matrice[*pos_riga_giocatore][*pos_colonna_giocatore].numero_pid = -1;
                    *pos_colonna_giocatore = *pos_colonna_giocatore -1;
                    /* Mandiamo conferma al client */
                    if( matrice[*pos_riga_giocatore][*pos_colonna_giocatore].indice_ogg > 0 )
                    {
                        numero_oggetti_nella_mappa--;
                        lista_oggetti(matrice[*pos_riga_giocatore][*pos_colonna_giocatore].indice_ogg,appoggio);
                        if( write(fd2,"g",1) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'o'.");
                            exit(1);
                        }
                        if( write(fd2,appoggio,sizeof(appoggio)) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'o'.");
                            exit(1);
                        }
                        matrice[*pos_riga_giocatore][*pos_colonna_giocatore].indice_ogg = 0;
                        if( pthread_mutex_lock(&lock_oggetti_trovati) < 0 )
                        {
                            perror("Errore lock ogg");
                            exit(1);
                        }
                        numero_oggetti_trovati++;
                        /* LOGGING */
                        snprintf(temp,sizeof temp, "\nTid: %d --- Numero oggetti trovati: %d --- Oggetti rimanenti: %d.",(int)pthread_self(),numero_oggetti_trovati,numero_oggetti_nella_mappa);
                        if( write(fd_log,temp,strlen(temp)) < 0 )
                        {
                            perror("Errore write in funzione_principale::1.");
                            exit(1);
                        }
                        if( pthread_mutex_unlock(&lock_oggetti_trovati) < 0 )
                        {
                            perror("Errore unlock ogg");
                            exit(1);
                        }
                        if( pthread_mutex_lock(&lock_lista_utenti) < 0 )
                        {
                            perror("Errore lock lista utenti");
                            exit(1);
                        }
                        InserisciOggettoUtente(ListaUtenti, nickname, appoggio);
                        if( pthread_mutex_unlock(&lock_lista_utenti) < 0 )
                        {
                            perror("Errore unlock lista utenti");
                            exit(1);
                        }
                        /* LOGGING */
                        snprintf(temp,sizeof temp, "\nTid: %d --- Utente %s spostato in posizione %c%c --- Trovato oggetto: %s.",(int)pthread_self(),nickname,(*pos_riga_giocatore+65),(*pos_colonna_giocatore+65),appoggio);
                        if( write(fd_log,temp,strlen(temp)) < 0 )
                        {
                            perror("Errore write in funzione_principale::1.");
                            exit(1);
                        }
                        /* Controllo oggetti */
                        if( read(fd2,&var_tmp_finzi,1) < 0 && var_tmp_finzi != 'o' )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Lettura conferma OGGETTI TERMINATI.");
                            exit(1);
                        }

                        if( numero_oggetti_nella_mappa == 0 )
                        {
                            /*
                                TERMINA PARTITA, RICOMINCIA IL TUTTO CREANDO UNA NUOVA MAPPA
                                INVIAMO UN CONTROLLO AL CLIENT
                            */
                            var_tmp_finzi = 'f';
                            if( pthread_mutex_lock(&lock_flag_partita) < 0 )
                            {
                                perror("Errore lock flag partita");
                                exit(1);
                            }
                            flag_partita = 1;
                            if( pthread_mutex_unlock(&lock_flag_partita) < 0 )
                            {
                                perror("Errore unlock flag partita");
                                exit(1);
                            }
                        }
                        else
                        {
                            var_tmp_finzi = 'o';
                        }
                        if( write(fd2,&var_tmp_finzi,sizeof(var_tmp_finzi)) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura conferma OGGETTI.");
                            exit(1);
                        }
                    }
                    else
                    {
                        if( write(fd2,"o",1) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'o'.");
                            exit(1);
                        }
                        /* LOGGING */
                        snprintf(temp,sizeof temp, "\nTid: %d --- Utente %s spostato in posizione %c%c.",(int)pthread_self(),nickname,(*pos_riga_giocatore+65),(*pos_colonna_giocatore+65));
                        if( write(fd_log,temp,strlen(temp)) < 0 )
                        {
                            perror("Errore write in funzione_principale::1.");
                            exit(1);
                        }
                    }
                }
                else /* NON PUO' ANDARE QUINDI MANDO ERRORE */
                {
                    if( matrice[*pos_riga_giocatore][*pos_colonna_giocatore-1].numero_pid == -1 )
                    {   /*
                            ALLORA C'E' UN MURO
                            Mandiamo l'errore al client
                        */
                        if( write(fd2,"e" ,1) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'e'.");
                            exit(1);
                        }
                    }
                    else
                    {   /*
                            C'È UN GIOCATORE
                            Mandiamo l'errore al client
                        */
                        if( write(fd2,"j" ,1) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'e'.");
                            exit(1);
                        }
                    }

                    /* LOGGING */
                    snprintf(temp,sizeof temp, "\nTid: %d --- Utente: %s --- Tentato spostamento non valido.",(int)pthread_self(),nickname);
                    if( write(fd_log,temp,strlen(temp)) < 0 )
                    {
                        perror("Errore write in funzione_principale::1.");
                        exit(1);
                    }
                }
                if( pthread_mutex_unlock(&sem) < 0 )
                {
                    perror("\nErrore mutex unlock caso A 1!");
                    exit(1);
                }
            }
            else /* NON PUO' ANDARE QUINDI MANDO ERRORE */
            {
                /* Mandiamo l'errore al client */
                if( write(fd2,"e" ,1) < 0 )
                {
                    perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'e'.");
                    exit(1);
                }
                /* LOGGING */
                snprintf(temp,sizeof temp, "\nTid: %d --- Utente: %s --- Tentato spostamento non valido.",(int)pthread_self(),nickname);
                if( write(fd_log,temp,strlen(temp)) < 0 )
                {
                    perror("Errore write in funzione_principale::1.");
                    exit(1);
                }
            }
            break;
        case 'K': /* Spostamento sopra */
            if( (*pos_riga_giocatore-1) >= 0)
            {
                if( pthread_mutex_lock(&sem) < 0 )
                {
                    perror("\nErrore mutex lock caso K 1!");
                    exit(1);
                }
                /* Controlliamo che la casella non sia un muro e che non sia già occupata da un giocatore */
                if(matrice[*pos_riga_giocatore-1][*pos_colonna_giocatore].flag_muro == 0 && matrice[*pos_riga_giocatore-1][*pos_colonna_giocatore].numero_pid == -1)
                {
                    matrice[*pos_riga_giocatore-1][*pos_colonna_giocatore].numero_pid = matrice[*pos_riga_giocatore][*pos_colonna_giocatore].numero_pid;
                    matrice[*pos_riga_giocatore][*pos_colonna_giocatore].numero_pid = -1;
                    *pos_riga_giocatore = *pos_riga_giocatore - 1;
                    /* Mandiamo conferma al client */
                    if( matrice[*pos_riga_giocatore][*pos_colonna_giocatore].indice_ogg > 0 )
                    {
                        numero_oggetti_nella_mappa--;
                        lista_oggetti(matrice[*pos_riga_giocatore][*pos_colonna_giocatore].indice_ogg,appoggio);
                        if( write(fd2,"g",1) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'o'.");
                            exit(1);
                        }
                        if( write(fd2,appoggio,sizeof(appoggio)) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'o'.");
                            exit(1);
                        }
                        matrice[*pos_riga_giocatore][*pos_colonna_giocatore].indice_ogg = 0;
                        if( pthread_mutex_lock(&lock_oggetti_trovati) < 0 )
                        {
                            perror("Errore lock ogg");
                            exit(1);
                        }
                        numero_oggetti_trovati++;
                        /* LOGGING */
                        snprintf(temp,sizeof temp, "\nTid: %d --- Numero oggetti trovati: %d --- Oggetti rimanenti: %d.",(int)pthread_self(),numero_oggetti_trovati,numero_oggetti_nella_mappa);
                        if( write(fd_log,temp,strlen(temp)) < 0 )
                        {
                            perror("Errore write in funzione_principale::1.");
                            exit(1);
                        }
                        if( pthread_mutex_unlock(&lock_oggetti_trovati) < 0 )
                        {
                            perror("Errore unlock ogg");
                            exit(1);
                        }
                        if( pthread_mutex_lock(&lock_lista_utenti) < 0 )
                        {
                            perror("Errore lock lista utenti");
                            exit(1);
                        }
                        InserisciOggettoUtente(ListaUtenti, nickname, appoggio);
                        if( pthread_mutex_unlock(&lock_lista_utenti) < 0 )
                        {
                            perror("Errore unlock lista utenti");
                            exit(1);
                        }
                        /* LOGGING */
                        snprintf(temp,sizeof temp, "\nTid: %d --- Utente %s spostato in posizione %c%c --- Trovato oggetto: %s.",(int)pthread_self(),nickname,(*pos_riga_giocatore+65),(*pos_colonna_giocatore+65),appoggio);
                        if( write(fd_log,temp,strlen(temp)) < 0 )
                        {
                            perror("Errore write in funzione_principale::1.");
                            exit(1);
                        }
                        /* Controllo oggetti */
                        if( read(fd2,&var_tmp_finzi,1) < 0 && var_tmp_finzi != 'o' )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Lettura conferma OGGETTI TERMINATI.");
                            exit(1);
                        }

                        if( numero_oggetti_nella_mappa == 0 )
                        {
                            /*
                                TERMINA PARTITA, RICOMINCIA IL TUTTO CREANDO UNA NUOVA MAPPA
                                INVIAMO UN CONTROLLO AL CLIENT
                            */
                            var_tmp_finzi = 'f';
                            if( pthread_mutex_lock(&lock_flag_partita) < 0 )
                            {
                                perror("Errore lock flag partita");
                                exit(1);
                            }
                            flag_partita = 1;
                            if( pthread_mutex_unlock(&lock_flag_partita) < 0 )
                            {
                                perror("Errore unlock flag partita");
                                exit(1);
                            }
                        }
                        else
                        {
                            var_tmp_finzi = 'o';
                        }
                        if( write(fd2,&var_tmp_finzi,sizeof(var_tmp_finzi)) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura conferma OGGETTI.");
                            exit(1);
                        }
                    }
                    else
                    {
                        if( write(fd2,"o",1) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'o'.");
                            exit(1);
                        }

                        /* LOGGING */
                        snprintf(temp,sizeof temp, "\nTid: %d --- Utente %s spostato in posizione %c%c.",(int)pthread_self(),nickname,(*pos_riga_giocatore+65),(*pos_colonna_giocatore+65));
                        if( write(fd_log,temp,strlen(temp)) < 0 )
                        {
                            perror("Errore write in funzione_principale::1.");
                            exit(1);
                        }
                    }
                }
                else /* NON PUO' ANDARE QUINDI MANDO ERRORE */
                {
                    if( matrice[*pos_riga_giocatore-1][*pos_colonna_giocatore].numero_pid == -1 )
                    {   /*
                            ALLORA C'E' UN MURO
                            Mandiamo l'errore al client
                        */
                        if( write(fd2,"e" ,1) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'e'.");
                            exit(1);
                        }
                    }
                    else
                    {   /*
                            C'È UN GIOCATORE
                            Mandiamo l'errore al client
                        */
                        if( write(fd2,"j" ,1) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'e'.");
                            exit(1);
                        }
                    }
                    /* LOGGING */
                    snprintf(temp,sizeof temp, "\nTid: %d --- Utente: %s --- Tentato spostamento non valido.",(int)pthread_self(),nickname);
                    if( write(fd_log,temp,strlen(temp)) < 0 )
                    {
                        perror("Errore write in funzione_principale::1.");
                        exit(1);
                    }
                }
                if( pthread_mutex_unlock(&sem) < 0 )
                {
                    perror("\nErrore mutex unlock caso K 1!");
                    exit(1);
                }
            }
            else /* NON PUO' ANDARE QUINDI MANDO ERRORE */
            {
                /* Mandiamo l'errore al client */
                if( write(fd2,"e" ,1) < 0 )
                {
                    perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'e'.");
                    exit(1);
                }

                /* LOGGING */
                snprintf(temp,sizeof temp, "\nTid: %d --- Utente: %s --- Tentato spostamento non valido.",(int)pthread_self(),nickname);
                if( write(fd_log,temp,strlen(temp)) < 0 )
                {
                    perror("Errore write in funzione_principale::1.");
                    exit(1);
                }
            }
            break;
        case 'Y': /* Spostamento sopra */
            if( (*pos_riga_giocatore+1) < misura_matrice )
            {
                if( pthread_mutex_lock(&sem) < 0 )
                {
                    perror("\nErrore mutex lock caso Y 1!");
                    exit(1);
                }
                /* Controlliamo che la casella non sia un muro e che non sia già occupata da un giocatore */
                if(matrice[*pos_riga_giocatore+1][*pos_colonna_giocatore].flag_muro == 0 && matrice[*pos_riga_giocatore+1][*pos_colonna_giocatore].numero_pid == -1)
                {
                    matrice[*pos_riga_giocatore+1][*pos_colonna_giocatore].numero_pid = matrice[*pos_riga_giocatore][*pos_colonna_giocatore].numero_pid;
                    matrice[*pos_riga_giocatore][*pos_colonna_giocatore].numero_pid = -1;
                    *pos_riga_giocatore = *pos_riga_giocatore + 1;
                    /* Mandiamo conferma al client */
                    if( matrice[*pos_riga_giocatore][*pos_colonna_giocatore].indice_ogg > 0 )
                    {
                        numero_oggetti_nella_mappa--;
                        lista_oggetti(matrice[*pos_riga_giocatore][*pos_colonna_giocatore].indice_ogg,appoggio);
                        if( write(fd2,"g",1) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'o'.");
                            exit(1);
                        }
                        if( write(fd2,appoggio,sizeof(appoggio)) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'o'.");
                            exit(1);
                        }
                        matrice[*pos_riga_giocatore][*pos_colonna_giocatore].indice_ogg = 0;
                        if( pthread_mutex_lock(&lock_oggetti_trovati) < 0 )
                        {
                            perror("Errore lock ogg");
                            exit(1);
                        }
                        numero_oggetti_trovati++;
                        /* LOGGING */
                        snprintf(temp,sizeof temp, "\nTid: %d --- Numero oggetti trovati: %d --- Oggetti rimanenti: %d.",(int)pthread_self(),numero_oggetti_trovati,numero_oggetti_nella_mappa);
                        if( write(fd_log,temp,strlen(temp)) < 0 )
                        {
                            perror("Errore write in funzione_principale::1.");
                            exit(1);
                        }
                        if( pthread_mutex_unlock(&lock_oggetti_trovati) < 0 )
                        {
                            perror("Errore unlock ogg");
                            exit(1);
                        }
                        if( pthread_mutex_lock(&lock_lista_utenti) < 0 )
                        {
                            perror("Errore lock lista utenti");
                            exit(1);
                        }
                        InserisciOggettoUtente(ListaUtenti, nickname, appoggio);
                        if( pthread_mutex_unlock(&lock_lista_utenti) < 0 )
                        {
                            perror("Errore unlock lista utenti");
                            exit(1);
                        }
                        /* LOGGING */
                        snprintf(temp,sizeof temp, "\nTid: %d --- Utente %s spostato in posizione %c%c --- Trovato oggetto: %s.",(int)pthread_self(),nickname,(*pos_riga_giocatore+65),(*pos_colonna_giocatore+65),appoggio);
                        if( write(fd_log,temp,strlen(temp)) < 0 )
                        {
                            perror("Errore write in funzione_principale::1.");
                            exit(1);
                        }
                        //Controllo oggetti
                        if( read(fd2,&var_tmp_finzi,1) < 0 && var_tmp_finzi != 'o' ) //Leggo la riga
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Lettura conferma OGGETTI TERMINATI.");
                            exit(1);
                        }

                        if( numero_oggetti_nella_mappa == 0 )
                        {
                            /*
                                TERMINA PARTITA, RICOMINCIA IL TUTTO CREANDO UNA NUOVA MAPPA
                                INVIAMO UN CONTROLLO AL CLIENT
                            */
                            var_tmp_finzi = 'f';
                            if( pthread_mutex_lock(&lock_flag_partita) < 0 )
                            {
                                perror("Errore lock flag partita");
                                exit(1);
                            }
                            flag_partita = 1;
                            if( pthread_mutex_unlock(&lock_flag_partita) < 0 )
                            {
                                perror("Errore unlock flag partita");
                                exit(1);
                            }
                        }
                        else
                        {
                            var_tmp_finzi = 'o';
                        }
                        if( write(fd2,&var_tmp_finzi,sizeof(var_tmp_finzi)) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura conferma OGGETTI.");
                            exit(1);
                        }
                    }
                    else
                    {
                        if( write(fd2,"o",1) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'o'.");
                            exit(1);
                        }

                        /* LOGGING */
                        snprintf(temp,sizeof temp, "\nTid: %d --- Utente %s spostato in posizione %c%c.",(int)pthread_self(),nickname,(*pos_riga_giocatore+65),(*pos_colonna_giocatore+65));
                        if( write(fd_log,temp,strlen(temp)) < 0 )
                        {
                            perror("Errore write in funzione_principale::1.");
                            exit(1);
                        }
                    }
                }
                else /* NON PUO' ANDARE QUINDI MANDO ERRORE */
                {
                    if( matrice[*pos_riga_giocatore+1][*pos_colonna_giocatore].numero_pid == -1 )
                    {   /*
                            ALLORA C'E' UN MURO
                            Mandiamo l'errore al client
                        */
                        if( write(fd2,"e" ,1) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'e'.");
                            exit(1);
                        }
                    }
                    else
                    {   /*
                            C'È UN GIOCATORE
                            Mandiamo l'errore al client
                        */
                        if( write(fd2,"j" ,1) < 0 )
                        {
                            perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'e'.");
                            exit(1);
                        }
                    }

                    /* LOGGING */
                    snprintf(temp,sizeof temp, "\nTid: %d --- Utente: %s --- Tentato spostamento non valido.",(int)pthread_self(),nickname);
                    if( write(fd_log,temp,strlen(temp)) < 0 )
                    {
                        perror("Errore write in funzione_principale::1.");
                        exit(1);
                    }
                }
                if( pthread_mutex_unlock(&sem) < 0 )
                {
                    perror("\nErrore mutex unlock caso Y 1!");
                    exit(1);
                }
            }
            else /* NON PUO' ANDARE QUINDI MANDO ERRORE */
            {
                /* Mandiamo l'errore al client */
                if( write(fd2,"e" ,1) < 0 )
                {
                    perror("Errore read in li_vuoi_quei_kiwi::Scrittura della 'e'.");
                    exit(1);
                }

                /* LOGGING */
                snprintf(temp,sizeof temp, "\nTid: %d --- Utente: %s --- Tentato spostamento non valido.",(int)pthread_self(),nickname);
                if( write(fd_log,temp,strlen(temp)) < 0 )
                {
                    perror("Errore write in funzione_principale::1.");
                    exit(1);
                }
            }
            break;
    }
    if( pthread_mutex_unlock(&lock_misura_matrice) < 0 )
    {
        perror("Errore unlock");
        exit(1);
    }
    return;
}

/* Funzione per il nome degli oggetti */
void lista_oggetti(int indice, char Oggetto[])
{
     switch(indice)
        {
            case 1:
                strcpy(Oggetto,"Pokeball");
                break;
             case 2:
                strcpy(Oggetto,"Pillola blu");
                break;
             case 3:
                strcpy(Oggetto,"Progetti di Architettura");
                break;
             case 4:
                strcpy(Oggetto,"Progetto di LSO");
                break;
             case 5:
                strcpy(Oggetto,"Un Bug");
                break;
             case 6:
                strcpy(Oggetto,"Esame di ASD");
                break;
             case 7:
                strcpy(Oggetto,"Laurea triennale");
                break;
             case 8:
                strcpy(Oggetto,"Un Lavoro");
                break;
             case 9:
                strcpy(Oggetto,"Un Bambino");
                break;
             case 10:
                strcpy(Oggetto,"Il mistero dell'esame perduto");
                break;
             case 11:
                strcpy(Oggetto,"Pikachu");
                break;
             case 12:
                strcpy(Oggetto,"5000 euro");
                break;
            case 13:
                strcpy(Oggetto,"Wally");
                break;
            case 14:
                strcpy(Oggetto,"Il sorgente di Matrix");
                break;
            case 15:
                strcpy(Oggetto,"Rivista ose'");
                break;
            case 16:
                strcpy(Oggetto,"Steve Jobs");
                break;
            case 17:
                strcpy(Oggetto,"L'oscar di DiCaprio");
                break;
            case 18:
                strcpy(Oggetto,"Dentiera della nonna");
                break;
            case 19:
                strcpy(Oggetto,"Cloroformio");
                break;
            case 20:
                strcpy(Oggetto,"Balzano ballerino");
                break;
            case 21:
                strcpy(Oggetto,"Chewbecca");
                break;
            case 22:
                strcpy(Oggetto,"Il lato oscuro della forza");
                break;
            case 23:
                strcpy(Oggetto,"Lettera minatoria");
                break;
            case 24:
                strcpy(Oggetto,"Felpa");
                break;
            case 25:
                strcpy(Oggetto,"Mutande con cuori");
                break;
            case 26:
                strcpy(Oggetto,"Biscotto al cioccolato");
                break;
            case 27:
                strcpy(Oggetto,"Salame affumicato");
                break;
            case 28:
                strcpy(Oggetto,"Calzino sporco");
                break;
            case 29:
                strcpy(Oggetto,"Il mio teSSoro");
                break;
            case 30:
                strcpy(Oggetto,"Gandhi con un guantone da boxe");
                break;
            case 31:
                strcpy(Oggetto,"L'albero azzurro");
                break;
            case 32:
                strcpy(Oggetto,"Un Hobbit");
                break;
            case 33:
                strcpy(Oggetto,"Spada +10 Critico");
                break;
            case 34:
                strcpy(Oggetto,"Batman");
                break;
            case 35:
                strcpy(Oggetto,"Oggetto mistico");
                break;
            case 36:
                strcpy(Oggetto,"Gnomo");
                break;
            case 37:
                strcpy(Oggetto,"Scivolizia");
                break;
            case 38:
                strcpy(Oggetto,"Tonio Cartonio");
                break;
            case 39:
                strcpy(Oggetto,"Lupo Lucio");
                break;
            case 40:
                strcpy(Oggetto,"L'arma del delitto");
                break;
            case 41:
                strcpy(Oggetto,"Backdoor");
                break;
            case 42:
                strcpy(Oggetto,"Cavallo di Troia");
                break;
            case 43:
                strcpy(Oggetto,"I sopravvisuti di lost");
                break;
            case 44:
                strcpy(Oggetto,"Un feto");
                break;
            case 45:
                strcpy(Oggetto,"Topo Gigio");
                break;
            case 46:
                strcpy(Oggetto,"Scudo incantato");
                break;
            case 47:
                strcpy(Oggetto,"Patata bollita");
                break;
            case 48:
                strcpy(Oggetto,"Katana Puttana");
                break;
            case 49:
                strcpy(Oggetto,"Bin Laden");
                break;
            case 50:
                strcpy(Oggetto,"Un croccantino");
                break;
        }
    return;
}

/* Funzione per l'inserimento di un utente appena loggato */
utenti *InserisciUtente(utenti* top, pthread_t tid, char nickname[], int flag)
{
	if( top == NULL )
	{
        top = (utenti *)malloc(sizeof(utenti));
		top->numero_tid = tid;
		strcpy(top->nickname,nickname);
		top->in_gioco = flag;
		top->numero_oggetti_trovati = 0;
	    top->oggetti_trovati = NULL;
	    top->dx = NULL;
	    top->sx = NULL;

        /* LOGGING */
        snprintf(temp,sizeof temp, "\nTid: %d --- Utente: %s --- Inserito in lista utenti collegati.",(int)pthread_self(),nickname);
        if( write(fd_log,temp,strlen(temp)) < 0 )
        {
            perror("Errore write in InserisciUtente.");
            exit(1);
        }

	    return top;
	}
	else
	{
		if( strcmp(top->nickname,nickname) > 0 )
		{
			top->sx = InserisciUtente(top->sx, tid, nickname, flag);
		}
		else
		{
            if( strcmp(top->nickname,nickname) < 0 )
            {
                top->dx = InserisciUtente(top->dx, tid, nickname, flag);
            }
            else
            {
                top->in_gioco = 1;
                /* LOGGING */
                snprintf(temp,sizeof temp, "\nTid: %d --- Utente: %s --- Inserito in lista utenti in gioco.",(int)pthread_self(),nickname);
                if( write(fd_log,temp,strlen(temp)) < 0 )
                {
                    perror("Errore write in InserisciUtente.");
                    exit(1);
                }
            }

		}
	}
	return top;
}

/* Invia lista utenti al client -- Server e client si scambiano una serie di messaggi */
void StampaListaUtenti(utenti* top,int fd)
{
    if( top != NULL )
    {
        StampaListaUtenti(top->sx,fd);
        char tmp = ' ';
        if( write(fd,"o",1) < 0 ) /* Scrivo il carattere di INIZIO nel file descriptor */
        {
            perror("Errore read in funzione_principale::ciclo while::caso B::Scrittura del carattere di inizio.");
            exit(1);
        }
        if( read(fd,&tmp,1) < 0 )
        {
            perror("Errore read in StampaListaUtenti::Lettura carattere di conferma.");
            exit(1);
        }
        if( tmp != 'o')
        {
            perror("Errore read in StampaListaUtenti::Carattere di conferma errato.");
            exit(1);
        }
        if( write(fd,top->nickname,strlen(top->nickname)) < 0 ) /* Scrivo il nickname nel file descriptor */
        {
            perror("Errore read in StampaListaUtenti::Scrittura del nickname.");
            exit(1);
        }
        if( read(fd,&tmp,1) < 0 )
        {
            perror("Errore read in StampaListaUtenti::Lettura carattere di conferma.");
            exit(1);
        }
        if( tmp != 'o')
        {
            perror("Errore read in StampaListaUtenti::Carattere di conferma errato.");
            exit(1);
        }
        char buf[MAXSIZE];
        snprintf(buf, sizeof buf, "%d", top->numero_oggetti_trovati);
        if( write(fd,buf,strlen(buf)) < 0 )
        {
            perror("Errore read in StampaListaUtenti::Scrittura del numero di oggetti trovati.");
            exit(1);
        }
        //LEGGO UN CARATTERE DI CONFERMA
        if( read(fd,&tmp,1) < 0 )
        {
            perror("Errore read in StampaListaUtenti::Lettura carattere di conferma.");
            exit(1);
        }
        if( tmp != 'o')
        {
            perror("Errore read in StampaListaUtenti::Carattere di conferma errato.");
            exit(1);
        }
        else
        {

        }
        StampaListaUtenti(top->dx,fd);
    }
}

/* Invia la lista di utenti collegati ma in gioco al client -- Stesso funzionamento della funzione precedente */
void StampaListaUtenti2(utenti* top,int fd)
{
    if( top != NULL )
    {
        StampaListaUtenti2(top->sx,fd);
        if( top->in_gioco != 0 )
        {
            char tmp = ' ';
            if( write(fd,"o",1) < 0 ) /* Scrivo il carattere di INIZIO nel file descriptor */
            {
                perror("Errore read in funzione_principale::ciclo while::caso B::Scrittura del carattere di inizio.");
                exit(1);
            }
            if( read(fd,&tmp,1) < 0 && tmp != 'o' )
            {
                perror("Errore read in StampaListaUtenti::Lettura carattere di conferma.");
                exit(1);
            }
            if( write(fd,top->nickname,strlen(top->nickname)) < 0 ) /* Scrivo il nickname nel file descriptor */
            {
                perror("Errore read in StampaListaUtenti::Scrittura del nickname.");
                exit(1);
            }
            if( read(fd,&tmp,1) < 0 && tmp != 'o' )
            {
                perror("Errore read in StampaListaUtenti::Lettura carattere di conferma.");
                exit(1);
            }
            oggetti*tmp2=NULL;
            if( top->oggetti_trovati != NULL ) /* CASO IN CUI L'UTENTE È IN GIOCO MA NON HA TROVATO OGGETTI, QUINDI LA LISTA È VUOTA */
            {
                if( write(fd,"o",1) < 0 ) /* Scrivo il carattere di INIZIO nel file descriptor */
                {
                    perror("Errore read in funzione_principale::ciclo while::caso L::errore invio");
                    exit(1);
                }
                for(tmp2=top->oggetti_trovati;tmp2!=NULL;tmp2=tmp2->next)
                {
                    if( write(fd,tmp2->nome_oggetto,strlen(tmp2->nome_oggetto)) < 0 ) /* Scrivo il nickname nel file descriptor */
                    {
                        perror("Errore read in StampaListaUtenti::scrittura oggetto");
                        exit(1);
                    }

                    if( read(fd,&tmp,1) < 0 && tmp != 'o' )
                    {
                        perror("Errore read in StampaListaUtenti::Lettura carattere di conferma.");
                        exit(1);
                    }
                    if(tmp2->next != NULL)
                    {
                        if( write(fd,"o",1) < 0 ) /* Scrivo il carattere di INIZIO nel file descriptor */
                        {
                            perror("Errore read in funzione_principale::ciclo while::caso L::errore invio");
                            exit(1);
                        }
                    }
                }
            }
            if( write(fd,"q",1) < 0 ) /* Scrivo il carattere di terminazione q */
            {
                perror("Errore read in funzione_principale::ciclo while::caso errore carattere fine oggetti");
                exit(1);
            }
        }
        StampaListaUtenti2(top->dx,fd);
    }
}

/* Invia il numero di oggetti rimanenti al client */
void NumeroOggRimanenti(int fd)
{
    char buf[MAXSIZE];
    snprintf(buf,sizeof buf,"%d",numero_oggetti_nella_mappa);
    if( write(fd,buf,strlen(buf)) < 0 )
    {
        perror("Errore read in NumeroOggTrovati::Scrittura numero di oggetti rimanenti.");
        exit(1);
    }
}

/* Invia il numero di oggetti trovati al client */
void NumeroOggTrovati(int fd)
{
    char buf[MAXSIZE];
    if( pthread_mutex_lock(&lock_oggetti_trovati) < 0 )
    {
        perror("Errore lock ogg");
        exit(1);
    }
    snprintf(buf,sizeof buf,"%d",numero_oggetti_trovati);
    if( pthread_mutex_unlock(&lock_oggetti_trovati) < 0 )
    {
        perror("Errore unlock ogg");
        exit(1);
    }
    if( write(fd,buf,strlen(buf)) < 0 )
    {
        perror("Errore read in NumeroOggTrovati::Scrittura numero di oggetti rimanenti.");
        exit(1);
    }
}

/* Inserisce l'utente nella mappa chiedendo la posizione di entrata */
void Gioca(int fd2, char username[], int *pos_riga_giocatore, int *pos_colonna_giocatore)
{
    int i=0, k=0, flag_oggetto_trovato = 0;
    char tmp = ' ', tmp_2 =' ', nome_oggetto[MAXSIZE];
    if( utenti_in_gioco < utenti_max ) /* controllo che non ci siano il numero max di utenti in gioco */
    {
        if( write(fd2,"o",1) < 0 ) /* Scrivo ok */
        {
            perror("Errore read in funzione_principale::ciclo while::caso A::Scrittura dell'ok.");
            exit(1);
        }
        if( pthread_mutex_lock(&lock_utenti_in_gioco) < 0 )
        {
            perror("Errore lock ogg");
            exit(1);
        }
        utenti_in_gioco++;
        if( pthread_mutex_unlock(&lock_utenti_in_gioco) < 0 )
        {
            perror("Errore unlock ogg");
            exit(1);
        }
        /* Invio della mappa al client */
        InvioMappaClient(fd2, pos_riga_giocatore, pos_colonna_giocatore);
        int rig = 0, col=0;
        do{
            if( read(fd2,&tmp,1) < 0 )
            {
                perror("Errore read in funzione_principale::ciclo while::Lettura riga.");
                exit(1);
            }

            if( read(fd2,&tmp_2,1) < 0 )
            {
                perror("Errore read in funzione_principale::ciclo while::Lettura colonna.");
                exit(1);
            }
            rig = (int) tmp;
            rig = rig - 65;
            col = (int) tmp_2;
            col = col - 65;
            /* Controllo che i dati ricevuti siano corretti */
            if( rig < misura_matrice && col < misura_matrice && rig >= 0 && col >= 0 )
            {
                if( pthread_mutex_lock(&sem) < 0 )
                {
                    perror("Mutex Lock error in Gioca 1");
                    exit(1);
                }
                /* CONTROLLO CHE LA CASELLA NON SIA UN MURO */
                if( matrice[rig][col].flag_muro == 1 || matrice[rig][col].numero_pid != -1 ) /* SE E' UN OGGETTO, ALLORA SE LO PRENDE E BASTA. LA CASELLA COMUNQUE E' VALIDA */
                {
                    if( write(fd2,"e" ,1) < 0 )
                    {
                        perror("Errore read in funzione_principale::ciclo while::caso A::Scrittura della 'e'.");
                        exit(1);
                    }
                }
                else
                {
                    if( matrice[rig][col].indice_ogg > 0 )
                    {
                        if( write(fd2,"g" ,1) < 0 ) /* SCRIVO il carattere per l'oggetto */
                        {
                            perror("Errore read in funzione_principale::ciclo while::caso A::Scrittura della 'g'.");
                            exit(1);
                        }

                        /* Inserisco il nome dell'oggetto nell'array nome_oggetto */
                        lista_oggetti(matrice[rig][col].indice_ogg,nome_oggetto);

                        if( write(fd2,nome_oggetto,strlen(nome_oggetto)) < 0 )
                        {
                            perror("Errore read in funzione_principale::ciclo while::caso A::Scrittura della 'g'.");
                            exit(1);
                        }
                        flag_oggetto_trovato = 1;
                        numero_oggetti_nella_mappa--;
                        matrice[rig][col].indice_ogg = 0;
                        if( pthread_mutex_lock(&lock_oggetti_trovati) < 0 )
                        {
                            perror("Errore lock ogg");
                            exit(1);
                        }
                        numero_oggetti_trovati++;
                        if( pthread_mutex_unlock(&lock_oggetti_trovati) < 0 )
                        {
                            perror("Errore unlock ogg");
                            exit(1);
                        }
                        InserisciOggettoUtente(ListaUtenti, username, nome_oggetto);
                    }
                    else
                    {
                        if( write(fd2,"o" ,1) < 0 )
                        {
                            perror("Errore read in funzione_principale::ciclo while::caso A::Scrittura della 'o'.");
                            exit(1);
                        }
                    }
                    /* AVVIAMO IL TEMPO */
                    if( pthread_mutex_lock(&lock_tempo) < 0 )
                    {
                        perror("Errore lock tempo");
                        exit(1);
                    }
                    if( flag_tempo == 0 )
                    {
                        tempo = (unsigned)time(NULL);
                        alarm(15);
                        flag_tempo = 1;
                    }
                    if( pthread_mutex_unlock(&lock_tempo) < 0 )
                    {
                        perror("Errore unlock tempo");
                        exit(1);
                    }
                }
                if( pthread_mutex_unlock(&sem) < 0 )
                {
                    perror("Mutex unLock error in Gioca 1");
                    exit(1);
                }
            }
            else
            {
                if( write(fd2,"e" ,1) < 0 )
                {
                    perror("Errore read in funzione_principale::ciclo while::caso A::Scrittura della 'e'.");
                    exit(1);
                }
            }
        }while( rig >= misura_matrice || col >= misura_matrice || matrice[rig][col].flag_muro == 1 || matrice[rig][col].numero_pid != -1 );
        if( pthread_mutex_lock(&lock_lista_utenti) < 0 )
        {
            perror("Errore lock lista utenti");
            exit(1);
        }
        ListaUtenti = InserisciUtente(ListaUtenti, pthread_self(), username, 1);
        if( pthread_mutex_unlock(&lock_lista_utenti) < 0 )
        {
            perror("Errore unlock lista utenti");
            exit(1);
        }
        if( pthread_mutex_lock(&sem) < 0 )
        {
            perror("Mutex Lock error in Gioca 2");
            exit(1);
        }
        matrice[rig][col].numero_pid = pthread_self();
        if( pthread_mutex_unlock(&sem) < 0 )
        {
            perror("Mutex unLock error in Gioca 2");
            exit(1);
        }
        *pos_colonna_giocatore = col;
        *pos_riga_giocatore = rig;
        /* LOGGING */
        if( flag_oggetto_trovato == 0 )
        {
            snprintf(temp,sizeof temp, "\nTid: %d --- Utente: %s --- Posizione respawn: %c%c.",(int)pthread_self(),username,tmp,tmp_2);
            if( write(fd_log,temp,strlen(temp)) < 0 )
            {
                perror("Errore write in Gioca.");
                exit(1);
            }
        }
        else
        {
            snprintf(temp,sizeof temp, "\nTid: %d --- Utente: %s --- Posizione respawn: %c%c --- Oggetto trovato: %s.",(int)pthread_self(),username,tmp,tmp_2,nome_oggetto);
            if( write(fd_log,temp,strlen(temp)) < 0 )
            {
                perror("Errore write in Gioca.");
                exit(1);
            }
        }
    }
    else
    {
        if( write(fd2,"e",1) < 0 ) //Scrivo e
        {
            perror("Errore read in funzione_principale::ciclo while::caso A::Scrittura dell'e.");
            exit(1);
        }
        /* LOGGING */
        char temp[MAXSIZE];
        snprintf(temp,sizeof temp, "\nTid: %d --- Utente: %s --- Impossibile entrare nella mappa: numero massimo di utenti raggiunto: %d.",(int)pthread_self(),username,utenti_max);
        if( write(fd_log,temp,strlen(temp)) < 0 )
        {
            perror("Errore write in funzione_principale::1.");
            exit(1);
        }
    }
    return;
}
//FUNZIONE PER DEBUG
void StampaMappa()
{
    int i=0,k=0;
    printf("Anteprima della mappa:\n");
    for(i=0;i<misura_matrice;i++)
    {
        puts("");
        for(k=0;k<misura_matrice;k++)
            {
                if( matrice[i][k].flag_muro==1) // MURO
                {
                    printf("\033[01;31m|M|\033[00m");
                }
                else
                    if( matrice[i][k].indice_ogg >=0) // OGGETTO
                    {
                        if( matrice[i][k].indice_ogg == 0 ) //OGGETTO GIÀ TROVATO, NON PIÙ PRESENTE
                        {
                            printf("\x1b[34m|O|\x1b[0m"); //Colore oggetti magenta
                        }
                        else printf("\x1b[35m|O|\x1b[0m"); //Colore oggetti magenta
                    }
                    else
                    {
                        if( matrice[i][k].numero_pid != -1 )
                        {
                            printf("\x1b[32m|G|\x1b[0m"); // POSIZIONE VUOTA
                        }
                        else
                        {
                            printf("|V|"); // POSIZIONE VUOTA
                        }
                    }
            }
    }
    puts("\n\nStampa della mappa completata!!!");
    return;
}

/* Svuota la lista di oggetti di ogni utente */
oggetti *RimuoviOggettiTrovati(oggetti *top)
{
    if(top!=NULL)
    {
        top->next = RimuoviOggettiTrovati(top->next);
        free(top);
        top = NULL;
    }
    return top;
}

/* Setta la flag in gioco degli utenti a zero */
void RimuoviGiocatori_dalla_Partita(utenti* top)
{
    if(top!=NULL)
    {
        RimuoviGiocatori_dalla_Partita(top->sx);
        RimuoviGiocatori_dalla_Partita(top->dx);
        top->in_gioco = 0;
        top->numero_oggetti_trovati = 0;
        top->oggetti_trovati = RimuoviOggettiTrovati(top->oggetti_trovati);
    }
    return;
}

/* Rimuove un giocatore dalla mappa */
utenti *RimuoviGiocatore_Dalla_Partita(utenti* top, int fd2, char nickname[MAXSIZE], int *pos_riga_giocatore, int *pos_colonna_giocatore)
{
    if( top != NULL )
    {
        if( strcmp(top->nickname,nickname) > 0 )
		{
			top->sx = RimuoviGiocatore_Dalla_Partita(top->sx, fd2, nickname, pos_riga_giocatore, pos_colonna_giocatore);
		}
		else
		{
			if( strcmp(top->nickname,nickname) < 0 )
			{
                top->dx = RimuoviGiocatore_Dalla_Partita(top->dx, fd2, nickname, pos_riga_giocatore, pos_colonna_giocatore);
			}
			else /* TROVATO */
			{
                if( *pos_riga_giocatore >= 0 && *pos_colonna_giocatore>= 0 && *pos_riga_giocatore <= misura_matrice && *pos_colonna_giocatore <= misura_matrice)
                {
                    if( pthread_mutex_lock(&sem) < 0 )
                    {
                        perror("\nMutex Lock error in RImuovigiocatore dalla partita 1");
                        exit(1);
                    }
                    matrice[*pos_riga_giocatore][*pos_colonna_giocatore].numero_pid = -1;
                    if( pthread_mutex_unlock(&sem) < 0 )
                    {
                        perror("\nMutex unLock error in RImuovigiocatore dalla partita 1");
                        exit(1);
                    }
                }
                top->in_gioco = 0; /* GIOCATORE NON PIÙ IN GIOCO */
                if( pthread_mutex_lock(&lock_utenti_in_gioco) < 0 )
                {
                    perror("Errore lock ogg");
                    exit(1);
                }
                utenti_in_gioco--;
                if( pthread_mutex_unlock(&lock_utenti_in_gioco) < 0 )
                {
                    perror("Errore unlock ogg");
                    exit(1);
                }
			}
		}
    }
    return top;
}

/* Scollega un utente eliminandolo dall'ABR degli utenti */
utenti *ScollegaUtente(utenti* top, int fd2, char nickname[MAXSIZE], int *pos_riga_giocatore, int *pos_colonna_giocatore)
{
    if( top != NULL )
    {
        if( strcmp(top->nickname,nickname) > 0 )
		{
			top->sx = ScollegaUtente(top->sx, fd2, nickname, pos_riga_giocatore, pos_colonna_giocatore);
		}
		else
		{
			if( strcmp(top->nickname,nickname) < 0 )
			{
                top->dx = ScollegaUtente(top->dx, fd2, nickname, pos_riga_giocatore, pos_colonna_giocatore);
			}
			else /* TROVATO */
			{
                utenti *nodo = NULL;
                if( top->dx == NULL || top->sx == NULL )
                {
                    nodo = top;/* DEALLOCA NODO */
                    if( top->dx == NULL )
                    {
                        top = top->sx;
                    }
                    else
                    {
                        top = top->dx;
                    }
                    if( pthread_mutex_lock(&lock_misura_matrice) < 0 )
                    {
                        perror("Errore lock");
                        exit(1);
                    }
                    if( *pos_riga_giocatore >= 0 && *pos_colonna_giocatore>= 0 && *pos_riga_giocatore <= misura_matrice && *pos_colonna_giocatore <= misura_matrice)
                    {
                        matrice[*pos_riga_giocatore][*pos_colonna_giocatore].numero_pid = -1;
                    }
                    if( pthread_mutex_unlock(&lock_misura_matrice) < 0 )
                    {
                        perror("Errore unlock");
                        exit(1);
                    }
                    free(nodo);
                    nodo = NULL;
                }
                else
                {
                    nodo = StaccaMinna(top->dx,top);
                    if( pthread_mutex_lock(&lock_misura_matrice) < 0 )
                    {
                        perror("Errore lock");
                        exit(1);
                    }
                    if( *pos_riga_giocatore >= 0 && *pos_colonna_giocatore>= 0 && *pos_riga_giocatore < misura_matrice && *pos_colonna_giocatore < misura_matrice)
                    {
                        matrice[*pos_riga_giocatore][*pos_colonna_giocatore].numero_pid = -1;
                    }
                    if( pthread_mutex_unlock(&lock_misura_matrice) < 0 )
                    {
                        perror("Errore unlock");
                        exit(1);
                    }
                    strcpy(top->nickname,nodo->nickname);
                    top->numero_oggetti_trovati = nodo->numero_oggetti_trovati;
                    top->in_gioco = nodo->in_gioco;
                    top->numero_tid = nodo->numero_tid;
                    top->oggetti_trovati = nodo->oggetti_trovati;
                    free(nodo);
                    nodo = NULL;
                }
            }
		}
    }
    return top;
}

/* Stacca il Minimo dell'ABR DEGLI UTENTI */
utenti *StaccaMinna(utenti *T, utenti *P)
{
    if( T != NULL )
    {
        if( T->sx != NULL )
            T->sx = StaccaMinna(T->sx,T);
        else
        {
            if( P != NULL )
            {
                if( P->dx == T )
                {
                    P->dx = T->dx;
                }
                else P->sx = T->dx;
            }
        }
    }
    return T;
}

/* Invio mappa al client */
void InvioMappaClient(int fd2, int *pos_riga_giocatore, int *pos_colonna_giocatore)
{
    char tmp = ' ';
    int i=0, k=0;
    for(i=0;i<misura_matrice;i++)
    {
        if( write(fd2,"4",strlen("4")) < 0 ) /* SCRIVO IL carattere \n */
        {
            perror("Errore read in funzione_principale::ciclo while::caso A::Scrittura della riga della matrice.");
            exit(1);
        }
        for(k=0;k<misura_matrice;k++)
        {
            if( matrice[i][k].flag_muro == 0 )
            {
                if( ((int)matrice[i][k].numero_pid) != -1 )
                {
                    if( (int)matrice[i][k].numero_pid == (int) pthread_self() )
                    {
                        tmp = '3';
                    }
                    else tmp = '2'; /* ALLORA C'E' UN GIOCATORE NELLA CASELLA */
                }
                else
                {
                    if( matrice[i][k].indice_ogg == 0 ) tmp = '5'; /* OGGETTO TROVATO GIÀ */
                    else tmp = '0'; /* NELLA CASELLA NON C'E' NIENTE */
                }
            }
            else
            {
                tmp = '1'; /* MURO */
            }
            if( write(fd2,&tmp ,1) < 0 ) /* SCRIVO IL TIPO DI CASELLA */
            {
                perror("Errore read in funzione_principale::ciclo while::caso A::Scrittura della riga della matrice.");
                exit(1);
            }
        }
    }
    if( write(fd2,"e",1) < 0 ) /* SCRIVO il carattere di terminazione */
    {
        perror("Errore read in funzione_principale::ciclo while::caso A::Scrittura della 'e'.");
        exit(1);
    }

    /* LOGGING */
    char temp[MAXSIZE];
    snprintf(temp,sizeof temp, "\nTid: %d --- Inviata mappa al Client.",(int)pthread_self());
    if( write(fd_log,temp,strlen(temp)) < 0 )
    {
        perror("Errore write in funzione_principale::1.");
        exit(1);
    }
    return;
}

/* Scollega l'utente ricercandolo per TID */
utenti *ScollegaUtente_tid(utenti *top)
{
    if( top != NULL )
    {
        if( top->numero_tid == pthread_self() )
        {
            if( top->in_gioco != 0 )
            {
                int i, j;
                if( pthread_mutex_lock(&lock_misura_matrice) < 0 )
                {
                    perror("Errore lock");
                    exit(1);
                }
                for(i=0;i<misura_matrice;i++)
                {
                    for(j=0;j<misura_matrice;j++)
                    {
                        if( matrice[i][j].numero_pid == pthread_self() )
                        {
                            matrice[i][j].numero_pid = -1;
                        }
                    }
                }
                if( pthread_mutex_unlock(&lock_misura_matrice) < 0 )
                {
                    perror("Errore unlock");
                    exit(1);
                }
                if( pthread_mutex_lock(&lock_utenti_in_gioco) < 0 )
                {
                    perror("Errore lock ogg");
                    exit(1);
                }
                utenti_in_gioco--;
                if( pthread_mutex_unlock(&lock_utenti_in_gioco) < 0 )
                {
                    perror("Errore unlock ogg");
                    exit(1);
                }
            }
            utenti *nodo = NULL;
            if( top->dx == NULL || top->sx == NULL )
            {
                nodo = top;/* DEALLOCA NODO */
                if( top->dx == NULL )
                {
                    top = top->sx;
                }
                else
                {
                    top = top->dx;
                }
                free(nodo);
                nodo = NULL;
            }
            else
            {
                nodo = StaccaMinna(top->dx,top);
                strcpy(top->nickname,nodo->nickname);
                top->numero_oggetti_trovati = nodo->numero_oggetti_trovati;
                top->in_gioco = nodo->in_gioco;
                top->numero_tid = nodo->numero_tid;
                top->oggetti_trovati = nodo->oggetti_trovati;
                free(nodo);
                nodo = NULL;
            }
        }
        else
        {
            top->sx = ScollegaUtente_tid(top->sx);
            top->dx = ScollegaUtente_tid(top->dx);
        }

    }
    return top;
}

/* Handler dei segnali */
void segnalami_tutta(int num_segnale)
{
    char temp[MAXSIZE];
    int *fd2 = (int *)pthread_getspecific(chiave_globale_fd);
    //int *pos_riga_giocatore = (int *)pthread_getspecific(chiave_globale_pos_riga);
    //int *pos_colonna_giocatore = (int *)pthread_getspecific(chiave_globale_pos_colonna);
    if( num_segnale==SIGPIPE )
    {
        if( pthread_mutex_lock(&lock_lista_utenti) < 0 )
        {
            printf("\nErrore lock lista utenti");
            exit(1);
        }
        ListaUtenti = ScollegaUtente_tid(ListaUtenti);
        if( pthread_mutex_unlock(&lock_lista_utenti) < 0 )
        {
            printf("\nErrore unlock lista utenti");
            exit(1);
        }
        /* LOGGING */
        snprintf(temp,sizeof temp, "\nTid: %d --- Terminato il client con CTRL-C.",(int)pthread_self());
        if( write(fd_log,temp,strlen(temp)) < 0 )
        {
            printf("\nErrore write in funzione_principale::7.");
            exit(1);
        }
        printf("\nClient terminato in maniera forzata! --- tid: %d\n",(int) pthread_self());
        close(*fd2);
        pthread_exit(NULL);
    }
    if( num_segnale==SIGALRM )
    {
        if( pthread_mutex_lock(&lock_flag_partita) < 0 )
        {
            perror("Errore lock flag partita");
            exit(1);
        }
        flag_partita = 1;
        if( pthread_mutex_unlock(&lock_flag_partita) < 0 )
        {
            perror("Errore unlock flag partita");
            exit(1);
        }
        /* LOGGING */
        snprintf(temp,sizeof temp, "\nTrascorsi 15 secondi. Partita chiusa.");
        if( write(fd_log,temp,strlen(temp)) < 0 )
        {
            perror("Errore write in funzione_principale::1.");
            exit(1);
        }
    }
    if ( num_segnale==SIGINT )
    {
        /* LOGGING */
        snprintf(temp,sizeof temp, "\nTerminato Server con CTRL-C!");
        if( write(fd_log,temp,strlen(temp)) < 0 )
        {
            perror("Errore write in funzione_principale::1.");
            exit(1);
        }
        printf("\nIl server e' stato terminato con CTRL-C!\n");
        exit(1);
    }
    if ( num_segnale==SIGTSTP )
    {
        /* LOGGING */
        snprintf(temp,sizeof temp, "\nTerminato Server con CTRL-Z!");
        if( write(fd_log,temp,strlen(temp)) < 0 )
        {
            perror("Errore write in funzione_principale::1.");
            exit(1);
        }
        printf("\nIl server e' stato terminato con CTRL-Z!\n");
        exit(1);
    }
}

/* Invio tempo trascorso al client */
void Invia_tempo_Client(int fd2)
{
    char tmp[MAXSIZE];
    snprintf(tmp,sizeof tmp,"%d",(int)((unsigned)time(NULL)-tempo));
    if( write(fd2,tmp,sizeof(tmp)) < 0 )
    {
        perror("Errore WRITE IN INVIA TEMPO.");
        exit(1);
    }
}

/* Controlla che un utente sia loggato o meno */
int RicercaUtenteLoggato(utenti *top,char nickname[])
{
    if(top!=NULL)
    {
        if( strcmp(top->nickname,nickname) > 0 )
		{
			return RicercaUtenteLoggato(top->sx, nickname);
		}
		else
		{
            if( strcmp(top->nickname,nickname) < 0 )
            {
                return RicercaUtenteLoggato(top->dx, nickname);
            }
            else //trovato
            {
                return -1;
            }

		}
    }
    return 0;
}

/* Inserisce il thread in un ABR di thread */
lista_thread *Inserisci_thread(lista_thread *top, pthread_t pid)
{
    if( top == NULL )
    {
        top = (lista_thread *)malloc(sizeof(lista_thread));
        top->pid = pid;
        top->dx = NULL;
        top->sx = NULL;
    }
    else
    {
        if( (int)pid < (int)top->pid )
        {
            top->sx = Inserisci_thread(top->sx,pid);
        }
        else
        {
            top->dx = Inserisci_thread(top->dx,pid);
        }
    }
    return top;
}

/* Rimuove il thread pid dall'ABR di thread */
lista_thread *Rimuovi_thread(lista_thread *top, pthread_t pid)
{
    if(top != NULL)
    {
        if( pid < top->pid )
        {
            top->sx = Inserisci_thread(top->sx,pid);
        }
        else
        {
            if( pid > top->pid )
            {
                top->dx = Inserisci_thread(top->dx,pid);
            }
            else
            {
                lista_thread *nodo=NULL;
                if( top->dx == NULL || top->sx == NULL)
                {
                    nodo = top;
                    if(top->dx == NULL)
                    {
                        top = top->sx;
                    }
                    else top = top->dx;
                    free(nodo);
                    nodo = NULL;
                }
                else
                {
                    nodo = StaccaCappello(top->dx,top);
                    top->pid = nodo->pid;
                    free(nodo);
                    nodo = NULL;
                }
            }
        }
    }
    return top;
}

/* Stacca il Minimo dell'albero ABR di thread */
lista_thread *StaccaCappello(lista_thread *T, lista_thread *P)
{
    if(T!=NULL)
    {
        if(T->sx != NULL)
        {
            T->sx = StaccaCappello(T->sx,T);
        }
        else
        {
            if( P!=NULL )
            {
                if( P->dx==T )
                {
                    P->dx = T->dx;
                }
                else P->sx = T->dx;
            }
        }
    }
    return T;
}

/* Dealloca la mappa MAPPA */
void cancellaMappa(casella **Mappa)
{
    if(Mappa!=NULL)
    {
        int i = 0, j = 0;
        if( pthread_mutex_lock(&lock_misura_matrice) < 0 )
        {
            perror("Errore lock in Cancella Mappa");
            exit(1);
        }
        for( i=0; i < misura_matrice; i++ )
        {
            free(Mappa[i]);
            Mappa[i] = NULL;
        }
        if( pthread_mutex_unlock(&lock_misura_matrice) < 0 )
        {
            perror("Errore unlock");
            exit(1);
        }
        free(Mappa);
        Mappa = NULL;
    }
}
