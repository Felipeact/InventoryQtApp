#pragma once

#include <QString>
#include <QTableWidget>

class ExportUtility
{
public:
    // Export table to CSV format
    static bool exportTableToCsv(const QTableWidget* table, const QString& filePath);

    // Export table to Excel format (requires external library)
    static bool exportTableToExcel(const QTableWidget* table, const QString& filePath);

    // Export table to PDF format (requires QPdfWriter)
    static bool exportTableToPdf(const QTableWidget* table, const QString& filePath);

    // Get file size as human-readable string
    static QString getFileSizeString(const QString& filePath);
};
