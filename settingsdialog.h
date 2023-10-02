#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include "mktextdocument.h"
#include "highlighter.h"

const QString previewText = QStringLiteral("\Multithreading Technologies in Qt"
"Qt offers many classes and functions for working with threads. Below are four different approaches that Qt programmers can use to implement multithreaded applications."
"QThread: Low-Level API with Optional Event Loops"
"QThread is the foundation of all thread control in Qt. Each QThread instance represents and controls one thread."
"\n"
"QThread can either be instantiated directly or subclassed. Instantiating a QThread provides a parallel event loop, allowing QObject slots to be invoked in a secondary thread. Subclassing a QThread allows the application to initialize the new thread before starting its event loop, or to run parallel code without an event loop.");

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

private slots:
    void executeFolderDialog();
public slots:
    void syntaxColorUpdateHandler(HighlightColor &colors);
private:
    Ui::SettingsDialog *ui;
    HighlightColor previewColors;
    MkTextDocument previewDocument;
    Highlighter previewHighligher;
};

#endif // SETTINGSDIALOG_H
