#include <iostream>
#include <fstream>
#include <limits>
#include "Funcs.h"
#include "Utils.h"
using namespace std;

// Меню пользователя
void userMenu(User& user, TourManager& tm, BookingManager& bm) {
    int choice;
    do {
        cout << endl << "=== User menu ===" << endl;
        cout << "1. Book tour" << endl;
        cout << "2. Cancel booking" << endl;
        cout << "3. My bookings" << endl;
        cout << "4. Change password" << endl;
        cout << "0. Logout" << endl;
        cout << "Choice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: bm.makeBooking(user, tm, "bookings.txt"); break;
            case 2: bm.cancelBooking(user, "bookings.txt"); break;
            case 3: bm.showBookings(user); break;
            case 4: user.changeOwnPassword("accounts.txt"); break;
            case 0: user.logout(); break;
            default: cout << RED << "Invalid choice." << RESET << endl;
        }
    } while (choice != 0 && user.isLoggedIn());
}

// Меню администратора
void adminMenu(User& user, TourManager& tm, BookingManager& bm) {
    int choice;
    do {
        cout << endl << "=== Admin menu ===" << endl;
        cout << "1. Add tour" << endl;
        cout << "2. Delete tour" << endl;
        cout << "3. Show tours" << endl;
        cout << "4. Show all bookings" << endl;
        cout << "5. Process password reset requests" << endl;
        cout << "0. Logout" << endl;
        cout << "Choice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: tm.addTour("tours.txt"); break;
            case 2: tm.removeTour("tours.txt"); break;
            case 3: tm.showTours(); break;
            case 4: bm.viewAllBookings(); break;
            case 5: adminViewAndProcessRequests("requests.txt", "accounts.txt"); break;
            case 0: user.logout(); break;
            default: cout << RED << "Invalid choice." << RESET << endl;
        }
    } while (choice != 0 && user.isLoggedIn());
}

int main() {

     setlocale(LC_ALL, "Russian");

    cout << GREEN << "=== Travel Agency System Started ===" << RESET << endl;

    // Проверка наличия администратора
    ifstream check("accounts.txt");
    bool adminFound = false;
    if (check.is_open()) {
        string u, p;
        while (check >> u >> p) {
            if (u == "admin") { adminFound = true; break; }
        }
        check.close();
    }
    if (!adminFound) {
        ofstream create("accounts.txt", ios::app);
        if (create.is_open()) {
            create << "admin admin" << endl;
            create.close();
        }
    }

    User user;
    TourManager tourManager;
    tourManager.loadTours("tours.txt");

    BookingManager bookingManager;
    bookingManager.loadBookings("bookings.txt");

    int choice;
    do {
        cout << endl << "=== Main menu ===" << endl;
        cout << BLUE << "1. Register" << RESET << endl;
        cout << GREEN << "2. Login" << RESET << endl;
        cout << "3. View tours" << endl;
        cout << YELLOW << "4. Request password reset" << RESET << endl;
        cout << RED << "0. Exit" << RESET << endl;
        cout << "Choice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: 
                user.registerAccount("accounts.txt"); 
                break;
            case 2:
                user.login("accounts.txt");
                if (user.isLoggedIn()) {
                    tourManager.loadTours("tours.txt");
                    bookingManager.loadBookings("bookings.txt");
                    if (user.isAdmin()) adminMenu(user, tourManager, bookingManager);
                    else userMenu(user, tourManager, bookingManager);
                }
                break;
            case 3: 
                tourManager.showTours(); 
                break;
            case 4: 
                requestPasswordReset("requests.txt"); 
                break;
            case 0: 
                cout << RED << "Exiting program." << RESET << endl; 
                break;
            default: 
                cout << RED << "Invalid choice." << RESET << endl;
        }
    } while (choice != 0);

    return 0;
}
