#include <QtGui>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton> 
#include <QGridLayout> 
#include <QLCDNumber> 
#include <QMessageBox>
#include <QInputDialog>
#include <vector>
#include <sstream>
#include "Headerdb.h"
#include "addressbookdb.h"
#include <QScrollArea>
#include <sqlite3.h>



string AddressBook::trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");

    return (start == string::npos) ? "" : str.substr(start, end - start + 1);
}

string AddressBook::join(const std::vector<std::string>& elements, const std::string& delimiter) {
    std::ostringstream oss;
    for (size_t i = 0; i < elements.size(); ++i) {
        oss << elements[i];
        if (i != elements.size() - 1) {
            oss << delimiter;
        }
    }
    return oss.str();
}


void AddressBook::addContact() {
    Contact newContact;

    newContact.firstName = trim(firstnameLine->text().toStdString());
    newContact.lastName = trim(lastnameLine->text().toStdString());
    newContact.middleName = trim(middlenameLine->text().toStdString());
    newContact.address = trim(addressText->text().toStdString());
    newContact.birthDate = trim(birthdate->text().toStdString());
    newContact.email = trim(email->text().toStdString());
    QString phoneInput = numbers->toPlainText();
    QStringList phoneList = phoneInput.split("\n", Qt::SkipEmptyParts); 

    for (const auto& phone : phoneList) {
        std::string phoneStr = trim(phone.toStdString());
        if (contactManager.isValidPhone(phoneStr)) {
            newContact.phoneNumbers.push_back(phoneStr);
        }
        else {
            QMessageBox::warning(this, tr("Error"), tr("Invalid phone number: %1").arg(phone));
            return;
        }
    }
    
    

    if (!contactManager.isValidName(newContact.firstName)) {
        QMessageBox::warning(this, tr("Error"), tr("Uncorrect name"));
        return;
    }
    if (!contactManager.isValidName(newContact.lastName)) {
        QMessageBox::warning(this, tr("Error"), tr("Uncorrect lastname"));
        return;
    }
    if (!contactManager.isValidName(newContact.middleName)) {
        QMessageBox::warning(this, tr("Error"), tr("Uncorrect middlename"));
        return;
    }
    if (!contactManager.isValidDate(newContact.birthDate)) {
        QMessageBox::warning(this, tr("Error"), tr("Uncorrect birthday"));
        return;
    }
    if (!contactManager.isValidEmail(newContact.email)) {
        QMessageBox::warning(this, tr("Error"), tr("Uncorrect email"));
        return;
    }

   
    contactManager.addContact(newContact);
    contactManager.saveToFile("contacts.txt");
   
    QMessageBox::information(this, tr("Error"), tr("Contact is completed!"));
}
void AddressBook::addContact2() {
   
    Contact newContact;
    newContact.firstName = trim(firstnameLine->text().toStdString());
    newContact.lastName = trim(lastnameLine->text().toStdString());
    newContact.middleName = trim(middlenameLine->text().toStdString());
    newContact.address = trim(addressText->text().toStdString());
    newContact.birthDate = trim(birthdate->text().toStdString());
    newContact.email = trim(email->text().toStdString());

    QString phoneInput = numbers->toPlainText();
    QStringList phoneList = phoneInput.split("\n", Qt::SkipEmptyParts);

    for (const auto& phone : phoneList) {
        std::string phoneStr = trim(phone.toStdString());
        if (contactManager.isValidPhone(phoneStr)) {
            newContact.phoneNumbers.push_back(phoneStr);
        }
        else {
            QMessageBox::warning(this, tr("Error"), tr("Invalid phone number: %1").arg(phone));
            return;
        }
    }

    
    if (!contactManager.isValidName(newContact.firstName)) {
        QMessageBox::warning(this, tr("Error"), tr("Incorrect name"));
        return;
    }
    if (!contactManager.isValidName(newContact.lastName)) {
        QMessageBox::warning(this, tr("Error"), tr("Incorrect surname"));
        return;
    }
    if (!contactManager.isValidDate(newContact.birthDate)) {
        QMessageBox::warning(this, tr("Error"), tr("Incorrect birth date"));
        return;
    }
    if (!contactManager.isValidEmail(newContact.email)) {
        QMessageBox::warning(this, tr("Error"), tr("Incorrect email"));
        return;
    }

    
    sqlite3* db;
    contactManager.initializeDatabase("contacts.db");
    int rc = sqlite3_open("contacts.db", &db);
    if (rc) {
        QMessageBox::critical(this, "Error", "Can't open database: " + QString::fromStdString(sqlite3_errmsg(db)));
        return;
    }

    
    std::string insertQuery = "INSERT INTO contacts (firstName, lastName, middleName, address, birthDate, email, phoneNumbers) VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    rc = sqlite3_prepare_v2(db, insertQuery.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        QMessageBox::critical(this, "Error", "Failed to prepare statement: " + QString::fromStdString(sqlite3_errmsg(db)));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, newContact.firstName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, newContact.lastName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, newContact.middleName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, newContact.address.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, newContact.birthDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, newContact.email.c_str(), -1, SQLITE_STATIC);

    std::string phones = join(newContact.phoneNumbers, ", ");
    sqlite3_bind_text(stmt, 7, phones.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        QMessageBox::critical(this, "Error", "Failed to insert data: " + QString::fromStdString(sqlite3_errmsg(db)));
    }
    else {
        QMessageBox::information(this, "Success", "Contact added successfully!");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

   
}


void AddressBook::displayContacts() {
    QString contactsInfo;

    for (const auto& contact : contactManager.getContacts()) {
        QString contactInfo = QString("Name: %1\nSurname: %2\nMiddle name: %3\nAddress: %4\nBirthday: %5\nEmail: %6\nPhone numbers: %7\n")
            .arg(QString::fromStdString(contact.firstName))
            .arg(QString::fromStdString(contact.lastName))
            .arg(QString::fromStdString(contact.middleName))
            .arg(QString::fromStdString(contact.address))
            .arg(QString::fromStdString(contact.birthDate))
            .arg(QString::fromStdString(contact.email))
            .arg(QString::fromStdString(join(contact.phoneNumbers, ", ")));
        contactsInfo += contactInfo + "\n";
    }

    if (contactsInfo.isEmpty()) {
        QMessageBox::information(this, tr("Contacts"), tr("No contacts available."));
    }
    else {

        QTextEdit* contactsTextEdit = new QTextEdit();
        contactsTextEdit->setText(contactsInfo);
        contactsTextEdit->setReadOnly(true);

        QScrollArea* scrollArea = new QScrollArea();
        scrollArea->setWidget(contactsTextEdit);
        scrollArea->setWidgetResizable(true);

        QVBoxLayout* layout = new QVBoxLayout();
        layout->addWidget(scrollArea);

        QWidget* contactsWindow = new QWidget();
        contactsWindow->setLayout(layout);
        contactsWindow->setWindowTitle("Contacts");
        contactsWindow->show();
    }
}
void AddressBook::displayContacts2() {
    sqlite3* db;
    int rc = sqlite3_open("contacts.db", &db);

    if (rc) {
        QMessageBox::critical(this, "Error", "Can't open database: " + QString::fromStdString(sqlite3_errmsg(db)));
        return;
    }

    const char* selectQuery = "SELECT firstName, lastName, middleName, address, birthDate, email, phoneNumbers FROM contacts;";
    sqlite3_stmt* stmt;

    rc = sqlite3_prepare_v2(db, selectQuery, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        QMessageBox::critical(this, "Error", "Failed to fetch data: " + QString::fromStdString(sqlite3_errmsg(db)));
        sqlite3_close(db);
        return;
    }

    QString contactsInfo;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        QString contactInfo = QString("Name: %1\nSurname: %2\nMiddle name: %3\nAddress: %4\nBirthday: %5\nEmail: %6\nPhone numbers: %7\n")
            .arg(QString::fromStdString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))))
            .arg(QString::fromStdString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))))
            .arg(QString::fromStdString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))))
            .arg(QString::fromStdString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3))))
            .arg(QString::fromStdString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))))
            .arg(QString::fromStdString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5))))
            .arg(QString::fromStdString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6))));

        contactsInfo += contactInfo + "\n";
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    if (contactsInfo.isEmpty()) {
        QMessageBox::information(this, tr("Contacts"), tr("No contacts available."));
    }
    else {
        QTextEdit* contactsTextEdit = new QTextEdit();
        contactsTextEdit->setText(contactsInfo);
        contactsTextEdit->setReadOnly(true);

        QScrollArea* scrollArea = new QScrollArea();
        scrollArea->setWidget(contactsTextEdit);
        scrollArea->setWidgetResizable(true);

        QVBoxLayout* layout = new QVBoxLayout();
        layout->addWidget(scrollArea);

        QWidget* contactsWindow = new QWidget();
        contactsWindow->setLayout(layout);
        contactsWindow->setWindowTitle("Contacts");
        contactsWindow->show();
    }
}


void AddressBook::deleteContact()
{
    QString nameToDelete = QInputDialog::getText(this, tr("Delete Contact"), tr("Enter the first name of the contact to delete:"));

    if (nameToDelete.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No name provided."));
        return;
    }

    bool contactFound = false;
    auto& contacts = contactManager.getContacts();
    for (auto it = contacts.begin(); it != contacts.end(); ++it) {
        if (QString::fromStdString(it->firstName).compare(nameToDelete, Qt::CaseInsensitive) == 0) {
            contacts.erase(it);
            contactFound = true;
            QMessageBox::information(this, tr("Success"), tr("Contact deleted successfully."));
            break;
        }
    }

    if (!contactFound) {
        QMessageBox::warning(this, tr("Error"), tr("Contact not found."));
    }
}

void AddressBook::editContact()
{
    QString lastNameToEdit = QInputDialog::getText(this, tr("Edit Contact"), tr("Enter surname to edit:"));

    if (lastNameToEdit.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No last name provided."));
        return;
    }

    auto& contacts = contactManager.getContacts();
    auto it = std::find_if(contacts.begin(), contacts.end(),
        [&lastNameToEdit](const Contact& c) {
            return QString::fromStdString(c.lastName).compare(lastNameToEdit, Qt::CaseInsensitive) == 0;
        });

    if (it == contacts.end()) {
        QMessageBox::warning(this, tr("Error"), tr("Contact not found."));
        return;
    }

   
    Contact& contact = *it;

    
    QDialog editDialog(this);
    editDialog.setWindowTitle(tr("Edit Contact"));

    
    QLineEdit* firstNameEdit = new QLineEdit(QString::fromStdString(contact.firstName));
    QLineEdit* lastNameEdit = new QLineEdit(QString::fromStdString(contact.lastName));
    QLineEdit* middleNameEdit = new QLineEdit(QString::fromStdString(contact.middleName));
    QLineEdit* addressEdit = new QLineEdit(QString::fromStdString(contact.address));
    QLineEdit* birthDateEdit = new QLineEdit(QString::fromStdString(contact.birthDate));
    QLineEdit* emailEdit = new QLineEdit(QString::fromStdString(contact.email));
    QTextEdit* phoneNumbersEdit = new QTextEdit(QString::fromStdString(join(contact.phoneNumbers, "\n")));

    
    QPushButton* saveButton = new QPushButton(tr("Save"));
    QPushButton* cancelButton = new QPushButton(tr("Cancel"));

    
    QGridLayout* layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("First Name:")), 0, 0);
    layout->addWidget(firstNameEdit, 0, 1);
    layout->addWidget(new QLabel(tr("Surname:")), 1, 0);
    layout->addWidget(lastNameEdit, 1, 1);
    layout->addWidget(new QLabel(tr("Middle Name:")), 2, 0);
    layout->addWidget(middleNameEdit, 2, 1);
    layout->addWidget(new QLabel(tr("Address:")), 3, 0);
    layout->addWidget(addressEdit, 3, 1);
    layout->addWidget(new QLabel(tr("Birth Date (YYYY-MM-DD):")), 4, 0);
    layout->addWidget(birthDateEdit, 4, 1);
    layout->addWidget(new QLabel(tr("Email:")), 5, 0);
    layout->addWidget(emailEdit, 5, 1);
    layout->addWidget(new QLabel(tr("Phone Numbers (one per line):")), 6, 0);
    layout->addWidget(phoneNumbersEdit, 6, 1);
    layout->addWidget(saveButton, 7, 0);
    layout->addWidget(cancelButton, 7, 1);

    editDialog.setLayout(layout);

    
    connect(cancelButton, &QPushButton::clicked, &editDialog, &QDialog::reject);

    
    connect(saveButton, &QPushButton::clicked, [&]() {
        QString newFirstName = firstNameEdit->text();
        QString newLastName = lastNameEdit->text();
        QString newMiddleName = middleNameEdit->text();
        QString newAddress = addressEdit->text();
        QString newBirthDate = birthDateEdit->text();
        QString newEmail = emailEdit->text();
        QStringList updatedPhones = phoneNumbersEdit->toPlainText().split("\n", Qt::SkipEmptyParts);

       
        bool isValid = true;
        QString errorMessage;

        
        if (newFirstName.isEmpty() || !contactManager.isValidName(newFirstName.toStdString())) {
            errorMessage += tr("Invalid or empty first name.\n");
            isValid = false;
        }
        if (newLastName.isEmpty() || !contactManager.isValidName(newLastName.toStdString())) {
            errorMessage += tr("Invalid or empty surname.\n");
            isValid = false;
        }
        if (!newMiddleName.isEmpty() && !contactManager.isValidName(newMiddleName.toStdString())) {
            errorMessage += tr("Invalid middle name.\n");
            isValid = false;
        }
        if (newAddress.isEmpty()) {
            errorMessage += tr("Address cannot be empty.\n");
            isValid = false;
        }
        if (!newBirthDate.isEmpty() && !contactManager.isValidDate(newBirthDate.toStdString())) {
            errorMessage += tr("Invalid birth date. Use YYYY-MM-DD format.\n");
            isValid = false;
        }
        if (!newEmail.isEmpty() && !contactManager.isValidEmail(newEmail.toStdString())) {
            errorMessage += tr("Invalid email address.\n");
            isValid = false;
        }

        
        vector<string> validPhones;
        for (const auto& phone : updatedPhones) {
            std::string phoneStr = trim(phone.toStdString());
            if (contactManager.isValidPhone(phoneStr)) {
                validPhones.push_back(phoneStr);
            }
            else {
                errorMessage += tr("Invalid phone number: %1\n").arg(phone);
                isValid = false;
            }
        }

        
        if (!isValid) {
            QMessageBox::warning(&editDialog, tr("Error"), errorMessage);
            return;
        }

        
        contact.firstName = newFirstName.toStdString();
        contact.lastName = newLastName.toStdString();
        contact.middleName = newMiddleName.toStdString();
        contact.address = newAddress.toStdString();
        contact.birthDate = newBirthDate.toStdString();
        contact.email = newEmail.toStdString();
        contact.phoneNumbers = validPhones;

        contactManager.saveToFile("contacts.txt");

        QMessageBox::information(&editDialog, tr("Success"), tr("Contact updated successfully!"));
        editDialog.accept();
        });

   
    editDialog.exec();
}

void AddressBook::searchContact()
{
    QString query = QInputDialog::getText(this, tr("Search Contact"), tr("Enter query string:"));

    if (query.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No query provided."));
        return;
    }

    QString results;
    auto& contacts = contactManager.getContacts();

    for (const auto& contact : contacts) {
       
        if (QString::fromStdString(contact.firstName).contains(query, Qt::CaseInsensitive) ||
            QString::fromStdString(contact.lastName).contains(query, Qt::CaseInsensitive) ||
            QString::fromStdString(contact.middleName).contains(query, Qt::CaseInsensitive) ||
            QString::fromStdString(contact.address).contains(query, Qt::CaseInsensitive) ||
            QString::fromStdString(contact.email).contains(query, Qt::CaseInsensitive) ||
            QString::fromStdString(join(contact.phoneNumbers, ", ")).contains(query, Qt::CaseInsensitive)) {

           
            QString contactInfo = QString("Name: %1\nSurname: %2\nMiddle name: %3\nAddress: %4\nBirthday: %5\nEmail: %6\nPhone Numbers: %7\n")
                .arg(QString::fromStdString(contact.firstName))
                .arg(QString::fromStdString(contact.lastName))
                .arg(QString::fromStdString(contact.middleName))
                .arg(QString::fromStdString(contact.address))
                .arg(QString::fromStdString(contact.birthDate))
                .arg(QString::fromStdString(contact.email))
                .arg(QString::fromStdString(join(contact.phoneNumbers, ", ")));
            results += contactInfo + "\n";
        }
    }

    
    if (results.isEmpty()) {
        QMessageBox::information(this, tr("Search Results"), tr("No contacts found."));
    }
    else {
        QMessageBox::information(this, tr("Search Results"), results);
    }
}

void AddressBook::sortContact()
{
    QStringList sortOptions = { tr("Name"), tr("Surname"), tr("Middle name"), tr("Birth date"), tr("Email"), tr("Address") };

    bool ok;
    QString choice = QInputDialog::getItem(this, tr("Sort Contacts"),
        tr("Choose a field to sort by:"), sortOptions, 0, false, &ok);

    if (!ok || choice.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No option selected. Sorting cancelled."));
        return;
    }

    auto& contacts = contactManager.getContacts();

    
    if (choice == tr("Name")) {
        std::sort(contacts.begin(), contacts.end(),
            [](const Contact& a, const Contact& b) { return a.firstName < b.firstName; });
    }
    else if (choice == tr("Surname")) {
        std::sort(contacts.begin(), contacts.end(),
            [](const Contact& a, const Contact& b) { return a.lastName < b.lastName; });
    }
    else if (choice == tr("Middle name")) {
        std::sort(contacts.begin(), contacts.end(),
            [](const Contact& a, const Contact& b) { return a.middleName < b.middleName; });
    }
    else if (choice == tr("Birth date")) {
        std::sort(contacts.begin(), contacts.end(),
            [](const Contact& a, const Contact& b) { return a.birthDate < b.birthDate; });
    }
    else if (choice == tr("Email")) {
        std::sort(contacts.begin(), contacts.end(),
            [](const Contact& a, const Contact& b) { return a.email < b.email; });
    }
    else if (choice == tr("Address")) {
        std::sort(contacts.begin(), contacts.end(),
            [](const Contact& a, const Contact& b) { return a.address < b.address; });
    }
    else {
        QMessageBox::warning(this, tr("Error"), tr("Invalid option. Sorting cancelled."));
        return;
    }

   
    QMessageBox::information(this, tr("Success"), tr("Contacts sorted successfully!"));

   
    contactManager.saveToFile("contacts.txt");
}

void AddressBook::clearContact()
{
    firstnameLine->clear();
    lastnameLine->clear();
    middlenameLine->clear();
    addressText->clear();
    birthdate->clear();
    email->clear();
    numbers->clear();
}


AddressBook::AddressBook(QWidget* parent) : QWidget(parent), contactManager("contacts.txt") {
    
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor(255, 240, 245));
    palette.setColor(QPalette::WindowText, Qt::black); 
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    QFont font("Verdana", 8);
    this->setFont(font);


    QLabel* firstnameLabel = new QLabel(tr("Name:"));
    firstnameLine = new QLineEdit;
    QLabel* lastnameLabel = new QLabel(tr("Surname:"));
    lastnameLine = new QLineEdit;
    QLabel* middlenameLabel = new QLabel(tr("Middle name:"));
    middlenameLine = new QLineEdit;
    QLabel* addressLabel = new QLabel(tr("Adress:"));
    addressText = new QLineEdit;
    QLabel* birthdateLabel = new QLabel(tr("Birthday(YYYY-MM-DD):"));
    birthdate = new QLineEdit;
    QLabel* emailLabel = new QLabel(tr("Email:"));
    email = new QLineEdit;
    QLabel* numbersLabel = new QLabel(tr("Phone numbers:"));
    numbers = new QTextEdit;

    addButton = new QPushButton(tr("Add contact(file)"));
    addButton2 = new QPushButton(tr("Add contact(db)"));
    displayButton = new QPushButton(tr("Show contacts(file)"));
    displayButton2 = new QPushButton(tr("Show contacts(db)"));
    deleteButton = new QPushButton(tr("Delete contact"));
    editButton = new QPushButton(tr("Edit contact"));
    searchButton = new QPushButton(tr("Search contact"));
    sortButton = new QPushButton(tr("Sort contacts"));
    clearButton = new QPushButton(tr("Clear contacts"));


    QGridLayout* mainLayout = new QGridLayout;
    mainLayout->addWidget(firstnameLabel, 0, 0);
    mainLayout->addWidget(firstnameLine, 0, 1);
    mainLayout->addWidget(lastnameLabel, 1, 0);
    mainLayout->addWidget(lastnameLine, 1, 1);
    mainLayout->addWidget(middlenameLabel, 2, 0);
    mainLayout->addWidget(middlenameLine, 2, 1);
    mainLayout->addWidget(addressLabel, 3, 0, Qt::AlignTop);
    mainLayout->addWidget(addressText, 3, 1);
    mainLayout->addWidget(birthdateLabel, 4, 0);
    mainLayout->addWidget(birthdate, 4, 1);
    mainLayout->addWidget(emailLabel, 5, 0);
    mainLayout->addWidget(email, 5, 1);
    mainLayout->addWidget(numbersLabel, 6, 0);
    mainLayout->addWidget(numbers, 6, 1);
   

    mainLayout->addWidget(addButton, 7, 0);
    mainLayout->addWidget(addButton2, 7, 1);
    mainLayout->addWidget(displayButton, 7, 2);
    mainLayout->addWidget(displayButton2, 7, 3);
    mainLayout->addWidget(deleteButton, 7, 4);
    mainLayout->addWidget(editButton, 7, 5);
    mainLayout->addWidget(searchButton, 7, 6);
    mainLayout->addWidget(sortButton, 7, 7);
    mainLayout->addWidget(clearButton, 0, 2);


    setLayout(mainLayout);
    setWindowTitle(tr("Address Book"));

    
    connect(addButton, &QPushButton::clicked, this, &AddressBook::addContact);
    connect(addButton2, &QPushButton::clicked, this, &AddressBook::addContact2);
    connect(displayButton, &QPushButton::clicked, this, &AddressBook::displayContacts);
    connect(displayButton2, &QPushButton::clicked, this, &AddressBook::displayContacts2);
    connect(deleteButton, &QPushButton::clicked, this, &AddressBook::deleteContact);
    connect(editButton, &QPushButton::clicked, this, &AddressBook::editContact);
    connect(searchButton, &QPushButton::clicked, this, &AddressBook::searchContact);
    connect(sortButton, &QPushButton::clicked, this, &AddressBook::sortContact);
    connect(clearButton, &QPushButton::clicked, this, &AddressBook::clearContact);
}
