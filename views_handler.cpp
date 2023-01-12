#include "views_handler.h"

#include <QTextStream>

QString ViewsHandler::getSavedPath()
{
    QFile configFile("config.txt");
    return getFileContent(configFile);
}

void ViewsHandler::initModels()
{
    modelTree.setRootPath(getSavedPath());
    modelTree.setFilter(QDir::NoDotAndDotDot|QDir::AllEntries);
}

void ViewsHandler::initViews(Ui::MainWindow &ui)
{
    initTreeView(ui);
    viewTree = ui.uiTreeView;
    viewText = ui.uiTextView;
}

void ViewsHandler::initTreeView(Ui::MainWindow &ui)
{
    ui.uiTreeView->setModel(&modelTree);
    ui.uiTreeView->setColumnHidden(1,true);
    ui.uiTreeView->setHeaderHidden(true);
    ui.uiTreeView->setRootIndex(modelTree.index(getSavedPath()));
    ui.uiTreeView->setRootIsDecorated(false);
    for(int column = 1; column < modelTree.columnCount(); column ++)
    {
        ui.uiTreeView->setColumnHidden(column,true);
    }
}

void ViewsHandler::initConnection(Ui::MainWindow &ui)
{
    QObject::connect(ui.uiTreeView, SIGNAL(pressed(QModelIndex)),
                      this, SLOT(fileDisplay(QModelIndex)));

    QObject::connect(ui.uiTextView,SIGNAL(textChanged()),
                     this, SLOT(fileSave()));
}

QString ViewsHandler::getFileContent(QFile& file)
{
    QString content;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
            while (!stream.atEnd())
            {
                QString line = stream.readLine();
                content.append(line);
            }
    }
    file.close();
    return content;
}

void ViewsHandler::fileDisplay(const QModelIndex& index)
{
    if(viewTree->isExpanded(index))
    {
        viewTree->collapse(index);
    }
    else
    {
        viewTree->expand(index);
    }

    viewText->clear();
    fileInfo = modelTree.fileInfo(index);
    QSharedPointer<QFile> file = QSharedPointer<QFile>(new QFile(fileInfo.absoluteFilePath()));
    QString fullContent = getFileContent(*file.get());
    viewText->setText(fullContent);
    viewText->update();
}

void ViewsHandler::fileSave()
{
    if(!viewText->hasFocus())
        return;

    QString fullContent = viewText->toMarkdown();
    QFile file(fileInfo.absoluteFilePath());
    if(file.open(QFile::WriteOnly))
    {
        QTextStream stream(&file);
        stream<<fullContent;
        file.close();
    }
}
