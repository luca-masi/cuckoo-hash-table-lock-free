# cuckoo-hash-table-lock-free
sviluppo di una cuckoo hash table lock-free multireader e singlewriter.
la cuckoo hash table è una tabella hash a indirizzamento aperto cioè quando due chiavi collidono vengono riposizionate in posizioni alternative.
Le hash table servono per mettere in corrsipondenza una data chiave con il suo realtivo valore.
La chiave è un identificativo univoco, in questo progetto è la 4-tupla formata da indirizzo sorgente e destinazione, porta sorgente e destinazione che servono ad identificare un pacchetto dati.
Le hash table sono le strutture dati usate negli algoritmi di ricerca basati su hashing e risultano particolarmente veloci e ottime per le ricerche in grandi quantità di dati, perchè la ricerca è costante indipendentemente dal numero di elementi che popolano la tabella.
l'hashing è un funzione matematica che converte una sequenza di bit di lughezza arbitraria (ovvero la chiave) in una stringa detta "hash" ovvero una sequenza di bit di lunghezza predefinita che sarà usata per indirizzare il valore nella tabella.
Il CRC32 è la funzione di hashing usata in questo lavoro, viene calcolato sfruttando la funzione: unsigned int _mm_crc32_u32(unsigned int crc, unsigned char data) messa a disposizone da intel per i processori a 64bit.
La versione della cuckoo sviluppata usa due tabelle per il riposizonamento delle chiavi, ogni tabella ha una funzione di hash associata con cui calcolare l'hash. Ogni bucket della tabella ha 4 slot a disposizione in cui memorizzare la coppia chiave-valore, questo perchè cosi la tabella puo raggiungere un load factor del 90%. Quando si verifica una collisione, viene estratto uno slot a caso nel bucket in cui si è verificata e riposizonata nell'altra tabella.
Le operazioni implementate sulla cuckoo sono: INSERT, LOOKUP, DELETE e UPDATE, tali operazioni sono tutte atomiche e quindi eseguibili in contemporanea su un applicazioni multithread.

# hashFunct.cpp
il file contiene l'implementazione hardware e software per il calcolo del "hash" con il CRC32, la funzione hash(..) restituisce appunto l'indirizzo della tabella in cui memorizzare l'elemento (coppia chiave-valore).

# cuckooHash.cpp
in questo file sono presenti le operazioni di:
1)INSERT: inserimento di un elemento nella cuckoo
2)LOOKUP: lettura di un valore associato ad una chiave
3)UPDATE: aggiornamento di un valore associato ad una chiave
4)DELETE: cancellazione di una chiave dalla cuckoo
INSERT sfrutta le funzioni atomiche: bool _atomic_compare_exchange (type *ptr, type *expected, type *desired, bool
weak, int success_memorder, int failure_memorder); e void _atomic_exchange (type *ptr, type *val, type *ret, int memorder) basate sull'istruzione messa a disposizone da intel: CMPXCHG16B per il compare and exchange con word a 16bit (al compilatore occorre indicare l'opzione -mcx16).
LOOKUP effettua una doppia lettura tramite la funzione ckh_getVal(..) che restituisce una struct indexTable con cui vengono confrontate le informazioni di valore e posizione per capire se il valore letto è quello associato alla giusta chiave.
DELETE e UPDATE usando il LOOKUP atomico per aggiornare o eliminare la giusta chiave.

# chkwrt_read.cpp
questo file usa le funzioni implementate nel file cuckooHash.cpp per realizzare un applicazione multithread che scrive e legge nella cuckoo, in particolare le funzioni write_all_incuckoo(..) e read_all_tocuckoo(..) servono appunto a scrivere e leggere un tot numero di elementi nella cuckoo.
__in scrittura essendo presente una sola vittima è possibile eseguire un massimo di un thread alla volta che va ad inserire chiavi.
__in lettura invece è multireader cioè possiamo lanciare più thread che vanno a leggere un valore dalla cuckoo.

# main.cpp
nel main di questo progetto, viene prima allocata e inizializzata la cuckoo hash table e poi vengono eseguiti diversi test per valutare le prestazioni della cuckoo. 
