#include "pch.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>
#include <algorithm>
#include <string>

using namespace std;
char encrypt_char(char c, int key) {
    if (!isalpha(c)) return c;
    char base = isupper(c) ? 'A' : 'a';
    return (c - base + key) % 26 + base;
}

char decrypt_char(char c, int key) {
    if (!isalpha(c)) return c;
    char base = isupper(c) ? 'A' : 'a';
    return (c - base - key + 26) % 26 + base;
}

string encrypt(const string& plain, int key) {
    string cipher;
    for (char c : plain) {
        cipher += encrypt_char(c, key);
    }
    return cipher;
}

string decrypt(const string& cipher, int key) {
    string plain;
    for (char c : cipher) {
        plain += decrypt_char(c, key);
    }
    return plain;
}

int known_plaintext_attack(const string& plain, const string& cipher) {
    for (int key = 0; key < 26; ++key) {
        if (encrypt(plain, key) == cipher) {
            return key;
        }
    }
    return -1;
}

void ciphertext_only_attack(const string& cipher) {
    cout << "Все варианты расшифровки:" << endl;
    for (int key = 0; key < 26; ++key) {
        cout << "Ключ " << key << ": " << decrypt(cipher, key) << endl;
    }
}

vector<string> load_dictionary(const string& filename) {
    vector<string> dict;
    ifstream file(filename);
    string word;
    while (file >> word) {
        transform(word.begin(), word.end(), word.begin(), ::toupper);
        dict.push_back(word);
    }
    return dict;
}

int count_matches(const string& text, const vector<string>& dict) {
    string word;
    int matches = 0;
    for (char c : text) {
        if (isalpha(c)) {
            word += toupper(c);
        }
        else if (!word.empty()) {
            if (find(dict.begin(), dict.end(), word) != dict.end()) {
                matches++;
            }
            word.clear();
        }
    }
    if (!word.empty() && find(dict.begin(), dict.end(), word) != dict.end()) {
        matches++;
    }
    return matches;
}

int ciphertext_only_attack_dict(const string& cipher, const vector<string>& dict) {
    int best_key = 0;
    int max_matches = -1;

    for (int key = 0; key < 26; ++key) {
        string plain = decrypt(cipher, key);
        int matches = count_matches(plain, dict);

        if (matches > max_matches) {
            max_matches = matches;
            best_key = key;
        }
    }
    return best_key;
}

void display_menu() {
    cout << "\n=== Шифр цезаря (c) Жиляев Максим ААМ-24 ===" << endl;
    cout << "1. Шифрование" << endl;
    cout << "2. Расшифрование" << endl;
    cout << "3. Атака по известному открытому тексту" << endl;
    cout << "4. Атака по шифрованному тексту" << endl;
    cout << "5. Атака по шифрованному тексту со словарём" << endl;
    cout << "6. Выход" << endl;
    cout << "Выберите пункт: ";
}

int main() {
    vector<string> dictionary;
    bool dict_loaded = false;
    string dict_file;
    setlocale(LC_CTYPE, "rus");

    while (true) {
        display_menu();
        int choice;
        cin >> choice;
        cin.ignore(); 
        switch (choice) {
        case 1: {
            string text;
            int key;
            cout << "Введите текст для шифровки: ";
            getline(cin, text);
            cout << "Введите ключ (0-25): ";
            cin >> key;
            cout << "Зашифрованный текст: " << encrypt(text, key) << endl;
            break;
        }
        case 2: {
            string text;
            int key;
            cout << "Введите текст для расшифровки: ";
            getline(cin, text);
            cout << "Введите ключ (0-25): ";
            cin >> key;
            cout << "Расшифрованный текст: " << decrypt(text, key) << endl;
            break;
        }
        case 3: {
            string plain, cipher;
            cout << "Введите открытый текст: ";
            getline(cin, plain);
            cout << "Введите зашифрованный текст: ";
            getline(cin, cipher);
            int key = known_plaintext_attack(plain, cipher);
            if (key != -1) {
                cout << "Найден ключ: " << key << endl;
            }
            else {
                cout << "Ключ не найден!" << endl;
            }
            break;
        }
        case 4: {
            string cipher;
            cout << "Введите зашифрованный текст: ";
            getline(cin, cipher);
            ciphertext_only_attack(cipher);
            break;
        }
        case 5: {
            if (!dict_loaded) {
                cout << "Введите путь к файлу: ";
                getline(cin, dict_file);
                dictionary = load_dictionary(dict_file);
                dict_loaded = true;
                cout << "Словарь загружен: " << dictionary.size() << " слов" << endl;
            }

            string cipher;
            cout << "Введите зашифрованный текст: ";
            getline(cin, cipher);
            int key = ciphertext_only_attack_dict(cipher, dictionary);
            cout << "Вероятный ключ: " << key << endl;
            cout << "Расшифрованный текст: " << decrypt(cipher, key) << endl;
            break;
        }
        case 6:
            return 0;
        default:
            cout << "Неверный ввод!" << endl;
         }
    }
}