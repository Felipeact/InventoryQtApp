#pragma once

#include <QDialog>
#include "ui_UploadReceiptDialog.h"

class UploadReceiptDialog : public QDialog
{
    Q_OBJECT

public:
    UploadReceiptDialog(QWidget* parent = nullptr);
    ~UploadReceiptDialog();

    QString getSelectedTruck() const;
    QString getTotalAmount() const;
    QString getFilePath() const;

private slots:
    void onBrowseClicked();
    void onUploadClicked();
    void onCancelClicked();
    void onTruckSelected(int index);

private:
    Ui::UploadReceiptDialogClass ui;
    QString selectedFilePath;

    void setupConnections();
    void loadTrucks();
    void updateFileDisplay(const QString& filePath);
    void loadPreview(const QString& filePath);
};
