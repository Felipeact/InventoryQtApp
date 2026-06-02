#include "StockTemplatesPage.h"
#include "Theme.h"
#include "AddEditTemplateDialog.h"

#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHBoxLayout>
#include <QWidget>
#include <algorithm>

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

    connect(ui.pageButton, &QPushButton::clicked,
        this, &StockTemplatesPage::onPreviousPageClicked);

    connect(ui.pageButton_3, &QPushButton::clicked,
        this, &StockTemplatesPage::onNextPageClicked);

    connect(ui.pageButton_2, &QPushButton::clicked,
        this, &StockTemplatesPage::onPage2Clicked);
}

void StockTemplatesPage::loadTemplates()
{
    if (!truckStockService) {
        return;
    }

    ui.templatesTable->clearContents();

    currentTemplates = truckStockService->getTemplates();

    filterTemplates();

    ui.templatesTable->horizontalHeader()->setStretchLastSection(true);
    ui.templatesTable->verticalHeader()->setVisible(false);
    ui.templatesTable->setColumnWidth(3, 260);
}

void StockTemplatesPage::refreshTemplatesList()
{
    loadTemplates();
}

void StockTemplatesPage::onNewTemplateClicked()
{
    AddEditTemplateDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        CreateTemplateRequest request;

        request.name = dialog.getTemplateName().toStdString();
        request.tradeType = dialog.getTradeType().toStdString();
        request.items = dialog.getItems();

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
    Q_UNUSED(text);
    filterTemplates();
}

void StockTemplatesPage::filterTemplates()
{
    QString searchText = ui.searchInput->text().trimmed();

    filteredTemplates.clear();

    for (const StockTemplateDto& stockTemplate : currentTemplates) {
        QString name =
            QString::fromStdString(stockTemplate.name);

        QString tradeType =
            QString::fromStdString(stockTemplate.tradeType);

        QString itemCount =
            QString::number(stockTemplate.itemCount);

        bool matches =
            searchText.isEmpty() ||
            name.contains(searchText, Qt::CaseInsensitive) ||
            tradeType.contains(searchText, Qt::CaseInsensitive) ||
            itemCount.contains(searchText, Qt::CaseInsensitive);

        if (matches) {
            filteredTemplates.push_back(stockTemplate);
        }
    }

    currentPage = 1;

    populateTable();
    updatePagination();
}

void StockTemplatesPage::populateTable()
{
    ui.templatesTable->clearContents();
    ui.templatesTable->setColumnCount(4);

    int totalItems =
        static_cast<int>(filteredTemplates.size());

    int startIndex =
        (currentPage - 1) * pageSize;

    int endIndex =
        (std::min)(startIndex + pageSize, totalItems);

    int rowCount =
        endIndex - startIndex;

    ui.templatesTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; ++row) {
        const StockTemplateDto& stockTemplate =
            filteredTemplates[startIndex + row];

        ui.templatesTable->setItem(
            row,
            0,
            new QTableWidgetItem(
                QString::fromStdString(stockTemplate.name)
            )
        );

        ui.templatesTable->setItem(
            row,
            1,
            new QTableWidgetItem(
                QString::fromStdString(stockTemplate.tradeType)
            )
        );

        ui.templatesTable->setItem(
            row,
            2,
            new QTableWidgetItem(
                QString::number(stockTemplate.itemCount)
            )
        );

        addActionButtons(row, stockTemplate.id);
    }
}

void StockTemplatesPage::addActionButtons(
    int row,
    const std::string& templateId
)
{
    QWidget* actionWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(actionWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    QPushButton* viewButton =
        new QPushButton("View", actionWidget);

    QPushButton* editButton =
        new QPushButton("Edit", actionWidget);

    QPushButton* deleteButton =
        new QPushButton("Delete", actionWidget);

    layout->addWidget(viewButton);
    layout->addWidget(editButton);
    layout->addWidget(deleteButton);
    layout->addStretch();

    ui.templatesTable->setCellWidget(row, 3, actionWidget);

    connect(viewButton, &QPushButton::clicked, this, [this, templateId]() {
        onViewTemplateClicked(templateId);
        });

    connect(editButton, &QPushButton::clicked, this, [this, templateId]() {
        onEditTemplateClicked(templateId);
        });

    connect(deleteButton, &QPushButton::clicked, this, [this, templateId]() {
        onDeleteTemplateClicked(templateId);
        });
}

void StockTemplatesPage::onViewTemplateClicked(
    const std::string& templateId
)
{
    TemplateDetailsDto details =
        truckStockService->getTemplateById(templateId);

    AddEditTemplateDialog dialog(this);

    dialog.setTemplateData(details, true);

    dialog.exec();
}

void StockTemplatesPage::onEditTemplateClicked(
    const std::string& templateId
)
{
    TemplateDetailsDto details =
        truckStockService->getTemplateById(templateId);

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
                templateId,
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

void StockTemplatesPage::onDeleteTemplateClicked(
    const std::string& templateId
)
{
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
        truckStockService->deleteTemplate(templateId);

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

void StockTemplatesPage::updatePagination()
{
    int totalItems =
        static_cast<int>(filteredTemplates.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage > totalPages) {
        currentPage = totalPages;
    }

    int startItem =
        totalItems == 0
        ? 0
        : ((currentPage - 1) * pageSize) + 1;

    int endItem =
        (std::min)(currentPage * pageSize, totalItems);

    ui.paginationLabel->setText(
        QString("Showing %1 to %2 of %3 templates")
        .arg(startItem)
        .arg(endItem)
        .arg(totalItems)
    );

    ui.activePageButton->setText(
        QString::number(currentPage)
    );

    ui.pageButton_2->setText(
        QString::number(currentPage + 1)
    );

    ui.pageButton->setEnabled(
        currentPage > 1
    );

    ui.pageButton_3->setEnabled(
        currentPage < totalPages
    );

    ui.pageButton_2->setVisible(
        currentPage < totalPages
    );
}

void StockTemplatesPage::onPreviousPageClicked()
{
    if (currentPage > 1) {
        currentPage--;

        populateTable();
        updatePagination();
    }
}

void StockTemplatesPage::onNextPageClicked()
{
    int totalItems =
        static_cast<int>(filteredTemplates.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage < totalPages) {
        currentPage++;

        populateTable();
        updatePagination();
    }
}

void StockTemplatesPage::onPage2Clicked()
{
    int totalItems =
        static_cast<int>(filteredTemplates.size());

    int totalPages =
        (std::max)(1, (totalItems + pageSize - 1) / pageSize);

    if (currentPage + 1 <= totalPages) {
        currentPage++;

        populateTable();
        updatePagination();
    }
}

void StockTemplatesPage::applyTheme(Theme::AppTheme theme)
{
    setStyleSheet(
        Theme::truckPageStyle(theme)
    );
}

