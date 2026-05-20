#pragma once

#include <QDialog>
#include "ui_AddEditTemplateDialog.h"

class AddEditTemplateDialog : public QDialog
{
    Q_OBJECT

public:
    AddEditTemplateDialog(QWidget* parent = nullptr);
    ~AddEditTemplateDialog();

    void setEditMode(const QString& templateId);
    QString getTemplateName() const;
    QString getDescription() const;

private slots:
    void onAddItemClicked();
    void onSaveClicked();
    void onCancelClicked();

private:
    Ui::AddEditTemplateDialogClass ui;

    void setupConnections();
};
