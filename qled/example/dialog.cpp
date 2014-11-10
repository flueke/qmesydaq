#include "dialog.h"
#include "ui_dialog.h"
#include <QSignalMapper>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    QSignalMapper *colorMapper = new QSignalMapper(this);
    colorMapper->setMapping(ui->radioButton, QLed::Red);
    colorMapper->setMapping(ui->radioButton_2, QLed::Green);
    colorMapper->setMapping(ui->radioButton_3, QLed::Yellow);
    colorMapper->setMapping(ui->radioButton_4, QLed::Grey);
    colorMapper->setMapping(ui->radioButton_5, QLed::Orange);
    colorMapper->setMapping(ui->radioButton_6, QLed::Purple);
    colorMapper->setMapping(ui->radioButton_7, QLed::Blue);

    connect(ui->radioButton, SIGNAL(clicked()), colorMapper, SLOT(map()));
    connect(ui->radioButton_2, SIGNAL(clicked()), colorMapper, SLOT(map()));
    connect(ui->radioButton_3, SIGNAL(clicked()), colorMapper, SLOT(map()));
    connect(ui->radioButton_4, SIGNAL(clicked()), colorMapper, SLOT(map()));
    connect(ui->radioButton_5, SIGNAL(clicked()), colorMapper, SLOT(map()));
    connect(ui->radioButton_6, SIGNAL(clicked()), colorMapper, SLOT(map()));
    connect(ui->radioButton_7, SIGNAL(clicked()), colorMapper, SLOT(map()));

    connect(colorMapper, SIGNAL(mapped(int)), ui->qLed, SLOT(setOnColor(int)));

    QSignalMapper *shapeMapper = new QSignalMapper(this);
    shapeMapper->setMapping(ui->radioButton_8, QLed::Circle);
    shapeMapper->setMapping(ui->radioButton_9, QLed::Square);
    shapeMapper->setMapping(ui->radioButton_10, QLed::Triangle);
    shapeMapper->setMapping(ui->radioButton_11, QLed::Rounded);

    connect(ui->radioButton_8, SIGNAL(clicked()), shapeMapper, SLOT(map()));
    connect(ui->radioButton_9, SIGNAL(clicked()), shapeMapper, SLOT(map()));
    connect(ui->radioButton_10, SIGNAL(clicked()), shapeMapper, SLOT(map()));
    connect(ui->radioButton_11, SIGNAL(clicked()), shapeMapper, SLOT(map()));

    connect(shapeMapper, SIGNAL(mapped(int)), ui->qLed, SLOT(setShape(int)));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
