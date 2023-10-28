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
    viewSettingBtn = ui.uiSettingBtn;

    QIcon SettingsIcon(":/icons/settings.png");
    viewSettingBtn->setIcon(SettingsIcon);

    viewSearch->setPlaceholderText("Search Files...");
    initTreeView();

    recentFileDocumentMap.insert("startup", QSharedPointer<MkTextDocument>(new MkTextDocument()));
    currentDocument = recentFileDocumentMap.value("startup");
    currentDocument->setPlainText(startupText);
    highlighter.setDocument(currentDocument.data());
    viewText->setDocument(currentDocument.data());

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

    QObject::connect(viewText,&MkEdit::syntaxColorUpdate,
                     settingsDialog,&SettingsDialog::syntaxColorUpdateHandler);

    QObject::connect(viewTree, &NavigationView::pressed,
                     this, &ViewsHandler::fileDisplay);

    QObject::connect(viewTree, &NavigationView::createFile,
                     &proxyModel, &NavigationProxyModel::createFileHandler);

    QObject::connect(viewTree, &NavigationView::createFolder,
                     &proxyModel, &NavigationProxyModel::createFolderHandler);

    QObject::connect(this,  &ViewsHandler::fileDelete,
                     &proxyModel, &NavigationProxyModel::deleteFileFolderHandler);

    QObject::connect(this,  &ViewsHandler::fileDeletePath,
                     recentFilesView, &RecentFilesDialog::removeRecentDeletedFileHandle);

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

    QObject::connect(viewSettingBtn,&QToolButton::pressed,
                     this,&ViewsHandler::showSettingsBtn);

    QObject::connect(viewTree, &NavigationView::fileRenamed,
                     this,&ViewsHandler::fileRenamedHandler);

    connectDocument();

}

void ViewsHandler::connectDocument()
{
    QObject::connect(viewText, &MkEdit::checkRightClockOnCodeBlock,
                     currentDocument.data(), &MkTextDocument::checkRightClockOnCodeBlockHandle);

    QObject::connect(viewText, &MkEdit::selectBlockCopy,
                     currentDocument.data(),&MkTextDocument::selectBlockCopyHandle);

    QObject::connect(viewText, &MkEdit::duplicateLine,
                     currentDocument.data(),&MkTextDocument::duplicateLineHandle);

    QObject::connect(viewText, &MkEdit::smartSelection,
                     currentDocument.data(),&MkTextDocument::smartSelectionHandle);

    QObject::connect(viewText,&MkEdit::cursorPosChanged,
                     currentDocument.data(),&MkTextDocument::cursorPosChangedHandle);

    QObject::connect(viewText,&MkEdit::enterKeyPressed,
                     currentDocument.data(),&MkTextDocument::enterKeyPressedHandle);

    QObject::connect(viewText,&MkEdit::quoteLeftKeyPressed,
                     currentDocument.data(),&MkTextDocument::quoteLeftKeyPressedHandle);

    QObject::connect(viewText,&MkEdit::removeAllMkData,
                     currentDocument.data(),&MkTextDocument::removeAllMkDataHandle);

    QObject::connect(viewText,&MkEdit::applyAllMkData,
                     currentDocument.data(),&MkTextDocument::applyAllMkDataHandle);

    QObject::connect(viewText,&MkEdit::setMarkdownStatus,
                     currentDocument.data(),&MkTextDocument::setMarkdownHandle);

    QObject::connect(currentDocument.data(),&MkTextDocument::clearUndoStack,
                     viewText,&MkEdit::clearUndoStackHandle);

    QObject::connect(viewText,&MkEdit::drawTextBlocks,
                     currentDocument.data(),&MkTextDocument::drawTextBlocksHandler);

    QObject::connect(viewText,&MkEdit::pushCheckBox,
                     currentDocument.data(),&MkTextDocument::pushCheckBoxHandle);

    QObject::connect(viewText,&MkEdit::pushLink,
                     currentDocument.data(),&MkTextDocument::pushLinkHandle);

    QObject::connect(viewText,&MkEdit::autoInsertSymbol,
                     currentDocument.data(),&MkTextDocument::autoInsertSymobolHandle);

    QObject::connect(viewText,&MkEdit::cursorUpdate,
                     currentDocument.data(),&MkTextDocument::cursorUpdateHandle);

}

void ViewsHandler::disconnectDocument()
{
    QObject::disconnect(viewText, &MkEdit::checkRightClockOnCodeBlock,
                        currentDocument.data(), &MkTextDocument::checkRightClockOnCodeBlockHandle);

    QObject::disconnect(viewText, &MkEdit::selectBlockCopy,
                        currentDocument.data(),&MkTextDocument::selectBlockCopyHandle);

    QObject::disconnect(viewText, &MkEdit::duplicateLine,
                        currentDocument.data(),&MkTextDocument::duplicateLineHandle);

    QObject::disconnect(viewText, &MkEdit::smartSelection,
                        currentDocument.data(),&MkTextDocument::smartSelectionHandle);

    QObject::disconnect(viewText,&MkEdit::cursorPosChanged,
                        currentDocument.data(),&MkTextDocument::cursorPosChangedHandle);

    QObject::disconnect(viewText,&MkEdit::enterKeyPressed,
                        currentDocument.data(),&MkTextDocument::enterKeyPressedHandle);

    QObject::disconnect(viewText,&MkEdit::quoteLeftKeyPressed,
                        currentDocument.data(),&MkTextDocument::quoteLeftKeyPressedHandle);

    QObject::disconnect(viewText,&MkEdit::removeAllMkData,
                        currentDocument.data(),&MkTextDocument::removeAllMkDataHandle);

    QObject::disconnect(viewText,&MkEdit::applyAllMkData,
                        currentDocument.data(),&MkTextDocument::applyAllMkDataHandle);

    QObject::disconnect(viewText,&MkEdit::setMarkdownStatus,
                        currentDocument.data(),&MkTextDocument::setMarkdownHandle);

    QObject::disconnect(currentDocument.data(),&MkTextDocument::clearUndoStack,
                     viewText,&MkEdit::clearUndoStackHandle);

    QObject::disconnect(viewText,&MkEdit::drawTextBlocks,
                        currentDocument.data(),&MkTextDocument::drawTextBlocksHandler);

    QObject::disconnect(viewText,&MkEdit::pushCheckBox,
                        currentDocument.data(),&MkTextDocument::pushCheckBoxHandle);

    QObject::disconnect(viewText,&MkEdit::pushLink,
                        currentDocument.data(),&MkTextDocument::pushLinkHandle);

    QObject::disconnect(viewText,&MkEdit::autoInsertSymbol,
                        currentDocument.data(),&MkTextDocument::autoInsertSymobolHandle);

    QObject::disconnect(viewText,&MkEdit::cursorUpdate,
                     currentDocument.data(),&MkTextDocument::cursorUpdateHandle);

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

ViewsHandler::DOCUMENT_STATUS ViewsHandler::setCurrentDocument(const QFileInfo &fileInfo)
{
    if (!fileInfo.isFile())
        return NOT_DOCUMENT;

    //disconnect signals from old current document
    disconnectDocument();

    //set current document to textview
    const QString &filePath = fileInfo.absoluteFilePath();
    const QString &fileName = fileInfo.baseName();
    currentDocument = recentFileDocumentMap.value(filePath);
    if(currentDocument==nullptr){
        recentFileDocumentMap.insert(filePath, QSharedPointer<MkTextDocument>(new MkTextDocument()));
        currentDocument = recentFileDocumentMap.value(filePath);
        currentDocument->setFilePath(filePath);
        currentDocument->setFileName(fileName);
        const QScopedPointer<QFile> file = QScopedPointer<QFile>(new QFile(filePath));
        const QString fullContent = getFileContent(*file.get());

        currentDocument->setDefaultFont(fontUi);
        viewText->setFont(fontUi);
        connectDocument();
        currentDocument->setPlainText(fullContent);

        currentDocument->setFilePath(fileInfo.absoluteFilePath());
        highlighter.setDocument(currentDocument.data());
        viewTitle->setText(currentDocument->getFileName());

        viewText->setDocument(currentDocument.data());
        viewText->initialialCursorPosition();
        return NEW_DOCUMENT;
    }else{

        currentDocument->setFilePath(fileInfo.absoluteFilePath());
        highlighter.setDocument(currentDocument.data());
        viewTitle->setText(currentDocument->getFileName());
        viewText->setDocument(currentDocument.data());

        QTextCursor cursor = viewText->textCursor();
        QTextBlock block = currentDocument->findBlockByNumber(this->currentDocument->getBlockNo());
        cursor.setPosition(block.position());
        cursor.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);

        const int characterNo = this->currentDocument->getCharacterNo();
        for(int rep = 0; rep < characterNo; ++rep){
            cursor.movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor);
        }
        connectDocument();
        viewText->setTextCursor(cursor);
        return OLD_DOCUMENT;
    }
}

void ViewsHandler::fileDisplay(const QModelIndex& index)
{
    QModelIndex sourceIndex = proxyModel.mapToSource(index);
    DOCUMENT_STATUS status = setCurrentDocument(modelTree.fileInfo(sourceIndex));

    if(NOT_DOCUMENT == status){
        return;
    }

    QString fullPath = this->currentDocument->getFilePath();
    currentFilePath = fullPath;

    QFileInfo fileInfo(currentFilePath);
    if(!fileInfo.isDir()){
        if (fullPath.startsWith(vaultPath)) {
            fullPath.replace(vaultPath, "");
        }
        emit updateRecentFile(fullPath);

        if(OLD_DOCUMENT == status){
            viewText->ensureCursorVisible();
        }
    }
}

void ViewsHandler::fileSaveHandle()
{
    if(!viewText->hasFocus())
        return;

    QString fullContent = viewText->toPlainText();

    QFile file(currentDocument.data()->getFilePath());

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

        QString currentFilePath = info.absoluteFilePath();
        if (currentFilePath.startsWith(vaultPath)) {
            currentFilePath.replace(vaultPath, "");
        }
        emit fileDeletePath(currentFilePath);
        currentDocument->setPlainText(startupText);
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

    setCurrentDocument(fileInfo);
    viewText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    viewText->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    viewText->update();

    QTextCursor cursor = viewText->textCursor();
    QTextBlock block = currentDocument->findBlockByNumber(blockNumber);

    cursor.setPosition(block.position());
    viewText->setTextCursor(cursor);

    cursor.setPosition(block.position()+positionInBlock);
    cursor.setPosition(block.position()+positionInBlock-searchTextLength,QTextCursor::KeepAnchor);
    viewText->setTextCursor(cursor);

}

void ViewsHandler::showSettingsBtn()
{
    settingsDialog->show();
}

void ViewsHandler::fileRenamedHandler(const QString& newName, const QString &oldName, const QModelIndex& index)
{
    QModelIndex sourceIndex = proxyModel.mapToSource(index);
    DOCUMENT_STATUS status = setCurrentDocument(modelTree.fileInfo(sourceIndex));

    if(NOT_DOCUMENT == status){
        return;
    }

    QString path = this->currentDocument->getFilePath();
    QFileInfo fileInfo(currentFilePath);
    if(!fileInfo.isDir()){
        if (path.startsWith(vaultPath)) {
            path.replace(vaultPath, "");
        }
        int lastIndex = path.lastIndexOf(newName);
        if(-1 != lastIndex){
            path.remove(lastIndex,newName.length());
            path.append(oldName);
            recentFilesView->removeRecentDeletedFileHandle(path);
        }
    }

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

            this->setCurrentDocument(fileInfo);

            viewText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            viewText->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            viewText->update();

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
