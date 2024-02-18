#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QSettings>

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
                    this,&SettingsDialog::updateFontHandler);

    QObject::connect(ui->ledit_font_size,&QLineEdit::textChanged,
                    this,&SettingsDialog::updateFontSizeHandler);

    QObject::connect(ui->cmb_stretch,&QComboBox::currentIndexChanged,
                    this,&SettingsDialog::updateStretchHandler);

    QObject::connect(ui->cmb_weight,&QComboBox::currentIndexChanged,
                    this,&SettingsDialog::updateWeightHandler);

    QObject::connect(ui->btn_dialog,&QDialogButtonBox::accepted,
                     this,&SettingsDialog::saveSettingsHandler);

    QObject::connect(ui->btn_plus,&QPushButton::clicked,
                    this,[this]()
                    {
                        int size = this->ui->ledit_font_size->text().toInt();
                        size = std::min(MAXIMUM_FONT_SIZE,size+1);
                        ui->ledit_font_size->setText(QString::number(size));
                    });

    QObject::connect(ui->btn_minus,&QPushButton::clicked,
                    this,[this]()
                    {
                        int size = this->ui->ledit_font_size->text().toInt();
                        size = std::max(MINIMUM_FONT_SIZE,size-1);
                        ui->ledit_font_size->setText(QString::number(size));
                    });
}

void SettingsDialog::setFont(const QFont &font)
{
    ui->btn_dialog->setFont(font);
    ui->cmb_font->setFont(font);
    ui->lbl_font->setFont(font);
    ui->lbl_font_size->setFont(font);
    ui->ledit_font_size->setFont(font);
    ui->edit_vaultRootPath->setFont(font);
    ui->lbl_vaultRootPath->setFont(font);
    ui->btn_vaultRootPath->setFont(font);
    ui->lbl_theme->setFont(font);
    ui->cmb_theme->setFont(font);
    ui->lbl_weight->setFont(font);
    ui->cmb_weight->setFont(font);
    ui->lbl_stretch->setFont(font);
    ui->cmb_stretch->setFont(font);
    ui->lbl_preview->setFont(font);
    ui->btn_dialog->setFont(font);
    ui->btn_dialog->button(QDialogButtonBox::Ok)->setFont(font);
    ui->btn_dialog->button(QDialogButtonBox::Cancel)->setFont(font);
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
    QFont font = ui->txt_preview->font();
    font.setFamily(f.family());
    ui->txt_preview->setFont(font);
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

void SettingsDialog::saveSettingsHandler()
{
    QSettings settings("Remini","Remini");
    const QFont font = ui->txt_preview->font();
    emit updateUiSettings(font);

    settings.setValue("font",font.family());
    settings.setValue("fontsize",font.pointSize());
    settings.setValue("stretch",font.stretch());
    settings.setValue("weight",font.weight());
}

void SettingsDialog::syntaxColorUpdateHandler(HighlightColor &colors)
{
    this->previewColors = colors;
    emit syntaxColorUpdate(previewColors);
}

void SettingsDialog::show(const QString &vaultPath, const QFont &font)
{
    ui->edit_vaultRootPath->setText(vaultPath);
    ui->cmb_font->setCurrentIndex(ui->cmb_font->findText(font.family()));
    ui->ledit_font_size->setText(QString::number(font.pointSize()));

    int index_weight = 0;
    switch(font.weight()){
    case QFont::Thin: index_weight = 0;break;
    case QFont::Light:index_weight = 1;break;
    case QFont::Normal:index_weight = 2;break;
    case QFont::Medium:index_weight = 3;break;
    case QFont::DemiBold:index_weight = 4;break;
    case QFont::Bold:index_weight = 5;break;
    case QFont::ExtraBold:index_weight = 6;break;
    case QFont::Black:index_weight = 7;break;
    default: break;
    }
    ui->cmb_weight->setCurrentIndex(index_weight);

    int index_stretch = 0;
    switch(font.stretch()){
    case QFont::AnyStretch:index_stretch = 0;break;
    case QFont::UltraCondensed:index_stretch = 1;break;
    case QFont::ExtraCondensed:index_stretch = 2;break;
    case QFont::Condensed:index_stretch = 3;break;
    case QFont::SemiCondensed:index_stretch = 4;break;
    case QFont::Unstretched:index_stretch = 5;break;
    case QFont::SemiExpanded:index_stretch = 6;break;
    case QFont::Expanded:index_stretch = 7;break;
    case QFont::ExtraExpanded:index_stretch = 8;break;
    case QFont::UltraExpanded:index_stretch = 9;break;
    default: break;
    }
    ui->cmb_stretch->setCurrentIndex(index_stretch);

    ui->txt_preview->setFont(font);
    ui->txt_preview->update();
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
