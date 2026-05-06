// AddProductDialog.h - Dialog for adding new products to the inventory
#pragma once

#include <QDialog>
#include "ui_AddProductDialog.h"

// Dialog window for collecting product information when adding new items
class AddProductDialog : public QDialog
{
	Q_OBJECT

public:
	// Constructor
	AddProductDialog(QWidget *parent = nullptr);
	// Destructor
	~AddProductDialog();

	// Getters for product information
	QString getProductName() const;  // Returns trimmed product name
	QString getBarcode() const;      // Returns trimmed barcode
	int getQuantity() const;         // Returns product quantity

	void setProductData( const QString& name, const QString& barcode, int quantity);
	void setViewMode(const QString& name,const QString& barcode,int quantity);

private slots:
	// Slot handlers for button clicks
	void onSaveButtonClicked();   // Validates input and accepts dialog
	void onCancelButtonClicked(); // Rejects dialog without saving
	void onCloseButtonClicked();  // Closes dialog

private:
	Ui::AddProductDialogClass ui;

	
};

