#ifndef CREATEMODULEDIALOG_H
#define CREATEMODULEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

class CreateModuleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateModuleDialog(QWidget* parent = nullptr);
    
    QString getModuleName() const;
    QString getImagePath() const;

private slots:
    void onBrowseImage();
    void onAccept();

private:
    QLineEdit* m_nameEdit;
    QLabel* m_imagePreview;
    QString m_imagePath;
    QPushButton* m_browseButton;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
};

#endif // CREATEMODULEDIALOG_H