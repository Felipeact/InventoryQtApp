#pragma once

#include <QDialog>
#include <QString>

#include "ui_AddEditTemplateItemDialog.h"
#include "TruckStockService.h"
#include "Theme.h"

class AddEditTemplateItemDialog : public QDialog
{
    Q_OBJECT

public:

    explicit AddEditTemplateItemDialog(QWidget* parent = nullptr);
    ~AddEditTemplateItemDialog();

    CreateTemplateItemRequest getItem() const;

    void applyTheme(Theme::AppTheme theme);
    void setItem(const CreateTemplateItemRequest& item);

private slots:
    void onSaveClicked();
    void onCancelClicked();

private:
    Ui::AddEditTemplateItemDialogClass ui;

    void setupConnections();
    bool validateForm();
};