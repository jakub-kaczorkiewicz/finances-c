### About

This application will help you to manage your finances. Incomes and expenses are all categorized and presented in a form of readable report, that you can use to make better financial decisions.

All messages are displayed in English, but they were reduced to an absolute minimum, so it's not required to know it. Also, the currency is not defined, it was assumed that it's always the same. Application has no graphical interface and all operations are executed from command line.

Application was written in ANSI C, using only standard libraries. It can be run on any device with C language compilator (even from the 90's). I used Raspberry Pi Zero computer, GCC compilator and Vim editor to create it.

### Idea

Money do not float in air or lay on the ground. In application, money is in the _accounts_. Best way to understand the _account_ is to imagine it as a pile of cash. There are three types of _accounts_: _incomes_, _states_ and _expenses_. _Incomes_ are those _accounts_ from which money comes to you, _states_ are the _accounts_ that you own and _expenses_ are the _accounts_ to which money go, when you're buying something. So obviously, there is only one way that money goes - from _incomes_ to _states_ and from _states_ to _expenses_. Each single move of money is the _transaction_. _Transaction_ contains informations about when it happened, how much money was it and from which _account_ to which _account_ they where transferred. There also can be an additional comment, but it's not obligatory.

### Data

All data is stored in two files: in `a` file we store informations about accounts and in `t` file we store informations about transactions.

In `a` file each line represents one account. Here is a definition of an account:

```
[account_name][tab][account_type][tab][initial_amount]
```

where:
- `[account_name]` is simply a name of an account, you can use up to 50 chars, except for tab,
- `[tab]` is the space created by pressing Tab key on keyboard,
- `[account_type]` is a single char representing type of account: `+` for incomes, `-` for expenses, `=` for states,
- second `[tab]` and `[initial_amount]` are optional and used only for state type accounts; `[initial_amount]` is the amount of money that is in the account, before any transaction was entered.

Let's take a look at this simple example. Tom works and receive a salary every month. He has a bank account and wallet to which sometimes he withdraw a cash from an ATM. He pays mainly for renting an apartment, food, clothes and pleasures. He has 1200 dollars in his account and 52 dollars and 36 cents in a wallet. He also has a 1000 dollars credit for a bike.

In his case we need to create one income account and name it "Work", three state accounts: "Wallet", "Bank account" and "Bike", and four expense accounts: "Rent", "Food", "Clothes", "Pleasures". So `a` file will look like this:

```
Work	+
Bank account	=	1200.00
Wallet	=	52.36
Bike	=	-1000.00
Rent	-
Food	-
Clothes	-
Pleasures	-
```

As you can see, values are entered without currency, without thousand separator and with a period as a decimal part separator. In other words, to enter values we use only digits, period and minus signs. Accounts are grouped by type here only for clarity, it is not required.

In `t` file we store all transactions. Each transaction is a single line defined like this:

```
[date][tab][from_account][tab][to_account][tab][amount][tab][comment]
```

where:
- `[date]` is a date of transaction in _yyyy.mm.dd_ format, e.g. `2020.06.27`,
- `[from_account]` is a name of account from which money were transferred, it must by account of type income or state,
- `[to_account]` is a name of account to which money were transferred, it must be a state or expense account,
- `[amount]` is how much money was transferred,
- next `[tab]` and `[comment]` are optional - it can be any text, application will ignore it anyway.

Let's use Tom case here too. Today is June 27th, 2020 and Tom received a 2000 dollars salary into account, so he payed 50 dollars for a bike and 1000 dollars for a rent. In his case, `t` file will look like this:

```
2020.06.27	Work	Bank account	2000.00
2020.06.27	Bank account	Bike	50.00
2020.06.27	Bank account	Rent	1000.00
```

Next day he withdraw 100 dollars from an ATM and buy a coffee for 2.99 dollars and a t-shirt for 10 dollars. To `t` file we add:

```
2020.06.28	Bank account	Wallet	100.00
2020.06.28	Wallet	Pleasures	2.99	Coffee
2020.06.28	Wallet	Clothes		10.00
```

Ultimately `t` file will store 6 transactions in 6 lines. Entering transactions in a date order is not required, but it will be much easier to search later and affects the time of generating the report. Entering transactions from oldest to newest or vice versa reduces that time. Tom wants to monitor his pleasure expenses, so he added a comment to that transaction. Of course you can add comment to every transaction if you want to.

In most often situations entered values are greater than zero, but there are some exceptions when it's better to use negative values. Let's assume that homeowner recalculated all the transactions with Tom and noticed 20 dollars of overpayment, so he returned them. It is better to add transaction like this:

```
2020.06.29	Bank account	Rent	-20.00	Excess payment
```

Reversed transaction like:

```
2020.06.29	Rent	Bank account	20.00	Excess payment
```

is not valid, because as was mentioned earlier, money flows only in one direction. Alternatively Tom could create another account for excess payments, but in this individual case it seems redundant. After all, Tom spent 980 dollars for a rent that month.

### Compilation

Application includes two programs: `calc` and `add`. First one is used for generating report based on entered data. Second one can be used to quickly enter transactions into `t` file. To use them, first thing to do is to compile application. If you use GCC compiler like me, you can execute commands below:

```
gcc -ansi -o calc source/calc.c source/account.c source/transaction.c source/report.c source/helper.c
gcc -ansi -o add source/add.c source/account.c source/transaction.c source/helper.c
```

or you can execute `compile.sh` script, that will do the same. After execution both programs will be generated. Compilation is a one time process and there is no need to do it again, except there is a new version of application.

Example output of the script run:

```
Compiling calc...
Compiling add...
Done!
```

### Calc

When the `a` and `t` files are ready, we can start to generate a report. Execute following command to do it:

```
./calc
```

Creating report is divided into three stages.
1. Stage one: loading accounts data into memory. `Loading accounts` text will appear, followed by dots. Each dot is a one account loaded into memory. If there is a problem with loading an account, you will see account definition and some information what could be wrong with it. You should then edit "a" file and try to fix a problem. Error do not stop execution of a program, but result will probably be invalid.
2. Stage two: loading transactions into memory. `Loading and calculating transactions` text will appear, followed by dots. Similarly to the first stage, each dot is a single transaction loaded into memory. If there is a problem with the transaction, you will see it's definition and some information what could be wrong with it. And it will not stop executing of a program too, but result will probably be invalid.
3. Stage three: generating report. Report is written to `r` file. Here could be a problem too, mainly related to exceeding maximum value supported by a program. Application support values up to 9999999.99, and that include sums. It is a result of trying to be compatible as much as possible with oldest devices available. If the problem appear, result will be invalid for sure.

Example output of calc application:

```
[0.013] Loading accounts .........................
[0.015] Loading and calculating transactions ..............................................................
[0.083] Generating report ..........
[0.107] Done!
```

Each stage starts with an information in a square brackets about how much time has passed since launch of a program. If everything went well, the `r` file will be generated. You can execute `makereport.sh` script, that will launch `calc` program and display report right after.

### Report

Report is quite wide and to see it entirely at least 200 columns screen is recommended. Script uses a `less` program to display report, so it’s easy to see whole report even on smaller screen, by using arrow keys to scroll.

Report is divided into three parts: incomes, expenses, actual state with summary. Accounts are displayed in rows, months are displayed in columns, so it looks like an Excel sheet a bit.

First let's look at incomes part. I’ve got all incomes accounts on the right side. For each month we have a total income and on the right side, just before account name, we have a summary for a whole year for that account. Last two rows are the sums for all accounts in each month and a difference from the previous month. Please note that accounts are ordered by total year income from largest to smallest.

Under that are expenses in a same manner, but with all expense accounts. Note that values are negative here, which means that this money has left in particular month or year.

Last part is a summary of a current state of our finances. We see the states of our accounts in past months also. For months that are still to come we always see a current state. Two last rows are the sums of all of our accounts and under that are a differences from the previous months.

There are many benefits of such a layout, including:
- incomes, expenses and states are sorted from largest to smallest, so it's easy to find out for example, which expenses are too big (I have a fast food account and after about a month I knew how much I spend on it - and it was a lot, although it seemed that it was only some small amounts),
- it's very easy to compare month to month or year to year, so we can track a progress of our strategy and see if it works,
- in a summary of our current state we always see how much money we have and if it grows or not.

### Calc

`Calc` program can be executed with a `csv` parameter:

```
./calc -csv
```

then instead of `r` file we generate `r.csv` file, which is in CSV format. Now you can open it in any popular spreadsheet application like Microsoft Excel, Apple Numbers or LibreOffice Calc. Convention is that all data is separated by comma, all texts are in quotation marks and numbers don't have a thousand separator and use period for decimal part.

### Add

There is an easier way to add transaction into `t` file and `add` program is just for that.

All accounts are loaded after launch, same as in `calc` program. Then you will be asked for a date of transaction. You need to enter it in _yyyy.mm.dd_ format, if it's a current one you can just press Enter. Then program will validate it and if it's wrong a proper message will appear.

In next step you need to indicate a source account, from which money will be transferred. You will see a list of accounts with an assigned number to each. You need to enter that number and press Enter. If the list is too big, you can search for an account, by entering full or a part of account name (letter size is important, so "food" and "Food" are two different things). If you want to go back to full list after searching just press Enter. If entered number is invalid a proper message will appear.

In next step (we are almost done) we enter a amount of transaction. We use the same rules as mentioned earlier - only digits, dot and minus sign. If entered amount is invalid a proper message will appear.

If everything went right, in next step you will see a line that will be written into `t` file and it's a last moment to verify transaction. You can enter a short comment here if you want or just press Enter to confirm it. Transaction will be written and program will end.

### Searching

If you are using this application in Unix environment, you can use system tools to search for a specific transactions, like `cat`, `grep` or `less`. Below are some of the examples:
- all transactions from April 2020:

```
cat t | grep "^2020.04"
```

- all transactions with account "Electronics":

```
cat t | grep "Electronics"
```

- all transactions from April 2020 with account "Electronics":

```
cat t | grep "^2020.04.*Electronics"
```

If the output is too large, you can add `less` (or other paging program) or redirect output to other file:

```
cat t | grep "^2020" | less
cat t | grep "^2020" > all2020t
```

There is a planty of options here, and you can use any tool you want – `t` file is just a plain text file after all. You can also add some of the searches into `makereport.sh` script if you do them frequently.

### What’s next

I personally use this application almost every day and every time I use it some new ideas or features comes to mind. Here are some of them, that I would like to add in a first place:
- graphical user interface using GTK library,
- charts,
- ability to group accounts and present them as a single item on report,
- ability to hide picked accounts on report,
- ability to tag transactions and present them as a single item on report,
- ability to quickly change account name,
- suggesting transaction value based on previous transaction with the same accounts pair.

Of course I’m aware that my code is far from perfect, but I’m trying to systematically improve it to be more readable, smaller and faster. If you find some error or a better way to code something, please let me know, I will be more than happy to hear about it.

### About me

My name is Jakub Kaczorkiewicz, I'm a software architect and developer of mobile applications. If you want to contact, please send me an email at jakub.kaczorkiewicz@me.com.
