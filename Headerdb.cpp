#include "Headerdb.h"

void ContactManager::initializeDatabase(const QString& dbFilename) {
    sqlite3* db;
    int rc = sqlite3_open(dbFilename.toStdString().c_str(), &db);

    if (rc) {
        throw std::runtime_error("Cannot open database");
    }

    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS Contacts (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            firstName TEXT,
            lastName TEXT,
            middleName TEXT,
            address TEXT,
            birthDate TEXT,
            email TEXT,
            phoneNumbers TEXT
        );
    )";

    char* errMsg = nullptr;
    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string error = "SQL error: " + std::string(errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(db);
        throw std::runtime_error(error);
    }

    sqlite3_close(db);
}
void ContactManager::saveToDatabase(const QString& dbFilename) const {
    sqlite3* db;
    int rc = sqlite3_open(dbFilename.toStdString().c_str(), &db);

    if (rc) {
        throw std::runtime_error("Cannot open database");
    }

    std::string sql = R"(
        INSERT INTO Contacts (firstName, lastName, middleName, address, birthDate, email, phoneNumbers)
        VALUES (?, ?, ?, ?, ?, ?, ?);
    )";

    sqlite3_stmt* stmt;
    for (const auto& contact : contacts) {
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            sqlite3_close(db);
            throw std::runtime_error("Failed to prepare statement");
        }

        sqlite3_bind_text(stmt, 1, contact.firstName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, contact.lastName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, contact.middleName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, contact.address.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, contact.birthDate.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, contact.email.c_str(), -1, SQLITE_TRANSIENT);

        std::string phoneNumbers = "";
        for (const auto& phone : contact.phoneNumbers) {
            phoneNumbers += phone + ",";
        }
        sqlite3_bind_text(stmt, 7, phoneNumbers.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            throw std::runtime_error("Failed to execute statement");
        }

        sqlite3_finalize(stmt);
    }

    sqlite3_close(db);
}
void ContactManager::loadFromDatabase(const QString& dbFilename) {
    sqlite3* db;
    int rc = sqlite3_open(dbFilename.toStdString().c_str(), &db);

    if (rc) {
        throw std::runtime_error("Cannot open database");
    }

    std::string sql = "SELECT firstName, lastName, middleName, address, birthDate, email, phoneNumbers FROM Contacts;";
    sqlite3_stmt* stmt;

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        throw std::runtime_error("Failed to prepare statement");
    }

    contacts.clear();
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Contact contact;
        contact.firstName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        contact.lastName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        contact.middleName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        contact.address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        contact.birthDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        contact.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        std::string phoneNumbers = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        std::istringstream phoneStream(phoneNumbers);
        std::string phone;
        while (std::getline(phoneStream, phone, ',')) {
            if (!phone.empty()) {
                contact.phoneNumbers.push_back(phone);
            }
        }

        contacts.push_back(contact);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

ContactManager::ContactManager(const QString& filename) {
    
    try {
        loadFromFile(filename); 
       
    }
    catch (const exception& e) {
        cerr << "Error loading contacts: " << e.what() << endl;
    }
}
ContactManager::ContactManager(const QString& dbFilename, bool isDatabase) {

    try {
        loadFromDatabase(dbFilename);

    }
    catch (const exception& e) {
        cerr << "Error loading contacts: " << e.what() << endl;
    }
}

 vector<Contact>& ContactManager::getContacts() {
    return contacts;
}

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
    string trimmedEmail = email;
    trimmedEmail.erase(remove(trimmedEmail.begin(), trimmedEmail.end(), ' '), trimmedEmail.end());
    regex emailRegex(R"(^(no|No|NO|[a-zA-Z0-9]+(\.[a-zA-Z0-9]+)*@[a-zA-Z0-9]+(\.[a-zA-Z0-9]+)+)$)");
    return regex_match(trimmedEmail, emailRegex);
}

bool ContactManager::isValidPhone(const string& phone)
{
    regex phoneRegex(R"(\+?\d{1,3}[- ]?\(?\d{1,4}?\)?[- ]?\d{1,4}[- ]?\d{1,4})");
    return regex_match(phone, phoneRegex);
}

bool ContactManager::isValidDate(const string& date)
{
    tm tm = {};
    std::istringstream ss(date);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    if (ss.fail()) {
        return false;
    }

    // Получаем текущую дату
    time_t t = time(nullptr);
    std::tm now;
    localtime_s(&now, &t);

    // Исправляем значения годов для сравнения
    tm.tm_year += 1900;
    now.tm_year += 1900;

    // Сравнение с текущей датой
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


void ContactManager::addContact(const Contact& contact) {
    contacts.push_back(contact); 
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


void ContactManager::saveToFile(const QString& filename) const
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error("Unable to open file for writing");
    }

    QTextStream out(&file);
    for (const auto& contact : contacts) {
        QStringList phoneNumbersList;
        for (const auto& phoneNumber : contact.phoneNumbers) {
            phoneNumbersList.append(QString::fromStdString(phoneNumber));
        }
        QString phoneNumbers = phoneNumbersList.join(',');

        out << QString::fromStdString(contact.firstName) << ";"
            << QString::fromStdString(contact.lastName) << ";"
            << QString::fromStdString(contact.middleName) << ";"
            << QString::fromStdString(contact.address) << ";"
            << QString::fromStdString(contact.birthDate) << ";"
            << QString::fromStdString(contact.email) << ";"
            << phoneNumbers << "\n";
    }

    file.close();
}

void ContactManager::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Unable to open file for reading");
    }

    QTextStream in(&file);
    contacts.clear(); 

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(';');

        if (parts.size() < 6) continue; 

        QString firstName = parts[0];
        QString lastName = parts[1];
        QString middleName = parts[2];
        QString address = parts[3];
        QString birthDate = parts[4];
        QString email = parts[5];
        QString phoneNumbersStr = parts.size() > 6 ? parts[6] : "";

        std::vector<std::string> phoneNumbers;
        QStringList phoneNumbersList = phoneNumbersStr.split(',');
        for (const QString& phone : phoneNumbersList) {
            phoneNumbers.push_back(phone.toStdString());
        }

        contacts.emplace_back(Contact{
            firstName.toStdString(),
            lastName.toStdString(),
            middleName.toStdString(),
            address.toStdString(),
            birthDate.toStdString(),
            email.toStdString(),
            phoneNumbers
            });
    }

    file.close();
}





