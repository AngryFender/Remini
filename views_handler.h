#ifndef VIEWS_HANDLER_H
#define VIEWS_HANDLER_H
#include <MkTextDocument.h>
#include <QSharedPointer>
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <mkedit.h>
#include <navigationview.h>
#include <navigationmodel.h>
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

public slots:
    void startSearchAllHandle();

private:
    ViewsHandler(QWidget*parent,Ui::MainWindow &ui){
        this->parent = parent;
        searchAllView = new SearchAllDialog(this->parent);
        initModels();
        initViews(ui);
        initConnection();
    }

    QWidget * parent;
    SearchAllDialog * searchAllView;
    NavigationModel modelTree;
    MkEdit* viewText;
    QLabel* viewTitle;
    NavigationView* viewTree;
    QFileInfo fileInfo;
    QDialog fileDeleteConfirm;

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
    void fileDelete(QModelIndex& index);

private slots:
    void fileDisplay(const QModelIndex& index);
    void fileSaveHandle();
    void fileDeleteDialogue(QModelIndex& index);
};

#endif // VIEWS_HANDLER_H
