#include <iostream>
#include <string>

using namespace std;

class BankAccount {
    // ENCAPSULATION
    // Data is Private. No one outside this class can touch it.
private:
    double balance;
    int pinCode;
    string ownerName;

public:
    BankAccount(string name, double initialBalance, int pin) {
        ownerName = name;
        balance = initialBalance;
        pinCode = pin;
    }

    // ABSTRACTION
    // The user calls this simple method, but internally it handles
    void withdraw(double amount, int enteredPin) {
        cout << "\n--- Attempting to withdraw $" << amount << " ---" << endl;

        // Step 1: Security Check (Decision 1)
        if (enteredPin != pinCode) {
            cout << "ERROR: Invalid PIN. " << endl;
            cout << "Transaction rejected." << endl;
            return;
        }

        // Step 2: Data Validation (Decision 2)
        if (amount <= 0) {
            cout << "ERROR: Amount must be positive." << endl;
            return;
        }

        // Step 3: Business Logic / Funds Check (Decision 3)
        if (amount > balance) {
            cout << "ERROR: Insufficient funds. " << endl;
            cout << "Current Balance: RM" << balance << endl;
        } else {
            balance = balance - amount;
            cout << "SUCCESS: Please take your cash." << endl;
            cout << "New Balance: RM" << balance << endl;
        }
    }
    
    void showInfo() {
        cout << "Account Holder: " << ownerName << endl;
    }
};

int main() {
    // Setup: Creating the account
    cout << "Welcome to the Secure ATM System" << endl;
    BankAccount myAccount("Joon Han", 5000.00, 1234);
    
    myAccount.showInfo();

    // SCENARIO 1: Attempting to 'hack' the data directly
    // myAccount.balance = 1000000; // ERROR! (This proves Encapsulation works)

    // SCENARIO 1: Failed attempt (Wrong PIN)
    myAccount.withdraw(200, 9999);

    // SCENARIO 2: Failed attempt (Not enough money)
    myAccount.withdraw(6000, 1234);

    // SCENARIO 3: Successful attempt
    myAccount.withdraw(500, 1234);

    system("pause");
}