#pragma once

#include <QDialog>
#include "ui_AddProductDialog.h"

class AddProductDialog : public QDialog
{
	Q_OBJECT

public:
	AddProductDialog(QWidget *parent = nullptr);
	~AddProductDialog();

private:
	Ui::AddProductDialogClass ui;
};

