#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "main.h"

Account *accounts = NULL;
unsigned long accounts_count = 0;

void parse_account_with_line(char line[]);
Account_type account_type_with_raw_type(char raw_type[]);

void load_accounts() {
	char line[LINE_MAX_LENGTH];
	FILE *f = fopen("a", "r");
	if (f) {
		print_time("Loading accounts ");
		while (fgets(line, LINE_MAX_LENGTH, f))
			parse_account_with_line(line);
		fclose(f);
	}
	else print_time("Cannot open accounts file!");
	printf("\n");
}

Account *account_with_name(char name[]) {
	unsigned long i;
	for (i = 0; i < accounts_count; i++)
		if (strcmp(accounts[i].name, name) == 0) return &accounts[i];
	return NULL;
}

void parse_account_with_line(char line[]) {
	Account a;
	double raw_amount = 0.0;
	char raw_type[2];
	if (sscanf(line, "%[^\t]\t%1[^\t]\t%lf", a.name, raw_type, &raw_amount) >= 2) {
		a.initial = store_amount(raw_amount);
		if (!amount_in_limit(&a.initial))
			printf("\nAccount initial amount is off limits: %s", line);
		else if ((a.type = account_type_with_raw_type(raw_type)) != UNKNOWN) {
			if (accounts_count == ULONG_MAX)
				printf("\nToo many accounts, ignoring: %s", line);
			else {
				accounts_count++;
				accounts = realloc(accounts, accounts_count * sizeof(Account));
				accounts[accounts_count - 1] = a;
			}
			printf(PROGRESS);
		}
		else printf("\nUnknown account type: %s", line);
	}
	else printf("\nUnknown account definition: %s", line);
}

Account_type account_type_with_raw_type(char raw_type[]) {
	switch (*raw_type) {
		case '+': return INCOME;
		case '=': return STATE;
		case '-': return OUTCOME;
	}
	return UNKNOWN;
}
