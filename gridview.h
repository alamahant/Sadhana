#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include <QWidget>
#include <QGridLayout>
#include <QScrollArea>
#include <QVector>
#include "modulesquare.h"
#include "deitymodule.h"
#include<QPushButton>

class GridView : public QWidget
{
    Q_OBJECT

public:
    explicit GridView(QWidget* parent = nullptr);

    void setModules(const QVector<DeityModule*>& modules);
    void refreshImages(); // Called when user changes images

signals:
    void moduleSelected(DeityModule* module);
    void createCustomModuleRequested();
    void deleteModuleRequested(DeityModule* module);
private:
    QGridLayout* m_gridLayout;
    QWidget* m_gridContainer;
    QScrollArea* m_scrollArea;
    QVector<ModuleSquare*> m_squares;
    //custom
    ModuleSquare* m_createNewSquare;
    QVector<DeityModule*> m_modules;
    void addContextMenu(ModuleSquare* square, DeityModule* module);
    QPushButton* m_createButton;

};

#endif // GRIDVIEW_H
