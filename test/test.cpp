#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;

// ==========================================
// SECTION 1: DECLARATIONS (THE "WHAT")
// ==========================================

// --- ADT 1: Ticket ---
struct Ticket {
    int id;
    string movieTitle;
    string date;
    string time;
    string experience;
    vector<string> seats;
    double totalPrice;
    string status; // "Active" or "Cancelled"

    string getDetails() const; // Helper to get string representation
};

// --- ADT 3: Movie ---
struct Movie {
    int id;
    string title;
    string genre;
    string director;
    string releaseDate;
    string runningTime;
    string language;
    double basePrice;
    vector<string> showtimes;
    vector<string> experiences;

    void displayDetails() const;
};

// --- ADT 2: User ---
class User {
public:
    string name;
    string email;
    string phone;
    string password;
    bool isStudent;
    string uniName;
    string studentID;
    vector<Ticket> myTickets; // The "Bag" of tickets

    User(string n, string e, string p, string pass, bool student, string uni = "", string sid = "");
    
    void addTicket(Ticket t);
    bool removeTicket(int ticketID); // Logically cancels ticket
    bool verifyLogin(string inputName, string inputPass);
};

// --- ADT 4: CinemaSystem (The Controller) ---
class CinemaSystem {
private:
    vector<User> users;      // UserBag
    vector<Movie> movies;    // MovieBag
    User* currentUser;
    int ticketCounter;

    // File Handling Helpers
    void saveData();
    void loadData();
    vector<string> split(const string& str, char delimiter);

public:
    CinemaSystem();
    ~CinemaSystem();

    // Core Operations
    void run();
    void debugDump(); // Debugging operation

    // UI & Flow Helpers
    void initMockData(); // Fallback if files don't exist
    void clearScreen();
    void pause();
    int getIntInput();

    // Pages
    void WelcomingPage();
    void LoginPage();
    void RegisterPage();
    void HomePage();
    void RecommendationPage();
    void AllMoviePage();
    void UserProfilePage();
    void MyTicketPage();
    void TicketDetailsPage(int index);

    // Booking Logic
    void MovieDetailsPage(Movie& m);
    void SelectDatePage(Movie& m);
    void SelectExperiencePage(Movie& m, string date);
    void SelectShowTimePage(Movie& m, string date, string exp);
    void SelectSeatPage(Movie& m, string date, string exp, string time);
    void CheckingTicketPage(Movie& m, string date, string exp, string time, vector<string> seats);
    void PaymentPage(Movie& m, string date, string exp, string time, vector<string> seats, double total);
    void ReceiptPage(Movie& m, string date, string exp, string time, vector<string> seats, double total);
};

// ==========================================
// SECTION 2: MAIN ENTRY POINT
// ==========================================

int main() {
    CinemaSystem app;
    app.run();
    return 0;
}

// ==========================================
// SECTION 3: DEFINITIONS (THE "HOW")
// ==========================================

// --- Ticket Implementation ---
string Ticket::getDetails() const {
    stringstream ss;
    ss << "ID: " << id << " | Movie: " << movieTitle << " | Status: " << status;
    return ss.str();
}

// --- Movie Implementation ---
void Movie::displayDetails() const {
    cout << "Title: " << title << "\nGenre: " << genre << "\nDirector: " << director 
         << "\nPrice: $" << basePrice << endl;
}

// --- User Implementation ---
User::User(string n, string e, string p, string pass, bool student, string uni, string sid) 
    : name(n), email(e), phone(p), password(pass), isStudent(student), uniName(uni), studentID(sid) {}

void User::addTicket(Ticket t) {
    myTickets.push_back(t);
}

bool User::removeTicket(int ticketID) {
    for (auto& t : myTickets) {
        if (t.id == ticketID && t.status == "Active") {
            t.status = "Cancelled";
            return true;
        }
    }
    return false;
}

bool User::verifyLogin(string inputName, string inputPass) {
    return (name == inputName && password == inputPass);
}

// --- CinemaSystem Implementation ---

// 1. Constructor / Destructor / Files
CinemaSystem::CinemaSystem() {
    currentUser = nullptr;
    ticketCounter = 1000; // Starting ID
    loadData(); // Load from files on startup
}

CinemaSystem::~CinemaSystem() {
    saveData(); // Save to files on exit
}

vector<string> CinemaSystem::split(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void CinemaSystem::saveData() {
    // Save Users & Tickets
    ofstream userFile("users.txt");
    if (userFile.is_open()) {
        for (const auto& u : users) {
            userFile << u.name << ";" << u.email << ";" << u.phone << ";" 
                     << u.password << ";" << u.isStudent << ";" 
                     << u.uniName << ";" << u.studentID << "\n";
            
            for (const auto& t : u.myTickets) {
                userFile << "TICKET;" << t.id << ";" << t.movieTitle << ";" 
                         << t.date << ";" << t.time << ";" << t.experience << ";" 
                         << t.totalPrice << ";" << t.status << ";";
                // Join seats
                for(size_t i=0; i<t.seats.size(); i++) {
                    userFile << t.seats[i] << (i < t.seats.size()-1 ? "," : "");
                }
                userFile << "\n";
            }
            userFile << "END_USER\n";
        }
        userFile.close();
    }

    // Save Movies
    ofstream movieFile("movies.txt");
    if (movieFile.is_open()) {
        for (const auto& m : movies) {
            movieFile << m.id << ";" << m.title << ";" << m.genre << ";" 
                      << m.director << ";" << m.releaseDate << ";" 
                      << m.runningTime << ";" << m.basePrice << "\n";
        }
        movieFile.close();
    }
}

void CinemaSystem::loadData() {
    users.clear();
    movies.clear();

    // Load Movies
    ifstream movieFile("movies.txt");
    string line;
    if (movieFile.is_open()) {
        while (getline(movieFile, line)) {
            vector<string> data = split(line, ';');
            if (data.size() >= 7) {
                Movie m;
                m.id = stoi(data[0]);
                m.title = data[1];
                m.genre = data[2];
                m.director = data[3];
                m.releaseDate = data[4];
                m.runningTime = data[5];
                m.basePrice = stod(data[6]);
                // Hardcoding schedules for simplicity as they aren't in simple file format
                m.showtimes = {"10:00 AM", "2:00 PM", "8:00 PM"};
                m.experiences = {"Standard", "IMAX", "Gold Class"};
                movies.push_back(m);
            }
        }
        movieFile.close();
    } else {
        initMockData();
    }

    // Load Users
    ifstream userFile("users.txt");
    if (userFile.is_open()) {
        User* currentUserLoading = nullptr;
        while (getline(userFile, line)) {
            if (line == "END_USER") {
                currentUserLoading = nullptr;
                continue;
            }
            vector<string> data = split(line, ';');
            if (data.empty()) continue;

            if (data[0] == "TICKET" && currentUserLoading != nullptr) {
                Ticket t;
                t.id = stoi(data[1]);
                t.movieTitle = data[2];
                t.date = data[3];
                t.time = data[4];
                t.experience = data[5];
                t.totalPrice = stod(data[6]);
                t.status = data[7];
                if(data.size() > 8) t.seats = split(data[8], ',');
                
                currentUserLoading->myTickets.push_back(t);
                if (t.id >= ticketCounter) ticketCounter = t.id + 1;
            } else if (data.size() >= 7) {
                bool isStudent = (data[4] == "1");
                users.push_back(User(data[0], data[1], data[2], data[3], isStudent, data[5], data[6]));
                currentUserLoading = &users.back();
            }
        }
        userFile.close();
    }
}

void CinemaSystem::initMockData() {
    movies.push_back({1, "Avengers: Secret Wars", "Action", "Russo Bros", "2026", "3h 10m", "English", 20.0, {"10:00 AM", "2:00 PM"}, {"Standard", "IMAX"}});
    movies.push_back({2, "Frozen 3", "Animation", "Jennifer Lee", "2025", "1h 45m", "English", 15.0, {"11:00 AM", "4:00 PM"}, {"Standard", "Kids Hall"}});
    users.push_back(User("Admin", "admin@test.com", "000", "123", false));
}

// 2. Debugging
void CinemaSystem::debugDump() {
    cout << "\n[DEBUG DUMP START]\n";
    cout << "Users: " << users.size() << "\n";
    for(const auto& u : users) {
        cout << " - " << u.name << " (" << u.myTickets.size() << " tickets)\n";
    }
    cout << "Movies: " << movies.size() << "\n";
    for(const auto& m : movies) {
        cout << " - " << m.title << "\n";
    }
    cout << "[DEBUG DUMP END]\n";
    pause();
}

// 3. UI Helpers
void CinemaSystem::clearScreen() { cout << string(50, '\n'); }
void CinemaSystem::pause() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}
int CinemaSystem::getIntInput() {
    int choice;
    while (!(cin >> choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid. Enter number: ";
    }
    return choice;
}

// 4. Flow Implementation
void CinemaSystem::run() {
    WelcomingPage();
}

void CinemaSystem::WelcomingPage() {
    while(true) {
        clearScreen();
        cout << "=== BOX OFFICE SYSTEM ===\n";
        cout << "1. Login\n2. Register\n3. Exit\n9. Debug Mode\nSelect: ";
        int choice = getIntInput();
        if (choice == 1) LoginPage();
        else if (choice == 2) RegisterPage();
        else if (choice == 3) { exit(0); }
        else if (choice == 9) debugDump();
    }
}

void CinemaSystem::LoginPage() {
    int attempts = 0;
    while (attempts < 3) {
        clearScreen();
        cout << "=== LOGIN ===\n";
        string name, pass;
        cout << "Name: "; cin.ignore(); getline(cin, name);
        cout << "Password: "; getline(cin, pass);

        for (auto &u : users) {
            if (u.verifyLogin(name, pass)) {
                currentUser = &u;
                HomePage();
                return;
            }
        }
        cout << "Invalid Credentials!\n";
        attempts++;
        pause();
    }
    cout << "Too many attempts.\n";
    pause();
}

void CinemaSystem::RegisterPage() {
    clearScreen();
    cout << "=== REGISTER ===\n";
    string name, email, phone, pass, uni = "", sid = "";
    bool isStudent = false;
    char studentChoice;

    cin.ignore();
    cout << "Name: "; getline(cin, name);
    cout << "Email: "; getline(cin, email);
    
    // Check duplicates
    for(const auto& u : users) {
        if(u.email == email) {
            cout << "Email already exists!\n";
            pause();
            return;
        }
    }

    cout << "Phone: "; getline(cin, phone);
    cout << "Password: "; getline(cin, pass);
    cout << "Student? (y/n): "; cin >> studentChoice;
    if (tolower(studentChoice) == 'y') {
        isStudent = true;
        cout << "Uni Name: "; cin.ignore(); getline(cin, uni);
        cout << "Student ID: "; getline(cin, sid);
    } 

    users.push_back(User(name, email, phone, pass, isStudent, uni, sid));
    saveData(); // Save immediately
    cout << "Success! Please Login.\n";
    pause();
}

void CinemaSystem::HomePage() {
    while (true) {
        clearScreen();
        cout << "Welcome, " << currentUser->name << "!\n";
        cout << "1. Recommendation\n2. All Movies\n3. My Tickets\n4. Profile\n5. Logout\nSelect: ";
        int choice = getIntInput();
        switch(choice) {
            case 1: RecommendationPage(); break;
            case 2: AllMoviePage(); break;
            case 3: MyTicketPage(); break;
            case 4: UserProfilePage(); break;
            case 5: currentUser = nullptr; return;
        }
    }
}

void CinemaSystem::RecommendationPage() {
    clearScreen();
    cout << "=== RECOMMENDED ===\n";
    for(int i=0; i<3 && i<movies.size(); i++) {
        cout << i+1 << ". " << movies[i].title << endl;
    }
    cout << "4. Back\nSelect: ";
    int choice = getIntInput();
    if(choice >= 1 && choice <= 3) MovieDetailsPage(movies[choice-1]);
}

void CinemaSystem::AllMoviePage() {
    while(true) {
        clearScreen();
        cout << "=== ALL MOVIES ===\n";
        for(size_t i=0; i<movies.size(); i++) {
            cout << i+1 << ". " << movies[i].title << endl;
        }
        cout << movies.size()+1 << ". Back\nSelect: ";
        int choice = getIntInput();
        if(choice > 0 && choice <= movies.size()) MovieDetailsPage(movies[choice-1]);
        else return;
    }
}

void CinemaSystem::MovieDetailsPage(Movie& m) {
    while(true) {
        clearScreen();
        cout << "=== " << m.title << " ===\n";
        m.displayDetails();
        cout << "\n1. Buy Ticket\n2. Back\nSelect: ";
        int choice = getIntInput();
        if(choice == 1) SelectDatePage(m);
        else return;
    }
}

void CinemaSystem::SelectDatePage(Movie& m) {
    clearScreen();
    cout << "=== SELECT DATE ===\n1. Today\n2. Tomorrow\n";
    int d = getIntInput();
    string date = (d==1) ? "Today" : "Tomorrow";
    SelectExperiencePage(m, date);
}

void CinemaSystem::SelectExperiencePage(Movie& m, string date) {
    clearScreen();
    cout << "=== EXPERIENCE ===\n";
    for(size_t i=0; i<m.experiences.size(); i++) cout << i+1 << ". " << m.experiences[i] << endl;
    int e = getIntInput();
    if (e < 1 || e > m.experiences.size()) return;
    SelectShowTimePage(m, date, m.experiences[e-1]);
}

void CinemaSystem::SelectShowTimePage(Movie& m, string date, string exp) {
    clearScreen();
    cout << "=== SHOWTIME ===\n";
    for(size_t i=0; i<m.showtimes.size(); i++) cout << i+1 << ". " << m.showtimes[i] << endl;
    int t = getIntInput();
    if (t < 1 || t > m.showtimes.size()) return;
    SelectSeatPage(m, date, exp, m.showtimes[t-1]);
}

void CinemaSystem::SelectSeatPage(Movie& m, string date, string exp, string time) {
    clearScreen();
    cout << "=== SEATS (" << m.title << ") ===\n";
    cout << "[ SCREEN ]\nA 1 2 3 4\nB 1 2 3 4\n";
    vector<string> seats;
    while(true) {
        string s;
        cout << "Enter Seat (or 'done'): "; cin >> s;
        if(s == "done") break;
        seats.push_back(s);
    }
    if(!seats.empty()) CheckingTicketPage(m, date, exp, time, seats);
}

void CinemaSystem::CheckingTicketPage(Movie& m, string date, string exp, string time, vector<string> seats) {
    clearScreen();
    cout << "=== CATEGORIES ===\nTotal Seats: " << seats.size() << endl;
    int adult, student;
    cout << "Adults: "; cin >> adult;
    cout << "Students: "; cin >> student;
    
    if(adult + student != seats.size()) {
        cout << "Count mismatch!\n"; pause(); return;
    }
    
    double total = (adult * m.basePrice) + (student * m.basePrice * 0.8);
    // Experience multiplier
    if (exp == "IMAX") total *= 1.5;
    else if (exp == "Gold Class") total *= 2.0;

    PaymentPage(m, date, exp, time, seats, total);
}

void CinemaSystem::PaymentPage(Movie& m, string date, string exp, string time, vector<string> seats, double total) {
    clearScreen();
    cout << "Total: $" << total << "\nConfirm Pay? (y/n): ";
    char c; cin >> c;
    if(tolower(c) == 'y') ReceiptPage(m, date, exp, time, seats, total);
}

void CinemaSystem::ReceiptPage(Movie& m, string date, string exp, string time, vector<string> seats, double total) {
    clearScreen();
    cout << "=== RECEIPT ===\nID: " << ticketCounter << "\nMovie: " << m.title << "\nPaid: $" << total << endl;
    
    Ticket t = {ticketCounter++, m.title, date, time, exp, seats, total, "Active"};
    currentUser->addTicket(t);
    saveData(); // Save Booking!
    
    cout << "\nSaved to My Tickets.\n";
    pause();
}

void CinemaSystem::MyTicketPage() {
    clearScreen();
    cout << "=== MY TICKETS ===\n";
    if(currentUser->myTickets.empty()) { cout << "No tickets.\n"; pause(); return; }
    
    for(auto& t : currentUser->myTickets) cout << t.getDetails() << endl;
    
    cout << "\nEnter ID to cancel or 0 to back: ";
    int id = getIntInput();
    if(id != 0) {
        if(currentUser->removeTicket(id)) {
            saveData();
            cout << "Ticket Cancelled.\n";
        } else {
            cout << "Ticket not found or already cancelled.\n";
        }
        pause();
    }
}

void CinemaSystem::UserProfilePage() {
    clearScreen();
    cout << "Name: " << currentUser->name << "\nEmail: " << currentUser->email << "\nStudent: " << (currentUser->isStudent ? "Yes" : "No") << endl;
    pause();
}