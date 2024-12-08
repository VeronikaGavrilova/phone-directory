
#include <QtWidgets/QApplication>
#include "addressbook.h"

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "ru_RU.UTF-8");
    QApplication app(argc, argv);

    AddressBook addressBook;
    addressBook.show();

    return app.exec();
}
