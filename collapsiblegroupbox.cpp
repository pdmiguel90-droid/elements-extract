#include "collapsiblegroupbox.h"

#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QVBoxLayout>

CollapsibleGroupBox::CollapsibleGroupBox(const QString &title, QWidget *parent)
    : QGroupBox(title, parent)
    , m_content(new QWidget(this))
    , m_rootLayout(new QVBoxLayout(this))
    , m_contentLayout(new QVBoxLayout(m_content))
    , m_title(title)
{
    m_rootLayout->setContentsMargins(0, 0, 0, 0);
    m_rootLayout->setSpacing(0);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_rootLayout->addWidget(m_content);

    setFocusPolicy(Qt::StrongFocus);
    setToolTip(tr("Pulsa la cabecera o Espacio para plegar/desplegar"));
    applyTitle();
}

void CollapsibleGroupBox::addPermanentWidget(QWidget *widget)
{
    if (widget) m_rootLayout->insertWidget(0, widget);
}

void CollapsibleGroupBox::setCollapsible(bool collapsible)
{
    m_collapsible = collapsible;
    if (!collapsible) m_expanded = true;
    setFocusPolicy(collapsible ? Qt::StrongFocus : Qt::NoFocus);
    m_content->setVisible(m_expanded);
    applyTitle();
    updateGeometry();
}

void CollapsibleGroupBox::setExpanded(bool expanded)
{
    if (!m_collapsible) expanded = true;
    m_expanded = expanded;
    m_content->setVisible(expanded);
    applyTitle();
    updateGeometry();
}

void CollapsibleGroupBox::setTitleText(const QString &title)
{
    m_title = title;
    applyTitle();
}

void CollapsibleGroupBox::setSummary(const QString &summary)
{
    m_summary = summary;
    applyTitle();
}

void CollapsibleGroupBox::applyTitle()
{
    QString text = m_title;
    if (m_collapsible) {
        text.prepend(m_expanded ? QStringLiteral("▾ ") : QStringLiteral("▸ "));
    }
    if (!m_summary.isEmpty()) {
        text += QStringLiteral("  — ") + m_summary;
    }
    QGroupBox::setTitle(text);
}

int CollapsibleGroupBox::headerHeight() const
{
    // El QSS sitúa el título en el margen superior (margin-top: 12px).
    return qMax(24, fontMetrics().height() + 12);
}

void CollapsibleGroupBox::toggleFromUser()
{
    if (!m_collapsible) return;
    setExpanded(!m_expanded);
    emit expandedChanged(m_expanded);
}

void CollapsibleGroupBox::mousePressEvent(QMouseEvent *event)
{
    if (m_collapsible && event && event->button() == Qt::LeftButton &&
        event->pos().y() <= headerHeight()) {
        toggleFromUser();
        return;
    }
    QGroupBox::mousePressEvent(event);
}

void CollapsibleGroupBox::keyPressEvent(QKeyEvent *event)
{
    if (m_collapsible && event &&
        (event->key() == Qt::Key_Space || event->key() == Qt::Key_Enter ||
         event->key() == Qt::Key_Return)) {
        toggleFromUser();
        return;
    }
    QGroupBox::keyPressEvent(event);
}
