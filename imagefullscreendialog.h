#ifndef IMAGEFULLSCREENDIALOG_H
#define IMAGEFULLSCREENDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QScrollArea>
#include <QWheelEvent>

class ImageFullscreenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageFullscreenDialog(const QString& imagePath, QWidget* parent = nullptr);
    
protected:
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
private:
    QLabel* m_imageLabel;
    QScrollArea* m_scrollArea;
    double m_zoomFactor;
    void zoom(double delta);
    QPixmap m_originalPixmap;
};

#endif
