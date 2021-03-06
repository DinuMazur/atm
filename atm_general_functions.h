//MAZUR DINU 313CB
#ifndef HATM_GENERAL
#define HATM_GENERAL
#define SUCCESS 0
#define FAILURE !SUCCESS
#define BLOCKED -1
#define NEW 0
#define ACTIVE 1
#define MAX_ATTEMPTS 3
#define PIN_DIGITS 4 // numarul curent de cifre in pin


// returneaza lsc -ul de pe poz si daca el lipseste aloca elemente de legatura 
// si il returneaza pe cel de pe poz
DB set_lsc(ADB database_adr, int poz) { // 
	DB lsc_adr_crt = *database_adr, lsc_adr_old, lsc_adr_aux;
	while (poz >= 0) {
		if (!lsc_adr_crt) {
			lsc_adr_aux = (DB)malloc(sizeof(TLSC));
			if (!lsc_adr_aux) {
				return NULL;
			}
			if (!(*database_adr)) {
				*database_adr = lsc_adr_aux;
			} else {
				lsc_adr_old->next_lsc = lsc_adr_aux;
			}
			lsc_adr_crt = lsc_adr_aux;
			lsc_adr_crt->next_lsc = NULL;
			lsc_adr_crt->next_card = NULL;
		}
		lsc_adr_old = lsc_adr_crt;
		lsc_adr_crt = lsc_adr_crt->next_lsc;
		--poz;
	}
	return lsc_adr_old;
}

int get_operation_id(char *operation_str) {
	if (strcmp(operation_str, "add_card") == 0) {
		return 0;
	} else if (strcmp(operation_str, "reverse_transaction") == 0) {
		return 1;
	} else if (strcmp(operation_str, "transfer_funds") == 0) {
		return 2;
	} else if (strcmp(operation_str, "insert_card") == 0) {
		return 3;
	} else if (strcmp(operation_str, "pin_change") == 0) {
		return 4;
	} else if (strcmp(operation_str, "recharge") == 0) {
		return 5;
	} else if (strcmp(operation_str, "cash_withdrawal") == 0) {
		return 6;
	} else if (strcmp(operation_str, "balance_inquiry") == 0) {
		return 7;
	} else if (strcmp(operation_str, "cancel") == 0) {
		return 8;
	} if (strcmp(operation_str, "show") == 0) {
		return 9;
	} else if (strcmp(operation_str, "unblock_card") == 0) {
		return 10;
	} else if (strcmp(operation_str, "delete_card") == 0) {
		return 11;
	}
	return -1;
}

unsigned long long get_ull(char *str) {
	unsigned long long num = 0;
	while (*str && (*str) >= 48 && (*str) <= 58) {
		num = num * 10 + *str - 48;
		++str;
	}
	return num;
}

int is_numeric(char *str) {
	while (*str) {
		if (*str < 48 || *str > 58) {
			return 0;
		}
		++str;
	}
	return 1;
}

void set_date(char *str, int *expiry_month, int *expiry_year) {
	*expiry_month = (int)get_ull(str);
	while (*str && *str != '/') {
		++str;
	}
	++str;
	*expiry_year = (int)get_ull(str);
}

int get_poz(unsigned long long card_number, int nr_max_carduri) {
	int poz = 0;
	while (card_number) {
		poz += card_number % 10;
		card_number /= 10;
	}
	poz %= nr_max_carduri;
	return poz;
}

LCARD get_card(DB lsc_adr_crt, unsigned long long card_number) {
	LCARD card_adr;
	while (lsc_adr_crt) {
		card_adr = lsc_adr_crt->next_card;
			while (card_adr) {
				if (card_adr->card_number == card_number) {
					return card_adr;
				}
				card_adr = card_adr->next_card;
			}
		lsc_adr_crt = lsc_adr_crt->next_lsc;
	}
	return NULL;
}

LCARD aloc_card(unsigned long long card_number, int pin, int cvv,
				int expiry_month,  int expiry_year) {
	LCARD card_adr = (LCARD)malloc(sizeof(TCARD));
	if (!card_adr) {
		return NULL;
	}
	card_adr->next_card = NULL;
	card_adr->next_history = NULL;
	card_adr->balance = 0;
	card_adr->status = 0; //0-new, 1-active, -1-blocked
	card_adr->attempts = 0; //3 is block m8
	card_adr->card_number = card_number;
	card_adr->pin = pin;
	card_adr->cvv = cvv;
	card_adr->expiry_month = expiry_month;
	card_adr->expiry_year = expiry_year;
	return card_adr;
}

//adauga history la card
void add_history(LCARD card_adr, LCARD card_adr_aux, int exitstatus,
					int operation_id, char *str) {
	LHISTORY history_adr_aux = (LHISTORY)malloc(sizeof(THISTORY));
	if (!history_adr_aux) {
		return;
	}
	history_adr_aux->card_aux = card_adr_aux;
	history_adr_aux->exitstatus = exitstatus;
	history_adr_aux->operation_id = operation_id;
	if (str) {
		strcpy(history_adr_aux->str, str);
	} else {
		strcpy(history_adr_aux->str, "\0");
	}
	history_adr_aux->next_history = card_adr->next_history;
	card_adr->next_history = history_adr_aux;
}

void delete_database(DB lsc_adr_crt) {
	DB lsc_adr_ant;
	LCARD card_adr_crt, card_adr_ant;
	LHISTORY history_adr_crt, history_adr_ant;
	while (lsc_adr_crt) {
		card_adr_crt = lsc_adr_crt->next_card;
		while (card_adr_crt) {
			history_adr_crt = card_adr_crt->next_history;
			while (history_adr_crt) {
				history_adr_ant = history_adr_crt;
				history_adr_crt = history_adr_crt->next_history;
				free(history_adr_ant);
			}
			card_adr_ant = card_adr_crt;
			card_adr_crt = card_adr_crt->next_card;
			free(card_adr_ant);
		}
		lsc_adr_ant = lsc_adr_crt;
		lsc_adr_crt = lsc_adr_crt->next_lsc;
		free(lsc_adr_ant);
	}
}

void realloc_database(ADB database_adr, int nr_max_carduri) {
	DB lsc_adr_crt = *database_adr, lsc_adr_ant;
	*database_adr = NULL;
	DB lsc_adr_aux;
	LCARD card_adr_crt, card_adr_next;
	int poz;
	while (lsc_adr_crt) {
		card_adr_crt = lsc_adr_crt->next_card;
		while (card_adr_crt) {
			poz = get_poz(card_adr_crt->card_number, nr_max_carduri);
			lsc_adr_aux = set_lsc(database_adr, poz);
			card_adr_next = card_adr_crt->next_card;
			card_adr_crt->next_card = lsc_adr_aux->next_card;						
			lsc_adr_aux->next_card = card_adr_crt;
			card_adr_crt = card_adr_next;
			
		}
		lsc_adr_ant = lsc_adr_crt;
		lsc_adr_crt = lsc_adr_crt->next_lsc;
		free(lsc_adr_ant);
	}
}
#endif