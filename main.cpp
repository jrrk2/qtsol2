// main.cpp
#include <QApplication>
#include "sol2qtmainwindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    Sol2QtMainWindow window;
    window.show();
    
    return app.exec();
}
