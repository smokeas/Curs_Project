#include "Funcs.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <limits>
using namespace std;

// Методы класса Tour
void Tour::showInfo() const {
    cout << "Tour: " << name << " | Country: " << country << " | Price: $" << price << endl;
}

// Методы класса TourManager
void TourManager::loadTours(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return;
    tours.clear();
    string n, c;
    double p;
    while (file >> n >> c >> p) {
        tours.emplace_back(n, c, p);
    }
}

void TourManager::showTours() const {
    cout << endl << "Available tours:" << endl;
    if (tours.empty()) {
        cout << "No tours available." << endl;
        return;
    }
    for (size_t i = 0; i < tours.size(); ++i) {
        cout << i + 1 << ". ";
        tours[i].showInfo();
    }
}

Tour TourManager::getTour(int index) const {
    if (index >= 0 && index < (int)tours.size())
        return tours[index];
    return Tour();
}

void TourManager::addTour(const string& filename) {
    string name, country;
    double price;
    cout << "Введите название тура: ";
    cin >> name;
    cout << "Введите страну: ";
    cin >> country;
    cout << "Введите цену: ";
    cin >> price;

    tours.emplace_back(name, country, price);

    ofstream file(filename, ios::app);
    if (file.is_open()) {
        file << name << " " << country << " " << price << endl;
    }
    cout << GREEN << "Тур добавлен." << RESET << endl;
}

void TourManager::removeTour(const string& filename) {
    if (tours.empty()) {
        cout << "Нет туров для удаления." << endl;
        return;
    }
    showTours();
    cout << "Введите номер тура для удаления: ";
    int choice;
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << RED << "Неверный ввод." << RESET << endl;
        return;
    }

    if (choice < 1 || choice > getToursCount()) {
        cout << RED << "Неверный ввод." << RESET << endl;
        return;
    }

    tours.erase(tours.begin() + (choice - 1));

    ofstream file(filename, ios::trunc);
    if (file.is_open()) {
        for (const auto& tour : tours) {
            file << tour.getName() << " " << tour.getCountry() << " " << tour.getPrice() << endl;
        }
    }
    cout << GREEN << "Тур удален." << RESET << endl;
}

// Методы класса Booking
void Booking::showInfo() const {
    cout << "Пользователь: " << username << " забронированный тур: " << tourName << endl;
}

// Методы класса BookingManager
void BookingManager::loadBookings(const string& filename) {
    ifstream file(filename);
    bookings.clear();
    if (!file.is_open()) return;
    string u, t;
    while (file >> u >> t) {
        bookings.emplace_back(u, t);
    }
}

void BookingManager::makeBooking(const User& user, const TourManager& tm, const string& filename) {
    if (!user.isLoggedIn()) {
        cout << RED << "Пожалуйста, сначала войдите в систему." << RESET << endl;
        return;
    }
    if (tm.getToursCount() == 0) {
        cout << "Никаких доступных туров нет." << endl;
        return;
    }

    tm.showTours();
    cout << "Введите номер тура для бронирования: ";
    int choice;
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << RED << "Неверный ввод." << RESET << endl;
        return;
    }

    if (choice < 1 || choice > tm.getToursCount()) {
        cout << RED << "Неверный ввод." << RESET << endl;
        return;
    }

    Tour tour = tm.getTour(choice - 1);
    bookings.emplace_back(user.getName(), tour.getName());

    ofstream file(filename, ios::app);
    if (file.is_open()) file << user.getName() << " " << tour.getName() << endl;

    cout << GREEN << "Бронирование прошло успешно." << RESET << endl;
}

void BookingManager::cancelBooking(const User& user, const string& filename) {
    if (!user.isLoggedIn()) {
        cout << RED << "Пожалуйста, сначала войдите в систему." << RESET << endl;
        return;
    }

    cout << endl << "Ваши заказы:" << endl;
    vector<int> indices;
    for (size_t i = 0; i < bookings.size(); ++i) {
        if (bookings[i].getUser() == user.getName()) {
            cout << indices.size() + 1 << ". " << bookings[i].getTour() << endl;
            indices.push_back(i);
        }
    }

    if (indices.empty()) {
        cout << "У вас нет бронирований." << endl;
        return;
    }

    cout << "Введите номер бронирования для отмены: ";
    int choice;
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << RED << "Неверный ввод." << RESET << endl;
        return;
    }

    if (choice < 1 || choice > (int)indices.size()) {
        cout << RED << "Неверный ввод." << RESET << endl;
        return;
    }

    bookings.erase(bookings.begin() + indices[choice - 1]);

    ofstream file(filename, ios::trunc);
    if (file.is_open()) {
        for (const auto& b : bookings) {
            file << b.getUser() << " " << b.getTour() << endl;
        }
    }
    cout << GREEN << "Бронирование отменено." << RESET << endl;
}

void BookingManager::showBookings(const User& user) const {
    if (!user.isLoggedIn()) {
        cout << RED << "Пожалуйста, сначала войдите в систему." << RESET << endl;
        return;
    }

    cout << endl << "Ваши заказы:" << endl;
    bool found = false;
    for (const auto& b : bookings) {
        if (b.getUser() == user.getName()) {
            b.showInfo();
            found = true;
        }
    }
    if (!found) cout << "У вас нет бронирований." << endl;
}

void BookingManager::viewAllBookings() const {
    cout << endl << "Все заказы:" << endl;
    if (bookings.empty()) {
        cout << "Никаких бронирований." << endl;
        return;
    }
    for (const auto& b : bookings) b.showInfo();
}
