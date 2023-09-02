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
    viewRightFrame = ui.uiRightPane;

    viewSearch->setPlaceholderText("Search Files...");
    initTreeView();

    recentFileDocumentMap.insert("startup", QSharedPointer<MkTextDocument>(new MkTextDocument()));
    currentdocument = recentFileDocumentMap.value("startup");
    currentdocument->setPlainText(startupText);
    highlighter.setDocument(currentdocument.data());
    viewText->setDocument(currentdocument.data());

    initFontDefault();
}

void ViewsHandler::initTreeView()
{
    modelTree.setReadOnly(false);
    modelTree.setFilter(QDir::NoDotAndDotDot|QDir::AllEntries);
    modelTree.setRootPath(getSavedPath());
    vaultPath = modelTree.rootPath();

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

    QObject::connect(viewTree,&NavigationView::deleteFileFolder,
                     this, &ViewsHandler::fileDeleteDialogue);

    QObject::connect(viewTree, &NavigationView::openLocation,
                     &proxyModel,&NavigationProxyModel::openLocationHandler);

    QObject::connect(viewTree, &NavigationView::copyFolderFilePath,
                     &proxyModel,&NavigationProxyModel::copyFileFolderHandler);

    QObject::connect(viewTree, &NavigationView::newFileCreated,
                     this, &ViewsHandler::fileDisplay);

    QObject::connect(viewText,&MkEdit::fileSave,
                     this,  &ViewsHandler::fileSaveHandle);

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

    QObject::connect(this,&ViewsHandler::updateRecentFile,
                     recentFilesView,&RecentFilesDialog::updateRecentFileHandle);

    QObject::connect(viewText,&MkEdit::cursorUpdate,
                     this,&ViewsHandler::cursorUpdateHandle);

    connectDocument();

}

void ViewsHandler::connectDocument()
{
    QObject::connect(viewText, &MkEdit::checkRightClockOnCodeBlock,
                     currentdocument.data(), &MkTextDocument::checkRightClockOnCodeBlockHandle);

    QObject::connect(viewText, &MkEdit::selectBlockCopy,
                     currentdocument.data(),&MkTextDocument::selectBlockCopyHandle);

    QObject::connect(viewText, &MkEdit::duplicateLine,
                     currentdocument.data(),&MkTextDocument::duplicateLineHandle);

    QObject::connect(viewText, &MkEdit::smartSelection,
                     currentdocument.data(),&MkTextDocument::smartSelectionHandle);

    QObject::connect(viewText,&MkEdit::cursorPosChanged,
                     currentdocument.data(),&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(viewText,&MkEdit::enterKeyPressed,
                     currentdocument.data(),&MkTextDocument::enterKeyPressedHandle);

    QObject::connect(viewText,&MkEdit::quoteLeftKeyPressed,
                     currentdocument.data(),&MkTextDocument::quoteLeftKeyPressedHandle);

    QObject::connect(viewText,&MkEdit::removeAllMkData,
                     currentdocument.data(),&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(viewText,&MkEdit::applyAllMkData,
                     currentdocument.data(),&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(viewText,&MkEdit::setMarkdownStatus,
                     currentdocument.data(),&MkTextDocument::setMarkdownHandle);

    QObject::connect(currentdocument.data(),&MkTextDocument::clearUndoStack,
                     viewText,&MkEdit::clearUndoStackHandle);

    QObject::connect(viewText,&MkEdit::drawTextBlocks,
                     currentdocument.data(),&MkTextDocument::drawTextBlocksHandler);

    QObject::connect(viewText,&MkEdit::pushCheckBox,
                     currentdocument.data(),&MkTextDocument::pushCheckBoxHandle);

    QObject::connect(viewText,&MkEdit::pushLink,
                     currentdocument.data(),&MkTextDocument::pushLinkHandle);

    QObject::connect(viewText,&MkEdit::autoInsertSymbol,
                     currentdocument.data(),&MkTextDocument::autoInsertSymobolHandle);
}

void ViewsHandler::disconnectDocument()
{
    QObject::disconnect(viewText, &MkEdit::checkRightClockOnCodeBlock,
                     currentdocument.data(), &MkTextDocument::checkRightClockOnCodeBlockHandle);

    QObject::disconnect(viewText, &MkEdit::selectBlockCopy,
                     currentdocument.data(),&MkTextDocument::selectBlockCopyHandle);

    QObject::disconnect(viewText, &MkEdit::duplicateLine,
                     currentdocument.data(),&MkTextDocument::duplicateLineHandle);

    QObject::disconnect(viewText, &MkEdit::smartSelection,
                     currentdocument.data(),&MkTextDocument::smartSelectionHandle);

    QObject::disconnect(viewText,&MkEdit::cursorPosChanged,
                     currentdocument.data(),&MkTextDocument::cursorPosChangedHandle);

    QObject::disconnect(viewText,&MkEdit::enterKeyPressed,
                     currentdocument.data(),&MkTextDocument::enterKeyPressedHandle);

    QObject::disconnect(viewText,&MkEdit::quoteLeftKeyPressed,
                     currentdocument.data(),&MkTextDocument::quoteLeftKeyPressedHandle);

    QObject::disconnect(viewText,&MkEdit::removeAllMkData,
                     currentdocument.data(),&MkTextDocument::removeAllMkDataHandle);

    QObject::disconnect(viewText,&MkEdit::applyAllMkData,
                     currentdocument.data(),&MkTextDocument::applyAllMkDataHandle);

    QObject::disconnect(viewText,&MkEdit::setMarkdownStatus,
                     currentdocument.data(),&MkTextDocument::setMarkdownHandle);

    QObject::disconnect(currentdocument.data(),&MkTextDocument::clearUndoStack,
                     viewText,&MkEdit::clearUndoStackHandle);

    QObject::disconnect(viewText,&MkEdit::drawTextBlocks,
                     currentdocument.data(),&MkTextDocument::drawTextBlocksHandler);

    QObject::disconnect(viewText,&MkEdit::pushCheckBox,
                     currentdocument.data(),&MkTextDocument::pushCheckBoxHandle);

    QObject::disconnect(viewText,&MkEdit::pushLink,
                     currentdocument.data(),&MkTextDocument::pushLinkHandle);

    QObject::disconnect(viewText,&MkEdit::autoInsertSymbol,
                     currentdocument.data(),&MkTextDocument::autoInsertSymobolHandle);
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

void ViewsHandler::setCurrentDocument(const QFileInfo &fileInfo)
{
    //disconnect signals from old current document
    //set current document to textview
    //connect signals to new current document

}

void ViewsHandler::fileDisplay(const QModelIndex& index)
{
    QModelIndex sourceIndex = proxyModel.mapToSource(index);
    fileInfo = modelTree.fileInfo(sourceIndex);
    if (!fileInfo.isFile()|| !fileInfo.exists())
        return;

    QScopedPointer<QFile> file =  QScopedPointer<QFile>(new QFile(fileInfo.absoluteFilePath()));
    QString fullContent = getFileContent(*file.get());

    QString fullPath = fileInfo.absoluteFilePath();
    currentFilePath = fullPath;
    if (fullPath.startsWith(vaultPath)) {
        fullPath.replace(vaultPath, "");
    }
    //recentFileCursorMap.insert(fullPath,QPair<int,int>(viewText->textCursor().blockNumber(), viewText->textCursor().positionInBlock()));
    emit updateRecentFile(fullPath);

    currentdocument->clear();
    currentdocument->setPlainText(fullContent);

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

    currentdocument->clear();
    currentdocument->setPlainText(fullContent);

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

void ViewsHandler::cursorUpdateHandle(const int blockNo, const int posInBlock)
{
    if(currentFilePath.isEmpty()){
        return;
    }
    qDebug()<<" current file path = "<<currentFilePath <<" blockno = "<<blockNo<<" posinblock "<<posInBlock;

    qDebug()<<" value before = "<<recentFileCursorMap.value(currentFilePath);
    recentFileCursorMap.insert(currentFilePath,QPair<int,int>(blockNo, posInBlock));
    qDebug()<<" value after = "<<recentFileCursorMap.value(currentFilePath);
}

void ViewsHandler::openRecentFilesDialogHandle(bool show)
{
    if(show){
        if(recentFilesView->isHidden()){
            QPoint pos =  this->parent->mapToGlobal( viewRightFrame->pos());
            recentFilesView->setGeometry(viewRightFrame->geometry());
            recentFilesView->move(pos);
            recentFilesView->show();
        }
    }
    else{
        if(!recentFilesView->isHidden()){
            const QString& relativePath = recentFilesView->getCurrentRelativeFile();

            if(relativePath.isEmpty()){
                recentFilesView->hide();
                return;
            }

            QString fullFilePath = vaultPath + relativePath;
            fileInfo = QFileInfo(fullFilePath);
            if (!fileInfo.isFile()|| !fileInfo.exists())
                return;

            QSharedPointer<QFile> file = QSharedPointer<QFile>(new QFile(fileInfo.absoluteFilePath()));
            QString fullContent = getFileContent(*file.get());

            QObject::disconnect(viewText,&MkEdit::cursorUpdate,
                             this,&ViewsHandler::cursorUpdateHandle);

            currentdocument->clear();
            currentdocument->setPlainText(fullContent);


            QPair<int,int> positionPair = recentFileCursorMap.value(fullFilePath);
            QTextCursor cursor = viewText->textCursor();
            cursor.setPosition(mkGuiDocument.findBlockByNumber(positionPair.first).position());
            cursor.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);

            for(int rep = 0; rep < positionPair.second; rep++){
                cursor.movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor);
            }

            viewText->setTextCursor(cursor);

            viewTitle->setText(fileInfo.baseName());
            viewText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            viewText->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            viewText->update();

            QObject::connect(viewText,&MkEdit::cursorUpdate,
                                this,&ViewsHandler::cursorUpdateHandle);

            emit updateRecentFile(relativePath);
        }
        recentFilesView->hide();
    }

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
