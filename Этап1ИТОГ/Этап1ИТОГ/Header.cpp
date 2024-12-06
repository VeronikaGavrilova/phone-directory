#include "Header.h"



void Contact::display() const
{
    cout << "Имя: " << firstName << ", Фамилия: " << lastName
        << ", Отчество: " << middleName << ", Адрес: " << address
        << ", Дата рождения: " << birthDate << ", Email: " << email
        << ", Телефоны: ";
    for (const auto& phone : phoneNumbers) {
        cout << phone << " ";
    }
    cout << endl;
}

bool ContactManager::isValidName(const wstring& name)
{
    wregex nameRegex(L"^[A-Za-zА-Яа-яЁё][A-Za-zА-Яа-яЁё0-9\\s-]*[A-Za-zА-Яа-яЁё0-9]$");
    return regex_match(name, nameRegex);
}

bool ContactManager::isValidName(const string& name)
{
    regex nameRegex("^[A-Za-zА-Яа-яЁё][A-Za-zА-Яа-яЁё0-9\\s-]*[A-Za-zА-Яа-яЁё0-9]$");
    return regex_match(name, nameRegex);
}

bool ContactManager::isValidEmail(const string& email)
{
    regex emailRegex(R"((\w+)(\.{0,1}\w+)*@(\w+)(\.\w+)+)");
    return regex_match(email, emailRegex);
}

bool ContactManager::isValidPhone(const string& phone)
{
    regex phoneRegex(R"(\+?\d{1,3}[- ]?\(?\d{1,4}?\)?[- ]?\d{1,4}[- ]?\d{1,4})");
    return regex_match(phone, phoneRegex);
}

bool ContactManager::isValidDate(const string& date)
{
    tm tm = {};
    istringstream ss(date);
    ss >> get_time(&tm, "%Y-%m-%d");
    if (ss.fail()) return false;

    time_t t = time(nullptr);
    std::tm now;
    localtime_s(&now, &t);

    if (tm.tm_year > now.tm_year ||
        (tm.tm_year == now.tm_year && tm.tm_mon > now.tm_mon) ||
        (tm.tm_year == now.tm_year && tm.tm_mon == now.tm_mon && tm.tm_mday > now.tm_mday)) {
        return false;
    }

    return true;
}

void ContactManager::loadContacts()
{
    ifstream file(filename);
    if (!file.is_open()) return;

    Contact contact;
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        getline(iss, contact.firstName, ',');
        getline(iss, contact.lastName, ',');
        getline(iss, contact.middleName, ',');
        getline(iss, contact.address, ',');
        getline(iss, contact.birthDate, ',');
        getline(iss, contact.email, ',');
        string phones;
        getline(iss, phones);
        istringstream phoneStream(phones);
        string phone;
        while (getline(phoneStream, phone, ' ')) {
            contact.phoneNumbers.push_back(phone);
        }
        contacts.push_back(contact);
    }
    file.close();
}

void ContactManager::saveContacts()
{
    ofstream file(filename);
    for (const auto& contract : contacts) {
        file << contract.firstName << ","
            << contract.lastName << ","
            << contract.middleName << ","
            << contract.address << ","
            << contract.birthDate << ","
            << contract.email << ",";
        for (const auto& phone : contract.phoneNumbers) {
            file << phone << " ";
        }
        file << endl;
    }
    file.close();
}

void ContactManager::addContact()
{
    Contact contact;
    cout << "Введите Имя: ";
    getline(cin, contact.firstName);
    if (!isValidName(contact.firstName)) {
        cout << "Некорректное имя!" << endl;
        return;
    }

    cout << "Введите Фамилию: ";
    getline(cin, contact.lastName);
    if (!isValidName(contact.lastName)) {
        cout << "Некорректная фамилия!" << endl;
        return;
    }

    cout << "Введите Отчество: ";
    getline(cin, contact.middleName);
    if (!isValidName(contact.middleName)) {
        cout << "Некорректное отчество!" << endl;
        return;
    }

    cout << "Введите Адрес: ";
    getline(cin, contact.address);

    cout << "Введите Дату рождения (YYYY-MM-DD): ";
    getline(cin, contact.birthDate);
    if (!isValidDate(contact.birthDate)) {
        cout << "Некорректная дата рождения!" << endl;
        return;
    }

    cout << "Введите Email: ";
    getline(cin, contact.email);
    if (!isValidEmail(contact.email)) {
        cout << "Некорректный email!" << endl;
        return;
    }

    string phone;
    cout << "Введите телефонные номера (введите 'end' для завершения): " << endl;
    while (true) {
        getline(cin, phone);
        if (phone == "end") break;
        if (isValidPhone(phone)) {
            contact.phoneNumbers.push_back(phone);
        }
        else {
            cout << "Некорректный номер телефона!" << endl;
        }
    }

    contacts.push_back(contact);
    saveContacts();
    cout << "Контакт успешно добавлен!" << endl;
}

void ContactManager::displayContacts()
{
    for (const auto& contract : contacts) {
        contract.display();
    }
}

void ContactManager::deleteContact()
{
    string lastName;
    cout << "Введите фамилию для удаления: ";
    getline(cin, lastName);
    contacts.erase(remove_if(contacts.begin(), contacts.end(),
        [&lastName](const Contact& c) { return c.lastName == lastName; }), contacts.end());
    saveContacts();
    cout << "Контакт успешно удален!" << endl;
}

void ContactManager::editContact()
{
    string lastName;
    cout << "Введите фамилию для редактирования: ";
    getline(cin, lastName);
    for (auto& contract : contacts) {
        if (contract.lastName == lastName) {
            cout << "Редактирование записи:" << endl;
            contract.display();
            cout << "Введите новое Имя (или оставьте пустым для пропуска): ";
            string newName;
            getline(cin, newName);
            if (!newName.empty() && isValidName(newName)) {
                contract.firstName = newName;
            }

            cout << "Введите новую Фамилию (или оставьте пустым для пропуска): ";
            string newLastName;
            getline(cin, newLastName);
            if (!newLastName.empty() && isValidName(newLastName)) {
                contract.lastName = newLastName;
            }

            cout << "Введите новое Отчество (или оставьте пустым для пропуска): ";
            string newMiddleName;
            getline(cin, newMiddleName);
            if (!newMiddleName.empty() && isValidName(newMiddleName)) {
                contract.middleName = newMiddleName;
            }

            cout << "Введите новый Адрес (или оставьте пустым для пропуска): ";
            string newAddress;
            getline(cin, newAddress);
            if (!newAddress.empty()) {
                contract.address = newAddress;
            }

            cout << "Введите новую Дату рождения (YYYY-MM-DD, или оставьте пустым для пропуска): ";
            string newBirthDate;
            getline(cin, newBirthDate);
            if (!newBirthDate.empty() && isValidDate(newBirthDate)) {
                contract.birthDate = newBirthDate;
            }

            cout << "Введите новый Email (или оставьте пустым для пропуска): ";
            string newEmail;
            getline(cin, newEmail);
            if (!newEmail.empty() && isValidEmail(newEmail)) {
                contract.email = newEmail;
            }

            cout << "Введите новые телефонные номера (введите 'end' для завершения): " << endl;
            contract.phoneNumbers.clear();
            string phone;
            while (true) {
                getline(cin, phone);
                if (phone == "end") break;
                if (isValidPhone(phone)) {
                    contract.phoneNumbers.push_back(phone);
                }
                else {
                    cout << "Некорректный номер телефона!" << endl;
                }
            }

            saveContacts();
            return;
        }
    }
    cout << "Запись с такой фамилией не найдена!" << endl;
}

void ContactManager::searchContacts()
{
    string query;
    cout << "Введите строку для поиска: ";
    getline(cin, query);
    for (const auto& contract : contacts) {
        if (contract.firstName.find(query) != string::npos ||
            contract.lastName.find(query) != string::npos ||
            contract.middleName.find(query) != string::npos ||
            contract.address.find(query) != string::npos ||
            contract.email.find(query) != string::npos) {
            contract.display();
        }
    }
}

void ContactManager::sortContacts()
{
    int choice;
    cout << "Выберите поле для сортировки:\n";
    cout << "1. Имя\n";
    cout << "2. Фамилия\n";
    cout << "3. Отчество\n";
    cout << "4. Дата рождения\n";
    cout << "5. Email\n";
    cout << "6. Адрес\n";
    cout << "Ваш выбор: ";
    cin >> choice;
    cin.ignore();

    switch (choice) {
    case 1:
        sort(contacts.begin(), contacts.end(),
            [](const Contact& a, const Contact& b) {
                return a.firstName < b.firstName;
            });
        break;
    case 2:
        sort(contacts.begin(), contacts.end(),
            [](const Contact& a, const Contact& b) {
                return a.lastName < b.lastName;
            });
        break;
    case 3:
        sort(contacts.begin(), contacts.end(),
            [](const Contact& a, const Contact& b) {
                return a.middleName < b.middleName;
            });
        break;
    case 4:
        sort(contacts.begin(), contacts.end(),
            [](const Contact& a, const Contact& b) {
                return a.birthDate < b.birthDate;
            });
        break;
    case 5:
        sort(contacts.begin(), contacts.end(),
            [](const Contact& a, const Contact& b) {
                return a.email < b.email;
            });
        break;
    case 6:
        sort(contacts.begin(), contacts.end(),
            [](const Contact& a, const Contact& b) {
                return a.address < b.address;
            });
        break;
    default:
        cout << "Неверный выбор. Сортировка не выполнена." << endl;
        return;
    }
    cout << "Контакты отсортированы!" << endl;
    displayContacts();

}


int main() {
    setlocale(LC_ALL, "Russian");
    ContactManager manager;
    manager.loadContacts();

    while (true) {
        cout << "1. Добавить контакт\n"
            << "2. Показать контакты\n"
            << "3. Удалить контакт\n"
            << "4. Редактировать контакт\n"
            << "5. Поиск контактов\n"
            << "6. Сортировка\n"
            << "7. Выход\n"
            << "Выберите действие: ";
        int choice;
        cin >> choice;
        cin.ignore(); // игнорируем символ новой строки после ввода числа

        switch (choice) {
        case 1:
            manager.addContact();
            break;
        case 2:
            manager.displayContacts();
            break;
        case 3:
            manager.deleteContact();
            break;
        case 4:
            manager.editContact();
            break;
        case 5:
            manager.searchContacts();
            break;
        case 6:
            manager.sortContacts();
            break;
        case 7:
            return 0;
        default:
            cout << "Некорректный выбор!" << endl;
        }
    }

    return 0;
}