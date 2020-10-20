#include <stdio.h>
#include <string.h>
#include "main.h"

#define INPUT_MAX_LENGTH 20

void get_date(int *year, int *month, int *day);
Account *get_account(char question[], Account_type exc_type);
long get_amount();
void get_comment(char comment[]);

int main(int argc, char *argv[]) {
	Transaction t;
	char raw[LINE_MAX_LENGTH];
	load_accounts();
	get_date(&t.year, &t.month, &t.day);
	t.from_account = get_account("From account", OUTCOME);
	t.to_account = get_account("To account", INCOME);
	t.amount = get_amount();
	sprintf(t.comment, "");
	raw_transaction(&t, raw);
	printf("%s", raw);
	get_comment(t.comment);
	save_transaction(&t);
	printf("Done!\n");
}

void get_date(int *year, int *month, int *day) {
	char input[INPUT_MAX_LENGTH];
	int iyear, imonth, iday;
	while (TRUE) {
		set_current_date(&iyear, &imonth, &iday);
		printf("Date (yyyy.mm.dd, enter for %04d.%02d.%02d): ", iyear, imonth, iday);
		fgets(input, INPUT_MAX_LENGTH, stdin);
		if (strlen(input) == 1 || (sscanf(input, "%04d.%02d.%02d", &iyear, &imonth, &iday) == 3 && date_ok(iyear, imonth, iday))) {
			*year = iyear;
			*month = imonth;
			*day = iday;
			break;
		}
		printf("Wrong date!\n");
	}
}

Account *get_account(char question[], Account_type exc_type) {
	extern Account *accounts;
	extern unsigned long accounts_count;
	char input[INPUT_MAX_LENGTH] = "";
	unsigned long i;
	while (TRUE) {
		for (i = 0; i < accounts_count; i++)
			if (accounts[i].type != exc_type && strstr(accounts[i].name, input) != NULL)
				printf("[%lu] %s\n", i, accounts[i].name);
		printf("%s (number, text to filter): ", question);
		fgets(input, INPUT_MAX_LENGTH, stdin);
		if (sscanf(input, "%lu", &i) == 1) {
			if (i < accounts_count && accounts[i].type != exc_type)
				return &accounts[i];
			else printf("Wrong account!\n");
		}
		left(input, strlen(input) - 1, input);
	}
}

long get_amount() {
	char input[INPUT_MAX_LENGTH];
	double raw_amount;
	long amount;
	while (TRUE) {
		printf("Amount: ");
		fgets(input, INPUT_MAX_LENGTH, stdin);
		if (sscanf(input, "%lf", &raw_amount) == 1) {
			amount = store_amount(raw_amount);
			if (amount_in_limit(&amount))
				return amount;
		}
		printf("Wrong amount!\n");
	}
}

void get_comment(char comment[]) {
	char input[NAME_MAX_LENGTH] = "";
	printf("Confirm (or optional comment): ");
	fgets(input, NAME_MAX_LENGTH, stdin);
	left(input, strlen(input) - 1, comment);
}
