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
private:
    vector<Contact> contacts;
    const string filename = "contracts.txt";

    bool isValidName(const wstring& name);
   
    bool isValidName(const string& name);

    bool isValidEmail(const string& email);

    bool isValidPhone(const string& phone);

    bool isValidDate(const string& date);

public:
    void loadContacts();

    void saveContacts();

    void addContact();

    void displayContacts();
   

    void deleteContact();

    void editContact();

    void searchContacts();

    void sortContacts();
};