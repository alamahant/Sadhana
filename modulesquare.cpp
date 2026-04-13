#include "modulesquare.h"
#include <QPixmap>
#include <QMouseEvent>
#include <QPainter>
#include <QEnterEvent>
#include<QPainter>
#include"custommodule.h"

ModuleSquare::ModuleSquare(DeityModule* module, QWidget* parent)
    : QWidget(parent)
    , m_module(module)
    , m_hovered(false)
    , m_isCreateNew(false)
{
    setFixedSize(200, 220);
    setCursor(Qt::PointingHandCursor);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    // Image label
    m_imageLabel = new QLabel(this);
    m_imageLabel->setFixedSize(180, 180);
    m_imageLabel->setScaledContents(true);
    m_imageLabel->setAlignment(Qt::AlignCenter);

    // Name label
    m_nameLabel = new QLabel(this);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #ffffff;");

    if (m_module) {
        QPixmap pixmap(m_module->effectiveImagePath());
        if (!pixmap.isNull()) {
            m_imageLabel->setPixmap(pixmap.scaled(180, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        m_nameLabel->setText(m_module->name());
    } else {
        // Placeholder for create new button
        m_imageLabel->setText("+");
        m_imageLabel->setStyleSheet("font-size: 48px; color: #ffd700;");
        m_nameLabel->setText("Create Custom");
    }

    layout->addWidget(m_imageLabel, 0, Qt::AlignCenter);
    layout->addWidget(m_nameLabel);

    setStyleSheet("background-color: #2a2a2a; border-radius: 10px;");
}


void ModuleSquare::updateImage()
{
    if (!m_module) return;

    QString imagePath;
    if (CustomModule* custom = dynamic_cast<CustomModule*>(m_module)) {
        imagePath = custom->userImagePath();  // Direct access, no fallback
    } else {
        imagePath = m_module->effectiveImagePath();  // For default modules
    }

    if (!imagePath.isEmpty()) {
        QPixmap pixmap(imagePath);
        if (!pixmap.isNull()) {
            m_imageLabel->setPixmap(pixmap.scaled(180, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            return;
        }
    }

    // No image set - show nothing or placeholder
    m_imageLabel->clear();
    m_imageLabel->setText("");  // Empty, no fallback
}



void ModuleSquare::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_isCreateNew || !m_module) {
            emit clicked(nullptr);  // Create new
        } else {
            emit clicked(m_module);
        }
    }
    QWidget::mousePressEvent(event);
}

void ModuleSquare::enterEvent(QEnterEvent* event)
{
    m_hovered = true;
    setStyleSheet("background-color: #3a3a3a; border-radius: 10px;");
    QWidget::enterEvent(event);
}

void ModuleSquare::leaveEvent(QEvent* event)
{
    m_hovered = false;
    setStyleSheet("background-color: #2a2a2a; border-radius: 10px;");
    QWidget::leaveEvent(event);
}

void ModuleSquare::setCreateNewMode(bool isCreateNew)
{
    m_isCreateNew = isCreateNew;
    if (m_isCreateNew) {
        m_nameLabel->setText("Create Custom");
        m_imageLabel->setText("+");
        m_imageLabel->setStyleSheet("font-size: 48px; color: #ffd700;");
        setStyleSheet("background-color: #2a2a2a; border: 2px dashed #ffd700; border-radius: 10px;");
    }
}

void ModuleSquare::paintEvent(QPaintEvent* event)
{
    if (m_isCreateNew) {
        QPainter painter(this);
        painter.setPen(QPen(QColor("#ffd700"), 2, Qt::DashLine));
        painter.drawRect(rect().adjusted(2, 2, -2, -2));
    }
    QWidget::paintEvent(event);
}
