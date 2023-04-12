#include "ControlPanel.h"


QGroupBox *MSDScriptControlPanel::createExecModeRadioButtonGroup()
{
    QGroupBox *groupBox = new QGroupBox();

    execModeButtonGroup = new QButtonGroup(groupBox);

    interpRadioButton = new QRadioButton("Calculate the Result");
    prettyPrintRadioButton = new QRadioButton("Beautify the Expression");

    execModeButtonGroup->addButton(interpRadioButton);
    execModeButtonGroup->addButton(prettyPrintRadioButton);

    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->addWidget(interpRadioButton);
    hBoxLayout->addWidget(prettyPrintRadioButton);

    groupBox->setLayout(hBoxLayout);

    return groupBox;
}

MSDScriptControlPanel::MSDScriptControlPanel(QWidget *parent)
    : QWidget{parent}
{
    setFixedSize(1000, 600);

    setWindowTitle("Grammar Calculator");

    expressionLabel = new QLabel("Expression : ");
    expressionTextEdit = new QTextEdit();

    importExpressionFromFileButton = new QPushButton("Import Expression From File");

    execModeLabel = new QLabel("Execution Mode : ");

    submitButton = new QPushButton("Submit");

    resultLabel = new QLabel("Result : ");
    resultTextEdit = new QTextEdit();

    resetButton = new QPushButton("Reset");

    formLayout = new QFormLayout(parent);
    formLayout->addRow(expressionLabel, expressionTextEdit);
    formLayout->addRow(importExpressionFromFileButton);
    formLayout->addRow(execModeLabel, createExecModeRadioButtonGroup());
    formLayout->addRow(submitButton);
    formLayout->addRow(resultLabel, resultTextEdit);
    formLayout->addRow(resetButton);

    connect(importExpressionFromFileButton, &QPushButton::released, this, &MSDScriptControlPanel::importExpressionFromFile);

    connect(resetButton, &QPushButton::released, this, &MSDScriptControlPanel::handleReset);

    connect(submitButton, &QPushButton::released, this, &MSDScriptControlPanel::handleSubmit);

    formLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    setLayout(formLayout);
}


void MSDScriptControlPanel::clearExecModeButtonGroup() {
    execModeButtonGroup->setExclusive(false);
    foreach (QAbstractButton* button, execModeButtonGroup->buttons()) {
        button->setChecked(false);
    }
    execModeButtonGroup->setExclusive(true);
}

void MSDScriptControlPanel::handleReset() {
    // Show a confirmation message
    QMessageBox::StandardButton confirmation = QMessageBox::question(this, tr("Reset Confirmation"), tr("Are you sure you want to reset all fields?"), QMessageBox::Yes | QMessageBox::No);

    // Check if the user clicked Yes
    if (confirmation == QMessageBox::Yes) {
        expressionTextEdit->clear();
        clearExecModeButtonGroup();
        resultTextEdit->clear();
    }
}

void MSDScriptControlPanel::importExpressionFromFile() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("Text Files (*.txt)"));

    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            file.close();

            expressionTextEdit->setText(content);
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to open file for reading"));
        }
    }
}


void MSDScriptControlPanel::handleSubmit() {
    // clear the last result
    resultTextEdit->clear();

    if (execModeButtonGroup->checkedButton() == nullptr) {
        QMessageBox::warning(this, "Execution Mode Required", "Please select an execution mode before submitting.");
        return;
    }


    QString execMode = execModeButtonGroup->checkedButton()->text();


    QString scriptExpression = expressionTextEdit->toPlainText();

    try {
        auto expr = parse_expression_str(scriptExpression.toStdString());
        std::string result;
        if (execMode == interpRadioButton->text()) {
            result = expr->interp()->to_string();
        } else if (execMode == prettyPrintRadioButton->text()) {
            result = expr->to_pretty_string();
        }
        resultTextEdit->setText(QString::fromStdString(result));
    } catch (const std::runtime_error& e) {
        QMessageBox::warning(this, "Runtime Error", QString::fromStdString(e.what()));
    }
}

