#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::WindowModal);

    previewDocument.setPlainText(previewText);
    previewHighligher.setDocument(&this->previewDocument) ;
    ui->txt_preview->setDocument(&this->previewDocument);

    QObject::connect(ui->btn_vaultRootPath, &QPushButton::pressed,
            this, &SettingsDialog::executeFolderDialog);

    QObject::connect(this,&SettingsDialog::syntaxColorUpdate,
                     &previewHighligher,&Highlighter::syntaxColorUpdateHandler);

    ui->cmb_theme->addItem("Light Theme");
    ui->cmb_theme->addItem("Dark Theme");

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::executeFolderDialog()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    if (dialog.exec()) {
        QStringList list = dialog.selectedFiles();
        const QString selectedDir = list.first();
        ui->edit_vaultRootPath->setText(selectedDir);
    }
}

void SettingsDialog::syntaxColorUpdateHandler(HighlightColor &colors)
{
    this->previewColors = colors;
    emit syntaxColorUpdate(previewColors);
}

const QString SettingsDialog::getVaultRootPath()
{
    QString vaultRootPath;
    //todo
    return vaultRootPath;
}
