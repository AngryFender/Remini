#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include "mktextdocument.h"
#include "highlighter.h"

const QString previewText = QStringLiteral("The header file declares several type definitions that guarantee a specified bit-size on all platforms supported by Qt for various basic types, for example qint8 which is a signed char guaranteed to be 8-bit on all platforms supported by Qt. The header file also declares the qlonglong type definition for long long int ( __int64 on Windows). "
"Several convenience type definitions are declared: qreal for double or float, uchar for unsigned char, uint for unsigned int, ulong for unsigned long and ushort for unsigned short.\n"
"\"Finally, the QtMsgType definition identifies the various messages that can be generated and sent to a Qt message handler; \"QtMessageHandler is a type definition for a pointer to a function with the signature void myMessageHandler(QtMsgType, const QMessageLogContext &, const char *). QMessageLogContext class contains the line, file, and function the message was logged at. This information is created by the QMessageLogger class. initialize the new thread before starting its event loop, or to run parallel code without an event loop."
"//Comment texts examples. "
"\n"
"cmake -G \"Visual Studio 17 2022\" -A x64 -S path_to_source -B \"build64\"" );

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

signals:
    void syntaxColorUpdate(HighlightColor &colors);
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
