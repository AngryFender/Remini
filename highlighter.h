#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QTextEdit>
#include <QTextDocument>

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
//    Q_PROPERTY(QColor typeColor READ typeColor WRITE setTypeColor NOTIFY typeColorChanged)
//    Q_PROPERTY(QColor methodColor READ methodColor WRITE setMethodColor NOTIFY methodColorChanged)
//    Q_PROPERTY(QColor argumentColor READ argumentColor WRITE setArgumentColor NOTIFY argumentColorChanged)
//    Q_PROPERTY(QColor commentColor READ commentColor WRITE setCommentColor NOTIFY commentColorChanged)
//    Q_PROPERTY(QColor quoteColor READ quoteColor WRITE setQuoteColor NOTIFY quoteColorChanged)
//    Q_PROPERTY(QColor keywordColor READ keywordColor WRITE keywordColor NOTIFY keywordColorChanged)
    Q_PROPERTY(QColor keywordColor MEMBER mKeywordColor NOTIFY keywordColorChanged)
public:
    explicit Highlighter(QTextDocument *parent = nullptr);
    void setView(QTextEdit *view);
    QColor typeColor() const;
    QColor methodColor() const;
    QColor argumentColor() const;
    QColor commentColor() const;
    QColor quoteColor() const;
    QColor keywordColor() const;
    void setTypeColor(const QColor& color);
    void setMethodColor(const QColor& color);
    void setArgumentColor(const QColor& color);
    void setCommentColor(const QColor& color);
    void setQuoteColor(const QColor& color);
    void keywordColor(const QColor& color);
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

    QColor mTypeColor;
    QColor mMethodColor;
    QColor mArgumentColor;
    QColor mCommentColor;
    QColor mQuoteColor;
    QColor mKeywordColor;
signals:
    void typeColorChanged(const QColor &color);
    void methodColorChanged(const QColor &color);
    void argumentColorChanged(const QColor &color);
    void commentColorChanged(const QColor &color);
    void quoteColorChanged(const QColor &color);
    void keywordColorChanged(const QColor &color);
};

#endif // HIGHLIGHTER_H
