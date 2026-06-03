#pragma once

#include <QDialog>
#include <QPoint>
#include <QString>

#include "ui_AddEditTemplateItemDialog.h"
#include "TruckStockService.h"
#include "Theme.h"

class QMouseEvent;

class AddEditTemplateItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddEditTemplateItemDialog(QWidget* parent = nullptr);
    ~AddEditTemplateItemDialog();

    CreateTemplateItemRequest getItem() const;

    void setItem(const CreateTemplateItemRequest& item);

    void applyTheme(Theme::AppTheme theme);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onCloseClicked();

private:
    Ui::AddEditTemplateItemDialogClass ui;

    QPoint dragPosition;

    void setupConnections();
    bool validateForm();
};