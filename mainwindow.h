#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QKeyEvent>
#include <QMainWindow>
#include <QSharedPointer>
#include <QStyle>
#include <QStyleFactory>
#include <QTimer>
#include <QtGlobal>

#include "./ui_mainwindow.h"
#include "theme.h"
#include "views_handler.h"
#include "windowapi.h"

// ---------------------------------------------------------------------------
// Double-Shift detection uses QKeyEvent::nativeScanCode(), which returns
// different values per platform:
//
//   Windows : hardware scan code  (left = 42, right = 54)
//   Wayland : evdev keycode       (left = 42, right = 54, same as Windows)
//   X11     : xkb keycode         (left = 50, right = 62, evdev + 8)
//
// keyReleaseEvent() picks the correct pair at runtime based on the Qt
// platform plugin name ("xcb" -> X11, "wayland" -> Wayland).
// ---------------------------------------------------------------------------
#ifdef Q_OS_LINUX
#define LEFT_SHIFT_KEY 42       // Wayland / evdev
#define RIGHT_SHIFT_KEY 54      // Wayland / evdev
#define LEFT_SHIFT_KEY_X11 50   // X11 / xkb
#define RIGHT_SHIFT_KEY_X11 62  // X11 / xkb
#else
#define LEFT_SHIFT_KEY 42
#define RIGHT_SHIFT_KEY 54
#endif

#define DOUBLE_SHIFT_TIMER_MS 200

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void keyReleaseEvent(QKeyEvent *event) override;

public slots:
    void recentFilesHandler(bool show);

    // Apply the theme whose `name` matches a Theme in themeAchieve::themeVec().
    // Unknown names fall back to the dark theme. Safe to call at any time,
    // including from a signal emitted while another dialog is open.
    void applyThemeByName(const QString &name);

private slots:
    void showHideApp();
    void shiftTimerHandle();

private:
    QTimer *rightShiftTimer = nullptr;
    QTimer *leftShiftTimer = nullptr;
    Ui::MainWindow *ui = nullptr;

    QString themeContents;
    ThemeState themeState = darkThemeState;

    QString darkTheme = themes::dark().qss;
    QString lightTheme = themes::light().qss;

    QSharedPointer<ViewsHandler> view_handler;

    QStyle *lightThemeStyle = nullptr;
    QStyle *darkThemeStyle = nullptr;

#ifdef Q_OS_WIN
    WindowApi *win = nullptr;
#endif

    void setup_views(QWidget *parent, Ui::MainWindow &ui);

signals:
    void openRecentFilesDialog(bool show);
    void editLock();
    void startSearchAll();
    void startFileSearch();
    void sendFocusToNavigationView();
};

#endif  // MAINWINDOW_H
