
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
    viewTree = ui.uiTreeView;
    viewText = ui.uiTextView;
    viewTitle = ui.uiTitle;

    initTreeView();
    initTitleView();

    viewText->setAttribute(Qt::WA_OpaquePaintEvent,false);
    viewText->setDocument(&mkGuiDocument);
    highlighter.setDocument(&mkGuiDocument);
}

void ViewsHandler::initTreeView()
{
    viewTree->setModel(&modelTree);
    viewTree->setColumnHidden(1,true);
    viewTree->setHeaderHidden(true);
    viewTree->setRootIndex(modelTree.index(getSavedPath()));
    viewTree->setRootIsDecorated(false);
    for(int column = 1; column < modelTree.columnCount(); column ++)
    {
        viewTree->setColumnHidden(column,true);
    }
}

void ViewsHandler::initTitleView()
{
    QFont font(FONT_FAMILY);
    font.setPointSize( 18 );
    font.setWeight( QFont::Bold );
    viewTitle->setFont( font );
    viewTitle->setStyleSheet("QLabel { padding-left:10; padding-right:10}");
}

void ViewsHandler::initConnection()
{
    QObject::connect(viewTree, SIGNAL(pressed(QModelIndex)),
                      this, SLOT(fileDisplay(QModelIndex)));

    QObject::connect(viewText,SIGNAL(textChanged()),
                     this, SLOT(fileSave()));

    QObject::connect(viewText,SIGNAL(sendUpdateMkGui( QTextDocument* , int)),
                     &mkGuiDocument,SLOT(updateMkGui( QTextDocument* , int)));

    QObject::connect(viewText,SIGNAL(keyEnterPressed(int)),
                     &mkGuiDocument,SLOT(KeyEnterPressedHandle(int )));


}

QString ViewsHandler::getFileContent(QFile& file)
{
    QString content;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        content = stream.readAll();

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

    //viewText->clear();
    fileInfo = modelTree.fileInfo(index);
    if (!fileInfo.isFile())
        return;

    QSharedPointer<QFile> file = QSharedPointer<QFile>(new QFile(fileInfo.absoluteFilePath()));
    QString fullContent = getFileContent(*file.get());

    mkGuiDocument.clear();
    mkGuiDocument.setPlainText(fullContent);
    //mkDocument.duplicateMkTextDoc(&mkGuiDocument);
    //viewText->setText(fullContent);
    viewTitle->setText(fileInfo.fileName());
    viewText->update();
}

void ViewsHandler::fileSave()
{
//    if(!viewText->hasFocus())
//        return;

//    QString fullContent = viewText->toPlainText();
//    //QString fullContent = mkGuiDocument.toPlainText();
//    QFile file(fileInfo.absoluteFilePath());
//    if(file.open(QFile::WriteOnly))
//    {
//        QTextStream stream(&file);
//        stream<<fullContent;
//        file.close();
//    }
}
