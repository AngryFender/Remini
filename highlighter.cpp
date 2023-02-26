#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(QColor("#FF9991"));
//    keywordFormat.setForeground(mKeywordColor);
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
        QStringLiteral("\\bchar\\b"), QStringLiteral("\\bclass\\b"), QStringLiteral("\\bconst\\b"),
        QStringLiteral("\\bdouble\\b"), QStringLiteral("\\benum\\b"), QStringLiteral("\\bexplicit\\b"),
        QStringLiteral("\\bfriend\\b"), QStringLiteral("\\binline\\b"), QStringLiteral("\\bint\\b"),
        QStringLiteral("\\blong\\b"), QStringLiteral("\\bnamespace\\b"), QStringLiteral("\\boperator\\b"),
        QStringLiteral("\\bprivate\\b"), QStringLiteral("\\bprotected\\b"), QStringLiteral("\\bpublic\\b"),
        QStringLiteral("\\bshort\\b"), QStringLiteral("\\bsignals\\b"), QStringLiteral("\\bsigned\\b"),
        QStringLiteral("\\bslots\\b"), QStringLiteral("\\bstatic\\b"), QStringLiteral("\\bstruct\\b"),
        QStringLiteral("\\btemplate\\b"), QStringLiteral("\\btypedef\\b"), QStringLiteral("\\btypename\\b"),
        QStringLiteral("\\bunion\\b"), QStringLiteral("\\bunsigned\\b"), QStringLiteral("\\bvirtual\\b"),
        QStringLiteral("\\bvoid\\b"), QStringLiteral("\\bvolatile\\b"), QStringLiteral("\\bbool\\b"),
        QStringLiteral("\\bcmake\\b")
    };
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);

    }


    //rule.format.setFontPointSize(15);
    m_format.setFontPointSize(15);
    highlightingRules.append(rule);

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(QColor("#EDCAE7"));
//    m_regex =QRegularExpression(QStringLiteral("#[^\\s+#]+.*"));
    m_regex =QRegularExpression(QStringLiteral("^#[^\\s#]+.*"));    //markdown heading 1
    rule.format = classFormat;
    rule.format.setFontPointSize(15);
    highlightingRules.append(rule);

    classFormat.setForeground(QColor("#FAB733"));
    rule.pattern = QRegularExpression(QStringLiteral("-[a-zA-Z]+"));
    rule.format = classFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::gray);
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::gray);

    quotationFormat.setForeground(QColor("#92DEE4"));
    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(QColor("#D291BC"));
//    functionFormat.setForeground(mMethodColor);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));

    headingFormat.setFontPointSize(1);
    headingFormat.setForeground(Qt::white);

    formatBlock.setFontPointSize(1);
    formatBlock.setForeground(Qt::white);
    regexCodeBlock.setPattern("^```+.*");

}

void Highlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
                keywordFormat.setForeground(mKeywordColor);
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }

    QRegularExpressionMatchIterator i = m_regex.globalMatch(text);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        int start = match.capturedStart();
        int end = match.capturedEnd();

        setFormat(start, 1,headingFormat);
        setFormat(start+1, end - start, m_format);
    }
}

void Highlighter::setView(QTextEdit *textEdit)
{
    this->textEdit = textEdit;
}

QColor Highlighter::typeColor() const
{
    return mTypeColor;
}

QColor Highlighter::methodColor() const
{
    return mMethodColor;
}

QColor Highlighter::argumentColor() const
{
    return mArgumentColor;
}

QColor Highlighter::commentColor() const
{
    return mCommentColor;
}

QColor Highlighter::quoteColor() const
{
    return mQuoteColor;
}

QColor Highlighter::keywordColor() const
{
    return mKeywordColor;
}

void Highlighter::setTypeColor(const QColor &color)
{
    mTypeColor = color;
    emit typeColorChanged(color);
}

void Highlighter::setMethodColor(const QColor &color)
{
    mMethodColor = color;
    emit methodColorChanged(color);
}

void Highlighter::setArgumentColor(const QColor &color)
{
    mArgumentColor = color;
    emit argumentColorChanged(color);
}

void Highlighter::setCommentColor(const QColor &color)
{
    mCommentColor = color;
    emit commentColorChanged(color);
}

void Highlighter::setQuoteColor(const QColor &color)
{
    mQuoteColor = color;
    emit quoteColorChanged(color);
}

void Highlighter::keywordColor(const QColor &color)
{
    mKeywordColor = color;
    rehighlight();
    emit keywordColorChanged(color);
}
