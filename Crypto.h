#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <fstream>
using namespace std;

// Простое хэширование паролей
// Это базовый метод для учебных целей (в реальных проектах используйте bcrypt/SHA-256)
class SimpleCrypto {
private:
    // Простая хэш-функция для паролей
    static unsigned int simpleHash(const string& str) {
        unsigned int hash = 5381;
        for (char c : str) {
            hash = ((hash << 5) + hash) + (unsigned char)c; // hash * 33 + c
        }
        return hash;
    }
    
public:
    // Хэширование пароля (логин + пароль)
    static string hashPassword(const string& login, const string& password) {
        string combined = login + password;
        unsigned int hash = simpleHash(combined);
        
        // Преобразуем в строку
        stringstream ss;
        ss << hash;
        return ss.str();
    }
    
    // Проверка пароля
    static bool verifyPassword(const string& login, const string& password, const string& storedHash) {
        return hashPassword(login, password) == storedHash;
    }
    
    // Автоматическая миграция паролей из старого формата в хэш
    static void autoMigratePasswords(const string& filename) {
        ifstream in(filename);
        if (!in.is_open()) return;
        
        vector<pair<string, string>> users; // login, password_or_hash
        string login, passData;
        bool needsMigration = false;
        
        // Читаем все записи
        while (in >> login >> passData) {
            // Проверяем, является ли это хэшем:
            // Хэш - это строка из цифр длиной 9-11 символов (результат simpleHash)
            bool isHash = true;
            
            // Проверка 1: Длина хэша обычно 9-11 символов
            if (passData.length() < 9 || passData.length() > 11) {
                isHash = false;
            } else {
                // Проверка 2: Все символы должны быть цифрами
                for (char c : passData) {
                    if (!isdigit(c)) {
                        isHash = false;
                        break;
                    }
                }
            }
            
            // Дополнительная проверка: если это выглядит как plaintext пароль
            if (passData.length() <= 20 && !isHash) {
                // Скорее всего это plaintext пароль
                needsMigration = true;
            }
            
            users.push_back({login, passData});
        }
        in.close();
        
        // Если нужна миграция
        if (needsMigration) {
            ofstream out(filename + ".migrated");
            for (const auto& user : users) {
                // Проверяем каждого пользователя отдельно
                bool isHash = true;
                
                if (user.second.length() < 9 || user.second.length() > 11) {
                    isHash = false;
                } else {
                    for (char c : user.second) {
                        if (!isdigit(c)) {
                            isHash = false;
                            break;
                        }
                    }
                }
                
                if (!isHash) {
                    // Хэшируем старый plaintext пароль
                    string hash = hashPassword(user.first, user.second);
                    out << user.first << " " << hash << endl;
                } else {
                    // Уже хэш - оставляем как есть
                    out << user.first << " " << user.second << endl;
                }
            }
            out.close();
            
            // Заменяем файл
            remove(filename.c_str());
            rename((filename + ".migrated").c_str(), filename.c_str());
        }
    }
};