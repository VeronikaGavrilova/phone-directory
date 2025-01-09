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
#include <sqlite3.h>
using namespace std;

struct Contact {
    string firstName;
    string lastName;
    string middleName;
    string address;
    string birthDate; 
    string email;
    vector<std::string> phoneNumbers;

    void display() const;
};

class ContactManager {
public:
    void initializeDatabase(const QString& dbFilename);
    void saveToDatabase(const QString& dbFilename) const;
    void loadFromDatabase(const QString& dbFilename);

    ContactManager(const QString& filename);
    ContactManager(const QString& dbFilename, bool isDatabase);

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
    void saveToDatabase(const Contact& contact, sqlite3* db) const;
    
};