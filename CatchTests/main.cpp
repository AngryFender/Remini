#define CATCH_CONFIG_RUNNER
#include <QApplication>
#include <QTextEdit>
#include <QtGui/QGuiApplication>
#include <catch2/catch.hpp>

int main(int argc, char** argv)
{
    int _argc = 0;
    QApplication* app = new QApplication(_argc, nullptr);
    return Catch::Session().run(argc, argv);

    delete app;
}
