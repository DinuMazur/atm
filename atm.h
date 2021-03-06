//MAZURDINU 313CB
#ifndef HATM
#define HATM
typedef struct THISTORY {
	struct THISTORY *next_history;
	struct TCARD *card_aux;
	char str[17]; //string pentru a memora parametrii eronati si normali
	int exitstatus;
	int operation_id; // dupa id se va intelege ce operatie a fost efectuata
} THISTORY, *LHISTORY;

typedef struct TCARD {
	struct TCARD *next_card;
	THISTORY *next_history;
	unsigned long long card_number;
	unsigned long long balance;
	int pin;
	int cvv;
	int expiry_month;
	int expiry_year;
	int attempts; // numarul curent de incercari la deplocare
	int status;
} TCARD, *LCARD;

typedef struct TLSC {
	struct TLSC *next_lsc;
	TCARD *next_card;
} TLSC, *DB, **ADB; // am optat pentru folosirea calificativului DB in loc de LC
#endif