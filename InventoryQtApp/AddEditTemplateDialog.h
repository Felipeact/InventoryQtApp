#pragma once

#include <QDialog>
#include "ui_AddEditTemplateDialog.h"
#include "TruckStockService.h"
#include "Theme.h"

class AddEditTemplateDialog : public QDialog
{
    Q_OBJECT

public:

    AddEditTemplateDialog(QWidget* parent = nullptr);
    ~AddEditTemplateDialog();

    void setEditMode(const QString& templateId);
    QString getTemplateName() const;
    QString getDescription() const;

    void applyTheme(Theme::AppTheme theme);
    QString getTradeType() const;
    std::vector<CreateTemplateItemRequest> getItems() const;

    void setTemplateData(const TemplateDetailsDto& templateDetails, bool readOnly = false);

private slots:
    void onAddItemClicked();
    void onSaveClicked();
    void onCancelClicked();

private:
    Ui::AddEditTemplateDialogClass ui;

    void setupConnections();

    void addItemRow(const CreateTemplateItemRequest& item);
    CreateTemplateItemRequest getItemFromRow(int row) const;
    void addItemActionButtons(int row);
    void onEditItemClicked(int row);
    void onDeleteItemClicked(int row);

    bool readOnlyMode = false;

};
