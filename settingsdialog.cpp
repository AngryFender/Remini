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

    ui->cmb_theme->addItem("Light Theme");
    ui->cmb_theme->addItem("Dark Theme");

    ui->cmb_stretch->addItem("AnyStretch");
    ui->cmb_stretch->addItem("UltraCondensed");
    ui->cmb_stretch->addItem("ExtraCondensed");
    ui->cmb_stretch->addItem("Condensed");
    ui->cmb_stretch->addItem("SemiCondensed");
    ui->cmb_stretch->addItem("Unstretched");
    ui->cmb_stretch->addItem("SemiExpanded");
    ui->cmb_stretch->addItem("Expanded");
    ui->cmb_stretch->addItem("ExtraExpanded");
    ui->cmb_stretch->addItem("UltraExpanded");

    ui->cmb_weight->addItem("Thin");
    ui->cmb_weight->addItem("ExtraLight");
    ui->cmb_weight->addItem("Light");
    ui->cmb_weight->addItem("Normal");
    ui->cmb_weight->addItem("Medium");
    ui->cmb_weight->addItem("DemiBold");
    ui->cmb_weight->addItem("Bold");
    ui->cmb_weight->addItem("ExtraBold");
    ui->cmb_weight->addItem("Black");

    ui->ledit_font_size->setValidator(new QIntValidator(6,30,ui->ledit_font_size));
    QObject::connect(ui->btn_vaultRootPath, &QPushButton::pressed,
                    this, &SettingsDialog::executeFolderDialog);

    QObject::connect(this,&SettingsDialog::syntaxColorUpdate,
                    &previewHighligher,&Highlighter::syntaxColorUpdateHandler);

    QObject::connect(ui->cmb_font,&QFontComboBox::currentFontChanged,
                    this, &SettingsDialog::updateFontHandler);

    QObject::connect(ui->ledit_font_size,&QLineEdit::textChanged,
                    this, &SettingsDialog::updateFontSizeHandler);

    QObject::connect(ui->cmb_stretch,&QComboBox::currentIndexChanged,
                    this, &SettingsDialog::updateStretchHandler);

    QObject::connect(ui->cmb_weight,&QComboBox::currentIndexChanged,
                    this, &SettingsDialog::updateWeightHandler);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::executeFolderDialog()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory(vaultRootPath);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    if (dialog.exec()) {
        QStringList list = dialog.selectedFiles();
        const QString selectedDir = list.first();
        ui->edit_vaultRootPath->setText(selectedDir);
    }
}

void SettingsDialog::updateFontHandler(const QFont &f)
{
    ui->txt_preview->setFont(f);
    ui->txt_preview->update();
}

void SettingsDialog::updateFontSizeHandler(const QString &text)
{
    QFont font = ui->txt_preview->font();
    font.setPointSize(text.toInt());
    ui->txt_preview->setFont(font);
}

void SettingsDialog::updateStretchHandler(const int index)
{
    QFont font = ui->txt_preview->font();
    switch(index){
    case 0: font.setStretch(QFont::AnyStretch);break;
    case 1: font.setStretch(QFont::UltraCondensed);break;
    case 2: font.setStretch(QFont::ExtraCondensed);break;
    case 3: font.setStretch(QFont::Condensed);break;
    case 4: font.setStretch(QFont::SemiCondensed);break;
    case 5: font.setStretch(QFont::Unstretched);break;
    case 6: font.setStretch(QFont::SemiExpanded);break;
    case 7: font.setStretch(QFont::Expanded);break;
    case 8: font.setStretch(QFont::ExtraExpanded);break;
    case 9: font.setStretch(QFont::UltraExpanded);break;
    default: break;
    }
    ui->txt_preview->setFont(font);
}

void SettingsDialog::updateWeightHandler(const int weight)
{
    QFont font = ui->txt_preview->font();
    switch(weight){
    case 0: font.setWeight(QFont::Thin);break;
    case 1: font.setWeight(QFont::Light);break;
    case 2: font.setWeight(QFont::Normal);break;
    case 3: font.setWeight(QFont::Medium);break;
    case 4: font.setWeight(QFont::DemiBold);break;
    case 5: font.setWeight(QFont::Bold);break;
    case 6: font.setWeight(QFont::ExtraBold);break;
    case 7: font.setWeight(QFont::Black);break;
    default: break;
    }
    ui->txt_preview->setFont(font);
}

void SettingsDialog::syntaxColorUpdateHandler(HighlightColor &colors)
{
    this->previewColors = colors;
    emit syntaxColorUpdate(previewColors);
}

void SettingsDialog::show()
{
    ui->edit_vaultRootPath->setText(getVaultRootPath());
    QDialog::show();
}

const QString SettingsDialog::getVaultRootPath()
{
    QString currentPath = QDir::currentPath();
    QString filePath = currentPath + CONFIG_FILE_NAME;
    QFile file(filePath);

    if(file.exists()){
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            vaultRootPath = stream.readAll();
        }
    }else{
        if (file.open(QIODevice::WriteOnly)){
            QTextStream stream(&file);
            stream << currentPath;
            vaultRootPath = currentPath;
        }
    }
    file.close();
    return vaultRootPath;
}
