#include "theme.h"
#include <QFile>
#include <QStyle>
#include <QStyleFactory>
#include <QTextStream>
#include <QPalette>
#include <QDebug>

ThemeManager *ThemeManager::instance()
{
    static ThemeManager mgr;
    return &mgr;
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent), m_theme(System), m_effective(Dark)
{
    loadSettings();
}

QString ThemeManager::themeName(Theme t)
{
    switch (t) {
        case Dark:   return QStringLiteral("dark");
        case Light:  return QStringLiteral("light");
        case System: return QStringLiteral("system");
    }
    return QStringLiteral("system");
}

ThemeManager::Theme ThemeManager::themeFromName(const QString &name)
{
    if (name == "dark")  return Dark;
    if (name == "light") return Light;
    return System;
}

ThemeManager::Theme ThemeManager::detectSystemTheme()
{
    QPalette palette = QPalette();
    if (palette.window().color().lightness() > 128) {
        return Light;
    }
    return Dark;
}

ThemeManager::Theme ThemeManager::currentTheme() const
{
    return m_theme;
}

ThemeManager::Theme ThemeManager::effectiveTheme() const
{
    return m_effective;
}

QString ThemeManager::loadQss(const QString &filename)
{
    // Try resource path first, then filesystem
    QStringList searchPaths;
    searchPaths << ":/themes/" + filename;
    searchPaths << QCoreApplication::applicationDirPath() + "/../share/elements-extract/themes/" + filename;
    searchPaths << QCoreApplication::applicationDirPath() + "/../themes/" + filename;
    searchPaths << QCoreApplication::applicationDirPath() + "/themes/" + filename;

    for (const QString &path : searchPaths) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            QString qss = stream.readAll();
            file.close();
            qDebug() << "[THEME] Loaded:" << path;
            return qss;
        }
    }
    qWarning() << "[THEME] Could not find theme file:" << filename;
    return QString();
}

void ThemeManager::applyTheme(Theme theme)
{
    m_theme = theme;
    Theme actual = (theme == System) ? detectSystemTheme() : theme;
    m_effective = actual;

    // Estilo consistente entre plataformas: Fusion se usa siempre,
    // incluso en modo System (donde el QSS luce mejor y mediante de forma estable).
    if (!m_styleIsFusion) {
        qApp->setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
        m_styleIsFusion = true;
    }

    QString qssFile = (actual == Dark) ? "dark.qss" : "light.qss";
    QString qss = loadQss(qssFile);

    if (!qss.isEmpty()) {
        qApp->setStyleSheet(qss);
    }

    saveSettings();
    emit themeChanged(actual);
}

void ThemeManager::setTheme(Theme theme)
{
    applyTheme(theme);
}

void ThemeManager::saveSettings()
{
    QSettings s("LocalApp", "FFmpegExtractor");
    s.setValue("theme", themeName(m_theme));
}

void ThemeManager::loadSettings()
{
    QSettings s("LocalApp", "FFmpegExtractor");
    m_theme = themeFromName(s.value("theme", "system").toString());
}
