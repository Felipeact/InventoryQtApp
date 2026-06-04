#include "ExportUtility.h"
#include <QFile>
#include <QTextStream>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QPdfWriter>
#include <QPainter>
#include <QFileInfo>
#include <QDateTime>
#include <QPageSize>
#include <QPageLayout>

// Try to include QXlsx if available
#ifdef QXLSX_LIBRARY
#include <xlsxdocument.h>
#include <xlsxcellrange.h>
#include <xlsxformat.h>
using namespace QXlsx;
#endif

bool ExportUtility::exportTableToCsv(const QTableWidget* table, const QString& filePath)
{
    if (!table) return false;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);

    // Write headers
    for (int col = 0; col < table->columnCount(); ++col) {
        if (col > 0) out << ",";
        QTableWidgetItem* headerItem = table->horizontalHeaderItem(col);
        if (headerItem) {
            out << "\"" << headerItem->text().replace("\"", "\"\"") << "\"";
        }
    }
    out << "\n";

    // Write rows
    for (int row = 0; row < table->rowCount(); ++row) {
        for (int col = 0; col < table->columnCount(); ++col) {
            if (col > 0) out << ",";
            QTableWidgetItem* item = table->item(row, col);
            if (item) {
                out << "\"" << item->text().replace("\"", "\"\"") << "\"";
            } else {
                out << "\"\"";
            }
        }
        out << "\n";
    }

    file.close();
    return true;
}

bool ExportUtility::exportTableToExcel(const QTableWidget* table, const QString& filePath)
{
    if (!table || !filePath.endsWith(".xlsx", Qt::CaseInsensitive)) {
        return false;
    }

#ifdef QXLSX_LIBRARY
    try {
        // Create XLSX document
        Document xlsx;

        // Write headers
        for (int col = 0; col < table->columnCount(); ++col) {
            QTableWidgetItem* headerItem = table->horizontalHeaderItem(col);
            QString headerText = headerItem ? headerItem->text() : "";

            // Write to cell (1-indexed in QXlsx)
            xlsx.write(1, col + 1, headerText);

            // Apply header formatting
            Format headerFormat;
            headerFormat.setFontBold(true);
            headerFormat.setFillColor(QColor(200, 200, 200));
            xlsx.write(1, col + 1, headerText, headerFormat);
        }

        // Write data rows
        for (int row = 0; row < table->rowCount(); ++row) {
            for (int col = 0; col < table->columnCount(); ++col) {
                QTableWidgetItem* item = table->item(row, col);
                QString cellText = item ? item->text() : "";
                xlsx.write(row + 2, col + 1, cellText);
            }
        }

        // Auto-fit columns
        for (int col = 0; col < table->columnCount(); ++col) {
            xlsx.setColumnWidth(col + 1, 15);
        }

        // Save to file
        return xlsx.saveAs(filePath);
    }
    catch (const std::exception& e) {
        qWarning() << "Excel export error:" << e.what();
        // Fall back to CSV if QXlsx fails
        return exportTableToCsv(table, filePath);
    }
#else
    // QXlsx not available, fall back to CSV
    qWarning() << "QXlsx library not available, exporting as CSV instead";
    return exportTableToCsv(table, filePath);
#endif
}

bool ExportUtility::exportTableToPdf(const QTableWidget* table, const QString& filePath)
{
    if (!table) return false;

    // Create PDF writer
    QPdfWriter pdfWriter(filePath);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setPageMargins(QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);

    QPainter painter(&pdfWriter);
    if (!painter.isActive()) {
        return false;
    }

    int pageWidth = pdfWriter.width();
    int pageHeight = pdfWriter.height();
    int y = 50;
    int x = 50;

    // Draw title
    QFont titleFont("Arial", 14);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.drawText(x, y, "Inventory Report");
    y += 40;

    // Draw timestamp
    QFont normalFont("Arial", 10);
    painter.setFont(normalFont);
    painter.drawText(x, y, "Generated: " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    y += 30;

    // Draw table
    int columnWidth = (pageWidth - 100) / table->columnCount();
    int rowHeight = 20;

    // Draw headers
    painter.drawRect(x, y, pageWidth - 100, rowHeight);
    painter.setFont(QFont("Arial", 9, QFont::Bold));

    for (int col = 0; col < table->columnCount(); ++col) {
        QTableWidgetItem* headerItem = table->horizontalHeaderItem(col);
        if (headerItem) {
            painter.drawText(x + col * columnWidth + 5, y + 15, headerItem->text());
        }
    }
    y += rowHeight;

    // Draw rows
    painter.setFont(normalFont);
    for (int row = 0; row < table->rowCount() && y < pageHeight - 100; ++row) {
        for (int col = 0; col < table->columnCount(); ++col) {
            QTableWidgetItem* item = table->item(row, col);
            if (item) {
                painter.drawText(x + col * columnWidth + 5, y + 15, item->text());
            }
        }
        y += rowHeight;
    }

    painter.end();
    return true;
}

QString ExportUtility::getFileSizeString(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    qint64 size = fileInfo.size();

    if (size < 1024) {
        return QString::number(size) + " B";
    } else if (size < 1024 * 1024) {
        return QString::number(size / 1024.0, 'f', 2) + " KB";
    } else {
        return QString::number(size / (1024.0 * 1024.0), 'f', 2) + " MB";
    }
}
