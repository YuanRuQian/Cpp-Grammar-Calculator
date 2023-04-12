#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QButtonGroup>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include <QRadioButton>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "parse.h"
#include "expr.hpp"
#include "val.hpp"
#include "env.h"

class MSDScriptControlPanel : public QWidget
{
    Q_OBJECT
public:
    explicit MSDScriptControlPanel(QWidget *parent = nullptr);

private:
    QLabel* expressionLabel;
    QTextEdit* expressionTextEdit;

    QPushButton* importExpressionFromFileButton;

    QLabel* execModeLabel;
    QButtonGroup* execModeButtonGroup;
    QRadioButton* interpRadioButton;
    QRadioButton* prettyPrintRadioButton;
    QGroupBox* createExecModeRadioButtonGroup();

    QPushButton* submitButton;

    QLabel* resultLabel;
    QTextEdit* resultTextEdit;

    QPushButton* resetButton;

    QFormLayout *formLayout;

private slots:
    void clearExecModeButtonGroup();
    void handleReset();
    void importExpressionFromFile();
    void handleSubmit();
};

#endif // CONTROLPANEL_H
