#ifndef VIEWS_HANDLER_H
#define VIEWS_HANDLER_H
#include <MkTextDocument.h>
#include <QFileSystemModel>
#include <QSharedPointer>
#include <QTreeWidgetItem>
#include <mkedit.h>

#include "./ui_mainwindow.h"
#define FONT_FAMILY "Helvetica [Adobe]"

class ViewsHandler: public QObject
{
    Q_OBJECT
public:
    ViewsHandler(ViewsHandler &other) = delete;
    void operator=(const ViewsHandler&) = delete;

    static QSharedPointer<ViewsHandler> getInstance(Ui::MainWindow &ui)
    {
        static QSharedPointer<ViewsHandler> handle{new ViewsHandler(ui) };
        return handle;
    }

private:
    ViewsHandler(Ui::MainWindow &ui){
        initModels();
        initViews(ui);
        initConnection();
    }

    QFileSystemModel modelTree;
    MkEdit* viewText;
    QLabel* viewTitle;
    QTreeView* viewTree;
    QFileInfo fileInfo;

    QFont fontUi;
    QString getSavedPath();
    void initModels();
    void initViews(Ui::MainWindow &ui);
    void initTreeView();
    void initFontDefault();
    void initConnection();
    QString getFileContent(QFile& file);
    MkTextDocument mkGuiDocument;
    Highlighter highlighter;

signals:
    void load_text(QString text);
    void clear_text();

private slots:
    void fileDisplay(const QModelIndex& index);
    void fileSaveHandle();
};

#endif // VIEWS_HANDLER_H
