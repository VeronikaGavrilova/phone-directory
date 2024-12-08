#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <QFile>
#include <QTextStream>
using namespace std;

struct Contact {
    string firstName;
    string lastName;
    string middleName;
    string address;
    string birthDate; // формат: YYYY-MM-DD
    string email;
    vector<std::string> phoneNumbers;

    void display() const;
};

class ContactManager {
public:

    ContactManager(const QString& filename);

    vector<Contact>& getContacts() ;

    vector<Contact> contacts;
    const string filename = "contracts.txt";

    bool isValidName(const wstring& name);

    bool isValidName(const string& name);

    bool isValidEmail(const string& email);

    bool isValidPhone(const string& phone);

    bool isValidDate(const string& date);

public:
    void loadContacts();

    void addContact(const Contact& contact);

    void searchContacts();

    void saveToFile(const QString& filename) const;
    void loadFromFile(const QString& filename);
};