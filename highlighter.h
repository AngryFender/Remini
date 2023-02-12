#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QTextEdit>
#include <QTextDocument>

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit Highlighter(QTextDocument *parent = nullptr);


    void setView(QTextEdit *view);
protected:
    void highlightBlock(const QString &text) override;
private:
    QTextEdit *textEdit;

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

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat headingFormat;

    QTextCharFormat formatBlock;
    QTextCharFormat formatNormal;

    QTextDocument *doc;
};

#endif // HIGHLIGHTER_H
