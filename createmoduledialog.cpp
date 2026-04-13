#include "createmoduledialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QPixmap>
#include"constants.h"

CreateModuleDialog::CreateModuleDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Create Custom Module");
    setMinimumSize(400, 300);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Module name
    QLabel* nameLabel = new QLabel("Module Name:", this);
    nameLabel->setStyleSheet("color: #ffd700;");
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setStyleSheet("QLineEdit { background-color: #111; color: #ccc; border: 1px solid #333; border-radius: 4px; padding: 6px; }");
    m_nameEdit->setPlaceholderText("e.g., Medicine Buddha Practice");
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(m_nameEdit);
    
    // Image selection (optional)
    QLabel* imageLabel = new QLabel("Default Image (optional):", this);
    m_browseButton = new QPushButton("Browse...", this);
    m_imagePreview = new QLabel(this);
    m_imagePreview->setFixedSize(100, 100);
    m_imagePreview->setStyleSheet("border: 1px solid #333; background-color: #111;");
    m_imagePreview->setAlignment(Qt::AlignCenter);
    m_imagePreview->setText("No image");
    
    QHBoxLayout* imageLayout = new QHBoxLayout();
    imageLayout->addWidget(m_browseButton);
    imageLayout->addStretch();
    
    mainLayout->addWidget(imageLabel);
    mainLayout->addLayout(imageLayout);
    mainLayout->addWidget(m_imagePreview, 0, Qt::AlignCenter);
    
    mainLayout->addStretch();
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_okButton = new QPushButton("Create", this);
    m_cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    mainLayout->addLayout(buttonLayout);
    
    connect(m_browseButton, &QPushButton::clicked, this, &CreateModuleDialog::onBrowseImage);
    connect(m_okButton, &QPushButton::clicked, this, &CreateModuleDialog::onAccept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QString CreateModuleDialog::getModuleName() const
{
    return m_nameEdit->text().trimmed();
}

QString CreateModuleDialog::getImagePath() const
{
    return m_imagePath;
}

void CreateModuleDialog::onBrowseImage()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        "Select Image for Module",
        Constants::imagesPath,
        "Image Files (*.png *.jpg *.jpeg *.bmp)");
    
    if (!filePath.isEmpty()) {
        m_imagePath = filePath;
        QPixmap pixmap(filePath);
        if (!pixmap.isNull()) {
            m_imagePreview->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}

void CreateModuleDialog::onAccept()
{
    if (getModuleName().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a module name.");
        return;
    }
    accept();
}
