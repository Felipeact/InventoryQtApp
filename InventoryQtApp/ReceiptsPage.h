#pragma once

#include <QWidget>
#include "ui_ReceiptsPage.h"

class ReceiptsPage : public QWidget
{
    Q_OBJECT

public:
    ReceiptsPage(QWidget* parent = nullptr);
    ~ReceiptsPage();

    void refreshReceipts();

private slots:
    void onUploadReceiptClicked();
    void onSearchChanged(const QString& text);
    void onPageChanged(int page);

private:
    Ui::ReceiptsPageClass ui;

    void setupConnections();
    void loadReceipts();
};
