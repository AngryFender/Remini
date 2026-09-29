#include "textsearchworker.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardItem>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextStream>

// Maximum size of a file that will be scanned. Files larger than this are
// skipped so a single huge file cannot exhaust memory.
static constexpr qint64 MAX_SCAN_FILE_SIZE = 50LL * 1024 * 1024;  // 50 MB

TextSearchWorker::TextSearchWorker(QObject *parent) : QObject(parent) {}

QStringList &TextSearchWorker::getListPaths() { return listPaths; }

void TextSearchWorker::setText(const QString &text) { this->text = text; }

void TextSearchWorker::setRootPath(QString rootPath)
{
    this->rootPath = rootPath;
}

void TextSearchWorker::doWork()
{
    model.clear();

    // An empty search string would loop forever inside findAllMatches
    // and would create one item per character position in every file.
    if (this->text.isEmpty())
    {
        emit updateTextSearchView(&model, 0);
        emit finished();
        return;
    }

    QDir dir(rootPath);
    int matchCount = 0;

    for (const QString &path : std::as_const(listPaths))
    {
        QFileInfo fileInfo(path);

        // Skip files that are too large to scan safely.
        if (fileInfo.size() > MAX_SCAN_FILE_SIZE)
            continue;

        // NOTE: QFileIconProvider / QIcon may touch the desktop environment
        // on Linux (KDE / GNOME / xdg-desktop-portal). If this worker runs in
        // a non-GUI thread, the icon request may be ignored or block. If that
        // becomes a problem, move the icon lookup to the GUI thread.
        const QIcon fileIcon = iconProvider.icon(fileInfo);
        const QString displayPath = dir.relativeFilePath(path);

        auto *fileItem = new QStandardItem();
        fileItem->setIcon(fileIcon);
        fileItem->setData(path, Qt::UserRole);

        QFile file(path);
        QTextDocument document;
        int childRow = 0;

        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            const QString content = stream.readAll();
            document.setPlainText(content);
            file.close();
        }

        findAllMatches(matchCount, fileItem, childRow, document, 0, this->text);
        fileItem->setData(displayPath + " (" + QString::number(childRow) + ")",
                          Qt::DisplayRole);

        if (!fileItem->hasChildren())
            delete fileItem;
        else
            model.appendRow(fileItem);
    }

    emit updateTextSearchView(&model, matchCount);
    emit finished();
}

// ---------------------------------------------------------------------------
// Iterative search: a recursive implementation would overflow the stack on
// files that contain many occurrences of a common word.
// ---------------------------------------------------------------------------
void TextSearchWorker::findAllMatches(int &matchCount,
                                      QStandardItem *parentItem, int &row,
                                      QTextDocument &document,
                                      int startPosition,
                                      const QString &searchText)
{
    if (searchText.isEmpty())
        return;

    int pos = startPosition;
    while (true)
    {
        const QTextCursor foundCursor = document.find(searchText, pos);
        if (foundCursor.isNull())
            break;

        const int positionInFile = foundCursor.position();
        const int blockNumber = foundCursor.blockNumber();
        const int positionInBlock = foundCursor.positionInBlock();

        auto *positionItem = new QStandardItem(QString::number(positionInFile));
        positionItem->setFlags(Qt::ItemNeverHasChildren | Qt::ItemIsSelectable |
                               Qt::ItemIsEnabled);
        positionItem->setData(extractNeighbourWords(document, positionInFile),
                              Qt::DisplayRole);
        positionItem->setData(searchText.length(), Qt::UserRole);
        positionItem->setData(blockNumber, Qt::UserRole + 1);
        positionItem->setData(positionInBlock, Qt::UserRole + 2);

        parentItem->setChild(row, 0, positionItem);
        matchCount++;
        row++;

        // Advance past the matched text so the next find() does not return
        // the same match again.
        pos = positionInFile + searchText.length();
    }
}

QString TextSearchWorker::extractNeighbourWords(QTextDocument &document,
                                                int position)
{
    QTextCursor cursor(&document);
    cursor.setPosition(position, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

    return cursor.selectedText().trimmed();
}
