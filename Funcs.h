#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
using namespace std;

// Проверка ввода и паролей
bool is_valid_input(const string& str, bool is_password);
bool is_password_strong(const string& password);
void evaluate_password_strength(const string& password);

// Пользователь
class User {
private:
    string username;
    string password;
    bool loggedIn = false;
    bool admin = false;
public:
    void registerAccount(const string& accountsFile);
    void login(const string& accountsFile);
    void logout();
    void changeOwnPassword(const string& accountsFile);

    string getName() const { return username; }
    bool isLoggedIn() const { return loggedIn; }
    bool isAdmin() const { return admin; }
};

// Тур
class Tour {
private:
    string name;
    string country;
    double price;
public:
    Tour(string n = "", string c = "", double p = 0.0)
        : name(n), country(c), price(p) {}
    string getName() const { return name; }
    string getCountry() const { return country; }
    double getPrice() const { return price; }
    void showInfo() const;
};

// Менеджер туров
class TourManager {
private:
    vector<Tour> tours;
public:
    void loadTours(const string& filename);
    void showTours() const;
    void addTour(const string& filename);
    void removeTour(const string& filename);
    Tour getTour(int index) const;
    int getToursCount() const { return (int)tours.size(); }
};

// Бронирование
class Booking {
private:
    string username;
    string tourName;
public:
    Booking(string u = "", string t = "") : username(u), tourName(t) {}
    string getUser() const { return username; }
    string getTour() const { return tourName; }
    void showInfo() const;
};

// Менеджер бронирований
class BookingManager {
private:
    vector<Booking> bookings;
public:
    void loadBookings(const string& filename);
    void makeBooking(const User& user, const TourManager& tm, const string& filename);
    void cancelBooking(const User& user, const string& filename);
    void showBookings(const User& user) const;
    void viewAllBookings() const;
};

// Админские функции
void adminViewAndProcessRequests(const string& requestsFile, const string& accountsFile);

// Запрос на сброс пароля (пользователь → админу)
void requestPasswordReset(const string& requestsFile);
