#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QTextEdit>
#include <QTextDocument>
#include <theme.h>

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit Highlighter(QObject *parent = nullptr);

public slots:
    void syntaxColorUpdateHandler(HighlightColor& colors);
    void updateSearchText(const QString &text);
protected:
    void highlightBlock(const QString &text) override;
private:

    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QRegularExpression m_regex;
    QTextCharFormat m_format;

    QList<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;
    QRegularExpression regexCodeBlock;
    QRegularExpression searchMatchExpression;

    QTextCharFormat tickMarkFormat;
    QTextCharFormat unTickMarkFormat;
    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat searchMatchFormat;

    QTextCharFormat formatBlock;
    QTextCharFormat formatNormal;

    QTextDocument *doc;

    HighlightColor syntaxColor;

    bool initialised;
    void initColors();
};

#endif // HIGHLIGHTER_H
