#include "StockTemplatesPage.h"
#include "AddEditTemplateDialog.h"

#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>

StockTemplatesPage::StockTemplatesPage(
    TruckStockService* truckStockService,
    QWidget* parent
)
    : QWidget(parent),
    truckStockService(truckStockService)
{
    ui.setupUi(this);
    setupConnections();
    loadTemplates();
}

StockTemplatesPage::~StockTemplatesPage()
{
}

void StockTemplatesPage::setupConnections()
{
    connect(ui.newTemplateButton, &QPushButton::clicked,
        this, &StockTemplatesPage::onNewTemplateClicked);

    connect(ui.searchInput, &QLineEdit::textChanged,
        this, &StockTemplatesPage::onSearchChanged);
}

void StockTemplatesPage::loadTemplates()
{
    ui.templatesTable->clearContents();

    currentTemplates = truckStockService->getTemplates();

    ui.templatesTable->setRowCount(
        static_cast<int>(currentTemplates.size())
    );

    ui.templatesTable->setColumnCount(4);

    for (int row = 0; row < static_cast<int>(currentTemplates.size()); ++row) {
        const StockTemplateDto& stockTemplate = currentTemplates[row];

        ui.templatesTable->setItem(
            row, 0,
            new QTableWidgetItem(QString::fromStdString(stockTemplate.name))
        );

        ui.templatesTable->setItem(
            row, 1,
            new QTableWidgetItem(QString::fromStdString(stockTemplate.tradeType))
        );

        ui.templatesTable->setItem(
            row, 2,
            new QTableWidgetItem(QString::number(stockTemplate.itemCount))
        );

        addActionButtons(row);
    }

    ui.templatesTable->horizontalHeader()->setStretchLastSection(true);
    ui.templatesTable->verticalHeader()->setVisible(false);

    ui.paginationLabel->setText(
        QString("Showing %1 templates").arg(currentTemplates.size())
    );

    ui.templatesTable->setColumnWidth(3, 260);
}

void StockTemplatesPage::refreshTemplates()
{
    loadTemplates();
}

void StockTemplatesPage::onNewTemplateClicked()
{
    AddEditTemplateDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {

        CreateTemplateRequest request;

        request.name =
            dialog.getTemplateName().toStdString();

        request.tradeType =
            dialog.getTradeType().toStdString();

        request.items =
            dialog.getItems();

        bool success =
            truckStockService->createTemplate(request);

        if (success) {
            QMessageBox::information(
                this,
                "Success",
                "Template created successfully."
            );

            loadTemplates();
        }
        else {
            QMessageBox::warning(
                this,
                "Error",
                "Failed to create template. Check backend console."
            );
        }
    }
}

void StockTemplatesPage::onSearchChanged(const QString& text)
{
    for (int row = 0; row < ui.templatesTable->rowCount(); ++row) {
        bool match = false;

        for (int col = 0; col < ui.templatesTable->columnCount(); ++col) {
            QTableWidgetItem* item = ui.templatesTable->item(row, col);

            if (item && item->text().contains(text, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }

        ui.templatesTable->setRowHidden(row, !match);
    }
}

void StockTemplatesPage::onPageChanged(int page)
{
    Q_UNUSED(page);
}

void StockTemplatesPage::addActionButtons(int row)
{
    QWidget* actionWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(actionWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    QPushButton* viewButton = new QPushButton("View", actionWidget);
    QPushButton* editButton = new QPushButton("Edit", actionWidget);
    QPushButton* deleteButton = new QPushButton("Delete", actionWidget);

    viewButton->setMinimumHeight(28);
    editButton->setMinimumHeight(28);
    deleteButton->setMinimumHeight(28);

    viewButton->setObjectName("viewButton");
    editButton->setObjectName("editButton");
    deleteButton->setObjectName("deleteButton");

    layout->addWidget(viewButton);
    layout->addWidget(editButton);
    layout->addWidget(deleteButton);
    layout->addStretch();

    ui.templatesTable->setCellWidget(row, 3, actionWidget);

    connect(viewButton, &QPushButton::clicked, this, [this, row]() {
        onViewTemplateClicked(row);
        });

    connect(editButton, &QPushButton::clicked, this, [this, row]() {
        onEditTemplateClicked(row);
        });

    connect(deleteButton, &QPushButton::clicked, this, [this, row]() {
        onDeleteTemplateClicked(row);
        });
}

void StockTemplatesPage::onViewTemplateClicked(int row)
{
    if (row < 0 || row >= static_cast<int>(currentTemplates.size())) {
        return;
    }

    const StockTemplateDto& selectedTemplate = currentTemplates[row];

    TemplateDetailsDto details =
        truckStockService->getTemplateById(selectedTemplate.id);

    AddEditTemplateDialog dialog(this);

    dialog.setTemplateData(details, true);

    dialog.exec();
}

void StockTemplatesPage::onEditTemplateClicked(int row)
{
    if (row < 0 || row >= static_cast<int>(currentTemplates.size())) {
        return;
    }

    const StockTemplateDto& selectedTemplate = currentTemplates[row];

    TemplateDetailsDto details =
        truckStockService->getTemplateById(selectedTemplate.id);

    AddEditTemplateDialog dialog(this);

    dialog.setTemplateData(details, false);

    if (dialog.exec() == QDialog::Accepted) {

        CreateTemplateRequest request;

        request.name =
            dialog.getTemplateName().toStdString();

        request.tradeType =
            dialog.getTradeType().toStdString();

        request.items =
            dialog.getItems();

        bool success =
            truckStockService->updateTemplate(
                selectedTemplate.id,
                request
            );

        if (success) {
            QMessageBox::information(
                this,
                "Success",
                "Template updated successfully."
            );

            loadTemplates();
        }
        else {
            QMessageBox::warning(
                this,
                "Error",
                "Failed to update template."
            );
        }
    }
}

void StockTemplatesPage::onDeleteTemplateClicked(int row)
{
    if (row < 0 || row >= static_cast<int>(currentTemplates.size())) {
        return;
    }

    const StockTemplateDto& selectedTemplate = currentTemplates[row];

    QMessageBox::StandardButton confirm =
        QMessageBox::question(
            this,
            "Delete Template",
            "Are you sure you want to delete this template?"
        );

    if (confirm != QMessageBox::Yes) {
        return;
    }

    bool success =
        truckStockService->deleteTemplate(selectedTemplate.id);

    if (success) {
        QMessageBox::information(
            this,
            "Success",
            "Template deleted successfully."
        );

        loadTemplates();
    }
    else {
        QMessageBox::warning(
            this,
            "Error",
            "Failed to delete template."
        );
    }
}