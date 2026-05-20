#pragma once

#include <QDialog>
#include "ui_AssignTemplateDialog.h"

class AssignTemplateDialog : public QDialog
{
    Q_OBJECT

public:
    AssignTemplateDialog(QWidget* parent = nullptr);
    ~AssignTemplateDialog();

    QString getSelectedTruck() const;
    QString getSelectedTemplate() const;

private slots:
    void onTruckSelected(int index);
    void onTemplateSelected(int index);
    void onAssignClicked();
    void onCancelClicked();

private:
    Ui::AssignTemplateDialogClass ui;

    void setupConnections();
    void loadTrucks();
    void loadTemplates();
    void loadTemplatePreview(const QString& templateName);
};
