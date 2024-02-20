#include "views_handler.h"

QString ViewsHandler::getVaultPath()
{
    QFile configFile("config.ini");
    if(configFile.exists()){
        QSettings settings("config.ini", QSettings::IniFormat);
        return settings.value("vault").toString();
    }

    QCoreApplication::setApplicationName("Remini");
    QSettings settings("Remini","Remini");
    settings.setPath(settings.IniFormat, settings.SystemScope, "config.ini");
    QString path = settings.value("vault").toString();

    QFile configAnotherFile(path);
    if(configAnotherFile.exists()){
        return path;
    }

    QString localVaultPath = QCoreApplication::applicationDirPath() + QDir::separator() + "Vault";
    QDir dir;
    if(!dir.exists(localVaultPath)){
        dir.mkdir(localVaultPath);
    }
    settings.setValue("vault",localVaultPath);
    return localVaultPath;
}

void ViewsHandler::setVaultPath(QString &path)
{
    QFile configFile("config.ini");
    if(configFile.exists()){
        QSettings settings("config.ini", QSettings::IniFormat);
        settings.setValue("vault",path);
    }else{
        QSettings settings("Remini","Remini");
        settings.setValue("vault",path);
    }
}

void ViewsHandler::initViews(Ui::MainWindow &ui)
{
    frameSearchFileTree = ui.uiFrameFileTree;
    frameSearchTextTree = ui.uiFrameSearchTree;
    viewTextSearchEdit = ui.uiSearchText;
    viewTextSearchTree = ui.uiSearchTree;
    viewTextSearchTree->setRowsEditable(false);
    viewTextSearchCount = ui.uiTextSearchCount;

    viewTextSearchEdit->setPlaceholderText("Search Texts...");

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

void ViewsHandler::initTreeView(QString path)
{
    modelTree.setReadOnly(false);
    modelTree.setFilter(QDir::NoDotAndDotDot|QDir::AllEntries);
    if(path.isEmpty()){
        path = getVaultPath();
    }
    modelTree.setRootPath(path);
    vaultPath = modelTree.rootPath();

    proxyModel.setSourceModel(&modelTree);
    viewTree->setModel(&proxyModel);
    viewTree->setRootIndex(proxyModel.setRootIndexFromPath(path));

    for(int column = 1; column < proxyModel.columnCount(); column ++)
    {
        viewTree->setColumnHidden(column,true);
    }
}

void ViewsHandler::initFontDefault()
{
    QSettings settings("Remini","Remini");
    QFont font;
    font.setFamily(settings.value("font","Calibri").toString());
    font.setPointSize(settings.value("fontsize",14).toInt());
    font.setStretch(settings.value("stretch",0).toInt());
    font.setWeight((QFont::Weight)settings.value("weight",1).toInt());
    updateUiSettingsHandler(font);
}

void ViewsHandler::initConnection()
{
    QObject::connect(viewText,&MkEdit::syntaxColorUpdate,
                     &highlighter,&Highlighter::syntaxColorUpdateHandler);

    QObject::connect(viewText,&MkEdit::syntaxColorUpdate,
                     settingsDialog,&SettingsDialog::syntaxColorUpdateHandler);

    QObject::connect(settingsDialog,&SettingsDialog::updateUiSettings,
                     this,&ViewsHandler::updateUiSettingsHandler);

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

    QObject::connect(viewText,&MkEdit::escapeFocus,
                     this,&ViewsHandler::sendFocusToSearchHandler);

    QObject::connect(viewSearch,&QLineEdit::textChanged,
                     this,&ViewsHandler::searchFileHandle);

    QObject::connect(&modelTree,&QFileSystemModel::directoryLoaded,
                     this,&ViewsHandler::navigationAllPathLoaded);

    QObject::connect(viewTree,&NavigationView::expansionComplete,
                     this,&ViewsHandler::navigationViewExpandedFilenameFilter);

    QObject::connect(&searchThread,&QThread::started,
                     &textSearchWorker,&TextSearchWorker::doWork);

    QObject::connect(&textSearchWorker,&TextSearchWorker::finished,
                     &searchThread, &QThread::quit);

    QObject::connect(this,&ViewsHandler::updateRecentFile,
                     recentFilesView,&RecentFilesDialog::updateRecentFileHandle);

    QObject::connect(viewSettingBtn,&QToolButton::pressed,
                     this,&ViewsHandler::showSettingsBtn);

    QObject::connect(viewTree, &NavigationView::fileRenamed,
                     this,&ViewsHandler::fileRenamedHandler);

    QObject::connect(viewTree, &NavigationView::setVaultPath,
                     this,&ViewsHandler::setVaultPathHandler);

    QObject::connect(viewText, &MkEdit::checkIfCursorInBlock,
                     this, &ViewsHandler::checkIfCursorInBlockHandler);

    QObject::connect(viewSearch, &QLineEdit::returnPressed,
                     this, &ViewsHandler::fileSearchReturnPressedHandler);

    connectDocument();

    QObject::connect(viewTextSearchEdit, &QLineEdit::textChanged,
            this,&ViewsHandler::textSearchChangedHandler);

    QObject::connect(viewTextSearchEdit, &QLineEdit::returnPressed,
            this,&ViewsHandler::textSearchReturnPressedHandler);

    QObject::connect(&textSearchWorker,&TextSearchWorker::updateTextSearchView,
                     this, &ViewsHandler::updateTextSearchViewHandler);

    QObject::connect(viewTextSearchTree,&NavigationView::pressed,
            this,&ViewsHandler::textSearchResultPositionSelected);

    QObject::connect(viewTextSearchTree,&NavigationView::sendFocusToSearch,
                     this,&ViewsHandler::sendFocusToSearchHandler);

    QObject::connect(viewTree,&NavigationView::sendFocusToSearch,
                     this,&ViewsHandler::sendFocusToSearchHandler);


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

    QObject::connect(viewText,&MkEdit::undoStackPushSignal,
                     currentDocument.data(),&MkTextDocument::undoStackPush);

    QObject::connect(viewText,&MkEdit::undoStackUndoSignal,
                     currentDocument.data(),&MkTextDocument::undoStackUndo);

    QObject::connect(viewText,&MkEdit::undoStackRedoSignal,
                     currentDocument.data(),&MkTextDocument::undoStackRedo);

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

    QObject::disconnect(viewText,&MkEdit::undoStackPushSignal,
                     currentDocument.data(),&MkTextDocument::undoStackPush);

    QObject::disconnect(viewText,&MkEdit::undoStackUndoSignal,
                     currentDocument.data(),&MkTextDocument::undoStackUndo);

    QObject::disconnect(viewText,&MkEdit::undoStackRedoSignal,
                     currentDocument.data(),&MkTextDocument::undoStackRedo);


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

    //set the cursor position of the current document
    QTextCursor cursor = viewText->textCursor();
    currentDocument->setCursorPos(cursor.blockNumber(),cursor.positionInBlock()) ;

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

        currentDocument->setDefaultFont(fontBase);
        viewText->setFont(fontBase);
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
        currentDocument->setDefaultFont(fontBase);
        viewText->setFont(fontBase);
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

void ViewsHandler::updateUiSettingsHandler(const QFont &font)
{
    fontBase = font;
    viewText->setFont(fontBase);

    QFont fontTitle = font;
    fontTitle.setPointSize(fontBase.pointSize()*2);
    fontTitle.setWeight(QFont::DemiBold);
    viewTitle->setFont(fontTitle);

    QFont fontView = font;
    fontView.setPointSize(10);
    fontView.setWeight(QFont::Light);
    fontView.setHintingPreference(QFont::HintingPreference::PreferNoHinting);

    viewTree->setFont(fontView);
    viewSearch->setFont(fontView);
    viewTextSearchCount->setFont(fontView);
    viewTextSearchEdit->setFont(fontView);
    viewTextSearchTree->setFont(fontView);
    settingsDialog->setFont(fontView);
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
            confirmBox->setFont(fontBase);
            confirmBox->setStandardButtons(QMessageBox::Ok);
            confirmBox->button(QMessageBox::Ok)->setFont(fontBase);
            confirmBox->exec();
            return;
        }
    }

    QScopedPointer<QMessageBox> confirmBox(new QMessageBox(parent));
    confirmBox->setStyleSheet(parent->styleSheet());
    confirmBox->setWindowTitle("Delete");
    confirmBox->setText("Are you sure you want to delete");
    confirmBox->setFont(fontBase);
    confirmBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox->setDefaultButton(QMessageBox::No);
    confirmBox->button(QMessageBox::Yes)->setFont(fontBase);
    confirmBox->button(QMessageBox::No)->setFont(fontBase);
   if(QMessageBox::Yes == confirmBox->exec()){
        emit fileDelete(index);

        QString currentFilePath = info.absoluteFilePath();
        if (currentFilePath.startsWith(vaultPath)) {
            currentFilePath.replace(vaultPath, "");
        }
        emit fileDeletePath(currentFilePath);

        viewTitle->setText("Startup Tips");
        currentDocument = recentFileDocumentMap.value("startup");
        currentDocument->setPlainText(startupText);
        highlighter.setDocument(currentDocument.data());
        viewText->setDocument(currentDocument.data());

        const QString filePath = info.absoluteFilePath();
        recentFileDocumentMap.remove(filePath);
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

    cursor.setPosition(block.position()+positionInBlock-searchTextLength, QTextCursor::MoveAnchor);
    cursor.setPosition(block.position()+positionInBlock,QTextCursor::KeepAnchor);
    viewText->setTextCursor(cursor);

}

void ViewsHandler::showSettingsBtn()
{
    settingsDialog->show(vaultPath, viewText->font());
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

QString ViewsHandler::setVaultPathHandler()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory(vaultPath);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);

    QString newPath;
    if (dialog.exec()) {
        QStringList list = dialog.selectedFiles();
        newPath = list.first();
    }
    initTreeView(newPath);
    setVaultPath(newPath);

    recentFilesList->clear();
    recentFileCursorMap.clear();
    return newPath;
    //recentFileDocumentMap.clear();
}

void ViewsHandler::checkIfCursorInBlockHandler(bool &isBlock, QTextCursor &cursor)
{
    BlockData* blockData = dynamic_cast<BlockData*>(cursor.block().userData());
    if(blockData)
    {
        isBlock = true;
        return;
    }
    isBlock = false;
}

void ViewsHandler::textSearchChangedHandler(const QString &text)
{
    if(!searchThread.isRunning()){
        textSearchWorker.setText(text);
        textSearchWorker.setRootPath(modelTree.rootPath());
        proxyModel.createAllFilesList(viewTree->rootIndex(), textSearchWorker.getListPaths());
        searchThread.start();
    }

    highlighter.updateSearchText(text);
}

void ViewsHandler::updateTextSearchViewHandler(QStandardItemModel *model, int matchCount)
{
    viewTextSearchCount->setText(QString::number(matchCount));

    viewTextSearchTree->setModel(model);
    if(model->rowCount()<=2)
        viewTextSearchTree->expandAll();

}

void ViewsHandler::textSearchResultPositionSelected(const QModelIndex &index)
{
    QModelIndex parentIndex = viewTextSearchTree->model()->parent(index);
    QString filePath = parentIndex.data(Qt::UserRole).toString();
    int searchTextLength = index.data(Qt::UserRole).toInt();
    int blockNumber = index.data(Qt::UserRole+1).toInt();
    int positionInBlock = index.data(Qt::UserRole+2).toInt();

    displayTextSearchedFilePosition(filePath, searchTextLength, blockNumber,  positionInBlock);
}

void ViewsHandler::sendFocusToSearchHandler(QWidget *view)
{
    if(view == viewTextSearchTree){
        this->viewTextSearchEdit->selectAll();
        this->viewTextSearchEdit->setFocus();
    }else if(view == viewTree){
        this->viewSearch->selectAll();
        this->viewSearch->setFocus();
    }else if(view == viewText){
        if(!frameSearchTextTree->isHidden()){
            viewTextSearchTree->setFocus();
        }else if (!frameSearchFileTree->isHidden()){
            viewTree->setFocus();
        }
    }
}

void ViewsHandler::textSearchReturnPressedHandler()
{
    this->viewTextSearchTree->setFocus();
}

void ViewsHandler::fileSearchReturnPressedHandler()
{
    this->viewTree->setFocus();
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
    frameSearchFileTree->hide();

    highlighter.updateSearchText(viewTextSearchEdit->text());
    frameSearchTextTree->show();

    viewTextSearchEdit->show() ;
    viewTextSearchTree->show() ;
    viewTextSearchEdit->setFocus();
}

void ViewsHandler::startFileSearchHandle()
{
    frameSearchTextTree->hide();
    highlighter.updateSearchText("");
    frameSearchFileTree->show();
    viewSearch->setFocus();
}
