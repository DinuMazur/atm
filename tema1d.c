//MAZUR DINU 313CB
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "atm.h"
#include "atm_general_functions.h"
#include "atm_admin.h"
#include "atm_client.h"

int main() {
	DB database = NULL;
	unsigned long long card_num, card_num_aux;
	int nr_max_carduri, nr_crt_carduri = 0;
	int cvv, operation_id = -1, expiry_month, expiry_year;
	char string[16], pin_str[16];
	LCARD card_adr = NULL, card_adr_aux = NULL;
	FILE *file_input = fopen("input.in", "r");
	FILE *log = fopen("output.out", "w");
	if (!file_input) {
		return FAILURE;
	}
	fscanf(file_input, "%d", &nr_max_carduri);
	while (fscanf(file_input, "%s", string) != EOF) {
		if (operation_id == -1) {
			operation_id = get_operation_id(string);
		} else if (operation_id == 9) {
			if (get_operation_id(string) != -1) {
				show(log, database);
				operation_id = get_operation_id(string);
			} else {
				show_card(log, get_card(database, get_ull(string)));
				operation_id = -1;
			}
		} else {
			card_num = get_ull(string);
			if (operation_id > 0) {
				card_adr = get_card(database, card_num);
				if (operation_id == 11) {
					delete_card(database, card_adr, &nr_crt_carduri);
				} else if (operation_id == 10) {
					unblock_card(card_adr);
				} else if (operation_id == 8) {
					cancel(card_adr);
				} else if (operation_id == 7) {
					balance_inquiry(log, card_adr);
				} else {
					fscanf(file_input, "%s", string);
					if (operation_id > 2) {
						if (operation_id == 3) {
							insert_card(log, card_adr, string);
						} else if (operation_id == 4){
							pin_change(log, card_adr, string);
						} else if (operation_id == 5) {
							recharge(log, card_adr, string);
						} else if (operation_id == 6) {
							cash_withdrawal(log, card_adr, string);
						}	
					} else {
						card_num_aux = get_ull(string);
						card_adr_aux = get_card(database, card_num_aux);
						fscanf(file_input, "%s", string);
						if (operation_id == 1) {
							reverse_transaction(log, card_adr, card_adr_aux,
												string);
						} else if (operation_id == 2) {
							transfer_funds(log, card_adr, card_adr_aux, string);
						}
					}
				}
			} else if (operation_id == 0) {
				fscanf(file_input, "%s", pin_str);
				fscanf(file_input, "%s", string);
				set_date(string, &expiry_month, &expiry_year);
				fscanf(file_input, "%d", &cvv);
				add_card(log, &database, card_num, &nr_max_carduri,
							&nr_crt_carduri, pin_str, cvv, expiry_month,
							expiry_year);
			}
			operation_id = -1;
		}
	}
	if (operation_id == 9) {
		show(log, database);
	}
	fclose(file_input);
	fclose(log);
	delete_database(database);
	return SUCCESS;
}