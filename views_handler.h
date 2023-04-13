#ifndef VIEWS_HANDLER_H
#define VIEWS_HANDLER_H
#include <MkTextDocument.h>
#include <QSharedPointer>
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QMutex>
#include <QThread>
#include <mkedit.h>
#include <navigationview.h>
#include <navigationmodel.h>
#include "textsearchworker.h"
#include "./ui_mainwindow.h"
#include "searchalldialog.h"

class ViewsHandler: public QObject
{
    Q_OBJECT
public:
    ViewsHandler(QWidget *parent, ViewsHandler &other) = delete;
    void operator=(const ViewsHandler&) = delete;

    static QSharedPointer<ViewsHandler> getInstance(QWidget *parent,Ui::MainWindow &ui)
    {
        static QSharedPointer<ViewsHandler> handle{new ViewsHandler(parent,ui) };
        return handle;
    }
    ~ViewsHandler(){
        delete textSearchAllView;
    }

public slots:
    void startTextSearchInAllFilesHandle();

private:
    ViewsHandler(QWidget*parent,Ui::MainWindow &ui){
        firstDirectoryLoad = true;
        this->parent = parent;
        textSearchAllView = new SearchAllDialog(this->parent);
        initViews(ui);
        initConnection();
        textSearchWorker.moveToThread(&searchThread);
    }


    QWidget *parent;
    QThread searchThread;
    SearchAllDialog *textSearchAllView;
    TextSearchWorker textSearchWorker;
    QFileSystemModel modelTree;
    NavigationProxyModel proxyModel;
    MkEdit* viewText;
    QLabel* viewTitle;
    QLineEdit* viewSearch;
    NavigationView* viewTree;
    QFileInfo fileInfo;
    QDialog fileDeleteConfirm;

    QFont fontUi;
    QString getSavedPath();
    void initViews(Ui::MainWindow &ui);
    void initTreeView();
    void initFontDefault();
    void initConnection();
    QString getFileContent(QFile& file);
    MkTextDocument mkGuiDocument;
    Highlighter highlighter;
    QString searchedFilename;
    QMutex fileSearchMutex;
    bool firstDirectoryLoad;
signals:
    void load_text(QString text);
    void clear_text();
    void fileDelete(QModelIndex& index);

private slots:
    void fileDisplay(const QModelIndex& index);
    void fileSaveHandle();
    void fileDeleteDialogue(QModelIndex& index);
    void searchFileHandle(const QString &filename);
    void navigationAllPathLoaded(QString path);
    void navigationViewExpandedFilenameFilter();
    void doSearchWork(QString &text);
    void displayTextSearchedFilePosition(QString &filePath,int searchTextLength,int blockNumber, int positionInBlock);

};

#endif // VIEWS_HANDLER_H
