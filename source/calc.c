#include <stdio.h>
#include "main.h"

int main(int argc, char *argv[]) {
	Bool csv = is_parameter(argc, argv, "-csv");
	load_accounts();
	load_transactions();
	save_report(csv);
	print_time("Done!\n");
	return 0;
}
