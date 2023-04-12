 #include <QApplication>
#include "ControlPanel.h"

int main(int argc, char **argv) {
    QApplication app (argc, argv);

    MSDScriptControlPanel* msdscriptControlPanel = new MSDScriptControlPanel();

    msdscriptControlPanel->show();

    return app.exec();
}
