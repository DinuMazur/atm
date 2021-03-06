//MAZUR DINU 313CB
#ifndef HATM_ADMIN
#define HATM_ADMIN
	#ifndef HATM
	#include "atm.h"
	#define HATM
	#endif
	#ifndef HATM_GENERAL
	#include "atm_general_functions.h"
	#define HATM_GENERAL
	#endif
void reverse_transaction(FILE *log, LCARD card_adr_src, LCARD card_adr_dst,
							char* sum_str) {
	unsigned long long sum = get_ull(sum_str);
	LHISTORY history_adr_crt, history_adr_ant = NULL;
	if (card_adr_dst->balance < sum) {
		fprintf(log, "The transaction cannot be reversed\n");
	} else {
		history_adr_crt = card_adr_dst->next_history;
		while (history_adr_crt) {
			// pentru a diferentia dintre transferul efectuat dinspre si spre
			// am folosit id-uri pentru operatii diferite 13 pentru tranzactii
			// primite si 2 pentru tranzactii efectuate
			if (history_adr_crt && history_adr_crt->operation_id == 13 &&
				history_adr_crt->card_aux == card_adr_src &&
				strcmp(sum_str, history_adr_crt->str) == 0) {
					if (!history_adr_ant) {
						card_adr_dst->next_history =
							history_adr_crt->next_history;
					} else {
						history_adr_ant->next_history =
							history_adr_crt->next_history;
					}
					free(history_adr_crt);
					break;
			}
			history_adr_ant = history_adr_crt;
			history_adr_crt = history_adr_crt->next_history;
		}
		card_adr_dst->balance -= sum;
		card_adr_src->balance += sum;
		add_history(card_adr_src, card_adr_dst, SUCCESS, 1, sum_str);
	}
}

void unblock_card(LCARD card_adr) {
	card_adr->status = ACTIVE;
	card_adr->attempts = 0;
}

void delete_card(DB lsc_adr_crt, LCARD card_adr, int *nr_crt_carduri) {
	LCARD card_adr_crt;
	LHISTORY history_adr_crt = card_adr->next_history, history_adr_nxt;
	while (history_adr_crt) {
		history_adr_nxt = history_adr_crt->next_history;
		free(history_adr_crt);
		history_adr_crt = history_adr_nxt;
	}
	while (lsc_adr_crt) {
		card_adr_crt = lsc_adr_crt->next_card;
		if (card_adr_crt == card_adr) {
			lsc_adr_crt->next_card = card_adr->next_card;
			(*nr_crt_carduri)--;
			free(card_adr);
			return;
		} else if (card_adr_crt) {
			while (card_adr_crt->next_card &&
						card_adr_crt->next_card != card_adr) {
				card_adr_crt = card_adr_crt->next_card;
			}
			if (card_adr_crt->next_card == card_adr) {
				card_adr_crt->next_card = card_adr->next_card;
				(*nr_crt_carduri)--;
				free(card_adr);
				return;
			}
		}
		lsc_adr_crt = lsc_adr_crt->next_lsc;
	}
}

void show_card(FILE *log, LCARD card_adr) {
	LHISTORY history_adr;
	int operation_id;
	fprintf(log, "(card number: %.16llu, ", card_adr->card_number);
	fprintf(log, "PIN: %.4d, ", card_adr->pin);
	fprintf(log, "expiry date: %.2d/%.2d, ", card_adr->expiry_month,
			card_adr->expiry_year);
	fprintf(log, "CVV: %.3d, ", card_adr->cvv);
	fprintf(log, "balance: %llu, ", card_adr->balance);
	fprintf(log, "status: ");
	if (card_adr->status == NEW) {
		fprintf(log, "NEW, ");
	} else if (card_adr->status == BLOCKED) {
		fprintf(log, "LOCKED, ");
	} else if (card_adr->status == ACTIVE) {
		fprintf(log, "ACTIVE, ");
	}
	history_adr = card_adr->next_history;
	fprintf(log, "history: [");
	while (history_adr) {
		operation_id = history_adr->operation_id;
		if (history_adr->exitstatus == SUCCESS) { 
			fprintf(log, "(SUCCESS, ");
		} else {
			fprintf(log, "(FAIL, ");
		}
		//dupa id se intelege care operatie trebuie afisata
		if (operation_id == 1) {
			fprintf(log, "reverse_transaction ");
		} else if (operation_id == 2 || operation_id == 13) {
			fprintf(log, "transfer_funds ");
		} else if (operation_id == 3) {
			fprintf(log, "insert_card ");
		} else if (operation_id == 4) {
			fprintf(log, "pin_change ");
		} else if (operation_id == 5) {
			fprintf(log, "recharge ");
		} else if (operation_id == 6) {
			fprintf(log, "cash_withdrawal ");
		} else if (operation_id == 7) {
			fprintf(log, "balance_inquiry ");
		} else if (operation_id == 8) {
			fprintf(log, "cancel ");
		}
		if (operation_id == 13) {
			//foloses %.16llu pentru a afisa numarul pe 16 pozitii
			fprintf(log, "%.16llu", history_adr->card_aux->card_number);
			fprintf(log, " %.16llu", card_adr->card_number);
		} else if (operation_id == 1 || operation_id == 2) {
			fprintf(log, "%.16llu", card_adr->card_number);
			fprintf(log, " %.16llu", history_adr->card_aux->card_number);
		} else {
			fprintf(log, "%.16llu", card_adr->card_number);
		}
		if (*(history_adr->str) != 0) {
			fprintf(log, " %s", history_adr->str);
		}
		fprintf(log, ")");
		history_adr = history_adr->next_history;
		if (history_adr) {
			fprintf(log, ", ");
		}
	}
	fprintf(log, "])\n");
}

void show(FILE *log, DB lsc_adr_crt) {
	int pos = 0;
	LCARD card_adr;
	while (lsc_adr_crt) {
		fprintf(log, "pos%d: [", pos);
		if (lsc_adr_crt->next_card) {
			fprintf(log, "\n");
			card_adr = lsc_adr_crt->next_card;
			while (card_adr) {
				show_card(log, card_adr);
				card_adr = card_adr->next_card;
			}
		}
		fprintf(log, "]\n");
		++pos;
		lsc_adr_crt = lsc_adr_crt->next_lsc;
	}
}

void add_card(FILE *log, ADB database_adr, unsigned long long card_number,
				int *nr_max_carduri, int *nr_crt_carduri, char *pin_str,
				int cvv, int expiry_month, int expiry_year) {
	DB lsc_adr_crt;
	LCARD card_adr_aux;
	int pin = (int)get_ull(pin_str);
	int poz;
	if (get_card(*database_adr, card_number)) {
		fprintf(log, "The card already exists\n");
		return;
	}
	if (*nr_max_carduri < *nr_crt_carduri + 1) {
		(*nr_max_carduri) *= 2;
		realloc_database(database_adr, *nr_max_carduri);
	}
	poz = get_poz(card_number, *nr_max_carduri);
	card_adr_aux = aloc_card(card_number, pin, cvv, expiry_month, expiry_year);
	lsc_adr_crt = set_lsc(database_adr, poz);
	if (!lsc_adr_crt) {
		fprintf(log, "The card was not added\n");
		return;
	}
	if (!card_adr_aux) {
		fprintf(log, "The card was not added\n");
		return;
	}
	(*nr_crt_carduri)++;
	card_adr_aux->next_card = lsc_adr_crt->next_card;
	lsc_adr_crt->next_card = card_adr_aux;
}
#endif