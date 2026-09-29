// Tests for NavigationView.
//
// Scope of this file:
//   * Signal-emission tests for the slots that do not require real files
//     (addFile, addFolder, setVaultHandler, openFileFolder,
//      copyFileFolderPath, renameFile with no index, deleteFile with no
//      selection, closeEditor).
//   * Key-event tests for the two keys NavigationView::keyPressEvent
//     reacts to (Backspace -> sendFocusToSearch, Enter/Return -> pressed).
//   * A model-backed fixture used by tests that need a real index
//     (rowClicked expand/collapse toggling).
//
// Deliberately NOT covered here:
//   * Actual filesystem side effects (create / rename / delete on disk).
//     Those belong to NavigationProxyModel, whose tests live in a
//     different file.
//   * Inline-editor lifetime (renameFile -> edit()).
//
// Note on signal observation: the project's signals use non-const
// reference parameters (e.g. `void createFile(QModelIndex &, QString &)`).
// QSignalSpy cannot reliably capture those across Qt versions, so this
// file connects lambdas directly with QObject::connect and records what
// it needs.

#include <QAbstractItemDelegate>
#include <QApplication>
#include <QDir>
#include <QFileSystemModel>
#include <QKeyEvent>
#include <QLineEdit>
#include <QTemporaryDir>
#include <catch2/catch.hpp>

#include "navigationmodel.h"
#include "navigationview.h"

// ===========================================================================
// Fixture: a NavigationView attached to a real QFileSystemModel through a
// NavigationProxyModel, rooted at a temporary directory. This gives the
// view a valid model plus a valid root index so that slots which consult
// the model (rowClicked, renameFile with an index, ...) do not bail out on
// an invalid currentIndex.
// ===========================================================================
struct NavigationViewFixture
{
    QTemporaryDir tempDir;
    QFileSystemModel fileModel;
    NavigationProxyModel proxyModel;
    NavigationView *view = nullptr;

    explicit NavigationViewFixture(bool editable = true)
    {
        REQUIRE(tempDir.isValid());

        // At least one entry so the tree has something to expand.
        QDir(tempDir.path()).mkdir(QStringLiteral("subdir"));

        view = new NavigationView(nullptr, editable);

        fileModel.setRootPath(tempDir.path());
        proxyModel.setSourceModel(&fileModel);
        view->setModel(&proxyModel);
        view->setRootIndex(proxyModel.setRootIndexFromPath(tempDir.path()));
    }

    ~NavigationViewFixture() { delete view; }
};

// ===========================================================================
// Original placeholder test, filled in.
// ===========================================================================
TEST_CASE("NavigationView Add file Test", "[NavigationView]")
{
    NavigationView view;

    int createFileCalls = 0;
    QModelIndex receivedIndex;
    QString receivedName;

    QObject::connect(&view, &NavigationView::createFile,
                     [&](QModelIndex &index, QString &name)
                     {
                         ++createFileCalls;
                         receivedIndex = index;
                         receivedName = name;
                     });

    int emptySearchCalls = 0;
    QObject::connect(&view, &NavigationView::emptySearch,
                     [&]() { ++emptySearchCalls; });

    view.addFile();

    // With no model attached, lastClickedIndex is invalid. addFile() must
    // still emit createFile exactly once, with an invalid index and an
    // empty default name (the receiver is expected to fill both in).
    REQUIRE(createFileCalls == 1);
    REQUIRE_FALSE(receivedIndex.isValid());
    REQUIRE(receivedName.isEmpty());

    // addFile() always clears the search box first.
    REQUIRE(emptySearchCalls == 1);
}

// ===========================================================================
// addFolder mirrors addFile: it emits createFolder and emptySearch.
// ===========================================================================
TEST_CASE("NavigationView Add folder Test", "[NavigationView]")
{
    NavigationView view;

    int createFolderCalls = 0;
    QModelIndex receivedIndex;
    QString receivedName;

    QObject::connect(&view, &NavigationView::createFolder,
                     [&](QModelIndex &index, QString &name)
                     {
                         ++createFolderCalls;
                         receivedIndex = index;
                         receivedName = name;
                     });

    int emptySearchCalls = 0;
    QObject::connect(&view, &NavigationView::emptySearch,
                     [&]() { ++emptySearchCalls; });

    view.addFolder();

    REQUIRE(createFolderCalls == 1);
    REQUIRE_FALSE(receivedIndex.isValid());
    REQUIRE(receivedName.isEmpty());
    REQUIRE(emptySearchCalls == 1);
}

// ===========================================================================
// setVaultHandler: pure signal relay, no state touched.
// ===========================================================================
TEST_CASE("NavigationView setVaultHandler emits setVaultPath",
          "[NavigationView]")
{
    NavigationView view;

    int setVaultPathCalls = 0;
    QObject::connect(&view, &NavigationView::setVaultPath,
                     [&]() { ++setVaultPathCalls; });

    view.setVaultHandler();

    REQUIRE(setVaultPathCalls == 1);
}

// ===========================================================================
// openFileFolder: relays lastClickedIndex as openLocation.
// ===========================================================================
TEST_CASE("NavigationView openFileFolder emits openLocation",
          "[NavigationView]")
{
    NavigationView view;

    int openLocationCalls = 0;
    QObject::connect(&view, &NavigationView::openLocation,
                     [&](QModelIndex &) { ++openLocationCalls; });

    view.openFileFolder();

    REQUIRE(openLocationCalls == 1);
}

// ===========================================================================
// copyFileFolderPath: relays lastClickedIndex as copyFolderFilePath.
// ===========================================================================
TEST_CASE("NavigationView copyFileFolderPath emits copyFolderFilePath",
          "[NavigationView]")
{
    NavigationView view;

    int copyCalls = 0;
    QObject::connect(&view, &NavigationView::copyFolderFilePath,
                     [&](QModelIndex &) { ++copyCalls; });

    view.copyFileFolderPath();

    REQUIRE(copyCalls == 1);
}

// ===========================================================================
// renameFile with no current index: must early-return silently.
// ===========================================================================
TEST_CASE("NavigationView renameFile with no current index does nothing",
          "[NavigationView]")
{
    NavigationView view;

    int renameCalls = 0;
    QObject::connect(&view, &NavigationView::fileRenamed,
                     [&](const QString &, const QString &, const QModelIndex &)
                     { ++renameCalls; });

    // No model, no current index -> renameFile() returns before calling
    // edit() and before emitting anything.
    view.renameFile();

    REQUIRE(renameCalls == 0);
}

// ===========================================================================
// deleteFile with an empty selection: must not emit per-index delete.
// ===========================================================================
TEST_CASE("NavigationView deleteFile with no selection emits nothing",
          "[NavigationView]")
{
    NavigationView view;

    int deleteCalls = 0;
    QObject::connect(&view, &NavigationView::deleteFileFolder,
                     [&](QModelIndex &) { ++deleteCalls; });

    view.deleteFile();

    REQUIRE(deleteCalls == 0);
}

// ===========================================================================
// keyPressEvent: Backspace -> sendFocusToSearch(this).
// ===========================================================================
TEST_CASE("NavigationView Backspace key emits sendFocusToSearch",
          "[NavigationView]")
{
    NavigationView view;

    QWidget *receivedView = nullptr;
    int sendFocusCalls = 0;
    QObject::connect(&view, &NavigationView::sendFocusToSearch,
                     [&](QWidget *v)
                     {
                         ++sendFocusCalls;
                         receivedView = v;
                     });

    QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
    view.keyPressEvent(&keyEvent);

    REQUIRE(sendFocusCalls == 1);
    // NavigationView passes `this` as the argument, which lets the
    // receiver distinguish "focus the search box" from other views.
    REQUIRE(receivedView == &view);
}

// ===========================================================================
// keyPressEvent: Enter / Return -> pressed(index).
// ===========================================================================
TEST_CASE("NavigationView Enter key emits pressed", "[NavigationView]")
{
    NavigationView view;

    int pressedCalls = 0;
    QObject::connect(&view, &NavigationView::pressed,
                     [&](const QModelIndex &) { ++pressedCalls; });

    QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    view.keyPressEvent(&keyEvent);

    // Enter/Return emits the inherited pressed() signal regardless of
    // whether currentIndex() is valid.
    REQUIRE(pressedCalls == 1);
}

// ===========================================================================
// expandTimerHandler: stops the timer and emits expansionComplete.
// ===========================================================================
TEST_CASE("NavigationView expandTimerHandler emits expansionComplete",
          "[NavigationView]")
{
    NavigationView view;

    int expansionCompleteCalls = 0;
    QObject::connect(&view, &NavigationView::expansionComplete,
                     [&]() { ++expansionCompleteCalls; });

    view.expandTimerHandler();

    REQUIRE(expansionCompleteCalls == 1);
}

// ===========================================================================
// rowClicked: toggles expansion of a valid index. Uses the fixture so
// rootIndex() is a real, populated index.
// ===========================================================================
TEST_CASE("NavigationView rowClicked toggles expansion", "[NavigationView]")
{
    NavigationViewFixture fixture;

    const QModelIndex root = fixture.view->rootIndex();
    REQUIRE(root.isValid());

    REQUIRE_FALSE(fixture.view->isExpanded(root));

    fixture.view->rowClicked(root);
    REQUIRE(fixture.view->isExpanded(root));

    fixture.view->rowClicked(root);
    REQUIRE_FALSE(fixture.view->isExpanded(root));
}

// ===========================================================================
// setRowsEditable: the flag is stored, and toggling it must not crash.
// The "editable" effect only becomes observable through the double-click
// path, which is out of scope here (needs an item delegate).
// ===========================================================================
TEST_CASE("NavigationView setRowsEditable does not crash", "[NavigationView]")
{
    NavigationView view;

    view.setRowsEditable(true);
    view.setRowsEditable(false);
    view.setRowsEditable(true);

    // No getter exists; the point of this test is that repeated calls
    // remain side-effect-free and safe.
    SUCCEED();
}

// ===========================================================================
// closeEditor: reads the line editor's text and always emits
// newFileCreated with the current index. Uses the fixture so that
// QTreeView::closeEditor has a valid model to consult.
// ===========================================================================
TEST_CASE("NavigationView closeEditor emits newFileCreated", "[NavigationView]")
{
    NavigationViewFixture fixture;

    int newFileCreatedCalls = 0;
    QObject::connect(fixture.view, &NavigationView::newFileCreated,
                     [&](const QModelIndex &) { ++newFileCreatedCalls; });

    QLineEdit editor;
    editor.setText(QStringLiteral("renamed_by_test"));

    // currentIndex() is invalid here, so the QModelIndex passed to
    // newFileCreated will be invalid. The observable behaviour we care
    // about is that closeEditor reads the editor text and emits the
    // signal without crashing.
    fixture.view->closeEditor(&editor, QAbstractItemDelegate::NoHint);

    REQUIRE(newFileCreatedCalls == 1);
}
