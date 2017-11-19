#include "errori.h"

void errori_generali(int err)
{
    puts("\033[01;31m* FATAL ERROR *\033[00m");
    switch(err)
    {
        case 0:
            perror ("\033[00;36m\ Tipo:\033[00m Errore dati input!\n\033[00;36m\ Ulteriori info:\033[00m Il client deve necesariamente essere eseguito tramite linea di comando specificando porta e IP.\n\033[00;36m\ Possibile soluzione:\033[00m Si prega di specificare il numero di porta e l'IP del server a cui connettersi!\n");
            break;
        case 1:
            perror("\033[00;36m\ Tipo:\033[00m Errore funzione Inet_aton!\n\033[00;36m\ Ulteriori info:\033[00m Inet_aton converte una stringa nell'indirizzo binario che viene memorizzato in un'opportuna struttura.\n\033[00;36m\ Possibile soluzione:\033[00m Rieseguire il client. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 2:
            perror("\033[00;36m\ Tipo:\033[00m Errore creazione socket!\n\033[00;36m\ Ulteriori info:\033[00m Errore dovuto a: Il kernel non ha memoria sufficiente a creare una nuova struttura per il socket - Si e' ecceduta la tabella dei file - Non si hanno privilegi per creare un socket nel dominio o con il protocollo specificato - Protocollo sconosciuto o dominio non disponibile - Non c'e' sufficiente memoria per creare il socket.\n\033[00;36m\ Possibile soluzione:\033[00m Vereficare la propria connessione internet. Attendere un paio di minuti e riavviare il client verificando la correttezza dell'Ip e porta a cui connettersi. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 3:
            perror("\033[00;36m\ Tipo:\033[00m Impossibile stabilire una connessione fra due socket!\n\033[00;36m\ Ulteriori info:\033[00m Errore dovuto a: Mon c'e' nessuno in ascolto sull'indirizzo remoto - Si e' avuto timeout durante il tentativo di connessione - La rete non e' raggiungibile - Il socket non e' bloccante e la connessione non puo' essere conclusa immediatamente o un tentativo precedente di connessione non si e' ancora concluso - Non ci sono più porte locali libere - L'indirizzo non ha una famiglia di indirizzi corretta nel relativo campo.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client verificando la correttezza dell'Ip e porta a cui connettersi. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 4:
              perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione della funzionalita' 'Accedi'!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi scritti non corrispondente al carattere scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 5:
             perror("\033[00;36m\ Tipo:\033[00m Errore invio Username/Nickname al Server relativo alla funzionalita' 'Accedi'!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi scritti non corrispondente a quello inviato.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n ");
            break;
        case 6:
            perror("\033[00;36m\ Tipo:\033[00m Errore lettura flag di controllo Username/Nickname relativo alla funzionalita' 'Accedi'!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
            break;
        case 7:
            perror("\033[00;36m\ Tipo:\033[00m Errore invio Password relativo alla funzionalita' 'Accedi'!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi scritti non corrispondente a quello inviato.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 8:
            perror("\033[00;36m\ Tipo:\033[00m Errore lettura flag di controllo Password relativo alla funzionalita' 'Accedi'!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
            break;
        case 9:
            perror("\033[00;36m\ Tipo:\033[00m Errore invio Username/Nickname al Server relativo alla funzionalita' 'Registrati'!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi scritti non corrispondente a quello inviato.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n ");
            break;
        case 10:
             perror("\033[00;36m\ Tipo:\033[00m Errore lettura flag di controllo Password relativo alla funzionalita' 'Registrati'!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
             break;
        case 11:
             perror("\033[00;36m\ Tipo:\033[00m Errore invio Password al Server relativo alla funzionalita' 'Registrati'!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi scritti non corrispondente a quello inviato.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n ");
            break;
        case 12:
              perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione della funzionalita' 'Registrati'!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi scritti non corrispondente al carattere scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 13:
             perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione della funzionalita' 'Exit'!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi scritti non corrispondente al carattere scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 14:
             perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione della funzionalita' 'Gioca'!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi scritti non corrispondente al carattere scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 15:
             perror("\033[00;36m\ Tipo:\033[00m Errore lettura flag di controllo 'Gioca' relativo alla funzionalita' 'Accedi->Gioca'!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
             break;
        case 16:
             perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione della funzionalita' 'Utenti collegati'!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi scritti non corrispondente al carattere scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 17:
             perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione della funzionalita' 'Vedi oggetti rimanenti'!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi scritti non corrispondente al carattere scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 18:
             perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione della funzionalita' 'Vedi oggetti trovati'!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi scritti non corrispondente al carattere scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 19:
             perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione della funzionalita' 'Torna indietro'!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi scritti non corrispondente al carattere scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 20:
             perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione relativa alla 'Riga' della matrice di gioco!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi scritti non corrispondente al carattere scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 21:
            perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione relativa alla 'Colonna' della matrice di gioco!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi scritti non corrispondente al carattere scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 22:
             perror("\033[00;36m\ Tipo:\033[00m Errore lettura flag di controllo 'Inserimento Riga-Colonna' relativo alla funzionalita' 'Gioca'!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
             break;
        case 23:
             perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione relativa a 'Stampa lista utenti' della funzionalità 'Gioca'!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi scritti non corrispondente al carattere scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 24:
            perror("\033[00;36m\ Tipo:\033[00m Errore lettura controllo 'Indici' relativo alla funzionalita' 'Stampa lista utenti' della funzionalità 'Gioca'!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
            break;
        case 25:
             perror("\033[00;36m\ Tipo:\033[00m Errore valore 'Contatore' nella funzione 'Stampa lista utenti' della funzionalità 'Gioca'!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
             break;
        case 26:
             perror("\033[00;36m\ Tipo:\033[00m Errore lettura 'Numero di oggetti rimanenti' della funzione 'Gioca' relativo alla funzionalita' 'Numero oggetti rimanenti'!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
             break;
        case 27:
             perror("\033[00;36m\ Tipo:\033[00m Errore lettura 'Numero di oggetti trovati' della funzione 'Gioca' relativo alla funzionalita' 'Numero oggetti trovati'!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
             break;
        case 28:
            perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione relativa a 'Controllo movimenti' per abilitare il giocatore nella scelta delle posizioni!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi scritti non corrispondente al carattere stesso scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 29:
            perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione relativa a 'Accedi->Gioca' per portare il giocatore nella matrice di gioco!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi scritti non corrispondente al carattere stesso scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 30:
            perror("\033[00;36m\ Tipo:\033[00m Errore lettura flag di controllo della funzione 'Accedi->Gioca'!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
            break;
        case 31:
            perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione relativa a 'Gioca->L' per mostrare la relativa lista utenti e gli oggetti trovati da ciascuno!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi scritti non corrispondente al carattere stesso scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 32:
            perror("\033[00;36m\ Tipo:\033[00m Errore lettura flag di controllo della funzione 'Gioca->L'!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
            break;
        case 33:
             perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione relativa a 'Gioca' per richiedere al Server di inviare la mappa di gioco!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi scritti non corrispondente al carattere stesso scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 34:
            perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione relativa a 'Gioca' per richiedere al Server l'invio della lista degli utenti con i relativi oggetti!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi scritti non corrispondente al carattere stesso scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 35:
            perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione relativa a 'Gioca' per richiedere al Server di verificare se la partita e' terminata o no!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi scritti non corrispondente al carattere stesso scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 36:
            perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione relativa a 'Gioca' per richiedere al Server  per richiedere il numero di oggetti rimanenti!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi scritti non corrispondente al carattere stesso scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 37:
            perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione relativa a 'Gioca' per richiedere al Server di far uscire il giocatore e di decrementarne il numero!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi scritti non corrispondente al carattere stesso scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 38:
            perror("\033[00;36m\ Tipo:\033[00m Errore lettura flag di controllo della funzione 'Gioca' per decrementare il numero di giocatori totali!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
            break;
        case 39:
            perror("\033[00;36m\ Tipo:\033[00m Errore lettura flag di controllo della funzione 'Gioca' per richiedere al Server di verificare se la partita e' terminata o no!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
            break;
        case 40:
            perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione relativa a 'Gioca' per verificare e notificare lo spostamento da effettuare al Server!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi scritti non corrispondente al carattere stesso scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 41:
             perror("\033[00;36m\ Tipo:\033[00m Errore lettura flag di controllo della funzione 'Gioca' per verificare e notificare lo spostamento da effettuare al Server!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
            break;
        case 42:
            perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione relativa a 'Gioca' per dire al server di spostare il giocatore in una delle quattro direzioni!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi scritti non corrispondente al carattere stesso scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 43:
            perror("\033[00;36m\ Tipo:\033[00m Errore lettura flag di controllo della funzione 'Gioca' per dire al server di spostare il giocatore in una delle quattro direzioni!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
            break;
        case 44:
            perror("\033[00;36m\ Tipo:\033[00m Errore lettura nome oggetto trovato dal giocatore relativo alla funzione 'Gioca'!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
            break;
       case 45:
            perror("\033[00;36m\ Tipo:\033[00m Errore invio flag di controllo della funzione 'Gioca' per richiedere al Server di verificare se la partita e' terminata o no!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
            break;
        case 46:
            perror("\033[00;36m\ Tipo:\033[00m Errore lettura flag di controllo della funzione 'Gioca' per controllare se la partita sia finita!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
            break;
        case 47:
            perror("\033[00;36m\ Tipo:\033[00m Errore invio carattere per la selezione relativa a 'Gioca->Richiesta tempo' per far ottenere dal server le informazioni riguardanti il tempo trascorso!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi scritti non corrispondente al carattere stesso scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il client e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 48:
            perror("\033[00;36m\ Tipo:\033[00m Errore lettura intero della funzione 'Gioca->Richiesta tempo' per controllare il tempo trascorso di una partita!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
            break;

        default:
             perror("\033[00;36m\ Tipo:\033[00m Sconosciuto!\n\033[00;36m\ Possibile soluzione:\033[00m Rieseguire il client. Se il problema persiste contattare l'amministratore di sistema.\n");
             break;
    }

    puts("\n\n");
    exit(1);
    return;
}



int controlloSocket(int fd)
{
    if( fd<0 )
    {
        perror("Errore socket:: errore in fd=socket(AF_INET,SOCK_STREAM,0)!!!!!!");
        return -3;
    }
    return 0;
}

int controlloIndirizzo(char str[])
{
    fflush(stdin);
    char *s = NULL, app[strlen(str)];
    int errore = 0, numero_iterazioni = 0;
    s = strtok(str,".");
    printf("\nstr: %d\ns: %d\n",strlen(str),strlen(s));
    if( strlen(s) >= 4 && strlen(s) <= 0 )
    {
        errore = -4; //Non ci sono punti. Esempio: 00000000000000000000000000000000
        puts("Err cazz");
    }
    else
    {
        while( errore != -1 && s != NULL )
        {
            strcpy(app,s);
            if( atoi(app) > 255 || atoi(app) < 0 )
            {
                errore = -5; //Un campo è minore di 0 oppure maggiore di 255
                puts("Err 5555");
            }
            s = strtok(NULL,".");
            numero_iterazioni++;
        }
    }
    printf("\nIterazioni: %d\n",numero_iterazioni);
    return errore;
}

