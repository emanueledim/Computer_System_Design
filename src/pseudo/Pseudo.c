int LOCK = 0;			//Variabile per la mutua esclusione
int POSS = 0;			//Indica chi possiede la risorsa (1 se è di B, 2 se è di C, 0 libera)

int BICHAR = 0;			//B Index Character: indice (e contatore) dei caratteri del singolo messaggio ricevuti da B.
int BIMSG = 0;			//B Index Message: indice (e contatore) dei messaggi ricevuti da B nell'iterazione.
char BMSG[2];			//B Message: buffer dove memorizzare temporaneamente i caratteri del messaggio ricevuti da B.

int CICHAR = 0;			//C Index Character: indice (e contatore) dei caratteri del singolo messaggio ricevuti da C.
int CIMSG = 0;			//C Index Message: indice (e contatore) dei messaggi ricevuti da C nell'iterazione.
char CMSG[2];			//C Message: buffer dove memorizzare temporaneamente i caratteri del messaggio ricevuti da C.

int RCVMSG = 0;			//Received Messages: contatore dei messaggi ricevuti in una certa iterazione.
int ITER = 0;			//Iterazioni: contatore delle iterazioni terminate.

int BMAXMSG	= 2;		//B Max Messages: indica il numero massimo di messaggi inviabili da B in una iterazione.
int CMAXMSG	= 1; 		//C Max Messages: indica il numero massimo di messaggi inviabili da C in una iterazione.
int K = 4;				//Numero delle iterazioni da svolgere
int N = 2;				//Numero di caratteri di cui è composto un messaggio

char STACKMSG[24]; 		//Stack Messages: area di memoria dove verranno memorizzati i messaggi completi. K*(BMAXMSG+CMAXMSG)
int IMSG = 0;			//Index Message: indice (e contatore) dei messaggi memorizzati in Stack Messages.

const CPIAD = 0x2004;	//C PIA Dato/Direzione
const CPIAD = 0x2005;	//C PIA Controllo

const CPIAD = 0x2008;	//B PIA Dato/Direzione
const CPIAD = 0x2009;	//B PIA Controllo

int main(){
	INITPIA();			//Inizializza le due PIA
	Smascheramento interrupt;	//Smaschera gli interrupt delle PIA
	while(1);			//Loop
}

//Inizializzazione delle due pia:
void INITPIA(){
	Reset del registro di controllo di B;
	Reset del registro D (Tutti 0 -> PIA in ricezione) di B;
	Set dei bit del registro di controllo per abilitare la IRQ di B;
	Lettura fittizia di B;
	
	Reset del registro di controllo di C;
	Reset del registro D (Tutti 0 -> PIA in ricezione) di C;
	Set dei bit del registro di controllo per abilitare la IRQ di C;
	Lettura fittizia di C;
}

//Interruzione di C di livello 3
CISR(){
	if(TAS(LOCK)){			//Tentativo di accesso alla sezione critica
		if(POSS != B){		//Se il possesso non è di B
			POSS = C;		//allora C lo acquisisce.
		}else{
			RTE;			//altrimenti C si sospende
		}
	}else{
		RTE;				//Se C non riesce ad accedere alla sezione critica, si sospende
	}
	LOCK = 0; 				//C lascia la sezione critica
	
	if(CIMSG == CMAXMSG){	//Se C ha già inviato tutti i messaggi previsti nell'iterazione
		POSS = 0;			//Libero la risorsa (per evitare le situazioni di deadlock)
		if(B è sospeso?){	//e controlla se B è sospeso. Se lo è
			POSS = B;		//assegna il possesso a B
			BHandle();		//e chiama il suo handler per poterlo leggere.
		}
	}else{					//Se C riesce ad acquisire il possesso e se non ha inviato tutti i messaggi dell'iterazione
		CHandle();			//allora chiama l'handler per poter leggere C.
	}
	RTE;					//Uscita dalla ISR
}

CHandle(){					//Handler di C
	Salvataggio contesto;	//Salvataggio del contesto
		
	Leggi C;				//Lettura del dato dalla PIA di C.
	CICHAR = CICHAR+1;		//Incremento il numero di caratteri letti.
	if(CICHAR == N){		//Se sono stati ricevuti tutti i caratteri del messaggio:
		CICHAR = 0;			//resetta il contatore dei caratteri
		CIMSG = CIMSG+1;	//incrementa il contatore dei messaggi
		OR(CMSG, carattere letto);	//concatena il carattere letto col resto del messaggio nel buffer CMSG.
		Salvataggio CMSG in STACKMSG+(2*IMSG);	//Sposta il messaggio nello stack+spiazzamento
		RCVMSG = RCVMSG+1;	//incrementa il numero di messaggi ricevuti in questa iterazione
		if(RCVMSG == 3){	//e se sono stati ricevuti tutti e 3 i messaggi, allora
			BIMSG = 0;		//resetta tutte le variabili di conteggio.
			CIMSG = 0;
			BICHAR = 0;
			CICHAR = 0;
			RCVMSG = 0;
			ITER = ITER+1;	//incrementa la variabile dell'iterazione
			if(ITER == K){	//Se sono state completate tutte le iterazioni, disabilita le PIA
				Disabilita PIA B;
				Disabilita PIA C;
			}
		}
		POSS = 0;			//Prima di controllare se B è sospeso, libero la risorsa (per evitare il deadlock)
		if(B è sospeso?){	//se B è sospeso
			POSS = B;		//allora assegnagli il possesso
			BHandle();		//e chiama il suo handler (per poter leggere B)
		}
	}else{					//Altrimenti se non sono stati ricevuti tutti i caratteri del messaggio
		Shift del carattere letto di 1 byte;	//shifta il carattere letto
		Salvataggio del carattere shiftato in BMSG;	//e salvalo nel buffer.
		return;				//uscita dall'handler
	}

	Ripristino contesto;	//Ripristino del contesto
	return;					//uscita dall'handler.
}

//Interruzione di B di livello 4
BISR(){
	if(TAS(LOCK)){			//Tentativo di accesso alla sezione critica
		if(POSS != C){		//Se il possesso non è di C
			POSS = B;		//allora B lo acquisisce.
		}else{
			RTE;			//altrimenti B si sospende
		}
	}else{
		RTE;				//Se B non riesce ad accedere alla sezione critica, si sospende
	}
	LOCK = 0;				//B lascia la sezione critica
	
	if(BIMSG == BMAXMSG){	//Se B ha già inviato tutti i messaggi previsti nell'iterazione
		POSS = 0;			//libero la risorsa (oss: B non potrà mai essere interrotto da C)
	}else{					//Se B riesce ad acquisire il possesso e se non ha inviato tutti i messaggi dell'iterazione
		BHandle();			//allora chiama l'handler per poter leggere B.
	}
	RTE;					//Uscita dalla ISR
}

BHandle(){					//Handler di B
	Salvataggio contesto;	//Salvataggio del contesto
		
	Leggi B;				//Lettura del dato dalla PIA di B.
	BICHAR = BICHAR+1;		//Incremento il numero di caratteri letti.
	if(BICHAR == N){		//Se sono stati ricevuti tutti i caratteri del messaggio:
		BICHAR = 0;			//resetta il contatore dei caratteri
		BIMSG = BIMSG+1;	//incrementa il contatore dei messaggi
		OR(BMSG, carattere letto);	//concatena il carattere letto col resto del messaggio nel buffer BMSG.
		Salvataggio BMSG in STACKMSG+(2*IMSG);	//Sposta il messaggio nello stack+spiazzamento
		RCVMSG = RCVMSG+1;	//incrementa il numero di messaggi ricevuti in questa iterazione
		if(RCVMSG == 3){	//e se sono stati ricevuti tutti e 3 i messaggi, allora
			BIMSG = 0;		//resetta tutte le variabili di conteggio.
			CIMSG = 0;
			BICHAR = 0;
			CICHAR = 0;
			RCVMSG = 0;
			ITER = ITER+1;	//incrementa la variabile dell'iterazione
			if(ITER == K){	//Se sono state completate tutte le iterazioni, disabilita le PIA
				Disabilita PIA B;
				Disabilita PIA C;
			}
		}
		POSS = 0;			//Prima di controllare se C è sospeso, libero la risorsa (per evitare il deadlock).
		if(C è sospeso?){	//se C è sospeso
			POSS = C;		//allora assegnagli il possesso
			CHandle();		//e chiama il suo handler (per poter leggere C)
		}
	}else{					//Altrimenti se non sono stati ricevuti tutti i caratteri del messaggio
		Shift del carattere letto di 1 byte;	//shifta il carattere letto
		Salvataggio del carattere shiftato in BMSG;	//e salvalo nel buffer.
		return;				//uscita dall'handler
	}

	Ripristino contesto;	//Ripristino del contesto
	return;					//uscita dall'handler.
}