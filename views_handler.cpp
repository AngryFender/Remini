#include "views_handler.h"

QString ViewsHandler::getSavedPath()
{
    QFile configFile("config.txt");
    return getFileContent(configFile);
}

void ViewsHandler::initViews(Ui::MainWindow &ui)
{
    QList<int> sizes;
    sizes << NAVIGATION_RATIO << EDITOR_RAIO;
    ui.splitter->setSizes(sizes);
    ui.splitter->setCollapsible(1,false);

    viewSearch = ui.uiSearch;
    viewTree = ui.uiTreeView;
    viewText = ui.uiTextView;
    viewTitle = ui.uiTitle;


    viewSearch->setPlaceholderText("Search Files...");
    initTreeView();

    viewText->setDocument(&mkGuiDocument);
    highlighter.setDocument(&mkGuiDocument);

    initFontDefault();
}

void ViewsHandler::initTreeView()
{
    modelTree.setReadOnly(false);
    modelTree.setFilter(QDir::NoDotAndDotDot|QDir::AllEntries);
    modelTree.setRootPath(getSavedPath());

    proxyModel.setSourceModel(&modelTree);
    viewTree->setModel(&proxyModel);
    viewTree->setRootIndex(proxyModel.setRootIndexFromPath(getSavedPath()));

    for(int column = 1; column < proxyModel.columnCount(); column ++)
    {
        viewTree->setColumnHidden(column,true);
    }
}

void ViewsHandler::initFontDefault()
{
    fontUi.setFamily("roboto");
    fontUi.setStretch(QFont::Unstretched);
    fontUi.setWeight(QFont::Normal);
    fontUi.setPointSize(13);
    viewText->setFont(fontUi);

    QFont fontTree;
    fontTree.setFamily("roboto");
    fontTree.setPointSize(10);
    fontTree.setWeight(QFont::Light);
    viewTree->setFont(fontTree);

    QFont fontTitle;
    fontTitle.setFamily("roboto");
    fontTitle.setPointSize(fontUi.pointSize()*2);
    fontTitle.setWeight(QFont::DemiBold);

    viewTitle->setFont(fontTitle);
}

void ViewsHandler::initConnection()
{
    QObject::connect(viewText,SIGNAL(syntaxColorUpdate(HighlightColor&)),
                     &highlighter,SLOT(syntaxColorUpdateHandler(HighlightColor&)));

    QObject::connect(viewTree, SIGNAL(pressed(QModelIndex)),
                      this, SLOT(fileDisplay(QModelIndex)));

    QObject::connect(viewTree, SIGNAL(createFile(QModelIndex&,QString&)),
                     &proxyModel, SLOT(createFileHandler(QModelIndex&,QString&)));

    QObject::connect(viewTree, SIGNAL(createFolder(QModelIndex&,QString&)),
                     &proxyModel, SLOT(createFolderHandler(QModelIndex&,QString&)));

    QObject::connect(this, SIGNAL(fileDelete(QModelIndex&)),
                     &proxyModel, SLOT(deleteFileFolderHandler(QModelIndex&)));

    QObject::connect(viewTree, SIGNAL(openLocation(QModelIndex&)),
                     &proxyModel, SLOT(openLocationHandler(QModelIndex&)));

    QObject::connect(viewText, SIGNAL(checkRightClockOnCodeBlock(int,bool&)),
                     &mkGuiDocument, SLOT(checkRightClockOnCodeBlockHandle(int,bool&)));

    QObject::connect(viewText, SIGNAL(selectBlockCopy(int,int&,int&)),
                     &mkGuiDocument, SLOT(selectBlockCopyHandle(int,int&,int&)));

    QObject::connect(viewText, SIGNAL(duplicateLine(int)),
                     &mkGuiDocument, SLOT(duplicateLineHandle(int)));

    QObject::connect(viewText, SIGNAL(smartSelection(int,QTextCursor&)),
                     &mkGuiDocument, SLOT(smartSelectionHandle(int,QTextCursor&)));

    QObject::connect(viewTree, SIGNAL(deleteFileFolder(QModelIndex&)),
                     this, SLOT(fileDeleteDialogue(QModelIndex&)));

    QObject::connect(viewTree, SIGNAL(openLocation(QModelIndex&)),
                     &proxyModel, SLOT(openLocationHandler(QModelIndex&)));

    QObject::connect(viewTree, SIGNAL(newFileCreated(QModelIndex)),
                     this, SLOT(fileDisplay(QModelIndex)));

    QObject::connect(viewText,SIGNAL(fileSave()),
                     this, SLOT(fileSaveHandle()));

    QObject::connect(viewText,SIGNAL(cursorPosChanged(bool,int,QRect)),
                     &mkGuiDocument,SLOT(cursorPosChangedHandle(bool,int,QRect)));

    QObject::connect(viewText,SIGNAL(enterKeyPressed(int)),
                     &mkGuiDocument,SLOT(enterKeyPressedHandle(int)));

    QObject::connect(viewText,SIGNAL(quoteLeftKeyPressed(int,bool&)),
                     &mkGuiDocument,SLOT(quoteLeftKeyPressedHandle(int,bool&)));

    QObject::connect(viewText,&MkEdit::removeAllMkData,
                     &mkGuiDocument,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(viewText,SIGNAL(applyAllMkData(bool,int,bool,QRect)),
                     &mkGuiDocument,SLOT(applyAllMkDataHandle(bool,int,bool,QRect)));

    QObject::connect(&mkGuiDocument,SIGNAL(clearUndoStack()),
                     viewText,SLOT(clearUndoStackHandle()));

    QObject::connect(viewText,SIGNAL(drawTextBlocks(bool,int,bool,QRect)),
                     &mkGuiDocument,SLOT(drawTextBlocksHandler(bool,int,bool,QRect)));

    QObject::connect(viewSearch,SIGNAL(textChanged(QString)),
                     this,SLOT(searchFileHandle(QString)));

    QObject::connect(&modelTree,SIGNAL(directoryLoaded(QString)),
                     this,SLOT(navigationAllPathLoaded(QString)));

    QObject::connect(viewTree,SIGNAL(expansionComplete()),
                     this,SLOT(navigationViewExpandedFilenameFilter()));

    QObject::connect(textSearchAllView, SIGNAL(startSearch(QString&)),
                     this,SLOT(doSearchWork(QString&)));

    QObject::connect(&searchThread, SIGNAL(started()),
                     &textSearchWorker,SLOT(doWork()));

    QObject::connect(&textSearchWorker,SIGNAL(finished()),
                     &searchThread, SLOT(quit()));

    QObject::connect(&textSearchWorker,SIGNAL(updateTextSearchView(QStandardItemModel*,int)),
                     textSearchAllView, SLOT(updateTextSearchViewHandle(QStandardItemModel*,int)));

    QObject::connect(textSearchAllView,SIGNAL(showSearchedTextInFile(QString&,int,int,int)),
                     this,SLOT(displayTextSearchedFilePosition(QString&,int,int,int)));

    QObject::connect(viewText,&MkEdit::pushCheckBox,
                     &mkGuiDocument,&MkTextDocument::pushCheckBoxHandle);

    QObject::connect(viewText,&MkEdit::pushLink,
                     &mkGuiDocument,&MkTextDocument::pushLinkHandle);

    QObject::connect(viewText,&MkEdit::autoInsertSymbol,
                     &mkGuiDocument,&MkTextDocument::autoInsertSymobolHandle);

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
    QModelIndex sourceIndex = proxyModel.mapToSource(index);
    fileInfo = modelTree.fileInfo(sourceIndex);
    if (!fileInfo.isFile()|| !fileInfo.exists())
        return;

    QSharedPointer<QFile> file = QSharedPointer<QFile>(new QFile(fileInfo.absoluteFilePath()));
    QString fullContent = getFileContent(*file.get());

    mkGuiDocument.clear();
    mkGuiDocument.setPlainText(fullContent);

    viewTitle->setText(fileInfo.baseName());
    viewText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    viewText->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    viewText->update();

    viewText->initialialCursorPosition();
    viewText->verticalScrollBar()->setSliderPosition(0);
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

void ViewsHandler::fileDeleteDialogue(QModelIndex &index)
{
    if(parent == nullptr){
        return;
    }

    QFileInfo info = proxyModel.getFileInfoMappedToSource(index);
    if(info.isDir()){
        QDir dir(info.absoluteFilePath());
        dir.setFilter( QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot );
        if(dir.count()){
            QScopedPointer<QMessageBox> confirmBox(new QMessageBox(parent));
            confirmBox->setWindowTitle("Unable to Delete");
            confirmBox->setText("The folder is not empty           ");
            confirmBox->setStandardButtons(QMessageBox::Ok);
            confirmBox->exec();
            return;
        }
    }

    QScopedPointer<QMessageBox> confirmBox(new QMessageBox(parent));
    confirmBox->setStyleSheet(parent->styleSheet());
    confirmBox->setWindowTitle("Delete");
    confirmBox->setText("Are you sure you want to delete");
    confirmBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox->setDefaultButton(QMessageBox::No);
    if(QMessageBox::Yes == confirmBox->exec()){
        emit fileDelete(index);
    }
}

void ViewsHandler::searchFileHandle(const QString &filename)
{
    if(filename.isEmpty()){
        fileSearchMutex.lock();
        proxyModel.setFilterRegularExpression("");
        searchedFilename = "";
        fileSearchMutex.unlock();
        return;
    }
    searchedFilename =filename.toLower();

    QStringList listPath;
    proxyModel.createAllFoldersList(viewTree->rootIndex(), listPath);
    for(QString &filePath:listPath){
        viewTree->setExpanded(proxyModel.mapFromSource(modelTree.index(filePath)),true);
    }
    //This will call a timer as delay to let all folders expand
    viewTree->expandEveryItems(viewTree->rootIndex());
}

void ViewsHandler::navigationAllPathLoaded(QString path)
{
//    qDebug()<<" all path loaded"<<path;
}

void ViewsHandler::navigationViewExpandedFilenameFilter()
{
    fileSearchMutex.lock();
    proxyModel.setFilterRegularExpression(searchedFilename);
    fileSearchMutex.unlock();
}

void ViewsHandler::doSearchWork(QString &text)
{
    textSearchWorker.setText(text);
    textSearchWorker.setRootPath(modelTree.rootPath());
    searchThread.start();
}

void ViewsHandler::displayTextSearchedFilePosition(QString &filePath,int searchTextLength,int blockNumber, int positionInBlock)
{
    fileInfo = QFileInfo(filePath);
    if (!fileInfo.isFile()|| !fileInfo.exists())
        return;

    QSharedPointer<QFile> file = QSharedPointer<QFile>(new QFile(fileInfo.absoluteFilePath()));
    QString fullContent = getFileContent(*file.get());

    mkGuiDocument.clear();
    mkGuiDocument.setPlainText(fullContent);

    viewTitle->setText(fileInfo.baseName());
    viewText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    viewText->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    viewText->update();

    QTextCursor cursor = viewText->textCursor();
    QTextBlock block = mkGuiDocument.findBlockByNumber(blockNumber);

    cursor.setPosition(block.position());
    viewText->setTextCursor(cursor);

    cursor.setPosition(block.position()+positionInBlock);
    cursor.setPosition(block.position()+positionInBlock-searchTextLength,QTextCursor::KeepAnchor);
    viewText->setTextCursor(cursor);

}

void ViewsHandler::startTextSearchInAllFilesHandle()
{
    textSearchAllView->show();
    proxyModel.createAllFilesList(viewTree->rootIndex(), textSearchWorker.getListPaths());
}
