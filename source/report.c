#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

typedef struct {
	char group[YEAR_LENGTH + 1];
} Year_row;

typedef struct {
	Year_row *year_row;
	Account *account;
	long month_amount[12], sum;
} Report_row;

Year_row *year_rows = NULL;
Report_row *report_rows = NULL;
unsigned long year_rows_count = 0, report_rows_count = 0;

void save_report_to_file(FILE *f, Bool csv);
void extract_years();
void sort_years();
void gather_data();
void update_states();
void save_csv_for_accounts_type(FILE *f, Account_type type, Bool reverse_sign);
void save_summary_for_accounts_type(FILE *f, Account_type type, Bool asc, Bool reverse_sign);

void save_report(Bool csv) {
	FILE *f = fopen(csv ? "r.csv" : "r", "w");
	if (f) {
		print_time("Generating report ");
		save_report_to_file(f, csv);
		fclose(f);
	}
	else print_time("Cannot create report file!");
	printf("\n");
}

void save_report_to_file(FILE *f, Bool csv) {
	extract_years();
	sort_years();
	gather_data();
	update_states();
	if (csv) {
		save_csv_for_accounts_type(f, INCOME, TRUE);
		save_csv_for_accounts_type(f, OUTCOME, TRUE);
		save_csv_for_accounts_type(f, STATE, FALSE);
	}
	else {
		save_summary_for_accounts_type(f, INCOME, TRUE, TRUE);
		save_summary_for_accounts_type(f, OUTCOME, FALSE, TRUE);
		save_summary_for_accounts_type(f, STATE, FALSE, FALSE);
	}
}

void extract_years() {
	extern Transactions_sum *transactions_sums;
	extern unsigned long transactions_sums_count;
	unsigned long i, i2;
	Bool found;
	for (i = 0; i < transactions_sums_count; i++)
		if (strlen(transactions_sums[i].group) == YEAR_LENGTH) {
			found = FALSE;
			for (i2 = 0; i2 < year_rows_count; i2++)
				if (strcmp(year_rows[i2].group, transactions_sums[i].group) == 0) {
					found = TRUE;
					break;
				}
			if (!found) {
				Year_row year_row;
				left(transactions_sums[i].group, YEAR_LENGTH, year_row.group);
				year_rows_count++;
				year_rows = realloc(year_rows, year_rows_count * sizeof(Year_row));
				year_rows[year_rows_count - 1] = year_row;
			}
		}
	printf(PROGRESS);
}

void sort_years() {
	unsigned long i;
	Year_row tmp_year_row;
	Bool found;
	if (year_rows_count > 1)
		do {
			found = FALSE;
			for (i = 0; i < year_rows_count - 1; i++)
				if (strcmp(year_rows[i].group, year_rows[i + 1].group) > 0) {
					tmp_year_row = year_rows[i];
					year_rows[i] = year_rows[i + 1];
					year_rows[i + 1] = tmp_year_row;
					found = TRUE;
				}
		} while (found);
	printf(PROGRESS);
}

void gather_data() {
	extern Account *accounts;
	extern Transactions_sum *transactions_sums;
	extern unsigned long accounts_count, transactions_sums_count;
	unsigned long i, i2, i4;
	char i3, month[YEAR_MONTH_LENGTH + 1];
	for (i = 0; i < year_rows_count; i++)
		for (i2 = 0; i2 < accounts_count; i2++) {
			Report_row report_row;
			report_row.year_row = &year_rows[i];
			report_row.account = &accounts[i2];
			report_row.sum = 0;
			for (i3 = 0; i3 < 12; i3++) {
				sprintf(month, "%s.%02d", year_rows[i].group, i3 + 1);
				for (i4 = 0; i4 < transactions_sums_count; i4++) {
					report_row.month_amount[i3] = 0;
					if (strcmp(transactions_sums[i4].group, month) == 0 && transactions_sums[i4].account == &accounts[i2]) {
						report_row.month_amount[i3] = transactions_sums[i4].sum;
						if (accounts[i2].type != STATE) {
							report_row.sum += transactions_sums[i4].sum;
							if (!amount_in_limit(&report_row.sum))
								printf("\nReport year sum is off limits: %s (%s)\n", accounts[i2].name, year_rows[i].group);
						}
						break;
					}
				}
			}
			report_rows_count++;
			report_rows = realloc(report_rows, report_rows_count * sizeof(Report_row));
			report_rows[report_rows_count - 1] = report_row;
		}
	printf(PROGRESS);
}

void update_states() {
	extern Account *accounts;
	extern unsigned long accounts_count;
	unsigned long i, i2;
	char i3;
	long diff;
	for (i = 0; i < accounts_count; i++)
		if (accounts[i].type == STATE) {
			diff = accounts[i].initial;
			for (i2 = 0; i2 < report_rows_count; i2++)
				if (report_rows[i2].account == &accounts[i]) {
					for (i3 = 0; i3 < 12; i3++) {
						diff += report_rows[i2].month_amount[i3];
						if (!amount_in_limit(&diff))
							printf("\nReport month amount is off limits: %s (%s.%02d)\n", accounts[i].name, report_rows[i2].year_row->group, i3 + 1);
						report_rows[i2].month_amount[i3] = diff;
					}
					report_rows[i2].sum = diff;
				}
		}
	printf(PROGRESS);
}

void save_csv_for_accounts_type(FILE *f, Account_type type, Bool reverse_sign) {
	extern Account *accounts;
	extern unsigned long accounts_count;
	unsigned long i, i2, i3;
	char i4, quoted_name[NAME_MAX_LENGTH * 2];
	/* Header */
	for (i = 0; i < year_rows_count; i++)
		for (i4 = 1; i4 < 13; i4++)
			fprintf(f, ",\"%s.%02d\"", year_rows[i].group, i4);
	fprintf(f, "\n");
	/* Body */
	for (i = 0; i < accounts_count; i++)
		if (accounts[i].type == type) {
			quote(accounts[i].name, quoted_name);
			fprintf(f, "\"%s\"", quoted_name);
			for (i2 = 0; i2 < year_rows_count; i2++)
				for (i3 = 0; i3 < report_rows_count; i3++)
					if (report_rows[i3].year_row == &year_rows[i2] && report_rows[i3].account == &accounts[i])
						for (i4 = 0; i4 < 12; i4++)
							fprintf(f, ",%.2f", print_amount(report_rows[i3].month_amount[i4], reverse_sign));
			fprintf(f, "\n");
		}
	printf(PROGRESS);
}

void save_summary_for_accounts_type(FILE *f, Account_type type, Bool asc, Bool reverse_sign) {
	unsigned long i, i2, i_from, i_to;
	char i3, month[YEAR_MONTH_LENGTH + 1];
	Report_row tmp_report_row;
	Bool found;
	long diff;
	/* Sorting sums in specific order */
	for (i = 0; i < year_rows_count; i++) {
		/* Search for year range */
		i_from = i_to = 0;
		for (i2 = 0; i2 < report_rows_count; i2++)
			if (report_rows[i2].year_row == &year_rows[i]) {
				i_from = i2;
				break;
			}
		for (i2 = i_from; i2 < report_rows_count; i2++)
			if (report_rows[i2].year_row != &year_rows[i]) {
				i_to = i2 - 1;
				break;
			}
		if (i_to == 0) i_to = report_rows_count - 1;
		/* Actual sorting */
		do {
			found = FALSE;
			for (i2 = i_from; i2 < i_to; i2++)
				if ((asc && report_rows[i2].sum > report_rows[i2 + 1].sum) || (!asc && report_rows[i2].sum < report_rows[i2 + 1].sum)) {
					tmp_report_row = report_rows[i2];
					report_rows[i2] = report_rows[i2 + 1];
					report_rows[i2 + 1] = tmp_report_row;
					found = TRUE;
				}
		} while (found);
	}
	printf(PROGRESS);
	/* Generating report to file */
	for (i = 0; i < year_rows_count; i++) {
		/* Header */
		for (i3 = 1; i3 < 13; i3++) {
			sprintf(month, "%s.%02d", year_rows[i].group, i3);
			fprintf(f, " %11s", month);
		}
		fprintf(f, "\n");
		for (i2 = 0; i2 < (type == STATE ? 12 : 13) * 12; i2++) fprintf(f, "-");
		fprintf(f, "\n");
		/* Body */
		for (i2 = 0; i2 < 12; i2++) tmp_report_row.month_amount[i2] = 0;
		for (i2 = 0; i2 < report_rows_count; i2++)
			if (report_rows[i2].year_row == &year_rows[i] && report_rows[i2].account->type == type) {
				for (i3 = 0; i3 < 12; i3++) {
					fprintf(f, " %11.2f", print_amount(report_rows[i2].month_amount[i3], reverse_sign));
					tmp_report_row.month_amount[i3] += report_rows[i2].month_amount[i3];
					if (!amount_in_limit(&tmp_report_row.month_amount[i3]))
						printf("\nReport month total is off limits: %s.%02d\n", year_rows[i].group, i3 + 1);
				}
				if (type != STATE) fprintf(f, " %11.2f", print_amount(report_rows[i2].sum, reverse_sign));
				fprintf(f, " %s\n", report_rows[i2].account->name);
			}
		/* Footer */
		for (i2 = 0; i2 < (type == STATE ? 12 : 13) * 12; i2++) fprintf(f, "-");
		fprintf(f, "\n");
		tmp_report_row.sum = 0;
		for (i2 = 0; i2 < 12; i2++) {
			fprintf(f, " %11.2f", print_amount(tmp_report_row.month_amount[i2], reverse_sign));
			if (type != STATE) {
				tmp_report_row.sum += tmp_report_row.month_amount[i2];
				if (!amount_in_limit(&tmp_report_row.sum))
					printf("\nReport year total is off limits: %s\n", year_rows[i].group);
			}
		}
		if (type != STATE) fprintf(f, " %11.2f", print_amount(tmp_report_row.sum, reverse_sign));
		fprintf(f, "\n");
		for (i2 = 0; i2 < 12; i2++) {
			if (i2 == 0) diff = 0;
			else diff = tmp_report_row.month_amount[i2] - tmp_report_row.month_amount[i2 - 1];
			if (!amount_in_limit(&diff))
				printf("\nReport month diff is off limits: %s.%02lu\n", year_rows[i].group, i2 + 1);
			fprintf(f, " %11.2f", print_amount(diff, reverse_sign));
			if (type != STATE) tmp_report_row.sum += tmp_report_row.month_amount[i2];
		}
		fprintf(f, "\n\n");
	}
	printf(PROGRESS);
}
