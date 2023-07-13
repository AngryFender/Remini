#ifndef STARTUPTEXT_H
#define STARTUPTEXT_H

#include <QString>

const QString startupText = QStringLiteral("Keyboard Shortcuts:\n"
                                           " - Alt + 1 to search file/folders\n"
                                           "        - \"/\" + name to search folders\n"
                                           " - Alt + 2 to navigate files/folders\n"
                                           " - Alt + 3 to start editing files\n"
                                           " - Quick double shift for searching words in all files\n"
                                           " - \"ch\" or \"check\" followed by tab for checkmark\n"
                                           " - \"uch\" or \"uncheck\" followed by tab for empty box\n"
                                           " - \"link\" or \"lk\"followed by tab for link markdown symbols\n"
                                           " - Right click \"Disable Markdown\" will show markdown symbols\n"
                                           " - Right click \"Enable Markdown\" will hide markdown symbols\n"
                                           "\n"
                                           "Path Config:\n"
                                           " - Check config.txt in the exe folder to update the root path\n"
                                           "\n"
                                           );

#endif // STARTUPTEXT_H
