#ifndef THEME_H
#define THEME_H

#include <QObject>
#include <QApplication>
#include <QPalette>
#include <QSettings>
#include <QString>

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    enum Theme { Dark, Light, System };

    static ThemeManager *instance();

    void applyTheme(Theme theme = System);
    void setTheme(Theme theme);
    Theme currentTheme() const;
    Theme effectiveTheme() const; // Returns actual Dark/Light after system detection

    static QString themeName(Theme t);
    static Theme themeFromName(const QString &name);

signals:
    void themeChanged(Theme newTheme);

private:
    explicit ThemeManager(QObject *parent = nullptr);
    Theme m_theme;
    Theme m_effective;
    bool m_styleIsFusion = false;
    QString loadQss(const QString &filename);
    Theme detectSystemTheme();
    void saveSettings();
    void loadSettings();
};

#endif // THEME_H
