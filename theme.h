#ifndef THEME_H
#define THEME_H

#include <QColor>
#include <QString>

struct HighlightColor
{
    QColor type;
    QColor method;
    QColor argument;
    QColor comment;
    QColor quote;
    QColor keyword;
    QColor searchMatch;
};

const QString lightTheme = QStringLiteral( "QMainWindow {"
                                           "	background-color:#f4f2f1;"
                                           "    border-style: none;"
                                           "}"
                                           "QDialog {"
                                           "	background-color:#f4f2f1;"
                                           "}"
                                           "QColorDialog {"
                                           "	background-color:#f4f2f1;"
                                           "}"
                                           "MkEdit{"
                                           "	qproperty-blockColor: #e8e4e1;"
                                           "    qproperty-methodColor: #9696fd;"
                                           "    qproperty-typeColor: #D291BC;"
                                           "    qproperty-argumentColor: #fd96c1;"
                                           "    qproperty-commentColor: #D291BC;"
                                           "    qproperty-quoteColor: #81b01e;"
                                           "    qproperty-keywordColor: #fd96a7;"
                                           "	background-color:#f4f2f1;"
                                           "	color: black;"
                                           "	selection-background-color: #3379B7;"
                                           "	padding-left:15; padding-top:10; padding-bottom:10; padding-right:10;"
                                           "}"
                                           "QTreeView {"
                                           "    background-color: #f4f2f1;"
                                           "    border-width: 1px; border-style: outset; border-color: gray transparent transparent transparent;"
                                           "    color: black; /* set the text color to white */"
                                           "    image: none;"
                                           "    padding-left:10; padding-right:10; padding-top:5;"
                                           "}"
                                           "QPushButton {"
                                           "    background-color: #d6d6d6;"
                                           "    color: black;"
                                           "    border-style: outset;"
                                           "    border-width: 2px;"
                                           "    border-radius: 7px;"
                                           "    border-color: transparent;"
                                           "    font: bold 14px;"
                                           "    min-width: 6em;"
                                           "    padding: 3px;"
                                           "}"
                                           "QPushButton:pressed {"
                                           "    background-color: #d6d6d6;"
                                           "    border-style: inset;"
                                           "}"
//                                           "QLabel {"
//                                           "	background: #f4f2f1;"
//                                           "    border-width: 1px; border-style: outset; border-color: gray transparent transparent transparent;"
//                                           "	color: black;"
//                                           "    padding-left:10; padding-top:10; "
//                                           "}"
                                           "QSplitter::handle {"
                                           "    background-color: #f4f2f1;"
                                           "}"
                                           ""
                                           "QSplitter::handle:horizontal {"
                                           "    width: 2px;"
                                           "}"
                                           ""
                                           "QSplitter::handle:vertical {"
                                           "    height: 2px;"
                                           "}"
                                           ""
                                           "QScrollBar:vertical {"
                                           "    background: transparent;"
                                           "    width: 8px; /* Set the width of the scrollbar */"
                                           "    margin: 0px 0px 0px 0px; /* Set the margin */"
                                           "}"
                                           ""
                                           "QScrollBar::handle:vertical {"
                                           "    background: #adacac;"
                                           "    border-radius: 2px; /* Set the border radius of the handle */"
                                           "}"
                                           ""
                                           "QScrollBar::add-line:vertical,"
                                           "QScrollBar::sub-line:vertical {"
                                           "    height: 0px;"
                                           "}"
                                           ""
                                           "QScrollBar::add-page:vertical,"
                                           "QScrollBar::sub-page:vertical {"
                                           "    background: none;"
                                           "}"
                                           ""
                                           "QScrollBar:horizontal {"
                                           "	background: transparent;"
                                           "    border: none;"
                                           "    height: 8px;"
                                           "}"
                                           ""
                                           "QScrollBar::handle:horizontal {"
                                           "    background: #adacac;"
                                           "    border-radius: 4px;"
                                           "}"
                                           ""
                                           "QScrollBar::add-line:horizontal,"
                                           "QScrollBar::sub-line:horizontal {"
                                           "    height: 0px;"
                                           "}"
                                           ""
                                           "QScrollBar::add-page:horizontal,"
                                           "QScrollBar::sub-page:horizontal {"
                                           "    background: none;"
                                           "}"
                                           "QWidget{"
                                           "    border-style: none;"
                                           "}");

const QString darkTheme = QStringLiteral( "QMessageBox { background-color: #353535; } "
                                          "QMainWindow {"
                                          "	background-color:#353535;"
                                          "	border-color: transparent;"
                                          "}"
                                          "QDialog {"
                                          "	background-color:#353535;"
                                          "}"
                                          "QColorDialog {"
                                          "	background-color:#353535;"
                                          "}"
                                         "MkEdit{"
                                          "    qproperty-blockColor: #1E1E1E;"
                                          "    qproperty-methodColor: #D291BC;"
                                          "    qproperty-typeColor: #D291BC;"
                                          "    qproperty-argumentColor: #fdfd96;"
                                          "    qproperty-commentColor: #737373;"
                                          "    qproperty-quoteColor: #92DEE4;"
                                          "    qproperty-searchMatchColor: #2BB340;"
                                          "    qproperty-keywordColor: #FF9991;"
                                          "		background-color:#353535;"
                                          "		border-style: none;"
                                          " 	color: white;"
                                          "		selection-background-color: #3379B7;"
                                          "     margin-top:10; margin-bottom:10;"
                                          "     margin-left:40; margin-right:20;"
                                          "}"
//                                          "QLabel {"
//                                          "	background:#353535;"
//                                          "	color: white;"
//                                          " padding-left:35;	padding-top:10;"
//                                          " qproperty-alignment: 'AlignVCenter | AlignLeft'; "
//                                          "}"
                                          "QTreeView {"
                                          "    background-color: #353535;"
                                          "    border-style: none;"
                                          "    color: white; /* set the text color to white */"
                                          "    image: none;"
                                          "    padding-left:10; padding-right:10; padding-top:5;"
                                          "}"
                                         "QTreeView::branch{"
                                             "background: #353535;"
                                         "}"
                                          "QTreeView::branch:has-siblings:!adjoins-item {"
                                             "border-image: url(:/icons/line) 0;"
                                             "selection-background-color: #353535;"
                                          "}"
                                         "QTreeView::branch:has-siblings:adjoins-item {"
                                         "border-image: url(:/icons/line) 0;"
                                         "selection-background-color:#353535;"
                                         "}"
                                          "QTreeView::branch:!has-siblings:adjoins-item{"
                                             "border-image: url(:/icons/angle) 0;"
                                             "selection-background-color: #353535;"
                                          "}"
                                            "QTreeView::branch:has-children:!has-siblings:closed,"
                                            "  QTreeView::branch:closed:has-children:has-siblings {"
                                            " border-image: url(:/icons/arrow);"
                                             "selection-background-color: #353535;"
                                            "}"

                                            "      QTreeView::branch:open:has-children:!has-siblings,"
                                            "  QTreeView::branch:open:has-children:has-siblings  {"
                                            "    border-image: url(:/icons/downarrow);"
                                             "selection-background-color: #353535;"
                                            "}"
                                          "QTreeView::item:selected {"
                                          "    background-color:#707070;"
                                          "    color: white;"
                                          "    image: none;"
                                          "}"
                                          "QLineEdit {"
                                          "    background-color:#707070;"
                                          "    color: white;"
                                          "    border-radius: 2px;"
                                          "    padding-left:3; padding-right:0; padding-top:0;"
                                          "    margin:  0px;"
                                          "}"
                                          "QPushButton {"
                                          "    background-color: #707070;"
                                          "    color: white;"
                                          "    border-style: outset;"
                                          "    border-width: 2px;"
                                          "    border-radius: 6px;"
                                          "    border-color: transparent;"
                                          "    font: bold 14px;"
                                          "    min-width: 7em;"
                                          "    padding: 3px;"
                                          "}"
                                          "QPushButton:pressed {"
                                          "    background-color: #707070;"
                                          "    border-style: inset;"
                                          "}"
                                          "QSplitter::handle {"
                                          "    background-color: #707070;"
                                          "}"
                                          ""
                                          "QSplitter::handle:horizontal {"
                                          "    width: 2px;"
                                          "}"
                                          ""
                                          "QSplitter::handle:vertical {"
                                          "    height: 2px;"
                                          "}"
                                          ""
                                          "QScrollBar:vertical {"
                                          "    background: transparent;"
                                          "    width: 8px; /* Set the width of the scrollbar */"
//                                          "    padding-left: 20px;"
                                          "    margin: 0px 0px 0px 0px; /* Set the margin */"
                                          "}"
                                          ""
                                          "QScrollBar::handle:vertical {"
                                          "    background: gray;"
                                          "    border-radius: 2px; /* Set the border radius of the handle */"
                                          "}"
                                          ""
                                          "QScrollBar::add-line:vertical,"
                                          "QScrollBar::sub-line:vertical {"
                                          "    height: 0px;"
                                          "}"
                                          ""
                                          "QScrollBar::add-page:vertical,"
                                          "QScrollBar::sub-page:vertical {"
                                          "    background: none;"
                                          "}"
                                          ""
                                          "QScrollBar:horizontal {"
                                          "	background: transparent;"
                                          "    border: none;"
                                          "    height: 8px;"
                                          "}"
                                          ""
                                          "QScrollBar::handle:horizontal {"
                                          "    background: gray;"
                                          "    border-radius: 4px;"
                                          "}"
                                          ""
                                          "QScrollBar::add-line:horizontal,"
                                          "QScrollBar::sub-line:horizontal {"
                                          "    height: 0px;"
                                          "}"
                                          ""
                                          "QScrollBar::add-page:horizontal,"
                                          "QScrollBar::sub-page:horizontal {"
                                          "    background: none;"
                                          "}"
                                          "QWidget{"
                                          "    border-style: none;"
                                          "}"
                                          "QLineEdit {"
                                          "    border-style: solid;"
                                          "    border-width: 1px;"
                                          "	   background-color:#353535;"
                                          "    border-radius: 3px;"
                                          "    border-color: #707070;"
                                          "}"
);

#endif // THEME_H
