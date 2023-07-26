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
    viewLeftFrame = ui.uiLeftPane;

    viewSearch->setPlaceholderText("Search Files...");
    initTreeView();

    mkGuiDocument.setPlainText(startupText);
    highlighter.setDocument(&mkGuiDocument);
    viewText->setDocument(&mkGuiDocument);

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
    QObject::connect(viewText,&MkEdit::syntaxColorUpdate,
                     &highlighter,&Highlighter::syntaxColorUpdateHandler);

    QObject::connect(viewTree, &NavigationView::pressed,
                     this, &ViewsHandler::fileDisplay);

    QObject::connect(viewTree, &NavigationView::createFile,
                     &proxyModel, &NavigationProxyModel::createFileHandler);

    QObject::connect(viewTree, &NavigationView::createFolder,
                     &proxyModel, &NavigationProxyModel::createFolderHandler);

    QObject::connect(this,  &ViewsHandler::fileDelete,
                     &proxyModel, &NavigationProxyModel::deleteFileFolderHandler);

    QObject::connect(viewTree, &NavigationView::openLocation,
                     &proxyModel,&NavigationProxyModel::openLocationHandler);

    QObject::connect(viewText, &MkEdit::checkRightClockOnCodeBlock,
                     &mkGuiDocument, &MkTextDocument::checkRightClockOnCodeBlockHandle);

    QObject::connect(viewText, &MkEdit::selectBlockCopy,
                     &mkGuiDocument,&MkTextDocument::selectBlockCopyHandle);

    QObject::connect(viewText, &MkEdit::duplicateLine,
                     &mkGuiDocument,&MkTextDocument::duplicateLineHandle);

    QObject::connect(viewText, &MkEdit::smartSelection,
                     &mkGuiDocument,&MkTextDocument::smartSelectionHandle);

    QObject::connect(viewTree,&NavigationView::deleteFileFolder,
                     this, &ViewsHandler::fileDeleteDialogue);

    QObject::connect(viewTree, &NavigationView::openLocation,
                     &proxyModel,&NavigationProxyModel::openLocationHandler);

    QObject::connect(viewTree, &NavigationView::newFileCreated,
                     this, &ViewsHandler::fileDisplay);

    QObject::connect(viewText,&MkEdit::fileSave,
                     this,  &ViewsHandler::fileSaveHandle);

    QObject::connect(viewText,&MkEdit::cursorPosChanged,
                     &mkGuiDocument,&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(viewText,&MkEdit::enterKeyPressed,
                     &mkGuiDocument,&MkTextDocument::enterKeyPressedHandle);

    QObject::connect(viewText,&MkEdit::quoteLeftKeyPressed,
                     &mkGuiDocument,&MkTextDocument::quoteLeftKeyPressedHandle);

    QObject::connect(viewText,&MkEdit::removeAllMkData,
                     &mkGuiDocument,&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(viewText,&MkEdit::applyAllMkData,
                     &mkGuiDocument,&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(viewText,&MkEdit::setMarkdownStatus,
                     &mkGuiDocument,&MkTextDocument::setMarkdownHandle);

    QObject::connect(&mkGuiDocument,&MkTextDocument::clearUndoStack,
                     viewText,&MkEdit::clearUndoStackHandle);

    QObject::connect(viewText,&MkEdit::drawTextBlocks,
                     &mkGuiDocument,&MkTextDocument::drawTextBlocksHandler);

    QObject::connect(viewSearch,&QLineEdit::textChanged,
                     this,&ViewsHandler::searchFileHandle);

    QObject::connect(&modelTree,&QFileSystemModel::directoryLoaded,
                     this,&ViewsHandler::navigationAllPathLoaded);

    QObject::connect(viewTree,&NavigationView::expansionComplete,
                     this,&ViewsHandler::navigationViewExpandedFilenameFilter);

    QObject::connect(textSearchAllView, &SearchAllDialog::startSearch,
                     this,&ViewsHandler::doSearchWork);

    QObject::connect(&searchThread,&QThread::started,
                     &textSearchWorker,&TextSearchWorker::doWork);

    QObject::connect(&textSearchWorker,&TextSearchWorker::finished,
                     &searchThread, &QThread::quit);

    QObject::connect(&textSearchWorker,&TextSearchWorker::updateTextSearchView,
                     textSearchAllView, &SearchAllDialog::updateTextSearchViewHandle);

    QObject::connect(textSearchAllView,&SearchAllDialog::showSearchedTextInFile,
                     this,&ViewsHandler::displayTextSearchedFilePosition);

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
    if(textSearchAllView->isHidden()){
        proxyModel.createAllFilesList(viewTree->rootIndex(), textSearchWorker.getListPaths());

        QPoint pos =  this->parent->mapToGlobal( viewLeftFrame->pos());
        pos.setY(pos.y()-QApplication::style()->pixelMetric(QStyle::PM_TitleBarHeight)-6);
        textSearchAllView->setGeometry(viewLeftFrame->geometry());
        textSearchAllView->move(pos);
        textSearchAllView->show();
    }else{
        textSearchAllView->activateWindow();
        textSearchAllView->setFocusAtSearch();
    }
}
