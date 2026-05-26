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

        ui.templatesTable->setItem(
            row, 3,
            new QTableWidgetItem("Edit / Delete")
        );
    }

    ui.templatesTable->horizontalHeader()->setStretchLastSection(true);
    ui.templatesTable->verticalHeader()->setVisible(false);

    ui.paginationLabel->setText(
        QString("Showing %1 templates").arg(currentTemplates.size())
    );
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