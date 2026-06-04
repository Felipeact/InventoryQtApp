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
    // This requires an external library like xlsxwriter or libxlsxwriter
    // For now, we'll default to CSV format
    if (filePath.endsWith(".xlsx", Qt::CaseInsensitive)) {
        // In production, integrate xlsxwriter library here
        // For this implementation, we'll just save as CSV with xlsx extension
        // A real implementation would use:
        // - libxlsxwriter (C library)
        // - QXlsx (Qt library)
        // - xlsx-writer (Qt library)
        return exportTableToCsv(table, filePath);
    }
    return false;
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
