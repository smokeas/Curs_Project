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
        cout << endl << "=== Пользовательское меню ===" << endl;
        cout << "1. Забронировать тур" << endl;
        cout << "2. Отменить бронирование" << endl;
        cout << "3. Мои заказы" << endl;
        cout << "4. Поменять пароль" << endl;
        cout << "0. Выход из системы" << endl;
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
        cout << endl << "=== Меню администратора ===" << endl;
        cout << "1. Добавить тур" << endl;
        cout << "2. Удалить тур" << endl;
        cout << "3. Показать туры" << endl;
        cout << "4. Показать все заказы" << endl;
        cout << "5. Обрабатывать запросы на сброс пароля" << endl;
        cout << "0. Выход из системы" << endl;
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

     //setlocale(LC_ALL, "Russian");

    cout << GREEN << "=== Запущена программа туристического  агентства ===" << RESET << endl;

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
        cout << endl << "=== Главное меню ===" << endl;
        cout << BLUE << "1. Регистрация" << RESET << endl;
        cout << GREEN << "2. Вход" << RESET << endl;
        cout << "3. Посмотреть туры" << endl;
        cout << YELLOW << "4. Забыл проль " << RESET << endl;
        cout << RED << "0. Выход" << RESET << endl;
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
