#include "highlighter.h"

Highlighter::Highlighter(QObject *parent)
    : QSyntaxHighlighter(parent),initialised(false)
{
}

void Highlighter::highlightBlock(const QString &text)
{
    if(!initialised)
        return;

    for (const HighlightingRule &rule : std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
                keywordFormat.setForeground(syntaxColor.keyword);
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

//    QRegularExpressionMatchIterator i = m_regex.globalMatch(text);
//    while (i.hasNext()) {
//        QRegularExpressionMatch match = i.next();
//        int start = match.capturedStart();
//        int end = match.capturedEnd();

//        setFormat(start, 1,headingFormat);
//        setFormat(start+1, end - start, m_format);
//    }
}

void Highlighter::initColors()
{
    HighlightingRule rule;

    keywordFormat.setForeground(syntaxColor.keyword);
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


//    rule.format.setFontPointSize(15);
//    //m_format.setFontPointSize(15);
//    highlightingRules.append(rule);

//    classFormat.setFontWeight(QFont::Bold);
//    classFormat.setForeground(QColor("#EDCAE7"));
////    m_regex =QRegularExpression(QStringLiteral("#[^\\s+#]+.*"));
////    m_regex =QRegularExpression(QStringLiteral("^#[^\\s#]+.*"));    //markdown heading 1
//    rule.format = classFormat;
//    rule.format.setFontPointSize(15);
////    highlightingRules.append(rule);

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(syntaxColor.argument);
//    rule.pattern = QRegularExpression(QStringLiteral("(?<=\\s)--?[a-zA-Z0-9]+"));
    rule.pattern = QRegularExpression(QStringLiteral("(?<=\\s)--?[a-zA-Z0-9_]+"));
    rule.format = classFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(syntaxColor.comment);
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(syntaxColor.comment);

    quotationFormat.setForeground(syntaxColor.quote);
    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(syntaxColor.method);
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

    initialised = true;
}

void Highlighter::syntaxColorUpdateHandler(HighlightColor &colors)
{
    syntaxColor.argument = colors.argument;
    syntaxColor.keyword = colors.keyword;
    syntaxColor.method = colors.method;
    syntaxColor.comment = colors.comment;
    syntaxColor.quote = colors.quote ;
    syntaxColor.type = colors.type ;

    initColors();
}
