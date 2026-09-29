#ifndef VIEWS_HANDLER_H
#define VIEWS_HANDLER_H
#include <mkedit.h>
#include <mktextdocument.h>
#include <navigationmodel.h>
#include <navigationview.h>
#include <startuptext.h>

#include <QFileDialog>
#include <QListWidget>
#include <QMessageBox>
#include <QMutex>
#include <QSettings>
#include <QSharedPointer>
#include <QThread>
#include <QTreeWidgetItem>

#include "recentfilesdialog.h"
#include "settingsdialog.h"
#include "textsearchworker.h"
#include "togglebutton.h"
#include "ui_mainwindow.h"

#define NAVIGATION_RATIO 100
#define EDITOR_RAIO 200

class ViewsHandler : public QObject
{
    Q_OBJECT
public:
    ViewsHandler(QWidget *parent, ViewsHandler &other) = delete;
    void operator=(const ViewsHandler &) = delete;

    enum DOCUMENT_STATUS
    {
        NEW_DOCUMENT,
        OLD_DOCUMENT,
        NOT_DOCUMENT,
    };

    static QSharedPointer<ViewsHandler> getInstance(QWidget *parent,
                                                    Ui::MainWindow &ui)
    {
        static QSharedPointer<ViewsHandler> handle{
            new ViewsHandler(parent, ui)};
        return handle;
    }
    ~ViewsHandler() {}

public slots:
    void openRecentFilesDialogHandle(bool show);
    void startTextSearchInAllFilesHandle();
    void startFileSearchHandle();
    void sendFocusToNavigationViewHandler();
    void editLockHandle();

private:
    ViewsHandler(QWidget *parent, Ui::MainWindow &ui)
    {
        firstDirectoryLoad = true;
        ui.uiFrameSearchTree->hide();
        this->parent = parent;
        recentFilesList = new QListWidget(this->parent);
        recentFilesView = new RecentFilesDialog(this->parent, recentFilesList);
        settingsDialog = new SettingsDialog(this->parent);
        initViews(ui);
        initConnection();
        textSearchWorker.moveToThread(&searchThread);
        QCoreApplication::setApplicationName("Remini");
        QSettings settings("Remini", "Remini");
        markdown = settings.value("markdown", true).toBool();
        viewText->setMkState(markdown);
        viewTitle->setText("Startup Tips");
    }

    QWidget *parent;
    QThread searchThread;
    RecentFilesDialog *recentFilesView;
    QListWidget *recentFilesList;
    QFrame *viewLeftFrame;
    QFrame *viewRightFrame;
    TextSearchWorker textSearchWorker;
    QFileSystemModel modelTree;
    NavigationProxyModel proxyModel;
    MkEdit *viewText;
    QLabel *viewTitle;
    QToolButton *viewSettingBtn;
    SettingsDialog *settingsDialog;
    QLineEdit *viewSearch;
    NavigationView *viewTree;
    QFileInfo fileInfo;
    QDialog fileDeleteConfirm;
    QFrame *frameSearchTextTree;
    QFrame *frameSearchFileTree;
    QLineEdit *viewTextSearchEdit;
    NavigationView *viewTextSearchTree;
    QLabel *viewTextSearchCount;
    ToggleButton *viewToggleBtnEdit;

    QFont fontBase;
    QString getVaultPath();
    void setVaultPath(const QString &path);

    void initViews(Ui::MainWindow &ui);
    bool initTreeView(QString path = NULL);
    void initFontDefault();
    void initConnection();
    void connectDocument();
    void disconnectDocument();

    QString getFileContent(QFile &file);
    DOCUMENT_STATUS setCurrentDocument(const QFileInfo &fileInfo);
    MkTextDocument mkGuiDocument;
    Highlighter highlighter;
    QString searchedFilename;
    QMutex fileSearchMutex;
    bool firstDirectoryLoad;

    QMap<QString, QPair<int, int>> recentFileCursorMap;
    QSharedPointer<MkTextDocument> currentDocument;
    QMap<QString, QSharedPointer<MkTextDocument>> recentFileDocumentMap;
    QString currentFilePath;
    QString vaultPath;
    bool markdown;
    bool lineWrap;
signals:
    void load_text(QString text);
    void clear_text();
    void fileDelete(QModelIndex &index);
    void fileDeletePath(const QString &path);
    void updateRecentFile(const QString &relativePath);

    // Relays SettingsDialog::themeChanged to whoever owns the top-level
    // window (currently MainWindow). ViewsHandler itself does not know
    // about MainWindow and does not touch the application style, so the
    // signal stays a pure pass-through.
    void themeChanged(const QString &themeName);

private slots:
    void fileDisplay(const QModelIndex &index);
    void updateUiSettingsHandler(const QFont &font);
    void fileSaveRawHandle();
    void fileDeleteDialogue(QModelIndex &index);
    void searchFileHandle(const QString &filename);
    void navigationAllPathLoaded(QString path);
    void navigationViewExpandedFilenameFilter();
    void doSearchWork(const QString &text);
    void displayTextSearchedFilePosition(QString &filePath,
                                         int searchTextLength, int blockNumber,
                                         int positionInBlock);
    void showSettingsBtn();
    void fileRenamedHandler(const QString &newName, const QString &oldName,
                            const QModelIndex &index);
    QString setVaultPathHandler();
    void checkIfCursorInBlockHandler(bool &isBlock, QTextCursor &cursor);
    void textSearchChangedHandler(const QString &text);
    void updateTextSearchViewHandler(QStandardItemModel *model, int matchCount);
    void textSearchResultPositionSelected(const QModelIndex &index);
    void sendFocusToSearchHandler(QWidget *view);
    void textSearchReturnPressedHandler();
    void fileSearchReturnPressedHandler();
    void emptySearchHandler();
};

#endif  // VIEWS_HANDLER_H
