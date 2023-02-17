
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

    initFontDefault();
    initTreeView();

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

void ViewsHandler::initFontDefault()
{
    fontUi.setFamily("roboto");
    fontUi.setStretch(QFont::Unstretched);
    fontUi.setWeight(QFont::Normal);
    fontUi.setPointSize(10);
    viewText->setFont(fontUi);
    viewTree->setFont(fontUi);

    QFont fontTitle;
    fontTitle.setFamily(fontUi.family());
    fontTitle.setPointSize(fontUi.pointSize()*2);
    fontTitle.setWeight(QFont::DemiBold);

    viewTitle->setFont(fontTitle);
    viewTitle->setStyleSheet("QLabel { padding-left:10; padding-right:10}");
}

void ViewsHandler::initConnection()
{
    QObject::connect(viewTree, SIGNAL(pressed(QModelIndex)),
                      this, SLOT(fileDisplay(QModelIndex)));

    QObject::connect(viewText,SIGNAL(fileSave()),
                     this, SLOT(fileSaveHandle()));

    QObject::connect(viewText,SIGNAL(cursorPosChanged(bool,int)),
                     &mkGuiDocument,SLOT(cursorPosChangedHandle(bool,int)));

    QObject::connect(viewText,SIGNAL(enterKeyPressed(int)),
                     &mkGuiDocument,SLOT(enterKeyPressedHandle(int)));

    QObject::connect(viewText,SIGNAL(quoteLeftKeyPressed(int,bool&)),
                     &mkGuiDocument,SLOT(quoteLeftKeyPressedHandle(int,bool&)));

    QObject::connect(viewText,SIGNAL(removeAllMkData()),
                     &mkGuiDocument,SLOT(removeAllMkDataHandle()));

    QObject::connect(viewText,SIGNAL(applyAllMkData(bool,int)),
                     &mkGuiDocument,SLOT(applyAllMkDataHandle(bool,int)));

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

    fileInfo = modelTree.fileInfo(index);
    if (!fileInfo.isFile())
        return;

    QSharedPointer<QFile> file = QSharedPointer<QFile>(new QFile(fileInfo.absoluteFilePath()));
    QString fullContent = getFileContent(*file.get());

    mkGuiDocument.clear();
    mkGuiDocument.setPlainText(fullContent);
    viewTitle->setText(fileInfo.fileName());
    viewText->update();
}

void ViewsHandler::fileSaveHandle()
{
    if(!viewText->hasFocus())
        return;

    QString fullContent = viewText->toPlainText();
    QFile file(fileInfo.absoluteFilePath());
    if(file.open(QFile::WriteOnly))
    {
        QTextStream stream(&file);
        stream<<fullContent;
        file.close();
    }
}
