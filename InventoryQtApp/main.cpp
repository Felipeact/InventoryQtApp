// main.cpp - Entry point for the Inventory Management System application
#include "InventoryQtApp.h"
#include <QtWidgets/QApplication>

// Main function initializes and runs the application
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    InventoryQtApp window;
    window.show();
    return app.exec();
}


