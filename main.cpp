#include "mainwindow.h"

#include <QApplication>
#include<QDir>
#include"constants.h"
#include<QIcon>
#include<QStandardPaths>
#include<QFontDatabase>
#include<QFont>

int main(int argc, char *argv[])
{

    QCoreApplication::setApplicationName("Sadhana");
    QCoreApplication::setOrganizationName("Alamahant");
    QCoreApplication::setApplicationVersion("1.0.0");


    QDir().mkpath(Constants::appDirPath);
    QDir().mkpath(Constants::imagesPath);
    QDir().mkpath(Constants::audioPath);
    QDir().mkpath(Constants::customModulesPath);
    QDir().mkpath(Constants::filesPath);
    QDir().mkpath(Constants::journalPath);
    QDir().mkpath(Constants::defaultModuleDataPath);

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/io.github.alamahant.Sadhana.png"));

    QString filesPath = QCoreApplication::applicationDirPath() + "/files";
    // Load Tibetan Machine Uni
    //QString tibetanFont = filesPath + "/TibMachUni-1.901.ttf";
    QString tibetanFont = filesPath + "/NotoSerifTibetan-Regular.ttf";

    if (QFile::exists(tibetanFont)) {
        int id = QFontDatabase::addApplicationFont(tibetanFont);
        if (id != -1) {
            QString family = QFontDatabase::applicationFontFamilies(id).at(0);
        }
    } else {
    }

    // Load Lohit Devanagari
    QString devanagariFont = filesPath + "/Lohit-Devanagari.ttf";
    if (QFile::exists(devanagariFont)) {
        int id = QFontDatabase::addApplicationFont(devanagariFont);
        if (id != -1) {
            QString family = QFontDatabase::applicationFontFamilies(id).at(0);
        }
    }

    // Load Noto Serif TC (Traditional Chinese)
    QString chineseFont = filesPath + "/NotoSerifTC-Regular.ttf";
    if (QFile::exists(chineseFont)) {
        int id = QFontDatabase::addApplicationFont(chineseFont);
        if (id != -1) {
            QString family = QFontDatabase::applicationFontFamilies(id).at(0);
        }
    }

    QFont defaultFont;
    //defaultFont.setFamily("Tibetan Machine Uni, Lohit Devanagari, Noto Serif TC, sans-serif");
    defaultFont.setFamily("Noto Serif Tibetan, Lohit Devanagari, Noto Serif TC, sans-serif");

    a.setFont(defaultFont);



    MainWindow w;
    w.show();
    return a.exec();
}
