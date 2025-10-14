#include "Funcs.h"
#include "Utils.h"
#include "Crypto.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
using namespace std;

// Запрос на сброс пароля
struct ResetRequest {
    string username;
    string newPassword;
};

// Загрузка запросов
static vector<ResetRequest> loadResetRequests(const string& requestsFile) {
    vector<ResetRequest> out;
    ifstream in(requestsFile);
    if (!in.is_open()) return out;
    string u, p;
    while (in >> u >> p) {
        out.push_back({ u, p });
    }
    return out;
}

// Сохранение запросов
static void saveResetRequests(const string& requestsFile, const vector<ResetRequest>& reqs) {
    ofstream out(requestsFile, ios::trunc);
    if (!out.is_open()) return;
    for (const auto& r : reqs) out << r.username << " " << r.newPassword << endl;
}

// Изменение пароля в accounts.txt
static bool changePasswordInAccounts(const string& accountsFile, const string& targetUser, const string& newPassword) {
    ifstream in(accountsFile);
    if (!in.is_open()) return false;

    string tempName = accountsFile + ".tmp";
    ofstream out(tempName);
    if (!out.is_open()) { in.close(); return false; }

    string u, p;
    bool updated = false;
    while (in >> u >> p) {
        if (u == targetUser) {
            out << u << " " << newPassword << endl;
            updated = true;
        } else {
            out << u << " " << p << endl;
        }
    }
    in.close();
    out.close();

    if (!updated) {
        remove(tempName.c_str());
        return false;
    }

    remove(accountsFile.c_str());
    rename(tempName.c_str(), accountsFile.c_str());
    return true;
}

// Обработка запросов администратором
void adminViewAndProcessRequests(const string& requestsFile, const string& accountsFile) {
    vector<ResetRequest> reqs = loadResetRequests(requestsFile);
    if (reqs.empty()) {
        cout << YELLOW << "Никаких запросов на сброс пароля." << RESET << endl;
        return;
    }

    cout << endl << "Запросы на сброс пароля:" << endl;
    for (size_t i = 0; i < reqs.size(); ++i) {
        cout << i + 1 << ". Пользователь: " << reqs[i].username 
             << " | Новый пароль: " << reqs[i].newPassword << endl;
    }

    cout << "Введите номер запроса для утверждения (0 - cancel): ";
    int choice;
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << RED << "Неверный ввод." << RESET << endl;
        return;
    }

    if (choice <= 0 || choice > (int)reqs.size()) {
        cout << YELLOW << "Отмененный." << RESET << endl;
        return;
    }

    ResetRequest toApprove = reqs[choice - 1];

    bool userExists = false;
    ifstream aIn(accountsFile);
    if (aIn.is_open()) {
        string u, p;
        while (aIn >> u >> p) if (u == toApprove.username) { userExists = true; break; }
    }

    if (!userExists) {
        cout << RED << "Пользователь не найден. Запрос удален." << RESET << endl;
        reqs.erase(reqs.begin() + (choice - 1));
        saveResetRequests(requestsFile, reqs);
        return;
    }

    if (changePasswordInAccounts(accountsFile, toApprove.username, toApprove.newPassword)) {
        cout << GREEN << "Пароль для " << toApprove.username << " измененный." << RESET << endl;
        reqs.erase(reqs.begin() + (choice - 1));
        saveResetRequests(requestsFile, reqs);
    } else {
        cout << RED << "Ошибка при обновлении файла учетных записей." << RESET << endl;
    }
}
