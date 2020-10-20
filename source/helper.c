#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include "main.h"

Bool is_parameter(int argc, char *argv[], char param[]) {
	int i;
	for (i = 1; i < argc; i++)
		if (strcmp(argv[i], param) == 0) return TRUE;
	return FALSE;
}

void print_time(char text[]) {
	printf("[%.3f] %s", (double)clock() / CLOCKS_PER_SEC, text);
}

void set_current_date(int *year, int *month, int *day) {
	time_t now = time(NULL);
	struct tm *tp = localtime(&now);
	*year = tp->tm_year + 1900;
	*month = tp->tm_mon + 1;
	*day = tp->tm_mday;
}

Bool date_ok(int year, int month, int day) {
	struct tm tp;
	tp.tm_year = (year -= 1900);
	tp.tm_mon = (month -= 1);
	tp.tm_mday = day;
	tp.tm_sec = tp.tm_min = tp.tm_hour = 0;
	tp.tm_isdst = -1;
	time_t time = mktime(&tp);
	if (time == -1 || tp.tm_year != year || tp.tm_mon != month || tp.tm_mday != day)
		return FALSE;
	return TRUE;
}

void left(char from[], int size, char to[]) {
	sprintf(to, "%.*s", size, from);
}

long store_amount(double amount) {
	amount *= 100.0;
	if (amount > LONG_MAX) amount = LONG_MAX;
	else if (amount < LONG_MIN) amount = LONG_MIN;
	return amount < 0.0 ? amount - 0.5 : amount + 0.5;
}

Bool amount_in_limit(long *amount) {
	if (*amount > AMOUNT_LIMIT) {
		*amount = AMOUNT_LIMIT;
		return FALSE;
	}
	else if (*amount< -AMOUNT_LIMIT) {
		*amount = -AMOUNT_LIMIT;
		return FALSE;
	}
	return TRUE;
}

double print_amount(long amount, Bool reverse_sign) {
	return amount != 0 ? amount / (reverse_sign ? -100.0 : 100.0) : 0.0;
}

void quote(char from[], char to[]) {
	int i, j;
	for (i = 0, j = 0; from[i] != '\0'; i++, j++) {
		to[j] = from[i];
		if (to[j] == '\"') {
			to[j + 1] = to[j];
			j++;
		}
	}
	to[j] = '\0';
}
