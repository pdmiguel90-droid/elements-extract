#include "mainwindow.h"
#include "theme.h"
#include <QApplication>
#include <QCoreApplication>
#include <QLocale>
#include <QSettings>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Elements Extract");
    a.setOrganizationName("LocalApp");

    QSettings::setDefaultFormat(QSettings::IniFormat);

    // Load saved language preference
    QSettings settings("LocalApp", "FFmpegExtractor");
    QString langCode = settings.value("language", "system").toString();

    // Install translator
    QTranslator *translator = new QTranslator(&a);
    if (langCode == "system") {
        langCode = QLocale::system().name(); // e.g. "es_ES", "en_US", "pt_BR"
        // Locale C/POSIX (CI, containers, AppImageHub tests): no language
        // assignable → English by default instead of the source Spanish.
        if (langCode == "C" || langCode.startsWith("C.") || langCode == "POSIX")
            langCode = "en";
    }

    // Try loading translation from resources
    if (translator->load("elements_extract_" + langCode, ":/translations")) {
        a.installTranslator(translator);
    } else if (translator->load("elements_extract_" + langCode,
                                QCoreApplication::applicationDirPath() + "/../share/elements-extract/translations")) {
        a.installTranslator(translator);
    } else {
        // Try without country code (e.g. "es" instead of "es_ES")
        QString shortLang = langCode.left(2);
        if (translator->load("elements_extract_" + shortLang, ":/translations")) {
            a.installTranslator(translator);
        } else if (translator->load("elements_extract_" + shortLang,
                                    QCoreApplication::applicationDirPath() + "/../share/elements-extract/translations")) {
            a.installTranslator(translator);
        } else {
            delete translator;
            translator = nullptr;
        }
    }

    // Apply saved theme
    ThemeManager::instance()->applyTheme(ThemeManager::instance()->currentTheme());

    MainWindow w;

    // Parse and load files passed via command line (e.g., right-click "Open With...")
    QStringList args = QCoreApplication::arguments();
    if (args.count() > 1) {
        args.removeFirst(); // Remove the executable name/path
        w.loadFiles(args);
    }

    w.show();
    int result = a.exec();
    delete translator;
    return result;
}
