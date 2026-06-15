#include "MyTruckStockPage.h"
#include "Theme.h"
#include "UseTruckItemDialog.h"
#include "TruckStockHistoryDialog.h"

#include <QAbstractItemView>
#include <QThread>
#include <QPointer>
#include <QFile>
#include <QFileDialog>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QWidget>

MyTruckStockPage::MyTruckStockPage(
    TruckStockService* truckStockService,
    QWidget* parent
)
    : QWidget(parent),
    truckStockService(truckStockService)
{
    ui.setupUi(this);

    applyTheme(Theme::AppTheme::Dark);

    setupTable();
    setupConnections();
    loadStock();
}

MyTruckStockPage::~MyTruckStockPage()
{
}

void MyTruckStockPage::setupConnections()
{
    connect(
        ui.searchInput,
        &QLineEdit::textChanged,
        this,
        &MyTruckStockPage::onSearchChanged
    );

    connect(
        ui.exportCsvButton,
        &QPushButton::clicked,
        this,
        &MyTruckStockPage::onExportCsvClicked
    );
}

void MyTruckStockPage::setupTable()
{
    ui.truckStockTable->setColumnCount(6);

    ui.truckStockTable->setHorizontalHeaderLabels(
        QStringList()
        << "Item"
        << "Category"
        << "Current Qty"
        << "Minimum Qty"
        << "Status"
        << "Actions"
    );

    ui.truckStockTable->verticalHeader()->setVisible(false);
    ui.truckStockTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.truckStockTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.truckStockTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.truckStockTable->setShowGrid(false);
    ui.truckStockTable->setFrameShape(QFrame::NoFrame);
    ui.truckStockTable->setFocusPolicy(Qt::NoFocus);
    ui.truckStockTable->setAlternatingRowColors(false);
    ui.truckStockTable->viewport()->setAutoFillBackground(false);

    ui.truckStockTable->horizontalHeader()->setHighlightSections(false);
    ui.truckStockTable->horizontalHeader()->setDefaultAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );

    ui.truckStockTable->horizontalHeader()->setFixedHeight(48);
    ui.truckStockTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.truckStockTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);

    ui.truckStockTable->setColumnWidth(5, 190);
    ui.truckStockTable->verticalHeader()->setDefaultSectionSize(52);

    ui.truckStockTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.truckStockTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void MyTruckStockPage::loadStock()
{
    ui.truckStockTable->clearContents();
    ui.truckStockTable->clearSpans();

    if (!truckStockService) {
        currentStock.items.clear();
        filteredItems.clear();

        populateTable();
        return;
    }

    QPointer<MyTruckStockPage> self(this);

    QThread* worker = QThread::create([self]() {
        if (!self) {
            return;
        }

        auto stock = self->truckStockService->getMyTruckStock();

        QMetaObject::invokeMethod(self, [self, stock]() {
            if (!self) {
                return;
            }

            self->currentStock = stock;
            self->filterStock();
        }, Qt::QueuedConnection);
    });

    connect(worker, &QThread::finished, worker, &QObject::deleteLater);
    worker->start();
}

void MyTruckStockPage::refreshStock()
{
    loadStock();
}

void MyTruckStockPage::filterStock()
{
    QString searchText =
        ui.searchInput->text().trimmed();

    filteredItems.clear();

    for (const MyTruckStockItemDto& item : currentStock.items) {
        QString productName =
            QString::fromStdString(item.productName);

        QString category =
            QString::fromStdString(item.category);

        QString status =
            QString::fromStdString(item.status);

        bool matches =
            searchText.isEmpty() ||
            productName.contains(searchText, Qt::CaseInsensitive) ||
            category.contains(searchText, Qt::CaseInsensitive) ||
            status.contains(searchText, Qt::CaseInsensitive) ||
            QString::number(item.currentQuantity).contains(searchText) ||
            QString::number(item.minimumQuantity).contains(searchText);

        if (matches) {
            filteredItems.push_back(item);
        }
    }

    populateTable();
}

void MyTruckStockPage::populateTable()
{
    ui.truckStockTable->clearContents();
    ui.truckStockTable->clearSpans();

    int rowCount =
        static_cast<int>(filteredItems.size());

    if (rowCount == 0) {
        ui.truckStockTable->setRowCount(1);

        ui.truckStockTable->setItem(
            0,
            0,
            new QTableWidgetItem("No truck stock items found")
        );

        ui.truckStockTable->setSpan(0, 0, 1, 6);

        return;
    }

    ui.truckStockTable->setRowCount(rowCount);

    for (int row = 0; row < rowCount; row++) {
        const MyTruckStockItemDto& item =
            filteredItems[row];

        ui.truckStockTable->setItem(
            row,
            0,
            new QTableWidgetItem(QString::fromStdString(item.productName))
        );

        ui.truckStockTable->setItem(
            row,
            1,
            new QTableWidgetItem(QString::fromStdString(item.category))
        );

        ui.truckStockTable->setItem(
            row,
            2,
            new QTableWidgetItem(QString::number(item.currentQuantity))
        );

        ui.truckStockTable->setItem(
            row,
            3,
            new QTableWidgetItem(QString::number(item.minimumQuantity))
        );

        ui.truckStockTable->setItem(
            row,
            4,
            new QTableWidgetItem(QString::fromStdString(item.status))
        );

        addActionButtons(row);
    }
}

void MyTruckStockPage::onSearchChanged(const QString& text)
{
    Q_UNUSED(text);

    filterStock();
}

void MyTruckStockPage::addActionButtons(
    int row
)
{
    QWidget* actionWidget =
        new QWidget(this);

    actionWidget->setObjectName("actionContainer");

    QHBoxLayout* layout =
        new QHBoxLayout(actionWidget);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    layout->setAlignment(Qt::AlignCenter);

    QPushButton* useButton =
        new QPushButton("Use", actionWidget);

    useButton->setObjectName("useButton");

    QPushButton* historyButton =
        new QPushButton("History", actionWidget);

    historyButton->setObjectName("historyButton");

    layout->addWidget(useButton);
    layout->addWidget(historyButton);

    ui.truckStockTable->setCellWidget(row, 5, actionWidget);

    connect(
        useButton,
        &QPushButton::clicked,
        this,
        [this, row]() {
            onUseItemClicked(row);
        }
    );

    connect(
        historyButton,
        &QPushButton::clicked,
        this,
        [this, row]() {
            onHistoryClicked(row);
        }
    );
}

void MyTruckStockPage::onUseItemClicked(
    int row
)
{
    if (row < 0 || row >= static_cast<int>(filteredItems.size())) {
        return;
    }

    if (!truckStockService) {
        QMessageBox::warning(
            this,
            "Error",
            "Truck stock service is unavailable."
        );

        return;
    }

    const MyTruckStockItemDto& item =
        filteredItems[row];

    UseTruckItemDialog dialog(this);

    dialog.setItem(
        QString::fromStdString(item.productName),
        item.currentQuantity
    );

    if (dialog.exec() == QDialog::Accepted) {
        UseTruckItemRequest request;

        request.itemId =
            item.id;

        request.quantityUsed =
            dialog.getQuantityToUse();

        request.notes =
            dialog.getNotes().toStdString();

        bool success =
            truckStockService->useTruckItem(request);

        if (success) {
            QMessageBox::information(
                this,
                "Success",
                "Item usage recorded successfully."
            );

            emit stockChanged();
            loadStock();
        }
        else {
            QMessageBox::warning(
                this,
                "Error",
                "Failed to use item. Check backend console."
            );
        }
    }
}

void MyTruckStockPage::onHistoryClicked(
    int row
)
{
    if (row < 0 || row >= static_cast<int>(filteredItems.size())) {
        return;
    }

    if (!truckStockService) {
        QMessageBox::warning(
            this,
            "Error",
            "Truck stock service is unavailable."
        );

        return;
    }

    const MyTruckStockItemDto& item =
        filteredItems[row];

    std::vector<TruckStockMovementDto> movements =
        truckStockService->getMovements();

    TruckStockHistoryDialog dialog(
        item,
        movements,
        this
    );

    dialog.exec();
}

void MyTruckStockPage::onExportCsvClicked()
{
    if (filteredItems.empty()) {
        QMessageBox::information(
            this,
            "Export CSV",
            "There is no truck stock data to export."
        );

        return;
    }

    QString filePath =
        QFileDialog::getSaveFileName(
            this,
            "Export Truck Stock CSV",
            "my_truck_stock.csv",
            "CSV Files (*.csv)"
        );

    if (filePath.isEmpty()) {
        return;
    }

    if (!filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        filePath += ".csv";
    }

    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(
            this,
            "Export Failed",
            "Could not create the CSV file."
        );

        return;
    }

    QTextStream out(&file);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#endif

    out
        << "Truck,"
        << escapeCsvField(QString::fromStdString(currentStock.truckNumber))
        << "\n";

    out
        << "Plate,"
        << escapeCsvField(QString::fromStdString(currentStock.plateNumber))
        << "\n\n";

    out
        << "Item,"
        << "Category,"
        << "Current Qty,"
        << "Minimum Qty,"
        << "Required Qty,"
        << "Status\n";

    for (const MyTruckStockItemDto& item : filteredItems) {
        out
            << escapeCsvField(QString::fromStdString(item.productName)) << ","
            << escapeCsvField(QString::fromStdString(item.category)) << ","
            << item.currentQuantity << ","
            << item.minimumQuantity << ","
            << item.requiredQuantity << ","
            << escapeCsvField(QString::fromStdString(item.status)) << "\n";
    }

    file.close();

    QMessageBox::information(
        this,
        "Export Complete",
        "Truck stock exported successfully."
    );
}

QString MyTruckStockPage::escapeCsvField(
    const QString& value
) const
{
    QString escaped =
        value;

    escaped.replace("\"", "\"\"");

    if (
        escaped.contains(",") ||
        escaped.contains("\"") ||
        escaped.contains("\n") ||
        escaped.contains("\r")
        ) {
        escaped =
            "\"" + escaped + "\"";
    }

    return escaped;
}

void MyTruckStockPage::applyTheme(
    Theme::AppTheme theme
)
{
    setStyleSheet(
        Theme::myTruckStockPageStyle(theme)
    );
}

void MyTruckStockPage::setSearchText(const QString& text)
{
    ui.searchInput->setText(text);
}