# Computer_System_Design
Progetto in assembly (m68K) per l'esame Computer System Design
## Descrizione del Progetto
Questo repository contiene un progetto scritto in Assembly che implementa un sistema di comunicazione sincronizzata tra tre nodi identificati come A, B e C. Il nodo A riceve dei caratteri dai nodi B e C in una sequenza prestabilita e ripetitiva. I caratteri sono inviato mediante un canale parallelo (PIA). La comunicazione segue un ordine specifico di ricezione:
* BBC
* BCB
* CBB
  
L'implementazione utilizza un sistema di lock nel nodo A per garantire che i caratteri siano ricevuti in quest'ordine prefissato, mantenendo la consistenza ed evitare situazioni di deadlock.
## Configurazione della memoria
M68k 1: C <br />
M68k 4: B <br />
M68k 7: A

Indirizzi delle ISR del ricevitore: <br />
CISR: $8500 in 0x6C <br />
BISR: $8600 in 0x70

Contenuto RAM con test case: <br />
1) BBC <br />
2) BCB <br />
3) CBB

Indirizzi dei device (versione terminale IT): <br />
Terminale 0xB e 0xC: <br />
Interrupt Enter: $8500 in 0x64 <br />
Interrupt buffer pieno: $8600 in 0x68

Trasmettitori con ACK PIA: <br />
$8300 in 0x6C <br />
$8400 in 0x70

## Struttura del Progetto
`RicevitoreA.a68`: Implementa la logica del nodo ricevitore A che deve garantire la sequenza prestabilita di messaggi. <br />
`src/TrasmettitoreB.a68`: Implementa la logica del nodo B, il quale invia dei caratteri prestabiliti, ogni volta che A legge il dato dalla PIA. <br />
`src/Trasmettitore_B_IT.a68`: Implementa la logica del nodo B, il quale invia dei caratteri prestabiliti, mediante una ISR scatenata con la lettura della PIA da parte di A. <br />
`src/TrasmettitoreC.a68`: Implementa la logica del nodo C, il quale invia dei caratteri prestabiliti, ogni volta che A legge il dato dalla PIA. <br />
`src/Trasmettitore_C_IT.a68`: Implementa la logica del nodo C, il quale invia dei caratteri prestabiliti, mediante una ISR scatenata con la lettura della PIA da parte di A. <br />
`src/TrasmettitoreTERM.a68`: Implementa la logica dei nodi B e C, che permette all'utente di inviare dei caratteri mediante terminale. <br />

`src/pseudo/Pseudo.c`: Implementa la logica del ricevitore A scritta in linguaggio pseduo-c. <br />

`src/cfg/3nodi_PIA.cfg`: File da utilizzare con l'emulatore che configura i 3 nodi (Processore, memoria, dispositivi PIA). <br />
`src/cfg/3nodi_PIA_terminale`: File da utilizzare con l'emulatore che configura i 3 nodi (Processore, memoria, dispositivi PIA e terminali). <br />

`src/exe/asim.exe`: Emulatore del sistema basato sul M68K. <br />
`src/exe/asimtool.exe`: IDE per scrivere e compilare il codice assembly M68K. <br />
