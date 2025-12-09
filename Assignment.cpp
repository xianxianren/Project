#include <iostream>
#include <string>

using namespace std;

class BankAccount {
    // === ENCAPSULATION ===
    // Data is PRIVATE. No one outside this class can touch it.
private:
    double balance;
    int pinCode;
    string ownerName;

public:
    // Constructor to initialize the object
    BankAccount(string name, double initialBalance, int pin) {
        ownerName = name;
        balance = initialBalance;
        pinCode = pin;
    }

    // === ABSTRACTION ===
    // The user calls this simple method, but internally it handles
    // complex decisions (Security -> Validation -> Execution).
    void withdraw(double amount, int enteredPin) {
        cout << "\n--- Attempting to withdraw $" << amount << " ---" << endl;

        // Step 1: Security Check (Decision 1)
        if (enteredPin != pinCode) {
            cout << "ERROR: Invalid PIN. Transaction rejected." << endl;
            return;
        }

        // Step 2: Data Validation (Decision 2)
        if (amount <= 0) {
            cout << "ERROR: Amount must be positive." << endl;
            return;
        }

        // Step 3: Business Logic / Funds Check (Decision 3)
        if (amount > balance) {
            cout << "ERROR: Insufficient funds. Current Balance: $" << balance << endl;
        } else {
            // If all checks pass, we modify the private data
            balance = balance - amount;
            cout << "SUCCESS: Please take your cash." << endl;
            cout << "New Balance: $" << balance << endl;
        }
    }

    // Getter for balance (Read-only access)
    double getBalance(int enteredPin) {
        if (enteredPin == pinCode) {
            return balance;
        } else {
            cout << "Access Denied." << endl;
            return -1;
        }
    }
    
    void showInfo() {
        cout << "Account Holder: " << ownerName << endl;
    }
};

int main() {
    // Setup: Creating the account
    cout << "Welcome to the Secure ATM System" << endl;
    BankAccount myAccount("John Doe", 5000.00, 1234);
    
    myAccount.showInfo();

    // SCENARIO 1: Attempting to 'hack' the data directly
    // myAccount.balance = 1000000; // COMPILER ERROR! (This proves Encapsulation works)

    // SCENARIO 2: Failed attempt (Wrong PIN)
    myAccount.withdraw(200, 9999);

    // SCENARIO 3: Failed attempt (Not enough money)
    myAccount.withdraw(6000, 1234);

    // SCENARIO 4: Successful attempt
    myAccount.withdraw(500, 1234);

    system("pause");
}