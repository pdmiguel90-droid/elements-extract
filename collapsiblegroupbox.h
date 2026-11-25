#pragma once

#include <QGroupBox>
#include <QString>

class QKeyEvent;
class QMouseEvent;
class QVBoxLayout;
class QWidget;

// QGroupBox con cabecera clicable. El título se usa como resumen incluso cuando
// el contenido está plegado, para que la información importante nunca desaparezca.
class CollapsibleGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit CollapsibleGroupBox(const QString &title, QWidget *parent = nullptr);

    QWidget *contentWidget() const { return m_content; }
    QVBoxLayout *contentLayout() const { return m_contentLayout; }

    // Fila siempre visible, situada entre la cabecera y el contenido plegable.
    void addPermanentWidget(QWidget *widget);

    bool isExpanded() const { return m_expanded; }
    bool isCollapsible() const { return m_collapsible; }
    void setCollapsible(bool collapsible);

    void setExpanded(bool expanded);
    void setTitleText(const QString &title);
    void setSummary(const QString &summary);

signals:
    void expandedChanged(bool expanded);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void applyTitle();
    void toggleFromUser();
    int headerHeight() const;

    QWidget *m_content;
    QVBoxLayout *m_rootLayout;
    QVBoxLayout *m_contentLayout;
    QString m_title;
    QString m_summary;
    bool m_expanded = true;
    bool m_collapsible = true;
};
