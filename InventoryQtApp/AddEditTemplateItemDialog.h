#pragma once

#include <QDialog>
#include <QString>

#include "ui_AddEditTemplateItemDialog.h"
#include "TruckStockService.h"

class AddEditTemplateItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddEditTemplateItemDialog(QWidget* parent = nullptr);
    ~AddEditTemplateItemDialog();

    CreateTemplateItemRequest getItem() const;

    void setItem(const CreateTemplateItemRequest& item);

private slots:
    void onSaveClicked();
    void onCancelClicked();

private:
    Ui::AddEditTemplateItemDialogClass ui;

    void setupConnections();
    bool validateForm();
};