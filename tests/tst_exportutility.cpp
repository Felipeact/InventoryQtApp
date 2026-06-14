// Tests for ExportUtility: CSV generation/escaping, the Excel-without-QXlsx
// fallback, and human-readable file sizes.
#include <QtTest>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>

#include "ExportUtility.h"

namespace
{
    QTableWidget* makeTable()
    {
        auto* table = new QTableWidget(2, 2);
        table->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
        table->setHorizontalHeaderItem(1, new QTableWidgetItem("Qty"));
        table->setItem(0, 0, new QTableWidgetItem("Bolt"));
        table->setItem(0, 1, new QTableWidgetItem("10"));
        // Row 1 col 0 intentionally left null to test empty-cell handling.
        table->setItem(1, 1, new QTableWidgetItem(R"(say "hi")"));
        return table;
    }

    QString readAll(const QString& path)
    {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return {};
        QTextStream in(&f);
        return in.readAll();
    }
}

class TestExportUtility : public QObject
{
    Q_OBJECT

private slots:
    void csv_writesHeadersAndRows()
    {
        QTemporaryDir dir;
        const QString path = dir.filePath("out.csv");

        QScopedPointer<QTableWidget> table(makeTable());
        QVERIFY(ExportUtility::exportTableToCsv(table.data(), path));

        const QString content = readAll(path);
        QVERIFY(content.contains("\"Name\",\"Qty\""));
        QVERIFY(content.contains("\"Bolt\",\"10\""));
    }

    void csv_escapesEmbeddedQuotes()
    {
        QTemporaryDir dir;
        const QString path = dir.filePath("out.csv");

        QScopedPointer<QTableWidget> table(makeTable());
        QVERIFY(ExportUtility::exportTableToCsv(table.data(), path));

        // "say \"hi\"" must be doubled to "say ""hi""".
        QVERIFY(readAll(path).contains(R"("say ""hi""")"));
    }

    void csv_emptyCellsBecomeEmptyQuotedFields()
    {
        QTemporaryDir dir;
        const QString path = dir.filePath("out.csv");

        QScopedPointer<QTableWidget> table(makeTable());
        QVERIFY(ExportUtility::exportTableToCsv(table.data(), path));

        // The null cell at row 1, col 0 should serialize as an empty field.
        QVERIFY(readAll(path).contains("\"\","));
    }

    void csv_returnsFalseForNullTable()
    {
        QTemporaryDir dir;
        QVERIFY(!ExportUtility::exportTableToCsv(nullptr, dir.filePath("x.csv")));
    }

    void excel_withoutQXlsx_fallsBackToCsv()
    {
        QTemporaryDir dir;
        const QString path = dir.filePath("out.xlsx");

        QScopedPointer<QTableWidget> table(makeTable());
        // Without QXLSX_LIBRARY defined, this writes CSV content but reports
        // success so the user still gets a file.
        QVERIFY(ExportUtility::exportTableToExcel(table.data(), path));
        QVERIFY(QFile::exists(path));
    }

    void excel_rejectsNonXlsxPath()
    {
        QTemporaryDir dir;
        QScopedPointer<QTableWidget> table(makeTable());
        QVERIFY(!ExportUtility::exportTableToExcel(table.data(), dir.filePath("out.txt")));
    }

    void fileSize_formatsBytes()
    {
        QTemporaryDir dir;
        const QString path = dir.filePath("b.bin");
        QFile f(path);
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write(QByteArray(100, 'x'));
        f.close();
        QCOMPARE(ExportUtility::getFileSizeString(path), QString("100 B"));
    }

    void fileSize_formatsKilobytes()
    {
        QTemporaryDir dir;
        const QString path = dir.filePath("kb.bin");
        QFile f(path);
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write(QByteArray(2048, 'x'));
        f.close();
        QCOMPARE(ExportUtility::getFileSizeString(path), QString("2.00 KB"));
    }
};

QTEST_MAIN(TestExportUtility)
#include "tst_exportutility.moc"
