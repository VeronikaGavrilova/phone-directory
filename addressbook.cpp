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
#include "Header.h"
#include "addressbook.h"


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

    // Считываем данные из текстовых полей
    newContact.firstName = trim(firstnameLine->text().toStdString());
    newContact.lastName = trim(lastnameLine->text().toStdString());
    newContact.middleName = trim(middlenameLine->text().toStdString());
    newContact.address = trim(addressText->text().toStdString());
    newContact.birthDate = trim(birthdate->text().toStdString());
    newContact.email = trim(email->text().toStdString());
    QString phoneInput = numbers->toPlainText(); // Предполагается, что phoneNumbersText — QTextEdit
    QStringList phoneList = phoneInput.split("\n", Qt::SkipEmptyParts); // Разбиваем номера по строкам

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

    // Добавляем контакт и сохраняем изменения
    contactManager.addContact(newContact);
    contactManager.saveToFile("contacts.txt");
    firstnameLine->clear();
    lastnameLine->clear();
    middlenameLine->clear();
    addressText->clear();
    birthdate->clear();
    email->clear();
    numbers->clear();
    QMessageBox::information(this, tr("Error"), tr("Contact is completed!"));
}


void AddressBook::displayContacts() {
    QString contactsInfo;

    for (const auto& contact : contactManager.getContacts()) { // Предположим, есть метод getContacts()
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
        QMessageBox::information(this, tr("Contacts"), contactsInfo);
    }
}

void AddressBook::deleteContact()
{
    QString nameToDelete = QInputDialog::getText(this, tr("Delete Contact"), tr("Enter the first name of the contact to delete:"));

    if (nameToDelete.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No name provided."));
        return;
    }

    // Ищем и удаляем контакт
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

    // Открываем окно для редактирования
    Contact& contact = *it;

    // Создаем диалоговое окно
    QDialog editDialog(this);
    editDialog.setWindowTitle(tr("Edit Contact"));

    // Поля для редактирования
    QLineEdit* firstNameEdit = new QLineEdit(QString::fromStdString(contact.firstName));
    QLineEdit* lastNameEdit = new QLineEdit(QString::fromStdString(contact.lastName));
    QLineEdit* middleNameEdit = new QLineEdit(QString::fromStdString(contact.middleName));
    QLineEdit* addressEdit = new QLineEdit(QString::fromStdString(contact.address));
    QLineEdit* birthDateEdit = new QLineEdit(QString::fromStdString(contact.birthDate));
    QLineEdit* emailEdit = new QLineEdit(QString::fromStdString(contact.email));
    QTextEdit* phoneNumbersEdit = new QTextEdit(QString::fromStdString(join(contact.phoneNumbers, "\n")));

    // Кнопки подтверждения и отмены
    QPushButton* saveButton = new QPushButton(tr("Save"));
    QPushButton* cancelButton = new QPushButton(tr("Cancel"));

    // Макет диалога
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

    // Обработчик кнопки отмены
    connect(cancelButton, &QPushButton::clicked, &editDialog, &QDialog::reject);

    // Обработчик кнопки сохранения
    connect(saveButton, &QPushButton::clicked, [&]() {
        QString newFirstName = firstNameEdit->text();
        QString newLastName = lastNameEdit->text();
        QString newMiddleName = middleNameEdit->text();
        QString newAddress = addressEdit->text();
        QString newBirthDate = birthDateEdit->text();
        QString newEmail = emailEdit->text();
        QStringList updatedPhones = phoneNumbersEdit->toPlainText().split("\n", Qt::SkipEmptyParts);

        // Флаг для проверки ошибок
        bool isValid = true;
        QString errorMessage;

        // Проверка каждого поля
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

        // Проверка номеров телефонов
        std::vector<std::string> validPhones;
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

        // Если есть ошибки, показываем сообщение и отменяем сохранение
        if (!isValid) {
            QMessageBox::warning(&editDialog, tr("Error"), errorMessage);
            return;
        }

        // Если все проверки пройдены, сохраняем данные
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

    // Запускаем диалог
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
        // Проверяем, содержится ли строка в одном из полей контакта
        if (QString::fromStdString(contact.firstName).contains(query, Qt::CaseInsensitive) ||
            QString::fromStdString(contact.lastName).contains(query, Qt::CaseInsensitive) ||
            QString::fromStdString(contact.middleName).contains(query, Qt::CaseInsensitive) ||
            QString::fromStdString(contact.address).contains(query, Qt::CaseInsensitive) ||
            QString::fromStdString(contact.email).contains(query, Qt::CaseInsensitive) ||
            QString::fromStdString(join(contact.phoneNumbers, ", ")).contains(query, Qt::CaseInsensitive)) {

            // Формируем информацию о контакте
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

    // Если результаты пустые, показываем сообщение, иначе отображаем список контактов
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

    // Выполняем сортировку в зависимости от выбранного поля
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

    // Сообщение об успешной сортировке
    QMessageBox::information(this, tr("Success"), tr("Contacts sorted successfully!"));

    // Сохраняем отсортированный список (если необходимо) и обновляем данные
    contactManager.saveToFile("contacts.txt");
}


AddressBook::AddressBook(QWidget* parent) : QWidget(parent), contactManager("contacts.txt") {
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor(100, 100, 100)); 
    palette.setColor(QPalette::WindowText, Qt::black); 
    this->setPalette(palette);
    this->setAutoFillBackground(true);

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


    addButton = new QPushButton(tr("Add contact"));
    displayButton = new QPushButton(tr("Show contacts"));
    deleteButton = new QPushButton(tr("Delete contact"));
    editButton = new QPushButton(tr("Edit contact"));
    searchButton = new QPushButton(tr("Search contact"));
    sortButton = new QPushButton(tr("Sort contacts"));

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
    mainLayout->addWidget(displayButton, 7, 1);
    mainLayout->addWidget(deleteButton, 7, 2);
    mainLayout->addWidget(editButton, 7, 3);
    mainLayout->addWidget(searchButton, 7, 4);
    mainLayout->addWidget(sortButton, 7, 5);


    setLayout(mainLayout);
    setWindowTitle(tr("Address Book"));

    
    connect(addButton, &QPushButton::clicked, this, &AddressBook::addContact);
    connect(displayButton, &QPushButton::clicked, this, &AddressBook::displayContacts);
    connect(deleteButton, &QPushButton::clicked, this, &AddressBook::deleteContact);
    connect(editButton, &QPushButton::clicked, this, &AddressBook::editContact);
    connect(searchButton, &QPushButton::clicked, this, &AddressBook::searchContact);
    connect(sortButton, &QPushButton::clicked, this, &AddressBook::sortContact);
}