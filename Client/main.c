/*


                            ╔╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╗
                            ║                                            CLIENT                                                   ║
                            ║   Autori: Simone Margio, Luca Ioffredo                                                              ║
                            ║   Data: 00/00/0000                                                                                  ║
                            ║   Descrizione: sistema client-server di gioco                                                       ║
                            ║   Ulteriori Info: https://github.com/ahw0                                                           ║
                            ║                                                                                                     ║
                            ╚╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╝



/* LIBRERIE */
#include <sys/socket.h>     /* Gestione socket */
#include <netinet/in.h>     /* Conversione indirizzi (htonl/ntohs) */
#include <stdio.h>          /* Standard input-output header */
#include <stdlib.h>         /* Exit */
#include <errno.h>          /* Perror */
#include <unistd.h>         /* Write, read, close */
#include <sys/types.h>      /* Open */
#include <sys/stat.h>       /* Open */
#include <fcntl.h>          /* Prototype delle funzioni open() e creat()... */
#include <string.h>         /* Gestione delle stringhe e delle aree di memoria */
#include <ctype.h>          /* Classificazione dei caratteri */
#include <sys/wait.h>       /* Wait(), waitpid() */
#include <arpa/inet.h>      /* Conversione indirizzi (inet)*/
#include <termios.h>        /* Per TCIFLUSH */
#include "errori.h"         /* Richiamo libreria per gestione errori */

/* DEFINE */
#define MAXSIZE 32       /* Costante usata per supporto  */
#define MAXSIZE_2 1000   /* Costante usata per supporto */


/* VATIABILI GLOBALI */
int contatore_accessi=0; /* Variabile utilizzata per contare il numero di giocatori presenti in una partita */
int fd_socket=0;         /* File descriptor per il socket */


/* PROTOTIPI DI FUNZIONI */
void segnalami_le_mele(int num_segnale);        /* Funzione per la gestione di segnali */
int CreaSocket(int porta, char* indirizzo);     /* Funzione per la creazione della socket */
void Menu();                                    /* Funzione che mostra il menu' principale per loggare o registarsi */
int Accedi(int fdsocket);                       /* Funzione relativa all'accesso di un utente */
void Registrati(int fd);                        /* Funzione relativa alla registrazione di un utente */
void menu_loggato();                            /* Funzione che mostra il menu' principale del gioco */
void funziona_principale(int fd);               /* Funzione relativa alla selezione delle opzioni disponibili nel secondo menu' di gioco */
void funzione_di_gioco(int fd);                 /* Funzione principale per inserire il giocatore all'interno della matrice di gioco */
void StampaListaUtenti(int fd);                 /* Funzione che stampa la lista degli utenti e i relativi oggetti trovati da essi. Funzione legata a 'funzione_principale' */
void StampaListaUtenti2(int fd);                /* Funzione che stampa la lista degli utenti e i relativi oggetti trovati da essi. Funzione legata al secondo menu' di gioco */
void StampaNumeroOggTrovati(int fd);            /* Funzione per la stampa del numero di oggetti TROVATI in una sessione di gioco */
void StampaNumeroOggRimanenti(int fd);          /* Funzione per la stampa del numero di oggetti RIMANENTI in una sessione di gioco */
void gioca_sub_controllo_movimenti(int fd);     /* Funzione principale per permettere al giocatore di spostarsi nella mappa */
void StampaMappa(int fd);                       /* Funzione relativa alla stampa della matrice di gioco */
void StampaTempo(int fd);                       /* Funzione relativa alla stampa del tempo trascorso in una partita */
int spostatiSopra(int fd);                      /* Funzione relativa allo spostamento dell'utente nella direzione: sopra */
int spostatiAdestra(int fd);                    /* Funzione relativa allo spostamento dell'utente nella direzione: destra */
int spostatiAsinistra(int fd);                  /* Funzione relativa allo spostamento dell'utente nella direzione: sinistra */
int spostatiGiu(int fd);                        /* Funzione relativa allo spostamento dell'utente nella direzione: sotto */
int ControlloRigaColonna(char c);               /* Funzione per verificare che l'utente abbia inserito corettamente caratteri alfabetici */


/*PROGRAMMA PRINCIPALE */
int main(int argc, char **argv)
{
    /* CONTROLLO NUMERO DI ARGOMENTI PASSATI IN INPUT */
    if (argc != 3) errori_generali(0);

    /* IMPOSTAZIONE GRANDEZZA TERMINALE PER CORETTA VISUALIZZAZIONE DI TUTTO L'AMBIENTE DI GIOCO */
    printf("\e[8;40;100t");

    /* DICHIARAZIONI VARIABILI */
    int fd = CreaSocket(atoi(argv[1]),argv[2]);
    fd_socket = fd;
    int contatore_accessi_super=0, scelta_menu = -1;

    signal(SIGPIPE,segnalami_le_mele);  /* imposto la funzione 'segnalami_le_mele' come handler del segnale SIGPIPE */
    signal(SIGINT,segnalami_le_mele);   /* imposto la funzione 'segnalami_le_mele' come handler del segnale SIGINT */
    signal(SIGTSTP,segnalami_le_mele);  /* imposto la funzione 'segnalami_le_mele' come handler del segnale SIGTSTP */

    do
    {
        system("clear");                         /* Effettuo una pulizia del terminale */
        Menu();                                  /* Chiamo la funzione per aprire il menu' principale */
        printf("Inserire valore numerico: ");
        tcflush(0, TCIFLUSH);                    /* Pulisco lo stream per evitare eventuali carrateri sporchi rimasti */
        scanf("%d",&scelta_menu);
        fflush(stdin);

        switch(scelta_menu)
        {

        /* Caso in cui il giocatore seleziona l'opzione 'Accedi' */
        case 1:
            if( write(fd,"1",1) < 0 ) errori_generali(4); /* Invio carattere '1' per l'apposita selezione lato Server. In questo caso il Server controllera' se l'username e passwors sono corretti */

            if( Accedi(fd) == 0 )        /* Una volta ritornato il valore 0 l'utente puo' loggarsi ed accedere al secondo menu' di gioco */
            {
                puts("Utente loggato!");
                funziona_principale(fd); /* Chiamo la funzione relativa alla selezione delle opzioni disponibili nel secondo menu' di gioco */
            }
            else puts("Dati login errati!");
            break;

        /* Caso in cui il giocatore seleziona l'opzione 'Registrati' */
        case 2:
            if( write(fd,"2",1) < 0 ) errori_generali(12); /* Invio carattere '2' per l'apposita selezione lato Server. In questo caso il Server controllera' se e' presente un utente con lo stesso username di quello scritto dall'utente stesso */
            Registrati(fd);                                /* Funzione relativa alla registrazione di un utente */
            break;

        /* Caso in cui il giocatore seleziona l'opzione 'Exit' */
        case 0:
            scelta_menu = 0;
            if( write(fd,"0",strlen("0")) < 0 ) errori_generali(13); /* Invio carattere '0' per l'apposita sezione lato Server. Esso sara' informato dell'uscita del relativo Client ed eseguirà la funzione 'Rimuovi_thread' */
            break;

        /* Caso in cui il giocatore seleziona una funzione non presente nel menu' */
        default:
            puts("\nErrore! Scelta non valida!\n");
        }
    }
    while(scelta_menu != 0); /* Continuo finche' non verra' scelto il caso '0' */

    system("clear"); /* Effettuo una pulizia del terminale */
    puts("\n\n\x1b[31m»-(¯`\x1b[36mthanks for playing\x1b[31m´¯)-»\x1b[0m\n");

    close(fd); /* Chiudo il file descriptor */
    return 0;  /* Programma terminato con successo */
}

/* FUNZIONI */

/* Funzione per la creazione della socket */
int CreaSocket(int porta, char* indirizzo)
{
    /* DICHIARAZIONI VARIABILI */
    int fd; /* Essa conterra' il descrittore della socket */

    /* STRUTTRA PER SPECIFICHE DI CREAZIONE SOCKET */
    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;         /* Specifico il tipo di protocolla da utilizzare */
    my_addr.sin_port = htons(porta);      /* Secifico il tipo di porta presa in input da terminale */

    if( inet_aton(indirizzo,&(my_addr.sin_addr)) == 0 ) errori_generali(1); /* Converto la stringa dell'indirizzo in un indirizzo IP */


    memset(&(my_addr.sin_zero), '\0', 8); /* Inserisco i caratteri di fine stringa negli ultimi otto posti */

    fd = socket(PF_INET, SOCK_STREAM,0);  /* Creazione della socket e assegnamento del valore di ritorno alla variabile fd */
    if( fd < 0 ) errori_generali(2);      /* Verifico che la funzione socket non abbia riportato errori */

    system("clear");                      /* Effettuo una pulizia del terminale */
    printf("\t––––•(-• \x1b[31mConnessione in corso\x1b[0m •-)•––––");
    printf("\n\t\t\x1b[36mPorta\x1b[0m: %d (\x1b[32m√\x1b[0m) \n\t\t\x1b[36mIndirizzo\x1b[0m: %s (\x1b[32m√\x1b[0m)",porta,indirizzo);
    printf("\n");
    sleep(2);

    if( connect(fd,(struct sockaddr *) &my_addr, sizeof(my_addr)) < 0 ) errori_generali(3); /* Connessione del socket locale al socket del server */

    return fd; /* Ritorno il descrittore della socket */
}

/* Funzione che mostra il menu' principale per loggare o registarsi */
void Menu()
{
    puts("\x1b[31m*\x1b[32m¯\x1b[33m`\x1b[34m·\x1b[35m.\x1b[36m_\x1b[31m.\x1b[32m·\x1b[31m[\x1b[0m   MENU' \x1b[31m ]\x1b[32m·\x1b[33m.\x1b[34m_\x1b[35m.\x1b[36m·\x1b[31m´\x1b[32m¯\x1b[33m*\x1b[0m");
    puts("\x1b[36m*                          \x1b[35m*\x1b[0m");
    puts("\x1b[32m*\x1b[0m 1\x1b[31m)\x1b[0m Accedi                \x1b[34m*\x1b[0m");
    puts("\x1b[34m*\x1b[0m 2\x1b[31m)\x1b[0m Registrati            \x1b[33m*\x1b[0m");
    puts("\x1b[33m*                          \x1b[31m*");
    puts("\x1b[35m*\x1b[0m 0\x1b[31m)\x1b[0m Exit                  \x1b[32m*\x1b[0m");
    puts("\x1b[32m*\x1b[33m-\x1b[34m-\x1b[35m-\x1b[36m-\x1b[31m-\x1b[32m-\x1b[33m-\x1b[34m-\x1b[35m-\x1b[36m-\x1b[31m-\x1b[32m-\x1b[33m-\x1b[34m-\x1b[34m-\x1b[35m-\x1b[36m-\x1b[31m-\x1b[32m-\x1b[33m-\x1b[34m-\x1b[35m-\x1b[36m-\x1b[31m-\x1b[32m-\x1b[33m-\x1b[34m*\x1b[0m");
    return;
}

/* Funzione che mostra il menu' principale del gioco */
void menu_loggato()
{
    puts("\x1b[31m*\x1b[32m¯\x1b[33m`\x1b[34m·\x1b[35m.\x1b[36m_\x1b[31m.\x1b[32m·\x1b[31m[\x1b[0m GAME'S MENU'\x1b[31m ]\x1b[32m·\x1b[33m.\x1b[34m_\x1b[35m.\x1b[36m·\x1b[31m´\x1b[32m¯\x1b[33m*\x1b[0m");
    puts("\x1b[36m*                              \x1b[35m*\x1b[0m");
    puts("\x1b[32m*\x1b[0m 1\x1b[31m)\x1b[0m Gioca                     \x1b[36m*\x1b[0m");
    puts("\x1b[31m*\x1b[0m 2\x1b[31m)\x1b[0m Utenti collegati          \x1b[35m*\x1b[0m");
    puts("\x1b[35m*\x1b[0m 3\x1b[31m)\x1b[0m Vedi oggetti rimanenti    \x1b[33m*\x1b[0m");
    puts("\x1b[32m*\x1b[0m 4\x1b[31m)\x1b[0m Vedi oggetti trovati      \x1b[32m*\x1b[0m");
    puts("\x1b[33m*                              \x1b[31m*\x1b[0m");
    puts("\x1b[31m*\x1b[0m 0\x1b[31m)\x1b[0m Torna indietro            \x1b[36m*\x1b[0m");
    puts("\x1b[32m*\x1b[33m-\x1b[34m-\x1b[35m-\x1b[36m-\x1b[31m-\x1b[32m-\x1b[33m-\x1b[34m-\x1b[35m-\x1b[36m-\x1b[31m-\x1b[32m-\x1b[33m-\x1b[34m-\x1b[34m-\x1b[35m-\x1b[36m-\x1b[31m-\x1b[32m-\x1b[33m-\x1b[34m-\x1b[35m-\x1b[36m-\x1b[31m-\x1b[32m-\x1b[33m-\x1b[36m-\x1b[35m-\x1b[33m-\x1b[31m-\x1b[34m*\x1b[0m");
    return;
}

/* Funzione relativa all'accesso di un utente */
int Accedi(int fdsocket)
{
    char nickname[MAXSIZE], password[MAXSIZE], flag_user = '-';
    fflush(stdin);
    printf("\n¯`·-\x1b[31m>\x1b[0m Username: ");
    scanf("%s",nickname);                           /* Chiedo l'inserimento dell'username */
    fflush(stdin);
    printf("\n¯`·-\x1b[31m>\x1b[0m Password: ");
    fflush(stdin);
    scanf("%s",password);                           /* Chiedo l'inserimento della password */

    if( write(fdsocket,nickname,strlen(nickname)) < 0 ) errori_generali(5);  /* Mando l'username al Server */
    if( read(fdsocket,&flag_user,1) <= 0 ) errori_generali(6);               /* Leggo la flag di controllo relativo all'username */

    if( flag_user == 'e' )
    {
        contatore_accessi++;            /* Incremento la variabile per dare un massimo di tre tentativi per accedere */
        if(contatore_accessi==3)        /* Verifico che la variabile di sopra raggiunga i tre tentativi */
        {
            system("clear");
            printf("\t\t\t––––•(-• \x1b[31mAttenzione\x1b[0m •-)•––––\n");
            puts("   Si e' superato il limite di tentativi. Verrai riportato al menu' principale!");
            sleep(2);
            contatore_accessi=0;        /* Riporto il contatore a 0 pronto per una succesiva richiesta di accesso */
            return -1;
        }
        system("clear");
        printf("\t\t  ––––•(-• \x1b[31mAttenzione\x1b[0m •-)•––––\n");
        puts("    Username e/o Password errati! Si prega di reinserire i dati.");
        return Accedi(fdsocket);
    }
    else
    {
        if( write(fdsocket,password,strlen(password)) < 0 ) errori_generali(7); /* Mando la password al server */

        if( read(fdsocket,&flag_user,1) <= 0 ) errori_generali(8);              /* Leggo la flag di controllo  */

        if( flag_user == 'e' )                          /* Verico il valore della flag appena ottenuta */
        {
            contatore_accessi++;                        /* Incremento la variabile per dare un massimo di tre tentativi per accedere */
            if(contatore_accessi==3)                    /* Verifico che la variabile di sopra raggiunga i tre tentativi */
            {
                system("clear");                        /* Effettuo una pulizia del terminale */
                printf("\t\t––––•(-• \x1b[31mAttenzione\x1b[0m •-)•––––\n");
                puts("Sono stati inseriti troppi tentativi! Verrete riportati al menu' iniziale!");
                contatore_accessi=0;                    /* Riporto il contatore a 0 pronto per una succesiva richiesta di accesso */
                return -1;
            }
            system("clear");                            /* Effettuo una pulizia del terminale */
            printf("\t––––•(-• \x1b[31mAttenzione\x1b[0m •-)•––––\n");
            puts("\t\tDati errati! Riprova!");
            return Accedi(fdsocket);
        }
    }
    return 0;
}

/* Funzione relativa alla registrazione di un utente */
void Registrati(int fd)
{
    /* DICHIARAZIONI VARIABILI */
    char utente[MAXSIZE],password[MAXSIZE],controllo_utente='-';

    do
    {
        printf("\n¯`·-\x1b[31m>\x1b[0m Username: ");
        scanf("%s",utente);                             /* Chiedo l'inserimento dell'username */
        fflush(stdin);
        printf("\n¯`·-\x1b[31m>\x1b[0m Password: ");
        scanf("%s",password);                           /* Chiedo l'inserimento della password */
        fflush(stdin);
        if( write(fd,utente,strlen(utente)) < 0) errori_generali(9); /* Mando il nome utente al Server */
        if( read(fd,&controllo_utente,1) < 0 ) errori_generali(10);  /* Verifico la flag di controllo */

        if(controllo_utente=='o') /* Se la flag letta prima corrisponde a 'o' significa che non e' presente un username con il relativo nome. Allora invio anche la password */
        {
            if( write(fd,password,strlen(password)) < 0) errori_generali(11); /* Invio della password al Server */
        }
        else /* SIgnifica che e' gia' presente un utente con l'username scelto in precedenza */
        {
            system("clear");
            printf("\t––––•(-• \x1b[31mAttenzione\x1b[0m •-)•––––\n");
            printf("Nome utente già presente, prego reinserire i dati!\n");
        }
    }
    while(controllo_utente != 'o');
    return;
}

/* Funzione relativa alla selezione delle opzioni disponibili nel secondo menu' di gioco */
void funziona_principale(int fd)
{
     /* DICHIARAZIONE VARIABILI */
     int scelta_menu = -1;
     char controllo_numero_utenti='x', controllo='m';

     system("clear"); /* Effettuo una pulizia del terminale */

     do
     {
        menu_loggato(); /* Chiamo il menu' relativo alle funzionalità di gioco */
        printf("Inserisci il numero relativo alla funzione:");  /* NON MI PIACE */
        tcflush(0, TCIFLUSH); /* Pulisco lo stream */
        scanf("%d",&scelta_menu);
        fflush(stdin);
        switch(scelta_menu)
         {
            /* Caso riguardante la scelta 'Gioca' */
            case 1:
                if( write(fd,"H",1) < 0 ) errori_generali(29);                   /* Invio carattere 'H' per l'apposita selezione lato Server */
                if( read(fd,&controllo,sizeof(char)) <= 0 ) errori_generali(30); /* Leggo la flag di controllo ricevuta dal Server */
                if( controllo != 'c' && controllo != 'o' )
                {
                    errori_generali(46);
                }

                if( write(fd,"A",strlen("A")) < 0 ) errori_generali(14);            /* Invio carattere 'A' per l'apposita selezione lato Server */
                if( read(fd,&controllo_numero_utenti,1) <= 0 ) errori_generali(15); /* Leggo la flag di controllo ricevuta dal Server */
                if( controllo_numero_utenti == 'o' ) /* Se la flag corrisponde ad 'o' abilitero' il giocatore per portarlo nella funzione 'funzione di gioco' */
                {
                    system("clear");            /* Effettuo una pulizia del terminale */
                    funzione_di_gioco(fd);      /* Funzione principale per inserire il giocatore all'interno della matrice di gioco */
                }
                else
                {
                    if( controllo_numero_utenti == 'e' ) /* Il Server informa il giocatore che non puo' entrare avendo superato il numero massimo di giocatori prenseti in gioco */
                    {
                        printf("\t\t\t––––•(-• \x1b[31mWarning\x1b[0m •-)•––––\n");
                        puts("\t\t\tNumero di utenti massimo!\nSi prega ri riprovare fra un paio di minuti o attendere la fine della partita!");
                        break;
                    }
                }
                system("clear");     /* Effettuo una pulizia del terminale */
                break;

            /* Caso riguardante la scelta 'Utenti collegati' */
            case 2:
                system("clear");                                            /* Effettuo una pulizia del terminale */
                if( write(fd,"B",strlen("B")) < 0 ) errori_generali(16);    /* Invio carattere 'B' per l'apposita selezione lato Server */
                StampaListaUtenti(fd);                                      /* Chiamo l'apposita funzione relativa alla funzionalita' scelta */
                break;

            /* Caso riguardante la scelta 'Vedi oggetti rimanenti' */
            case 3:
                system("clear");                                            /* Effettuo una pulizia del terminale */
                if( write(fd,"C",strlen("C")) < 0 ) errori_generali(17);    /* Invio carattere 'C' per l'apposita selezione lato Server */
                StampaNumeroOggRimanenti(fd);                               /* Chiamo l'apposita funzione relativa alla funzionalita' scelta */
                break;

            /* Caso riguardante la scelta 'Vedi oggetti trovati' */
            case 4:
                system("clear");                                            /* Effettuo una pulizia del terminale */
                if( write(fd,"D",strlen("D")) < 0 ) errori_generali(18);    /* Invio carattere 'D' per l'apposita selezione lato Server */
                StampaNumeroOggTrovati(fd);                                 /* Chiamo l'apposita funzione relativa alla funzionalita' scelta */
                break;

            /* Caso riguardante la scelta 'Torna indietro */
            case 0:
                if( write(fd,"Z",strlen("Z")) < 0 ) errori_generali(19);    /* Invio carattere 'Z' per l'apposita selezione lato Server */
         }
    }while(scelta_menu!=0); /* Ciclo finche' la variabile 'scelta menu' non e' 0 */
 return;
}

/* Funzione principale per inserire il giocatore all'interno della matrice di gioco */
void funzione_di_gioco(int fd)
{
    /* DICHIARAZIONI VARIABILI */
    int cont=0, indice = 0;
    char riga=' ',colonna=' ', nome_oggetto[MAXSIZE_2], dim=' ',contatore_riga='A', i='A',controllo='e';

    StampaMappa(fd); /* Richiamo alla funzione per stampare la matrice di gioco */
    printf("\n\n\x1b[31m*\x1b[32m¯\x1b[33m`\x1b[34m·\x1b[35m.\x1b[36m_\x1b[31m.\x1b[32m·\x1b[31m[\x1b[0m   Welcome \x1b[31m ]\x1b[32m·\x1b[33m.\x1b[34m_\x1b[35m.\x1b[36m·\x1b[31m´\x1b[32m¯\x1b[33m*\x1b[0m");

    do
    {
        do{
            printf("\n      Dove vuoi iniziare?\n\n╚\x1b[31m>\x1b[0m Inserisci la coordinata per \x1b[32mriga\x1b[0m:");
            while(getchar()!='\n');                                                  /* Pulisco lo stream per evitare eventuali carrateri sporchi rimasti */
            scanf("%c",&riga);                                                       /* Prendo la riga relativa alla matrice stampata */
            printf("╚\x1b[31m>\x1b[0m Inserisci la coordinata per \x1b[32mcolonna\x1b[0m: ");
            while(getchar()!='\n');                                                  /* Pulisco lo stream per evitare eventuali carrateri sporchi rimasti */
            scanf("%c",&colonna);                                                    /* Prendo la colonna relativa alla matrice stampata */

            riga = toupper(riga);           /* Evito problemi nel caso in cui il giocatore inserisca una lettera minuscola dato che la selezione e spostamento avviene tramite lettere maiuscole */
            colonna = toupper(colonna);
            if( ControlloRigaColonna(riga) != 0 || ControlloRigaColonna(colonna) != 0 ) /* Verifico che l'utente abbia inserito corettamente caratteri alfabetici */
            {
                printf("\n\x1b[31mAttenzione\x1b[0m: dati non corretti! Inserisci di nuovo i dati.\n");
            }
        }while( ControlloRigaColonna(riga) != 0 || ControlloRigaColonna(colonna) != 0 ); /* Stesso controllo, in questo caso d'uscita visto sopra nell'IF */

        if( write(fd,&riga,1) < 0 ) errori_generali(20);         /* Invio carattere 'Riga' per l'apposita selezione lato Server */
        if( write(fd,&colonna,1) < 0 ) errori_generali(21);      /* Invio carattere 'Colonna' per l'apposita selezione lato Server */
        if( read(fd,&controllo,1) <= 0 ) errori_generali(22);    /* Leggo la flag di controllo ricevuta dal Server */
        if(controllo=='e')                                       /* Significa che sono stati inseriti caratteri al di fuori della lunghezza della matrice di gioco */
        {
                 printf("\n\x1b[31mAttenzione\x1b[0m: dati non corretti! Inserisci di nuovo i dati.\n");
        }

    }while( controllo == 'e' ); /* Uscita dal ciclo solo quando tutti i dati inserite 'Riga' e 'Colonna' saranno corretti */

    if( controllo == 'g' ) /* Se il Server ha inviato come carattere di controllo una g significa che nella casella scelta dal giocatore per partire e' presente un oggetto, ed esso viene preso */
    {
        if( (indice=read(fd,nome_oggetto,MAXSIZE_2)) < 0 ) errori_generali(24);               /* Leggo il nome dell'oggetto appena trovato */
        nome_oggetto[indice] = '\0';                                                          /* Inserisco il carattere di fine stringa per evitare problemi di stampa/lettura */
        printf("\x1b[33mComplimenti\x1b[0m! Hai trovato \x1b[32m%s\x1b[0m!\n",nome_oggetto);  /* Notifico l'utende dell'oggetto appena trovato */
        sleep(1);                                                                             /* Do il tempo all'utente di leggere l'oggetto appena trovato per poi ricaricare la mappa */
    }

    if( write(fd,"H",1) < 0 ) errori_generali(18);                    /* Invio carattere 'H' per verificare se e' presente ancora una partita in gioco oppure e' stata generata un'altra sessione */
    if( read(fd,&controllo,sizeof(char)) <= 0 ) errori_generali(15); /* Leggo la flag di controllo ricevuta dal Server */
    if( controllo == 'c' )  /* Significa che la mappa del giocatore non si e' aggiornata dato che il giocatore stesso non si e' mosso, quindi gli verra' notificato che la partita a cui giocava e' terminata */
    {
        system("clear");    /* Effettuo una pulizia del terminale */
        printf("\t––––•(-• \x1b[31mAttenzione\x1b[0m •-)•––––\n");
        printf("\nLa partita in corso e' terminata ed e' stata generata una nuova sessione di gioco!\n");
        puts("Premi il punto(\x1b[32m.\x1b[0m) per tornare al Menu!");
        system ("/bin/stty raw");
        while(getchar()!='.'); /* Pulisco per evitare eventuali caratteri sporchi*/
        system ("/bin/stty cooked");
        return;
    }

    gioca_sub_controllo_movimenti(fd); /* Chiamo la funzione principale per permettere al giocatore di spostarsi ovunque nella mappa */

 return;
}

/* Funzione che stampa la lista degli utenti e i relativi oggetti trovati da essi. Funzione legata al secondo menu' di gioco */
void StampaListaUtenti2(int fd)
{
    /* DICHIARAZIONE VARIABILI */
    char utente[MAXSIZE_2], tmp = ' ', nome_oggetti[MAXSIZE_2];
    int cont = 0, indice = 0, costanzo = 0, flag = 0;

    while( (cont=read(fd,&tmp,1)) > 0 && tmp != 'e' && tmp == 'o' )         /* Eseguo finche' non ottengo un carattere diverso da 'o', uguale ad 'e' o un errore */
    {
        if( write(fd,"o",1) < 0 ) errori_generali(23);                      /* Invio conferma in attesa che succesivamente il Server mi mandi il nikname/username dell'utente */
        if( (indice=read(fd,utente,MAXSIZE_2)) < 0 ) errori_generali(24);   /* Eseguo la lettura di cio' che e' stato descritto sopra */
        utente[indice] = '\0';                                              /* Inserisco carattere fine stringa per evitare problemi di caratteri sporchi o lettura */
        if( write(fd,"o",1) < 0 ) errori_generali(23);

        flag = 0;
        while( (costanzo=read(fd,&tmp,1)) > 0 && tmp != 'q' && tmp == 'o' )  /* Eseguo finche' non ottengo un carattere diverso da 'o', uguale ad 'q' o un errore */
        {

            if( (indice=read(fd,nome_oggetti,MAXSIZE_2)) < 0 ) errori_generali(24); /* Invio conferma in attesa che succesivamente il Server mi mandi il nome del primo oggetto della lista dell'utente */
            nome_oggetti[indice] = '\0';                                            /* Inserisco carattere fine stringa per evitare problemi di caratteri sporchi o lettura */
            flag++;
            printf("\nUtente: %s  ---- ",utente);
            printf("Oggetto: %s.",nome_oggetti);
            if( write(fd,"o",1) < 0 ) errori_generali(23);
        }
        if( flag == 0 ) printf("\nUtente: %s  ---- Non ha trovato oggetti.",utente);
    }

    if( costanzo < 0 ) errori_generali(25);
    if( cont < 0 ) errori_generali(25);
}

/* Funzione che stampa la lista degli utenti e i relativi oggetti trovati da essi. Funzione legata a 'funzione_principale' */
void StampaListaUtenti(int fd)
{
    /* DICHIARAZIONE VARIABILI */
    char utente[MAXSIZE_2], tmp = ' ', num_oggetti[MAXSIZE_2];
    int cont = 0, indice = 0;

    while( (cont=read(fd,&tmp,1)) > 0 && tmp != 'e' )                          /* Eseguo finche' non ottengo il carattere di terminazione 'e' o un errore */
    {
        if( write(fd,"o",1) < 0 ) errori_generali(23);                         /* Invio conferma in attesa che succesivamente il Server mi mandi il nikname/username dell'utente */
        if( (indice=read(fd,utente,MAXSIZE_2)) < 0 ) errori_generali(24);      /* Eseguo la lettura di cio' che e' stato descritto sopra */
        utente[indice] = '\0';                                                 /* Inserisco carattere fine stringa per evitare problemi di caratteri sporchi o lettura */
        if( write(fd,"o",1) < 0 ) errori_generali(23);                         /* Invio conferma in attesa che succesivamente il Server mi mandi l'oggetto trovato dall'utente */
        if( (indice=read(fd,num_oggetti,MAXSIZE_2)) < 0 ) errori_generali(24); /* Eseguo la lettura di cio' che e' stato descritto sopra */
        num_oggetti[indice] = '\0';                                            /* Inserisco carattere fine stringa per evitare problemi di caratteri sporchi o lettura */

        printf("Utente: %s  ---- ",utente);
        printf("Oggetti trovati: %s\n",num_oggetti);

        if( write(fd,"o",1) < 0 ) errori_generali(23);                          /* Invio un carattere di conferma al server */
    }
    if( cont < 0 ) errori_generali(25);
}

/* Funzione per la stampa del numero di oggetti RIMANENTI in una sessione di gioco */
void StampaNumeroOggRimanenti(int fd)
{
    /* DICHIARAZIONI VARIABILI */
    char buf[MAXSIZE_2];
    int indice = 0;

    if( (indice=read(fd,buf,MAXSIZE_2)) <= 0 ) errori_generali(26); /* Leggo il numero di oggetti rinamenti in gioco */
    buf[indice] = '\0'; /* Inserisco carattere fine stringa per evitare problemi di caratteri sporchi o lettura */
    printf("Oggetti rimanenti: %s.\n",buf); /* Mostro al giocatore l'informazione richiesta */
}

/* Funzione relativa alla stampa del tempo trascorso in una partita */
void StampaTempo(int fd)
{
    if( write(fd,"V",1) < 0 ) errori_generali(47); /* Invio carattere 'V' per rihiedere al server il tempo trascorso di una determinata partita */

    /* DICHIARAZIONI VARIABILI */
    char buf[MAXSIZE_2];
    int indice = 0;

    if( (indice=read(fd,buf,MAXSIZE_2)) <= 0 ) errori_generali(48); /* Leggo il numero corrispondente al tempo trascorso in una partita */
    buf[indice] = '\0'; /* Inserisco carattere fine stringa per evitare problemi di caratteri sporchi o lettura */
    printf("✿ Tempo: \x1b[33m%s\x1b[0m ✿\n",buf);   /* Stampo il tempo trascorso con dei meravigliosi fiorelleni ai lati */
}

/* Funzione per la stampa del numero di oggetti TROVATI in una sessione di gioco */
void StampaNumeroOggTrovati(int fd)
{
    /* DICHIARAZIONI VARIABILI */
    char buf[MAXSIZE_2];
    int indice = 0;

    if( (indice=read(fd,buf,MAXSIZE_2)) <= 0 ) errori_generali(27); /* Leggo il numero di oggetti trovati in gioco */
    buf[indice] = '\0'; /* Inserisco carattere fine stringa per evitare problemi di caratteri sporchi o lettura */
    printf("Oggetti trovati: %s.\n",buf); /* Mostro al giocatore l'informazione richiesta */
}

/* Funzione relativa alla stampa della matrice di gioco */
void StampaMappa(int fd)
{
    /* DICHIARAZIONI VARIABILI */
    char riga=' ',colonna=' ', dim=' ',contatore_riga='A', i='A',controllo='e';


    while( read(fd,&dim,1) > 0 && dim!='e' ) /* Una volta effettuata la write con selezione 'W' il Server inizierà a inviare la mappa fino al carattere di controllo finale 'e' */
    {
        /* A seconda del carattere inviato dal Server, il Client stampera' l'opportuna selezione specificata dai case */
        switch(dim)
        {
            case '1':
                printf("\033[01;31m|M|\033[00m");   /* Significa che in quella posizione e' presente un muro */
                break;
            case '2':
                printf("\x1b[33m|G|\033[00m");      /* Significa che in quella posizione e' presente un altro giocatore */
                break;
            case '3':
                printf("\x1b[32m|G|\033[00m");      /* Significa che in quella posizione e' presente il giocatore stesso */
                break;
            case '5':
                printf("\x1b[34m|O|\x1b[0m");       /* Significa che in quella posizione e' stato trovato un oggetto */
                break;
            case '0':
                printf("|V|");                      /* Significa che in quella posizione e' presente una posizione vuota */
                break;
            case '4':
                printf("\n%c  ",contatore_riga++);  /* Stampa per visualizzare in modo corretto la matrice di gioco */
                break;
        }
    }
    printf("\n\n   "); /* Stampa per visualizzare in modo corretto la matrice di gioco */
    for(i = 'A';i < contatore_riga;i++) printf(" %c ",i); /* Stampa per visualizzare in modo corretto la matrice di gioco */
    return;
}

/* Funzione relativa allo spostamento dell'utente nella direzione: destra */
int spostatiAdestra(int fd)
{
    /* DICHIARAZIONI VARIABILI */
    char c = ' ', controllo = 'M', nome_oggetto[MAXSIZE_2], var_tmp = 'M';
    int caratteri_letti = 0;

    if( write(fd,"H",1) < 0 ) errori_generali(35);                      /* Invio carattere 'H' per verificare se e' presente ancora una partita in gioco oppure e' stata generata un'altra sessione */
    if( read(fd,&controllo,sizeof(char)) <= 0 ) errori_generali(39);    /* Lettura flag di controllo relativa alla write sopra */
    if( controllo == 'c' )
    {
         system("clear"); /* Effettuo una pulizia del terminale */
         printf("\t––––•(-• \x1b[31mAttenzione\x1b[0m •-)•––––\n");
         printf("\nLa partita in corso e' terminata ed e' stata generata una nuova sessione di gioco!\n");
         puts("Premi il punto(\x1b[32m.\x1b[0m) per tornare al Menu!");
         system ("/bin/stty raw");           /* Chiamata di sistema per modificare il terminale. Esso accettera' i comandi di movimento in input senza che l'utende debba confermare la selezione tramite l'uso dell'invio */
         while(getchar()!='.');              /* Pulisco lo stream per evitare eventuali carrateri sporchi rimasti */
         system ("/bin/stty cooked");        /* Riporto il terminale nello stato precendente alla chiamata '/bin/stty raw' */
         return 1;
    }

    if( write(fd,"X",1) < 0 ) errori_generali(40);                       /* Invio carattere 'X' per verificare e notificare lo spostamento da effettuare al Server */
    if( read(fd,&controllo,sizeof(char)) <= 0 ) errori_generali(41);     /* Lettura flag di controllo relativa alla write sopra */
    if( controllo == 'o' )                                               /* Tutto procede secondo i piani */
    {
        system("clear");
    }
    else
    {
        errori_generali(41);
    }

    if( write(fd,"D",1) < 0 ) errori_generali(42);        /* Invio carattere 'D' per dire al server di spostare il giocatore a destra */
    if( read(fd,&controllo,1) <= 0 ) errori_generali(43); /* Lettura flag di controllo relativa alla write sopra */

    switch(controllo)
    {
        /* Il giocatore puo' spostarsi a destra */
        case 'o':
            printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
            printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
            StampaTempo(fd); /* Funzione per stampare il tempo trascorso */
            if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio carattere 'W' per l'apposita selezione lato Server che invierà la mappa al Client */
            StampaMappa(fd); /* Funzione per stampare la mappa di gioco */
            printf("\nTi sei spostato a: \x1b[32mdestra\x1b[0m!\n");
            break;

        /* Il giocatore NON puo' spostarsi a destra perche' e' presente un muro */
        case 'e':
            printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
            printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");

            StampaTempo(fd); /* Funzione per stampare il tempo trascorso */
            if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio carattere 'W' per l'apposita selezione lato Server che invierà la mappa al Client */
            StampaMappa(fd); /* Funzione per stampare la mappa di gioco */
            printf("\nNon puoi andare a \x1b[32mdestra\x1b[0m! C'e' un \x1b[32mmuro\x1b[0m!\n");
            break;

        /* Il giocatore NON puo' spostarsi a destra perche' e' presente un altro giocatore */
        case 'j':
            printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
            printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
            StampaTempo(fd); /* Funzione per stampare il tempo trascorso */
            if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio carattere 'W' per l'apposita selezione lato Server che invierà la mappa al Client */
            StampaMappa(fd); /* Funzione per stampare la mappa di gioco */
            printf("\nNon puoi andare a \x1b[32mdestra\x1b[0m! C'e' un \x1b[32mgiocatore\x1b[0m!\n");
            break;

        /* Controllo partita e oggetto */
        case 'g':
            if( (caratteri_letti=read(fd,nome_oggetto,MAXSIZE_2)) <= 0 ) errori_generali(44); /* Leggo il nome dell'oggetto che mi e' stato mandato */
            nome_oggetto[caratteri_letti] = '\0';                                             /* Inserisco il carattere di fine stringa per evitare problemi di caratteri sporchi o lettura */

            if( write(fd,"o",strlen("o")) < 0 ) errori_generali(39);  /* Invio carattere 'o' per verificare se la partita e' ancora in corso o no */

            if( read(fd,&var_tmp,1) <= 0 ) errori_generali(45);       /* Lettura flag di controllo relativa alla write sopra */
            if( var_tmp == 'f') /* SIgnifica che la partita e' finita */
            {
                system("clear"); /* Effettuo una pulizia del terminale */
                printf("\t––––•(-• \x1b[31mAttenzione\x1b[0m •-)•––––\n");
                printf("\nLa partita in corso e' terminata ed e' stata generata una nuova sessione di gioco!\n");
                puts("Premi il punto(\x1b[32m.\x1b[0m) per tornare al Menu!");
                system ("/bin/stty raw");           /* Chiamata di sistema per modificare il terminale. Esso accettera' i comandi di movimento in input senza che l'utende debba confermare la selezione tramite l'uso dell'invio */
                while(getchar()!='.');              /* Pulisco lo stream per evitare eventuali carrateri sporchi rimasti */
                system ("/bin/stty cooked");        /* Riporto il terminale nello stato precendente alla chiamata '/bin/stty raw' */
                return 1;                           /* Torno al menu' di gioco */
            }
            printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
            printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
            StampaTempo(fd); /* Funzione per stampare il tempo trascorso */
            if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio carattere 'W' per l'apposita selezione lato Server che invierà la mappa al Client */
            StampaMappa(fd); /* Funzione per stampare la mappa di gioco */
            printf("\nHai trovato \x1b[33m%s\x1b[0m! \x1b[36mCongratulazioni\x1b[0m!\n", nome_oggetto); /* Notifico l'utente */
            break;
        default:
            printf("\n\x1b[31mERRORE\x1b[0m");
            break;
    }
    return 0;
}

/* Funzione relativa allo spostamento dell'utente nella direzione: sinistra */
int spostatiAsinistra(int fd)
{
     /* DICHIARAZIONI VARIABILI */
    char c = ' ', controllo = 'M', nome_oggetto[MAXSIZE_2], var_tmp = 'M';
    int caratteri_letti = 0;

    if( write(fd,"H",1) < 0 ) errori_generali(35);                      /* Invio carattere 'H' per verificare se e' presente ancora una partita in gioco oppure e' stata generata un'altra sessione */
    if( read(fd,&controllo,sizeof(char)) <= 0 ) errori_generali(39);    /* Lettura flag di controllo relativa alla write sopra */
    if( controllo == 'c' )
    {
        system("clear"); /* Effettuo una pulizia del terminale */
        printf("\t––––•(-• \x1b[31mAttenzione\x1b[0m •-)•––––\n");
        printf("\nLa partita in corso e' terminata ed e' stata generata una nuova sessione di gioco!\n");
        puts("Premi il punto(\x1b[32m.\x1b[0m) per tornare al Menu!");
        system ("/bin/stty raw");           /* Chiamata di sistema per modificare il terminale. Esso accettera' i comandi di movimento in input senza che l'utende debba confermare la selezione tramite l'uso dell'invio */
        while(getchar()!='.');              /* Pulisco lo stream per evitare eventuali carrateri sporchi rimasti */
        system ("/bin/stty cooked");        /* Riporto il terminale nello stato precendente alla chiamata '/bin/stty raw' */
        return 1;                           /* Ritorno al menu' */
    }

    if( write(fd,"X",1) < 0 ) errori_generali(40);                     /* Invio carattere 'X' per verificare e notificare lo spostamento da effettuare al Server */
    if( read(fd,&controllo,sizeof(char)) <= 0 ) errori_generali(41);   /* Lettura flag di controllo relativa alla write sopra */
    if( controllo == 'o' )
    {
        system("clear");
    }
    else
    {
        errori_generali(41);
    }

    if( write(fd,"A",1) < 0 ) errori_generali(42);          /* Invio carattere 'D' per dire al server di spostare il giocatore a sinistra */
    if( read(fd,&controllo,1) <= 0 ) errori_generali(43);   /* Lettura flag di controllo relativa alla write sopra */

    switch(controllo)
    {
        /* Il giocatore puo' spostarsi a sinistra */
        case 'o':
            printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
            printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
            StampaTempo(fd);  /* Funzione per stampare il tempo trascorso */
            if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio carattere 'W' per l'apposita selezione lato Server che invierà la mappa al Client */
            StampaMappa(fd); /* Funzione per stampare la mappa di gioco */
            printf("\nTi sei spostato a: \x1b[32msinistra\x1b[0m!\n");
            break;

        /* Il giocatore NON puo' spostarsi a sinistra perche' e' presente un muro */
        case 'e':
            printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
            printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
            StampaTempo(fd);  /* Funzione per stampare il tempo trascorso */
            if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio carattere 'W' per l'apposita selezione lato Server che invierà la mappa al Client */
            StampaMappa(fd); /* Funzione per stampare la mappa di gioco */
            printf("\nNon puoi andare a \x1b[32msinistra\x1b[0m! C'e' un \x1b[32mmuro\x1b[0m!\n");
            break;

        /* Il giocatore NON puo' spostarsi a sinista perche' e' presente un altro giocatore */
        case 'j':
            printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
            printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
            StampaTempo(fd);  /* Funzione per stampare il tempo trascorso */
            if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio carattere 'W' per l'apposita selezione lato Server che invierà la mappa al Client */
            StampaMappa(fd); /* Funzione per stampare la mappa di gioco */
            printf("\nNon puoi andare a \x1b[32mdestra\x1b[0m! C'e' un \x1b[32mgiocatore\x1b[0m!\n");
            break;

        /* Controllo partita e oggetto */
        default:
            if( (caratteri_letti=read(fd,nome_oggetto,MAXSIZE_2)) <= 0 ) errori_generali(44); /* Leggo il nome dell'oggetto che mi e' stato mandato */
            nome_oggetto[caratteri_letti] = '\0';                                             /* Inserisco il carattere di fine stringa per evitare problemi di caratteri sporchi o lettura */

            if( write(fd,"o",strlen("o")) < 0 ) errori_generali(39); /* Invio carattere 'o' per verificare se la partita e' ancora in corso o no */
            if( read(fd,&var_tmp,1) <= 0 ) errori_generali(45);     /* Lettura flag di controllo relativa alla write sopra */
            if( var_tmp == 'f') /* Significa che la partita e' finita */
            {
                system("clear"); /* Effettuo una pulizia del terminale */
                printf("\t––––•(-• \x1b[31mAttenzione\x1b[0m •-)•––––\n");
                printf("\nLa partita in corso e' terminata ed e' stata generata una nuova sessione di gioco!\n");
                puts("Premi il punto(\x1b[32m.\x1b[0m) per tornare al Menu!");
                system ("/bin/stty raw");           /* Chiamata di sistema per modificare il terminale. Esso accettera' i comandi di movimento in input senza che l'utende debba confermare la selezione tramite l'uso dell'invio */
                while(getchar()!='.');              /* Pulisco lo stream per evitare eventuali carrateri sporchi rimasti */
                system ("/bin/stty cooked");        /* Riporto il terminale nello stato precendente alla chiamata '/bin/stty raw' */
                return 1;                           /* Torno al menu' di gioco */
            }
            printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
            printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
            StampaTempo(fd);  /* Funzione per stampare il tempo trascorso */
            if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio carattere 'W' per l'apposita selezione lato Server che invierà la mappa al Client */
            StampaMappa(fd); /* Funzione per stampare la mappa di gioco */
            printf("\nHai trovato \x1b[33m%s\x1b[0m! \x1b[36mCongratulazioni\x1b[0m!\n", nome_oggetto); /* Notifico l'utente */
            break;
    }
    return 0;
}

/* Funzione relativa allo spostamento dell'utente nella direzione: sotto */
int spostatiGiu(int fd)
{
    /* DICHIARAZIONI VARIABILI */
    char c = ' ', controllo = 'M', nome_oggetto[MAXSIZE_2], var_tmp = 'M';
    int caratteri_letti = 0;

    if( write(fd,"H",1) < 0 ) errori_generali(35);                      /* Invio carattere 'H' per verificare se e' presente ancora una partita in gioco oppure e' stata generata un'altra sessione */
    if( read(fd,&controllo,sizeof(char)) <= 0 ) errori_generali(39);    /* Lettura flag di controllo relativa alla write sopra */
    if( controllo == 'c' )
    {
        system("clear"); /* Effettuo una pulizia del terminale */
        printf("\t––––•(-• \x1b[31mAttenzione\x1b[0m •-)•––––\n");
        printf("\nLa partita in corso e' terminata ed e' stata generata una nuova sessione di gioco!\n");
        puts("Premi il punto(\x1b[32m.\x1b[0m) per tornare al Menu!");
        system ("/bin/stty raw");           /* Chiamata di sistema per modificare il terminale. Esso accettera' i comandi di movimento in input senza che l'utende debba confermare la selezione tramite l'uso dell'invio */
        while(getchar()!='.');              /* Pulisco lo stream per evitare eventuali carrateri sporchi rimasti */
        system ("/bin/stty cooked");        /* Riporto il terminale nello stato precendente alla chiamata '/bin/stty raw' */
        return 1;                           /* Ritorno al menu' */
    }

    if( write(fd,"X",1) < 0 ) errori_generali(40);                       /* Invio carattere 'X' per verificare e notificare lo spostamento da effettuare al Server */
    if( read(fd,&controllo,sizeof(char)) <= 0 ) errori_generali(41);     /* Lettura flag di controllo relativa alla write sopra */
    if( controllo == 'o' )
    {
        system("clear");
    }
    else
    {
        errori_generali(41);
    }

    if( write(fd,"Y",1) < 0 ) errori_generali(42);           /* Invio carattere 'Y' per dire al server di spostare il giocatore sotto */
    if( read(fd,&controllo,1) <= 0 ) errori_generali(43);    /* Lettura flag di controllo relativa alla write sopra */

    switch(controllo)
    {
         /* Il giocatore puo' spostarsi sotto */
        case 'o':
            printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
            printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
            StampaTempo(fd); /* Funzione per stampare il tempo trascorso */
            if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio carattere 'W' per l'apposita selezione lato Server che invierà la mappa al Client */
            StampaMappa(fd); /* Funzione per stampare la mappa di gioco */
            printf("\nTi sei spostato: \x1b[32msotto\x1b[0m!\n");
            break;

        /* Il giocatore NON puo' spostarsi sotto perche' e' presente un muro */
        case 'e':
            printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
            printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
            StampaTempo(fd); /* Funzione per stampare il tempo trascorso */
            if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio carattere 'W' per l'apposita selezione lato Server che invierà la mappa al Client */
            StampaMappa(fd); /* Funzione per stampare la mappa di gioco */
            printf("\nNon puoi andare \x1b[32msotto\x1b[0m! C'e' un \x1b[32mmuro\x1b[0m!\n");
            break;

         /* Il giocatore NON puo' spostarsi a sinista perche' e' presente un altro giocatore */
        case 'j':
            printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
            printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
            StampaTempo(fd); /* Funzione per stampare il tempo trascorso */
            if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio carattere 'W' per l'apposita selezione lato Server che invierà la mappa al Client */
            StampaMappa(fd); /* Funzione per stampare la mappa di gioco */
            printf("\nNon puoi andare \x1b[32msotto\x1b[0m! C'e' un \x1b[32mgiocatore\x1b[0m!\n");
            break;

        /* Controllo partita e oggetto */
        default:
            if( (caratteri_letti=read(fd,nome_oggetto,MAXSIZE_2)) <= 0 ) errori_generali(44);   /* Leggo il nome dell'oggetto che mi e' stato mandato */
            nome_oggetto[caratteri_letti] = '\0';                                               /* Inserisco il carattere di fine stringa per evitare problemi di caratteri sporchi o lettura */

            if( write(fd,"o",strlen("o")) < 0 ) errori_generali(39); /* Invio carattere 'o' per verificare se la partita e' ancora in corso o no */
            if( read(fd,&var_tmp,1) <= 0 ) errori_generali(45);     /* Lettura flag di controllo relativa alla write sopra */
            if( var_tmp == 'f') /* Significa che la partita e' finita */
            {
                system("clear"); /* Effettuo una pulizia del terminale */
                printf("\t––––•(-• \x1b[31mAttenzione\x1b[0m •-)•––––\n");
                printf("\nLa partita in corso e' terminata ed e' stata generata una nuova sessione di gioco!\n");
                puts("Premi il punto(\x1b[32m.\x1b[0m) per tornare al Menu!");
                system ("/bin/stty raw");           /* Chiamata di sistema per modificare il terminale. Esso accettera' i comandi di movimento in input senza che l'utende debba confermare la selezione tramite l'uso dell'invio */
                while(getchar()!='.');              /* Pulisco lo stream per evitare eventuali carrateri sporchi rimasti */
                system ("/bin/stty cooked");        /* Riporto il terminale nello stato precendente alla chiamata '/bin/stty raw' */
                return 1;                           /* Torno al menu' di gioco */
            }
            printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
            printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
            StampaTempo(fd); /* Funzione per stampare il tempo trascorso */
            if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio carattere 'W' per l'apposita selezione lato Server che invierà la mappa al Client */
            StampaMappa(fd); /* Funzione per stampare la mappa di gioco */
            printf("\nHai trovato \x1b[33m%s\x1b[0m! \x1b[36mCongratulazioni\x1b[0m!\n", nome_oggetto); /* Notifico l'utente */
            break;
    }
    return 0;
}

/* Funzione relativa allo spostamento dell'utente nella direzione: sopra */
int spostatiSopra(int fd)
{
    /* DICHIARAZIONI VARIABILI */
    char c = ' ', controllo = 'M', nome_oggetto[MAXSIZE_2], var_tmp = 'M';
    int caratteri_letti = 0;

    if( write(fd,"H",1) < 0 ) errori_generali(35);                     /* Invio carattere 'H' per verificare se e' presente ancora una partita in gioco oppure e' stata generata un'altra sessione */
    if( read(fd,&controllo,sizeof(char)) <= 0 ) errori_generali(39);   /* Lettura flag di controllo relativa alla write sopra */
    if( controllo == 'c' )
    {
        system("clear"); /* Effettuo una pulizia del terminale */
        printf("\t––––•(-• \x1b[31mAttenzione\x1b[0m •-)•––––\n");
        printf("\nLa partita in corso e' terminata ed e' stata generata una nuova sessione di gioco!\n");
        puts("Premi il punto(\x1b[32m.\x1b[0m) per tornare al Menu!");
        system ("/bin/stty raw");           /* Chiamata di sistema per modificare il terminale. Esso accettera' i comandi di movimento in input senza che l'utende debba confermare la selezione tramite l'uso dell'invio */
        while(getchar()!='.');              /* Pulisco lo stream per evitare eventuali carrateri sporchi rimasti */
        system ("/bin/stty cooked");        /* Riporto il terminale nello stato precendente alla chiamata '/bin/stty raw' */
        return 1;                           /* Ritorno al menu' */
    }

    if( write(fd,"X",1) < 0 ) errori_generali(40);                      /* Invio carattere 'X' per verificare e notificare lo spostamento da effettuare al Server */
    if( read(fd,&controllo,sizeof(char)) <= 0 ) errori_generali(41);    /* Lettura flag di controllo relativa alla write sopra */
    if( controllo == 'o' )
    {
        system("clear");
    }
    else
    {
        errori_generali(41);
    }

    if( write(fd,"K",1) < 0 ) errori_generali(42);          /* Invio carattere 'Y' per dire al server di spostare il giocatore sopra */
    if( read(fd,&controllo,1) <= 0 ) errori_generali(43);   /* Lettura flag di controllo relativa alla write sopra */

    switch(controllo)
    {
         /* Il giocatore puo' spostarsi sotto */
        case 'o':
            printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
            printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
            StampaTempo(fd); /* Funzione per stampare il tempo trascorso */
            if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio carattere 'W' per l'apposita selezione lato Server che invierà la mappa al Client */
            StampaMappa(fd); /* Funzione per stampare la mappa di gioco */
            printf("\nTi sei spostato: \x1b[32msopra\x1b[0m!\n");
            break;

        /* Il giocatore NON puo' spostarsi sotto perche' e' presente un muro */
        case 'e':
            printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
            printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
            StampaTempo(fd); /* Funzione per stampare il tempo trascorso */
            if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio carattere 'W' per l'apposita selezione lato Server che invierà la mappa al Client */
            StampaMappa(fd); /* Funzione per stampare la mappa di gioco */
            printf("\nNon puoi andare \x1b[32msopra\x1b[0m! C'e' un \x1b[32mmuro\x1b[0m!\n");
            break;

        /* Il giocatore NON puo' spostarsi a sinista perche' e' presente un altro giocatore */
        case 'j':
            printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
            printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
            StampaTempo(fd); /* Funzione per stampare il tempo trascorso */
            if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio carattere 'W' per l'apposita selezione lato Server che invierà la mappa al Client */
            StampaMappa(fd); /* Funzione per stampare la mappa di gioco */
            printf("\nNon puoi andare \x1b[32msopra\x1b[0m! C'e' un \x1b[32mgiocatore\x1b[0m!\n");
            break;

        /* Controllo partita e oggetto */
        default:
            if( (caratteri_letti=read(fd,nome_oggetto,MAXSIZE_2)) <= 0 ) errori_generali(44);  /* Leggo il nome dell'oggetto che mi e' stato mandato */
            nome_oggetto[caratteri_letti] = '\0';                                              /* Inserisco il carattere di fine stringa per evitare problemi di caratteri sporchi o lettura */

            if( write(fd,"o",strlen("o")) < 0 ) errori_generali(39); /* Invio carattere 'o' per verificare se la partita e' ancora in corso o no */
            if( read(fd,&var_tmp,1) <= 0 ) errori_generali(45);      /* Lettura flag di controllo relativa alla write sopra */
            if( var_tmp == 'f') /* Significa che la partita e' terminata */
            {
                system("clear"); /* Effettuo una pulizia del terminale */
                printf("\t––––•(-• \x1b[31mAttenzione\x1b[0m •-)•––––\n");
                printf("\nLa partita in corso e' terminata ed e' stata generata una nuova sessione di gioco!\n");
                puts("Premi il punto(\x1b[32m.\x1b[0m) per tornare al Menu!");
                system ("/bin/stty raw");           /* Chiamata di sistema per modificare il terminale. Esso accettera' i comandi di movimento in input senza che l'utende debba confermare la selezione tramite l'uso dell'invio */
                while(getchar()!='.');              /* Pulisco lo stream per evitare eventuali carrateri sporchi rimasti */
                system ("/bin/stty cooked");        /* Riporto il terminale nello stato precendente alla chiamata '/bin/stty raw' */
                return 1;                           /* Ritorno al menu' */
            }
            printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
            printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
            StampaTempo(fd); /* Funzione per stampare il tempo trascorso */
            if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio carattere 'W' per l'apposita selezione lato Server che invierà la mappa al Client */
            StampaMappa(fd); /* Funzione per stampare la mappa di gioco */
            printf("\nHai trovato \x1b[33m%s\x1b[0m! \x1b[36mCongratulazioni\x1b[0m!\n", nome_oggetto); /* Notifico l'utente */
            break;
    }
    return 0;
}


/* Funzione principale per permettere al giocatore di spostarsi nella mappa */
void gioca_sub_controllo_movimenti(int fd)
{
    fflush(stdin); /* Pulisco lo stdin per evitare caratteri sporchi */

    /* DICHIARAZIONI VARIABILI */
    char c = ' ', controllo = 'M', nome_oggetto[MAXSIZE_2], var_tmp = 'M';
    int caratteri_letti = 0;

    system("clear"); /* Effettuo una pulizia del terminale */
    printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
    printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
    StampaTempo(fd); /* Funzione per stampare il tempo trascorso */

    if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio carattere 'W' per l'apposita selezione lato Server che invierà la mappa al Client */
    StampaMappa(fd);                                         /* Funzione per stampare la mappa di gioco */

    system ("/bin/stty raw");                               /* Chiamata di sistema per modificare il terminale. Esso accettera' i comandi di movimento in input senza che l'utende debba confermare la selezione tramite l'uso dell'invio */
    while((c=getchar())!= '.')
    {
        system ("/bin/stty cooked");                        /* Riporto il terminale nello stato precendente alla chiamata '/bin/stty raw' */
        c = toupper(c);                                     /* Serve per evitare problemi nel caso in cui l'utente prema caratteri minuscoli */
        switch(c)
        {
            case 'D':
                if(spostatiAdestra(fd) == 1) return;        /* Funzione per spostare a destra l'utente */
                break;
            case 'A':
                if(spostatiAsinistra(fd) == 1) return;      /* Funzione per spostare a sinistra l'utente */
                break;
            case 'W':
                if(spostatiSopra(fd) == 1) return;          /* Funzione per spostare sopra l'utente */
                break;
            case 'S':
                if(spostatiGiu(fd) == 1) return;            /* Funzione per spostare sotto l'utente */
                break;
            case 'L':
                if( write(fd,"H",1) < 0 ) errori_generali(35);                   /* Invio il carattere 'H' al Server per la relativa funzione di stampa utenti */
                if( read(fd,&controllo,sizeof(char)) <= 0 ) errori_generali(32); /* Leggo la flag di controllo che mi e' stata mandata dal server */
                if( controllo == 'c' )  /* Verifico che la partita presente non sia terminata, in caso contrario il Server mi manda come flag di controllo una 'c'*/
                {
                    system("clear"); /* Effettuo una pulizia del terminale */
                    printf("\t––––•(-• \x1b[31mAttenzione\x1b[0m •-)•––––\n");
                    printf("\nLa partita in corso e' terminata ed e' stata generata una nuova sessione di gioco!\n");
                    puts("Premi il punto(\x1b[32m.\x1b[0m) per tornare al Menu!");
                    system ("/bin/stty raw");           /* Chiamata di sistema per modificare il terminale. Esso accettera' i comandi di movimento in input senza che l'utende debba confermare la selezione tramite l'uso dell'invio */
                    while(getchar()!='.');              /* Pulisco lo stream per evitare eventuali carrateri sporchi rimasti */
                    system ("/bin/stty cooked");        /* Riporto il terminale nello stato precendente alla chiamata '/bin/stty raw' */
                    return;
                }

                system("clear");   /* Effettuo una pulizia del terminale */
                printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
                printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
                StampaTempo(fd);   /* Funzione per stampare il tempo trascorso */
                if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33); /* Invio il carattere 'W' al Server per richiedere l'invio della mappa */
                StampaMappa(fd);   /* Funzione per stampare il tempo trascorso */

                if( write(fd,"L",1) < 0 ) errori_generali(34); /* Invio il carattere 'L' al Server per richiedere l'invio della lista degli utenti con i relativi oggetti */
                StampaListaUtenti2(fd);                        /* Funzione per la stampa della lista utenti con i relativi oggetti trovati */
               break;

            /* Casi relativi allo spostamento dell'utente tramite le frecce direzionali */
            case '[':
                switch(c=getchar())
                {
                    case 'A':
                        if(spostatiSopra(fd) == 1) return;       /* Funzione per spostare sopra l'utente */
                        break;
                    case 'D':
                        if(spostatiAsinistra(fd) == 1) return;  /* Funzione per spostare a sinistra l'utente */
                        break;
                    case 'C':
                        if(spostatiAdestra(fd) == 1) return;    /* Funzione per spostare a destra l'utente */
                        break;
                    case 'B':
                        if(spostatiGiu(fd) == 1) return;        /* Funzione per spostare sotto l'utente */
                        break;
                }
                break;

            /* Vengono stampati il numero di oggetti rimanenti e viene controllata che la partita non sia finita */
            case 'O':
                if( write(fd,"H",1) < 0 ) errori_generali(35);                   /* Invio carattere 'H' per verificare se e' presente ancora una partita in gioco oppure e' stata generata un'altra sessione */
                if( read(fd,&controllo,sizeof(char)) <= 0 ) errori_generali(15); /* Leggo la flag di constrollo ricevuta dal server in merito alla write sopra */
                if( controllo == 'c' ) /* Verifico che la partita presente non sia terminata, in caso contrario il Server mi manda come flag di controllo una 'c'*/
                {
                    system("clear");    /* Effettuo una pulizia del terminale */
                    printf("\t––––•(-• \x1b[31mAttenzione\x1b[0m •-)•––––\n");
                    printf("\nLa partita in corso e' terminata ed e' stata generata una nuova sessione di gioco!\n");
                    puts("Premi il punto(\x1b[32m.\x1b[0m) per tornare al Menu!");
                    system ("/bin/stty raw");        /* Chiamata di sistema per modificare il terminale. Esso accettera' i comandi di movimento in input senza che l'utende debba confermare la selezione tramite l'uso dell'invio */
                    while(getchar()!='.');           /* Pulisco lo stream per evitare eventuali carrateri sporchi rimasti */
                    system ("/bin/stty cooked");     /* Riporto il terminale nello stato precendente alla chiamata '/bin/stty raw' */
                    return;
                }
                system("clear");    /* Effettuo una pulizia del terminale */
                printf("\t\t\t  —(•·÷\x1b[31m[\x1b[0m Leggenda \x1b[31m]\x1b[0m÷·•)—\n\n\t\033[01;31m|M|\033[00m-> Muro - \x1b[32m|G|\x1b[0m-> Tu - \x1b[33m|G|\x1b[0m-> Altri giocatori - |V|-> Casella vuota\n");
                printf("\n¯`·-\x1b[31m>\x1b[0m Per muoverti usa: W(\x1b[32m▲\x1b[0m) - A(\x1b[32m◄\x1b[0m) - S(\x1b[32m▼\x1b[0m) - D(\x1b[32m►\x1b[0m) oppure le frecce direzionali!\n      L-> Utenti presenti con relativi oggetti - O-> Oggetti rimanenti - Punto(\x1b[32m.\x1b[0m)-> Esci\n\t\t\t\t ");
                StampaTempo(fd);
                if( write(fd,"W",strlen("W")) < 0 ) errori_generali(33);    /* Invio il carattere 'W' al Server per richiedere l'invio della mappa */
                StampaMappa(fd);                                            /* Funzione per stampare la mappa di gioco */
                puts("");                                                   /* Migliorare visivamente la stampa degli oggetti rimanenti */
                if( write(fd,"C",strlen("C")) < 0 ) errori_generali(36);    /* Invio il carattere 'C' al Server per richiedere il numero di oggetti rimanenti */
                StampaNumeroOggRimanenti(fd);                               /* Funzione per stampare il numero di oggetti rimanenti */
                break;
            default:
                if( write(fd,"H",1) < 0 ) errori_generali(35);                   /* Invio carattere 'H' per verificare se e' presente ancora una partita in gioco oppure e' stata generata un'altra sessione */
                if( read(fd,&controllo,sizeof(char)) <= 0 ) errori_generali(15); /* Leggo la flag di constrollo ricevuta dal server in merito alla write sopra */
                if( controllo == 'c' )
                {
                    system("clear");    /* Effettuo una pulizia del terminale */
                    printf("\t––––•(-• \x1b[31mAttenzione\x1b[0m •-)•––––\n");
                    printf("\nLa partita in corso e' terminata ed e' stata generata una nuova sessione di gioco!\n");
                    puts("Premi il punto(\x1b[32m.\x1b[0m) per tornare al Menu!");
                    system ("/bin/stty raw");        /* Chiamata di sistema per modificare il terminale. Esso accettera' i comandi di movimento in input senza che l'utende debba confermare la selezione tramite l'uso dell'invio */
                    while(getchar()!='.');           /* Pulisco lo stream per evitare eventuali carrateri sporchi rimasti */
                    system ("/bin/stty cooked");     /* Riporto il terminale nello stato precendente alla chiamata '/bin/stty raw' */
                    return;
                }
                printf("\nPremi uno dei tasti specificati sopra per muoverti!\n");
                break;
        }
        system ("/bin/stty raw");   /* Chiamata di sistema per modificare il terminale. Esso accettera' i comandi di movimento in input senza che l'utende debba confermare la selezione tramite l'uso dell'invio */
    }
    system ("/bin/stty cooked");   /* Riporto il terminale nello stato precendente alla chiamata '/bin/stty raw' */


    if( write(fd,"P",1) < 0 ) errori_generali(37);                           /* Invio il carattere 'P' al Server per far uscire il giocatore e decrementarne il numero  */
    if( read(fd,&controllo,sizeof(char)) <= 0 ) errori_generali(38);         /* Leggo la flag di constrollo ricevuta dal server in merito alla write sopra */
    if( controllo == 'o') printf("\nUscita dalla partita con successo!\n");  /* Notifico l'utente */
    else printf("\nErrore durante l'uscita dalla partita!\n");
    return;
}


/* Funzione per la gestione di segnali */
void segnalami_le_mele(int num_segnale)
{
    system("clear");            /* Effettuo una pulizia del terminale */
    if( num_segnale==SIGPIPE)   /* Segnale relativo alla scrittura su pipe/socket in cui il client e' stato chiuso/terminato */
    {
        printf("\t––––•(-• \x1b[31mWarning\x1b[0m •-)•––––\n    Il server e' crashato o terminato!\n\n");
        exit(1); /* Terminazione del programma */
    }
    if( num_segnale==SIGINT )   /* Segnale relativo alla cattura di un segnale di interruzione da terminale */
    {
        printf("\t––––•(-• \x1b[31mWarning\x1b[0m •-)•––––\n  Il Client e' stato terminato con CTRL-C!\n\n");
        exit(1); /* Terminazione del programma */
    }
    if( num_segnale==SIGTSTP ) /* Segnale relativo alla cattura di un segnale di suspend da terminale */
    {
        printf("\t––––•(-• \x1b[31mWarning\x1b[0m •-)•––––\n  Il Client e' stato terminato con CTRL-Z!\n\n");
        exit(1); /* Terminazione del programma */
    }
}

/* Funzione per verificare che l'utente abbia inserito corettamente caratteri alfabetici */
int ControlloRigaColonna(char c)
{
    switch(c)
    {
        case 'A':
            return 0;
            break;
        case 'B':
            return 0;
            break;
        case 'C':
            return 0;
            break;
        case 'D':
            return 0;
            break;
        case 'E':
            return 0;
            break;
        case 'F':
            return 0;
            break;
        case 'G':
            return 0;
            break;
        case 'H':
            return 0;
            break;
        case 'I':
            return 0;
            break;
        case 'J':
            return 0;
            break;
        case 'K':
            return 0;
            break;
        case 'L':
            return 0;
            break;
        case 'M':
            return 0;
            break;
        case 'N':
            return 0;
            break;
        case 'O':
            return 0;
            break;
        case 'P':
            return 0;
            break;
        case 'Q':
            return 0;
            break;
        case 'R':
            return 0;
            break;
        case 'S':
            return 0;
            break;
        case 'T':
            return 0;
            break;
        case 'U':
            return 0;
            break;
        case 'V':
            return 0;
            break;
        case 'W':
            return 0;
            break;
        case 'X':
            return 0;
            break;
        case 'Y':
            return 0;
            break;
        case 'Z':
            return 0;
            break;
        default:
            return 1;
            break;
    }
    return 1;
}
