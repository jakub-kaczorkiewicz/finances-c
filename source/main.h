#define NAME_MAX_LENGTH 50
#define LINE_MAX_LENGTH 255
#define YEAR_MONTH_LENGTH 7
#define YEAR_LENGTH 4
#define PROGRESS "."
#define AMOUNT_LIMIT 999999999

typedef enum { FALSE, TRUE } Bool;
typedef enum { INCOME, STATE, OUTCOME, UNKNOWN } Account_type;

typedef struct {
	char name[NAME_MAX_LENGTH];
	Account_type type;
	long initial;
} Account;

typedef struct {
	int year, month, day;
	Account *from_account, *to_account;
	long amount;
	char comment[NAME_MAX_LENGTH];
} Transaction;

typedef struct {
	char group[YEAR_MONTH_LENGTH];
	Account *account;
	long sum;
} Transactions_sum;

/* Account */
void load_accounts();
Account *account_with_name(char name[]);

/* Transaction */
void load_transactions();
void raw_transaction(Transaction *t, char raw[]);
void save_transaction(Transaction *t);

/* Report */
void save_report(Bool csv);

/* Helper */
Bool is_parameter(int argc, char *argv[], char param[]);
void print_time(char text[]);
void set_current_date(int *year, int *month, int *day);
Bool date_ok(int year, int month, int day);
void left(char from[], int size, char to[]);
long store_amount(double amount);
Bool amount_in_limit(long *amount);
double print_amount(long amount, Bool reverse_sign);
void quote(char from[], char to[]);
