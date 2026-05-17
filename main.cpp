#include "mainwindow.h"
#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Parse and load files passed via command line (e.g., right-click "Open With...")
    QStringList args = QCoreApplication::arguments();
    if (args.count() > 1) {
        args.removeFirst(); // Remove the executable name/path
        w.loadFiles(args);
    }

    w.show();
    return a.exec();
}

