#include "imagefullscreendialog.h"
#include <QVBoxLayout>
#include <QPixmap>
#include <QKeyEvent>

ImageFullscreenDialog::ImageFullscreenDialog(const QString& imagePath, QWidget* parent)
    : QDialog(parent)
    , m_zoomFactor(1.0)
{
    setWindowTitle("Fullscreen Image");
    setWindowState(windowState() | Qt::WindowFullScreen);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->viewport()->installEventFilter(this);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    //m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setAlignment(Qt::AlignCenter);
    
    m_imageLabel = new QLabel();
    m_imageLabel->setAlignment(Qt::AlignCenter);
    
    m_originalPixmap.load(imagePath);  // Store original
    if (!m_originalPixmap.isNull()) {
        m_imageLabel->setPixmap(m_originalPixmap);
        m_imageLabel->resize(m_originalPixmap.size());
    }
    
    m_scrollArea->setWidget(m_imageLabel);
    layout->addWidget(m_scrollArea);
    
    // Instructions
    setWindowTitle("Image Viewer - Use mouse wheel to zoom, drag to pan, Esc to close");
}


void ImageFullscreenDialog::wheelEvent(QWheelEvent* event)
{
    zoom(event->angleDelta().y() > 0 ? 0.1 : -0.1);
    event->accept();
}


bool ImageFullscreenDialog::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_scrollArea->viewport() && event->type() == QEvent::Wheel) {
        // Send the wheel event to the dialog's wheelEvent for zoom
        wheelEvent(static_cast<QWheelEvent*>(event));
        return true;  // Prevent scroll area from scrolling
    }
    return QDialog::eventFilter(obj, event);
}

void ImageFullscreenDialog::zoom(double delta)
{
    m_zoomFactor += delta;
    if (m_zoomFactor < 0.3) m_zoomFactor = 0.3;
    if (m_zoomFactor > 3.0) m_zoomFactor = 3.0;

    if (!m_originalPixmap.isNull()) {
        QSize newSize = m_originalPixmap.size() * m_zoomFactor;
        QPixmap scaled = m_originalPixmap.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_imageLabel->setPixmap(scaled);
        m_imageLabel->resize(newSize);
    }
}

void ImageFullscreenDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
    QDialog::keyPressEvent(event);
}
