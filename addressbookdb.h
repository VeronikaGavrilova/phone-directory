#pragma once
#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

#include <QWidget>
#include <QPushButton>
#include "Headerdb.h"

class QLabel;
class QLineEdit;
class QTextEdit;

class AddressBook : public QWidget
{
    Q_OBJECT

public:
    AddressBook(QWidget* parent = 0);

private:
    QLineEdit* firstnameLine;
    QLineEdit* lastnameLine;
    QLineEdit* middlenameLine;
    QLineEdit* addressText;
    QLineEdit* birthdate;
    QLineEdit* email;
    QTextEdit* numbers;
   
    QPushButton* addButton;
    QPushButton* addButton2;
    QPushButton* displayButton;
    QPushButton* displayButton2;
    QPushButton* deleteButton;
    QPushButton* editButton;
    QPushButton* searchButton;
    QPushButton* sortButton;
    QPushButton* clearButton;

    ContactManager contactManager;
    vector<Contact> contacts;

    string trim(const string& str);
    string join(const std::vector<std::string>& vec, const std::string& delimiter);

private slots:
    void addContact();
    void addContact2();
    void displayContacts();
    void displayContacts2();
    void deleteContact();
    void editContact();
    void searchContact();
    void sortContact();
    void clearContact();
};

#endif