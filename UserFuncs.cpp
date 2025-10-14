#include "Funcs.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <limits>
#include <cctype>
using namespace std;

// Проверка символов
bool is_valid_input(const string& str, bool is_password) {
    for (unsigned char ch : str) {
        if (is_password) {
            if (!isprint(ch)) return false;
        } else {
            if (!isalnum(ch)) return false;
        }
    }
    return true;
}

// Проверка сложности пароля
bool is_password_strong(const string& password) {
    if (password.length() < 8) return false;
//Флаги
    bool has_upper = false, has_lower = false, has_digit = false, has_special = false;
    for (char c : password) {
        if (isupper((unsigned char)c)) has_upper = true;
        else if (islower((unsigned char)c)) has_lower = true;
        else if (isdigit((unsigned char)c)) has_digit = true;
        else if (ispunct((unsigned char)c)) has_special = true;
    }
    return has_upper && has_lower && has_digit && has_special;
}

// Анализ пароля
void evaluate_password_strength(const string& password) {
    cout << "\nПароль: " << password << endl;
    if (is_password_strong(password)) {
        cout << GREEN << " Надежный пароль" << RESET << endl;
    } else {
        cout << RED << " Слабый пароль" << RESET << endl;
        cout << "Требования: минимум 8 символов, заглавная, строчная, цифра, спецсимвол." << endl;
    }
}

// Регистрация
void User::registerAccount(const string& accountsFile) {
    string login, pass, confirm;
    cout << BLUE << "\n=== Регистрация ===" << RESET << endl;

    while (true) {
        cout << "Логин: ";
        cin >> login;
        if (!is_valid_input(login, false)) {
            cout << RED << "Недопустимые символы!" << RESET << endl;
            continue;
        }

        ifstream in(accountsFile);
        bool exists = false;
        string u, p;
        while (in >> u >> p) if (u == login) { exists = true; break; }
        if (exists) {
            cout << RED << "Такой логин уже существует." << RESET << endl;
            continue;
        }
        break;
    }

    while (true) {
        cout << "Пароль: ";
        cin >> pass;
        evaluate_password_strength(pass);
        if (!is_password_strong(pass)) continue;

        cout << "Подтвердите пароль: ";
        cin >> confirm;
        if (confirm != pass) {
            cout << RED << "Пароли не совпадают." << RESET << endl;
            continue;
        }
        break;
    }

    ofstream out(accountsFile, ios::app);
    out << login << " " << pass << endl;
    cout << GREEN << "Регистрация завершена!" << RESET << endl;
}

// Вход
void User::login(const string& accountsFile) {
    string u, p;
    cout << GREEN << "\n=== Вход ===" << RESET << endl;
    cout << "Логин: "; cin >> u;
    cout << "Пароль: "; cin >> p;

    ifstream in(accountsFile);
    bool ok = false;
    string fileUser, filePass;
    while (in >> fileUser >> filePass) {
        if (fileUser == u && filePass == p) {
            ok = true;
            break;
        }
    }

    if (ok) {
        username = u; password = p; loggedIn = true;
        if (u == "admin") admin = true;
        cout << GREEN << "Успешный вход!" << RESET << endl;
    } else {
        cout << RED << "Неверный логин или пароль." << RESET << endl;
    }
}

// Выход
void User::logout() {
    loggedIn = false; admin = false;
    username.clear(); password.clear();
    cout << YELLOW << "Вы вышли из системы." << RESET << endl;
}

// Смена пароля
void User::changeOwnPassword(const string& accountsFile) {
    if (!isLoggedIn()) {
        cout << RED << "Сначала войдите в систему." << RESET << endl;
        return;
    }

    string newPass, confirm;
    while (true) {
        cout << "Новый пароль: ";
        cin >> newPass;
        evaluate_password_strength(newPass);
        if (!is_password_strong(newPass)) continue;

        cout << "Подтвердите пароль: ";
        cin >> confirm;
        if (confirm != newPass) {
            cout << RED << "Пароли не совпадают." << RESET << endl;
            continue;
        }
        break;
    }

    ifstream in(accountsFile);
    ofstream out("tmp.txt");
    string u, p;
    while (in >> u >> p) {
        if (u == username) out << u << " " << newPass << endl;
        else out << u << " " << p << endl;
    }
    in.close(); out.close();
    remove(accountsFile.c_str());
    rename("tmp.txt", accountsFile.c_str());

    password = newPass;
    cout << GREEN << "Пароль изменен." << RESET << endl;
}

// Отправка запроса на сброс пароля (пользователь → админу)
void requestPasswordReset(const string& requestsFile) {
    string username, newPass, confirm;

    cout << BLUE << "\n=== Запрос на сброс пароля ===" << RESET << endl;
    cout << "Введите ваш логин: ";
    cin >> username;

    while (true) {
        cout << "Введите новый пароль: ";
        cin >> newPass;
        evaluate_password_strength(newPass);
        if (!is_password_strong(newPass)) continue;

        cout << "Подтвердите пароль: ";
        cin >> confirm;
        if (confirm != newPass) {
            cout << RED << "Пароли не совпадают." << RESET << endl;
            continue;
        }
        break;
    }

    ofstream out(requestsFile, ios::app);
    if (out.is_open()) {
        out << username << " " << newPass << endl;
        cout << GREEN << "Запрос отправлен админу. Ожидайте подтверждения." << RESET << endl;
    } else {
        cout << RED << "Ошибка записи в файл запросов." << RESET << endl;
    }
}
