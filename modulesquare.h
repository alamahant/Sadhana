#ifndef MODULESQUARE_H
#define MODULESQUARE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include "deitymodule.h"

class ModuleSquare : public QWidget
{
    Q_OBJECT

public:
    explicit ModuleSquare(DeityModule* module, QWidget* parent = nullptr);

    void updateImage(); // Called when user overrides image
    void savePdfPath(const QString& moduleId, const QString& path);
        void setCreateNewMode(bool isCreateNew);
signals:
    void clicked(DeityModule* module);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;  // Changed: QEnterEvent*
    void leaveEvent(QEvent* event) override;       // leaveEvent still uses QEvent*
    void paintEvent(QPaintEvent* event) override;
private:
    DeityModule* m_module;
    QLabel* m_imageLabel;
    QLabel* m_nameLabel;
    bool m_hovered;
    bool m_isCreateNew;

};

#endif // MODULESQUARE_H
