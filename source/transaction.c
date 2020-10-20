#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "main.h"

#define TRANSACTION_FILE "t"

Transactions_sum *transactions_sums = NULL;
unsigned long transactions_sums_count = 0;

void parse_transaction_with_line(char line[]);
void calculate_sums_for_transaction(Transaction *t);
void calculate_sum_for_group(char group[], Account *account, long amount);

void load_transactions() {
	char line[LINE_MAX_LENGTH];
	FILE *f = fopen(TRANSACTION_FILE, "r");
	if (f) {
		print_time("Loading and calculating transactions ");
		while (fgets(line, LINE_MAX_LENGTH, f))
			parse_transaction_with_line(line);
		fclose(f);
	}
	else print_time("Cannot open transactions file!");
	printf("\n");
}

void raw_transaction(Transaction *t, char raw[]) {
	sprintf(raw, "%04d.%02d.%02d\t%s\t%s\t%.2f", t->year, t->month, t->day, t->from_account->name, t->to_account->name, t->amount / 100.0);
	if (strlen(t->comment) > 0) sprintf(raw, "%s\t%s", raw, t->comment);
	sprintf(raw, "%s\n", raw);
}

void save_transaction(Transaction *t) {
	FILE *f = fopen(TRANSACTION_FILE, "a");
	if (f) {
		char raw[LINE_MAX_LENGTH];
		raw_transaction(t, raw);
		fprintf(f, "%s", raw);
		fclose(f);
	}
	else printf("Cannot open transaction file!\n");
}

void parse_transaction_with_line(char line[]) {
	Transaction t;
	char raw_from[NAME_MAX_LENGTH], raw_to[NAME_MAX_LENGTH], comment[NAME_MAX_LENGTH];
	double raw_amount = 0.0;
	if (sscanf(line, "%04d.%02d.%02d\t%[^\t]\t%[^\t]\t%lf\t%[^\n]", &t.year, &t.month, &t.day, raw_from, raw_to, &raw_amount, comment) >= 6) {
		t.amount = store_amount(raw_amount);
		if (!date_ok(t.year, t.month, t.day))
			printf("\nTransaction date is invalid: %s", line);
		else if (!amount_in_limit(&t.amount))
			printf("\nTransaction amount is off limits: %s", line);
		else if (!(t.from_account = account_with_name(raw_from)) || (t.from_account->type != INCOME && t.from_account->type != STATE))
			printf("\nInvalid source account: %s", line);
		else if (!(t.to_account = account_with_name(raw_to)) || (t.to_account->type != STATE && t.to_account->type != OUTCOME))
			printf("\nInvalid destination account: %s", line);
		else {
			calculate_sums_for_transaction(&t);
			printf(PROGRESS);
		}
	}
	else printf("\nUnknown transaction definition: %s", line);
}

void calculate_sums_for_transaction(Transaction *t) {
	char month[YEAR_MONTH_LENGTH + 1], year[YEAR_LENGTH + 1];
	sprintf(month, "%04d.%02d", t->year, t->month);
	sprintf(year, "%04d", t->year);
	calculate_sum_for_group(month, t->from_account, -t->amount);
	calculate_sum_for_group(year, t->from_account, -t->amount);
	calculate_sum_for_group(month, t->to_account, t->amount);
	calculate_sum_for_group(year, t->to_account, t->amount);
}

void calculate_sum_for_group(char group[], Account *account, long amount) {
	unsigned long i;
	for (i = transactions_sums_count; i > 0; i--)
		if (strcmp(transactions_sums[i - 1].group, group) == 0 && transactions_sums[i - 1].account == account) {
			transactions_sums[i - 1].sum += amount;
			if (!amount_in_limit(&transactions_sums[i - 1].sum))
				printf("\nAggregate sum is off limits: %s (%s)\n", account->name, group);
			return;
		}
	Transactions_sum transactions_sum;
	left(group, strlen(group), transactions_sum.group);
	transactions_sum.account = account;
	transactions_sum.sum = amount;
	if (transactions_sums_count == ULONG_MAX)
		printf("\nToo many aggregates, ignoring: %s (%s)\n", account->name, group);
	else {
		transactions_sums_count++;
		transactions_sums = realloc(transactions_sums, transactions_sums_count * sizeof(Transactions_sum));
		transactions_sums[transactions_sums_count - 1] = transactions_sum;
	}
}
