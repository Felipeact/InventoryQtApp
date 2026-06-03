#pragma once

#include <QDialog>
#include <QPoint>
#include <QString>
#include <vector>

#include "ui_AddEditTemplateDialog.h"
#include "TruckStockService.h"
#include "Theme.h"

class QMouseEvent;

class AddEditTemplateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddEditTemplateDialog(QWidget* parent = nullptr);
    ~AddEditTemplateDialog();

    void setEditMode(const QString& templateId);

    QString getTemplateName() const;
    QString getDescription() const;
    QString getTradeType() const;

    std::vector<CreateTemplateItemRequest> getItems() const;

    void setTemplateData(
        const TemplateDetailsDto& templateDetails,
        bool readOnly = false
    );

    void applyTheme(Theme::AppTheme theme);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void onAddItemClicked();
    void onSaveClicked();
    void onCancelClicked();
    void onCloseClicked();

private:
    Ui::AddEditTemplateDialogClass ui;

    bool readOnlyMode = false;
    QPoint dragPosition;

    void setupConnections();
    void setupTable();

    void addItemRow(const CreateTemplateItemRequest& item);
    CreateTemplateItemRequest getItemFromRow(int row) const;

    void addItemActionButtons(int row);
    void onEditItemClicked(int row);
    void onDeleteItemClicked(int row);
};