#include "settingsdialog.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFontDatabase>
#include <QIntValidator>
#include <QSettings>
#include <QSignalBlocker>
#include <QStringList>
#include <QTextStream>

#include "mktextdocument.h"
#include "theme.h"
#include "ui_settingsdialog.h"
// ---------------------------------------------------------------------------
// Default monospace font per platform.
// "Cascadia Mono" ships with Windows Terminal, so it is only a safe default
// there. Linux and macOS get a font that is essentially guaranteed to exist.
// ---------------------------------------------------------------------------
static QString defaultMonospaceFont()
{
#ifdef Q_OS_WIN
    return QStringLiteral("Cascadia Mono");
#elif defined(Q_OS_MACOS)
    return QStringLiteral("Menlo");
#else
    return QStringLiteral("Monospace");
#endif
}

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::WindowModal);

    previewDocument.setPlainText(previewText);
    previewHighligher.setDocument(&this->previewDocument);
    ui->txt_preview->setDocument(&this->previewDocument);

    ui->cmb_mkState->addItem("Disabled");
    ui->cmb_mkState->addItem("Enabled");

    ui->cmb_lineWrap->addItem("Disabled");
    ui->cmb_lineWrap->addItem("Enabled");

    for (const Theme &t : themeAchieve::themeVec())
    {
        ui->cmb_theme->addItem(t.name);
    }

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

    ui->ledit_font_size->setValidator(
        new QIntValidator(6, 30, ui->ledit_font_size));

    QObject::connect(ui->btn_vaultRootPath, &QPushButton::pressed, this,
                     &SettingsDialog::executeFolderDialog);

    QObject::connect(this, &SettingsDialog::syntaxColorUpdate,
                     &previewHighligher,
                     &Highlighter::syntaxColorUpdateHandler);

    QObject::connect(ui->cmb_font, &QFontComboBox::currentFontChanged, this,
                     &SettingsDialog::updateFontHandler);

    QObject::connect(ui->ledit_font_size, &QLineEdit::textChanged, this,
                     &SettingsDialog::updateFontSizeHandler);

    QObject::connect(ui->cmb_stretch, &QComboBox::currentIndexChanged, this,
                     &SettingsDialog::updateStretchHandler);

    QObject::connect(ui->cmb_weight, &QComboBox::currentIndexChanged, this,
                     &SettingsDialog::updateWeightHandler);

    QObject::connect(ui->btn_dialog, &QDialogButtonBox::accepted, this,
                     &SettingsDialog::saveSettingsHandler);

    QObject::connect(ui->btn_plus, &QPushButton::clicked, this,
                     [this]()
                     {
                         int size = this->ui->ledit_font_size->text().toInt();
                         size = std::min(MAXIMUM_FONT_SIZE, size + 1);
                         ui->ledit_font_size->setText(QString::number(size));
                     });

    QObject::connect(ui->btn_minus, &QPushButton::clicked, this,
                     [this]()
                     {
                         int size = this->ui->ledit_font_size->text().toInt();
                         size = std::max(MINIMUM_FONT_SIZE, size - 1);
                         ui->ledit_font_size->setText(QString::number(size));
                     });
}

SettingsDialog::~SettingsDialog() { delete ui; }

void SettingsDialog::setFont(const QFont &font)
{
    ui->btn_dialog->setFont(font);
    ui->cmb_mkState->setFont(font);
    ui->lbl_lineWrap->setFont(font);
    ui->cmb_lineWrap->setFont(font);
    ui->cmb_font->setFont(font);
    ui->lbl_font->setFont(font);
    ui->lbl_font_size->setFont(font);
    ui->ledit_font_size->setFont(font);
    ui->edit_vaultRootPath->setFont(font);
    ui->lbl_vaultRootPath->setFont(font);
    ui->btn_vaultRootPath->setFont(font);
    ui->lbl_markdown->setFont(font);
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

void SettingsDialog::executeFolderDialog()
{
    // Pass `this` as parent so the dialog is properly associated with this
    // window. On Wayland this matters for centering and stacking.
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory(vaultRootPath);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);

    if (dialog.exec())
    {
        const QStringList list = dialog.selectedFiles();
        if (list.isEmpty())
            return;

        ui->edit_vaultRootPath->setText(list.first());
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
    switch (index)
    {
    case 0:
        font.setStretch(QFont::AnyStretch);
        break;
    case 1:
        font.setStretch(QFont::UltraCondensed);
        break;
    case 2:
        font.setStretch(QFont::ExtraCondensed);
        break;
    case 3:
        font.setStretch(QFont::Condensed);
        break;
    case 4:
        font.setStretch(QFont::SemiCondensed);
        break;
    case 5:
        font.setStretch(QFont::Unstretched);
        break;
    case 6:
        font.setStretch(QFont::SemiExpanded);
        break;
    case 7:
        font.setStretch(QFont::Expanded);
        break;
    case 8:
        font.setStretch(QFont::ExtraExpanded);
        break;
    case 9:
        font.setStretch(QFont::UltraExpanded);
        break;
    default:
        break;
    }
    ui->txt_preview->setFont(font);
}

void SettingsDialog::updateWeightHandler(const int weight)
{
    QFont font = ui->txt_preview->font();
    switch (weight)
    {
    case 0:
        font.setWeight(QFont::Thin);
        break;
    case 1:
        font.setWeight(QFont::Light);
        break;
    case 2:
        font.setWeight(QFont::Normal);
        break;
    case 3:
        font.setWeight(QFont::Medium);
        break;
    case 4:
        font.setWeight(QFont::DemiBold);
        break;
    case 5:
        font.setWeight(QFont::Bold);
        break;
    case 6:
        font.setWeight(QFont::ExtraBold);
        break;
    case 7:
        font.setWeight(QFont::Black);
        break;
    default:
        break;
    }
    ui->txt_preview->setFont(font);
}

void SettingsDialog::saveSettingsHandler()
{
    QSettings settings("Remini", "Remini");
    const QFont font = ui->txt_preview->font();

    settings.setValue("font", font.family());
    settings.setValue("fontsize", font.pointSize());
    settings.setValue("stretch", font.stretch());
    settings.setValue("weight", font.weight());
    // Store markdown / linewrap as booleans to keep the type consistent with
    // the values written by MkEdit::diableMarkdown_internal().
    settings.setValue("markdown", ui->cmb_mkState->currentIndex() != 0);
    settings.setValue("linewrap", ui->cmb_lineWrap->currentIndex() != 0);
    settings.setValue("vaultPath", ui->edit_vaultRootPath->text());

    // ------------------------------------------------------------------
    // Theme: read the previously saved value before overwriting it so we
    // can decide whether to notify the rest of the app.
    //
    // We deliberately do NOT emit on every OK click: the signal triggers
    // a QApplication::setStyle() + setStyleSheet() cycle, which rebuilds
    // every widget's style, and there is no reason to pay that cost when
    // the user only changed the font size.
    // ------------------------------------------------------------------
    const QString previousTheme =
        settings.value(QStringLiteral("theme"), themes::dark().name).toString();
    const QString newTheme = ui->cmb_theme->currentText();
    settings.setValue("theme", newTheme);

    emit updateUiSettings(font);

    if (newTheme != previousTheme)
        emit themeChanged(newTheme);
}

void SettingsDialog::syntaxColorUpdateHandler(HighlightColor &colors)
{
    this->previewColors = colors;
    emit syntaxColorUpdate(previewColors);
}

void SettingsDialog::show()
{
    QSettings settings("Remini", "Remini");

    const QString fontFamily =
        settings.value("font", defaultMonospaceFont()).toString();
    const int fontSize = settings.value("fontsize", 11).toInt();
    const bool markdown = settings.value("markdown", true).toBool();
    const bool linewrap = settings.value("linewrap", true).toBool();
    const int stretch = settings.value("stretch", QFont::Unstretched).toInt();
    const int weight = settings.value("weight", QFont::Normal).toInt();
    vaultRootPath = settings.value("vaultPath", QDir::currentPath()).toString();

    ui->cmb_mkState->setCurrentIndex(markdown ? 1 : 0);
    ui->cmb_lineWrap->setCurrentIndex(linewrap ? 1 : 0);
    ui->edit_vaultRootPath->setText(vaultRootPath);

    // ------------------------------------------------------------------
    // Restore the saved theme selection.
    //
    // Without this the combo box would always show its first entry (Light)
    // no matter what the user last picked, so opening the dialog and
    // pressing OK would silently overwrite the theme back to Light.
    //
    // QSignalBlocker guards against any connection on currentTextChanged
    // firing during this initial programmatic change.
    // ------------------------------------------------------------------
    const QString savedTheme =
        settings.value(QStringLiteral("theme"), themes::dark().name).toString();
    const int themeIndex = ui->cmb_theme->findText(savedTheme);
    if (themeIndex >= 0)
    {
        QSignalBlocker blocker(ui->cmb_theme);
        ui->cmb_theme->setCurrentIndex(themeIndex);
    }

    // If the stored font is not present (e.g. "Cascadia Mono" on Linux),
    // findText() returns -1. Fall back to the first available font so the
    // combo box is never left empty.
    int fontIndex = ui->cmb_font->findText(fontFamily);
    if (fontIndex < 0)
    {
        fontIndex = ui->cmb_font->findText(defaultMonospaceFont());
        if (fontIndex < 0 && ui->cmb_font->count() > 0)
            fontIndex = 0;
    }
    if (fontIndex >= 0)
        ui->cmb_font->setCurrentIndex(fontIndex);

    ui->ledit_font_size->setText(QString::number(fontSize));

    int index_weight;
    switch (weight)
    {
    case QFont::Thin:
        index_weight = 0;
        break;
    case QFont::Light:
        index_weight = 1;
        break;
    case QFont::Normal:
        index_weight = 2;
        break;
    case QFont::Medium:
        index_weight = 3;
        break;
    case QFont::DemiBold:
        index_weight = 4;
        break;
    case QFont::Bold:
        index_weight = 5;
        break;
    case QFont::ExtraBold:
        index_weight = 6;
        break;
    case QFont::Black:
        index_weight = 7;
        break;
    default:
        index_weight = 2;
    }
    ui->cmb_weight->setCurrentIndex(index_weight);

    int index_stretch = 0;
    switch (stretch)
    {
    case QFont::AnyStretch:
        index_stretch = 0;
        break;
    case QFont::UltraCondensed:
        index_stretch = 1;
        break;
    case QFont::ExtraCondensed:
        index_stretch = 2;
        break;
    case QFont::Condensed:
        index_stretch = 3;
        break;
    case QFont::SemiCondensed:
        index_stretch = 4;
        break;
    case QFont::Unstretched:
        index_stretch = 5;
        break;
    case QFont::SemiExpanded:
        index_stretch = 6;
        break;
    case QFont::Expanded:
        index_stretch = 7;
        break;
    case QFont::ExtraExpanded:
        index_stretch = 8;
        break;
    case QFont::UltraExpanded:
        index_stretch = 9;
        break;
    default:
        index_stretch = 6;
    }
    ui->cmb_stretch->setCurrentIndex(index_stretch);

    QFont font(fontFamily, fontSize, weight);
    font.setStretch(stretch);
    ui->txt_preview->setFont(font);
    ui->txt_preview->update();

    QDialog::show();
}

const QString SettingsDialog::getVaultRootPath()
{
    const QString currentPath = QDir::currentPath();
    // QDir::filePath() handles the separator, avoiding "path"+"file"
    // concatenation bugs when CONFIG_FILE_NAME has no leading slash.
    const QString filePath = QDir(currentPath).filePath(CONFIG_FILE_NAME);
    QFile file(filePath);

    if (file.exists())
    {
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            vaultRootPath = stream.readAll();
        }
    }
    else
    {
        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream stream(&file);
            stream << currentPath;
            vaultRootPath = currentPath;
        }
    }
    file.close();
    return vaultRootPath;
}
