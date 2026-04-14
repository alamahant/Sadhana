#include "donationdialog.h"
#include <QApplication>
#include <QClipboard>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QToolTip>

DonationDialog::DonationDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void DonationDialog::setupUI()
{
    setWindowTitle(tr("Support %1").arg(QApplication::applicationName()));
    setMinimumSize(480, 420);

    setStyleSheet(R"(
        QDialog {
            background-color: #1a1a1a;
        }
        QLabel {
            color: #ffffff;
            background-color: transparent;
        }
        QFrame {
            background-color: #2a2a2a;
            border: 1px solid #444;
            border-radius: 6px;
        }
        QPushButton {
            background-color: #3a6ea5;
            color: white;
            border: none;
            padding: 6px 12px;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #4a7eb5;
        }
        QPushButton:pressed {
            background-color: #2a5e95;
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(25, 25, 25, 25);

    // ========== HEADER ==========
    QLabel *heartLabel = new QLabel("❤️", this);
    heartLabel->setAlignment(Qt::AlignCenter);
    heartLabel->setStyleSheet("font-size: 32px;");
    mainLayout->addWidget(heartLabel);

    QLabel *titleLabel = new QLabel(tr("Support %1").arg(QApplication::applicationName()), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    QLabel *messageLabel = new QLabel(this);
    messageLabel->setWordWrap(true);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setText(tr(
        "If you find %1 useful and you enjoy using it, "
        "please consider supporting its development. "
        "Your donation helps maintain and improve this application, "
        "ensuring it remains free and actively developed."
    ).arg(QApplication::applicationName()));
    mainLayout->addWidget(messageLabel);

    QLabel *thanksLabel = new QLabel(this);
    thanksLabel->setWordWrap(true);
    thanksLabel->setAlignment(Qt::AlignCenter);
    thanksLabel->setText(tr(
        "Every contribution, no matter how small, makes a difference "
        "and is greatly appreciated! Choose your preferred platform below."
    ));
    mainLayout->addWidget(thanksLabel);

    mainLayout->addSpacing(10);

    // ========== DONATION PLATFORMS ==========
    QGridLayout *platformsLayout = new QGridLayout();
    platformsLayout->setHorizontalSpacing(10);
    platformsLayout->setVerticalSpacing(8);

    int row = 0;

    // Buy Me a Coffee
    QLabel *bmcIcon = new QLabel("☕", this);
    bmcIcon->setStyleSheet("font-size: 18px;");
    bmcIcon->setAlignment(Qt::AlignCenter);
    platformsLayout->addWidget(bmcIcon, row, 0);

    QLabel *bmcLabel = new QLabel(tr("Buy Me a Coffee:"), this);
    platformsLayout->addWidget(bmcLabel, row, 1);

    QLabel *bmcLink = new QLabel(this);
    bmcLink->setTextInteractionFlags(Qt::TextSelectableByMouse);
    bmcLink->setText("buymeacoffee.com/Alamahant");
    platformsLayout->addWidget(bmcLink, row, 2);

    QPushButton *bmcCopy = new QPushButton(tr("Copy"), this);
    bmcCopy->setFixedWidth(60);
    connect(bmcCopy, &QPushButton::clicked, [this]() {
        copyToClipboard("https://buymeacoffee.com/Alamahant");
    });
    platformsLayout->addWidget(bmcCopy, row, 3);
    row++;

    // Ko-fi
    QLabel *kofiIcon = new QLabel("❤️", this);
    kofiIcon->setStyleSheet("font-size: 18px;");
    kofiIcon->setAlignment(Qt::AlignCenter);
    platformsLayout->addWidget(kofiIcon, row, 0);

    QLabel *kofiLabel = new QLabel(tr("Ko-fi:"), this);
    platformsLayout->addWidget(kofiLabel, row, 1);

    QLabel *kofiLink = new QLabel(this);
    kofiLink->setTextInteractionFlags(Qt::TextSelectableByMouse);
    kofiLink->setText("ko-fi.com/alamahant");
    platformsLayout->addWidget(kofiLink, row, 2);

    QPushButton *kofiCopy = new QPushButton(tr("Copy"), this);
    kofiCopy->setFixedWidth(60);
    connect(kofiCopy, &QPushButton::clicked, [this]() {
        copyToClipboard("https://ko-fi.com/alamahant");
    });
    platformsLayout->addWidget(kofiCopy, row, 3);
    row++;

    // PayPal
    QLabel *ppIcon = new QLabel("$", this);
    ppIcon->setStyleSheet("font-size: 18px; font-weight: bold;");
    ppIcon->setAlignment(Qt::AlignCenter);
    platformsLayout->addWidget(ppIcon, row, 0);

    QLabel *ppLabel = new QLabel(tr("PayPal:"), this);
    platformsLayout->addWidget(ppLabel, row, 1);

    QLabel *ppLink = new QLabel(this);
    ppLink->setTextInteractionFlags(Qt::TextSelectableByMouse);
    ppLink->setText("paypal.me/Alamahant");
    platformsLayout->addWidget(ppLink, row, 2);

    QPushButton *ppCopy = new QPushButton(tr("Copy"), this);
    ppCopy->setFixedWidth(60);
    connect(ppCopy, &QPushButton::clicked, [this]() {
        copyToClipboard("https://paypal.me/Alamahant");
    });
    platformsLayout->addWidget(ppCopy, row, 3);

    platformsLayout->setColumnStretch(2, 1);
    mainLayout->addLayout(platformsLayout);

    mainLayout->addSpacing(15);

    // ========== SUPPORT ENABLES ==========
    QFrame *supportFrame = new QFrame(this);
    supportFrame->setFrameShape(QFrame::StyledPanel);
    supportFrame->setLineWidth(1);

    QVBoxLayout *supportLayout = new QVBoxLayout(supportFrame);

    QLabel *supportTitle = new QLabel(tr("✨ Your support enables:"), this);
    QFont supportFont = supportTitle->font();
    supportFont.setBold(true);
    supportTitle->setFont(supportFont);
    supportLayout->addWidget(supportTitle);

    QLabel *supportList = new QLabel(this);
    supportList->setText(tr(
        "• New features and improvements\n"
        "• Bug fixes and maintenance\n"
        "• Future updates and compatibility"
    ));
    supportLayout->addWidget(supportList);

    mainLayout->addWidget(supportFrame);

    mainLayout->addStretch();

    // ========== CLOSE BUTTON ==========
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
}

void DonationDialog::copyToClipboard(const QString &url)
{
    QApplication::clipboard()->setText(url);
    QToolTip::showText(QCursor::pos(), tr("Copied to clipboard!"), this, QRect(), 1500);
}
