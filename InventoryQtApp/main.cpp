// main.cpp - Entry point for the Inventory Management System application
#include "InventoryQtApp.h"
#include <QtWidgets/QApplication>
#include "Theme.h"

// Main function initializes and runs the application
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    Theme::applyApplication();
    InventoryQtApp window;
    window.show();
    return app.exec();
}


