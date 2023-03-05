#ifndef VIEWS_HANDLER_H
#define VIEWS_HANDLER_H
#include <MkTextDocument.h>
#include <QFileSystemWatcher>
#include <QSharedPointer>
#include <QTreeWidgetItem>
#include <mkedit.h>
#include <navigationview.h>
#include <navigationmodel.h>

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

    QFileSystemWatcher modelWatcher;
    NavigationModel modelTree;
    MkEdit* viewText;
    QLabel* viewTitle;
    NavigationView* viewTree;
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
    void uniqueName(QString &fullName);

signals:
    void load_text(QString text);
    void clear_text();

private slots:
    void fileDisplay(const QModelIndex& index);
    void fileSaveHandle();

//    void manageFilesHandle();
};

#endif // VIEWS_HANDLER_H
