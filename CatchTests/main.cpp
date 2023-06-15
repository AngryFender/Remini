#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <QtGui/QGuiApplication>
#include <QApplication>
#include <QTextEdit>

int main(int argc, char** argv)
{
    int _argc = 0;
    QApplication* app = new QApplication(_argc, nullptr);
    return Catch::Session().run(argc, argv);

    delete app;
}
