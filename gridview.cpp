#include "gridview.h"
#include <QScrollBar>
#include<QMenu>
#include"custommodule.h"
#include<QStandardPaths>
#include<QFileDialog>
#include"modulemanager.h"
#include"constants.h"


GridView::GridView(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);


    // Top bar with create button
    QHBoxLayout* topBar = new QHBoxLayout();
    topBar->addStretch();

    m_createButton = new QPushButton("+ Create Custom Module", this);
    m_createButton->setFixedHeight(32);
    m_createButton->setStyleSheet("QPushButton { background-color: #3a6ea5; border-radius: 4px; } QPushButton:hover { background-color: #4a7eb5; }");
    connect(m_createButton, &QPushButton::clicked, this, &GridView::createCustomModuleRequested);



    topBar->addWidget(m_createButton);

    mainLayout->addLayout(topBar);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setStyleSheet("QScrollArea { background: #1a1a1a; border: none; }");

    m_gridContainer = new QWidget();
    m_gridContainer->setStyleSheet("background-color: #1a1a1a;");

    m_gridLayout = new QGridLayout(m_gridContainer);
    m_gridLayout->setContentsMargins(40, 40, 40, 40);
    m_gridLayout->setSpacing(20);
    m_gridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_scrollArea->setWidget(m_gridContainer);
    mainLayout->addWidget(m_scrollArea);


}

void GridView::setModules(const QVector<DeityModule*>& modules)
{
    m_modules = modules;  // Store modules

    // Clear existing squares
    qDeleteAll(m_squares);
    m_squares.clear();

    // Clear layout
    QLayoutItem* item;
    while ((item = m_gridLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    int columns = 4;

    // Add real modules
    for (int i = 0; i < m_modules.size(); ++i) {
        int row = i / columns;
        int col = i % columns;

        ModuleSquare* square = new ModuleSquare(m_modules[i], this);
        connect(square, &ModuleSquare::clicked, this, &GridView::moduleSelected);
        addContextMenu(square, m_modules[i]);
        m_gridLayout->addWidget(square, row, col);
        m_squares.append(square);
    }

    // Add create new square
    //addCreateNewSquare();

    m_gridLayout->setRowStretch(m_gridLayout->rowCount(), 1);
}

void GridView::refreshImages()
{
    for (ModuleSquare* square : m_squares) {
        square->updateImage();
    }
}

void GridView::addContextMenu(ModuleSquare* square, DeityModule* module)
{
    square->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(square, &QWidget::customContextMenuRequested, this, [this, module, square](const QPoint& pos) {
        QMenu menu;
        QAction* setGridImageAction = menu.addAction("Change Grid Image");

        QAction* deleteAction = menu.addAction("Delete Module");
        connect(deleteAction, &QAction::triggered, this, [this, module]() {
            emit deleteModuleRequested(module);
        });
        connect(setGridImageAction, &QAction::triggered, this, [this, module]() {
            // Open file dialog to select new grid image
            QString filePath = QFileDialog::getOpenFileName(nullptr,
                "Select Grid Image for " + module->name(),
                Constants::imagesPath,
                "Image Files (*.png *.jpg *.jpeg *.bmp)");

            if (!filePath.isEmpty()) {
                // Cast to CustomModule and set userImagePath
                CustomModule* custom = dynamic_cast<CustomModule*>(module);
                if (custom) {
                    custom->setUserImagePath(filePath);
                    ModuleManager::instance().saveCustomModule(custom);
                    refreshImages();  // Refresh grid display
                }
            }
        });
        menu.exec(square->mapToGlobal(pos));
    });
}
