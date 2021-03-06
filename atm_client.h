//MAZUR DINU 313CB
#ifndef HATM_CLIENT
#define HATM_CLIENT
	#ifndef HATM
	#include "atm.h"
	#define HATM
	#endif
	#ifndef HATM_GENERAL
	#include "atm_general_functions.h"
	#define HATM_GENERAL
	#endif
void insert_card(FILE *log, LCARD card_adr, char *pin_str) {
	int exitstatus = SUCCESS;
	if (card_adr->status == BLOCKED) {
		fprintf(log,
				"The card is blocked. Please contact the administrator.\n");
		exitstatus = FAILURE;
	} else if (!is_numeric(pin_str) || strlen(pin_str) != PIN_DIGITS ||
				card_adr->pin != (int)get_ull(pin_str)) {
		fprintf(log, "Invalid PIN\n");
		++(card_adr->attempts);
		if (!(card_adr->attempts < MAX_ATTEMPTS)) {
			card_adr->status = BLOCKED;
			fprintf(log,
					"The card is blocked. Please contact the administrator.\n");
		}
		exitstatus = FAILURE;
	} else {
		if (card_adr->status == NEW) {
			fprintf(log, "You must change your PIN.\n");
		}
		card_adr->attempts = 0;
	}
	//in add_history este transmis id-ul operatiei aici 3 pentru insert_card
	add_history(card_adr, NULL, exitstatus, 3, pin_str);
}
void recharge(FILE *log, LCARD card_adr, char *sum_str) {
	int exitstatus = SUCCESS;
	unsigned long long sum = get_ull(sum_str);
	if (card_adr->status == BLOCKED) {
		fprintf(log,
				"The card is blocked. Please contact the administrator.\n");
		exitstatus = FAILURE;
	} else if (sum % 10 != 0) {
		fprintf(log, "The added amount must be multiple of 10\n");
		exitstatus = FAILURE;
	} else {
		card_adr->balance += sum;
		fprintf(log, "%llu\n", card_adr->balance);
	}
	add_history(card_adr, NULL, exitstatus, 5, sum_str);
}
void cash_withdrawal(FILE *log, LCARD card_adr, char *sum_str) {
	int exitstatus = SUCCESS;
	unsigned long long sum = get_ull(sum_str);
	if (card_adr->status == BLOCKED) {
		fprintf(log,
				"The card is blocked. Please contact the administrator.\n");
		exitstatus = FAILURE;
	} else if (sum % 10 != 0) {
		fprintf(log, "The requested amount must be multiple of 10\n");
		exitstatus = FAILURE;
	} else if (card_adr->balance < sum) {
		fprintf(log, "Insufficient funds\n");
		exitstatus = FAILURE;
	} else {
		card_adr->balance -= sum;
		fprintf(log, "%llu\n", card_adr->balance);
	}
	add_history(card_adr, NULL, exitstatus, 6, sum_str);
}
void balance_inquiry(FILE *log, LCARD card_adr) {
	int exitstatus = SUCCESS;
	if (card_adr->status == BLOCKED) {
		fprintf(log,
				"The card is blocked. Please contact the administrator.\n");
		exitstatus = FAILURE;
	} else {
		fprintf(log, "%llu\n", card_adr->balance);
	}
	add_history(card_adr, NULL, exitstatus, 7, NULL);
}
void transfer_funds(FILE *log, LCARD card_adr_src, LCARD card_adr_dst,
						char *sum_str) {
	int exitstatus = SUCCESS;
	unsigned long long sum = get_ull(sum_str);
	if (card_adr_src->status == BLOCKED) {
		fprintf(log,
				"The card is blocked. Please contact the administrator.\n");
		exitstatus = FAILURE;
	} else if (sum % 10 != 0) {
		fprintf(log, "The transferred amount must be multiple of 10\n");
		exitstatus = FAILURE;
	} else if (card_adr_src->balance < sum) {
		fprintf(log, "Insufficient funds\n");
		exitstatus = FAILURE;
	} else {
		card_adr_dst->balance += sum;
		card_adr_src->balance -= sum;
		fprintf(log, "%llu\n", card_adr_src->balance);
		add_history(card_adr_dst, card_adr_src, exitstatus, 13, sum_str);
	}
	add_history(card_adr_src, card_adr_dst, exitstatus, 2, sum_str);
}
void pin_change(FILE *log, LCARD card_adr, char *pin_str) {
	int exitstatus = SUCCESS;
	if (card_adr->status == BLOCKED) {
		fprintf(log,
				"The card is blocked. Please contact the administrator.\n");
		exitstatus = FAILURE;
	} else if (!is_numeric(pin_str) || strlen(pin_str) != PIN_DIGITS) {
		fprintf(log, "Invalid PIN\n");
		exitstatus = FAILURE;
	} else {
		card_adr->pin = (int)get_ull(pin_str);
		card_adr->status = ACTIVE;
	}
	add_history(card_adr, NULL, exitstatus, 4, pin_str);
}
void cancel(LCARD card_adr) {
	add_history(card_adr, NULL, SUCCESS, 8, NULL);
}
#endif