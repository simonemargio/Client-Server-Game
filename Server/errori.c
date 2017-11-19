#include "errori.h"

void errori_generali(int err)
{
    //perror("\033[00;36m\ Tipo:\033[00m Errore lettura 'Numero di oggetti rimanenti' della funzione 'Gioca' relativo alla funzionalita' 'Numero oggetti rimanenti'!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il client. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");

    puts("\033[01;31m* FATAL ERROR *\033[00m");
    switch(err)
    {
        case 0:
            perror ("\033[00;36m\ Tipo:\033[00m Errore dati input!\n\033[00;36m\ Ulteriori info:\033[00m Il Server deve necesariamente essere eseguito tramite linea di comando specificando soltanto il numero della porta che si vuole usare.\n\033[00;36m\ Possibile soluzione:\033[00m Si prega di specificare il numero di porta che usera' il server!\n");
            break;
        case 1:
            perror("\033[00;36m\ Tipo:\033[00m Errore apertura file 'Log.txt'!\n\033[00;36m\ Ulteriori info: Impossibile stabilire un canale di comunicazione e/o creare il file.\033[00m I .\n\033[00;36m\ Possibile soluzione:\033[00m Verificare che si dispone dei permetti necessari. Controllare che il file non sia corrotto. Rieseguire il Server. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 2:
            perror("\033[00;36m\ Tipo:\033[00m Errore scrittura caratteri all'interno del file 'Log.txt'!\n\033[00;36m\ Ulteriori info:\033[00m Impossibile stabilire un canale di comunicazione con il file per tracrivere informazioni.\n\033[00;36m\ Possibile soluzione:\033[00m Verificare che si dispone dei permetti necessari. Controllare che il file non sia corrotto. Rieseguire il Server. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
            break;
        case 3:
            perror("\033[00;36m\ Tipo:\033[00m Impossibile assegnare un indirizzo al socket!\n\033[00;36m\ Ulteriori info:\033[00m Serve per specificare sul lato server la porta (e gli eventuali indirizzi locali) su cui poi ci si porrà in ascolto.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il server verificando la correttezza della porta. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 4:
            perror("\033[00;36m\ Tipo:\033[00m Errore funzione listen!\n\033[00;36m\ Ulteriori info:\033[00m Errore dovuto a: l'argomento sockfd non è un file descriptor valido - L'argomento sockfd non è un socket - Il socket è di un tipo che non supporta questa operazione.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il server e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 5:
            perror("\033[00;36m\ Tipo:\033[00m Errore spostamento puntatore nel file 'log.txt'!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno di un numero negativo non corrispondente allo spostamento del puntatore all'inizio del file.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il server, cancellare il file 'log.txt' presente nella cartella princiapale dove riside il programma e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n ");
            break;
        case 6:
            perror("\033[00;36m\ Tipo:\033[00m Errore lettura flag di controllo 'Funzione principale' relativo alle possibili scelti di menu' nel client!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il Server. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
            break;
        case 7:
            perror("\033[00;36m\ Tipo:\033[00m Errore apertura file 'database.txt'!\n\033[00;36m\ Ulteriori info:\033[00m  Errore relativo alla funzionalita' 'case 1' nella 'Funzione principale'.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il Server, controllare se si dispone dei relativi permessi per aprire il file. Verificare la non presenza di carratteri sporchi. Per evitare problemi si consiglia di cancellare il file e riavviare il Server. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 8:
            perror("\033[00;36m\ Tipo:\033[00m Errore lettura username relativo a 'Funzione principale' per il controllo Username/Password!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi letti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Attendere un paio di minuti e riavviare il Server. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
            break;
        case 9:
            perror("\033[00;36m\ Tipo:\033[00m Errore invio flag di 'errore' relativo alla funzionalita' Username/Nickname!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi scritti non corrispondente a quello inviato.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il Server e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n ");
            break;
        case 10:
             perror("\033[00;36m\ Tipo:\033[00m Errore scrittura nel file 'log.txt' dell'infomrazioni relative al login errato di un utente!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi scritti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Verificare che il file 'log'.txt disponga dei giusti permessi. Attendere un paio di minuti e riavviare il Server. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
             break;
        case 11:
             perror("\033[00;36m\ Tipo:\033[00m Errore scrittura nel file 'log.txt' dell'infomrazioni relative al login corretto di un utente!\n\033[00;36m\ Ulteriori info:\033[00m Numero di elementi scritti non corrispondenti.\n\033[00;36m\ Possibile soluzione:\033[00m Controllare la propria connessione internet. Verificare che il file 'log'.txt disponga dei giusti permessi. Attendere un paio di minuti e riavviare il Server. Se il problema persiste contattare l'amministratore di sistema.\n\033[00m\n");
             break;
        case 12:
            perror("\033[00;36m\ Tipo:\033[00m Errore invio al client del carattere di controllo per la verifica corretta dell'Username!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi scritti non corrispondente al carattere scritto.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il Server e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 13:
            perror("\033[00;36m\ Tipo:\033[00m Errore lettura dei caratteri della funzione 'Accedi' relativi alla lettura della Password!\n\033[00;36m\ Ulteriori info:\033[00m Ritorno del numero di elementi letti non corrispondendi ai caratteri scritti dal Client.\n\033[00;36m\ Possibile soluzione:\033[00m Riavviare il Server e riprovare. Se il problema persiste contattare l'amministratore di sistema.\n");
            break;
        case 14:

          // DA QUI
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
        default:
             perror("\033[00;36m\ Tipo:\033[00m Sconosciuto!\n\033[00;36m\ Possibile soluzione:\033[00m Rieseguire il client. Se il problema persiste contattare l'amministratore di sistema.\n");
             break;
    }

    puts("\n\n");
    exit(1);
    return;
}
