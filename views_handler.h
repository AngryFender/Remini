#ifndef VIEWS_HANDLER_H
#define VIEWS_HANDLER_H
#include <QFileSystemModel>
#include <QSharedPointer>
#include <QTreeWidgetItem>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

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
        initConnection(ui);
    }

    QFileSystemModel modelTree;
    QTextEdit* viewText;
    QFileInfo fileInfo;

    QString getSavedPath();
    void initModels();
    void initViews(Ui::MainWindow &ui);
    void initTreeView(Ui::MainWindow &ui);
    void initConnection(Ui::MainWindow &ui);
    QString getFileContent(QFile& file);

signals:
    void load_text(QString text);
    void clear_text();

private slots:
    void fileDisplay(const QModelIndex& index);
    void fileSave();
};

#endif // VIEWS_HANDLER_H
