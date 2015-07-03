

#include "ConfigFileDialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>

#include "logging.h"

ConfigFileDialog::ConfigFileDialog(QWidget *parent, Qt::WindowFlags flags)
	: QFileDialog(parent, flags)
{
	init();
}

ConfigFileDialog::ConfigFileDialog(QWidget *parent, const QString &caption, const QString &directory, const QString &filter, const QString &comment)
	: QFileDialog(parent, caption, directory, filter)
{
	init();
	m_comment->setText(comment);
}

ConfigFileDialog::~ConfigFileDialog()
{
}

void ConfigFileDialog::init(void)
{
	setOption(DontUseNativeDialog);
	setAcceptMode(AcceptSave);
	setDefaultSuffix("mcfg");

	QGridLayout *grid = dynamic_cast<QGridLayout *>(layout());
	int rowCount = grid->rowCount();
	QWidget *w00 = grid->itemAtPosition(rowCount - 2, 0)->widget();
	QWidget *w10 = grid->itemAtPosition(rowCount - 1, 0)->widget();
	QWidget *w01 = grid->itemAtPosition(rowCount - 2, 1)->widget();
	QWidget *w11 = grid->itemAtPosition(rowCount - 1, 1)->widget();
	QWidget *w02 = grid->itemAtPosition(rowCount - 1, 2)->widget();

	grid->addWidget(w00, rowCount -1, 0, 1, 1);
	grid->addWidget(w10, rowCount, 0, 1, 1);
	grid->addWidget(w01, rowCount -1, 1, 1, 1);
	grid->addWidget(w11, rowCount, 1, 1, 1);
	grid->addWidget(w02, rowCount -1, 2, 2, 1);

	rowCount = grid->rowCount();
	QLabel *l = new QLabel(tr("&Comment:"));
	grid->addWidget(l, rowCount - 3, 0);
	m_comment = new QLineEdit(this);
	grid->addWidget(m_comment, rowCount - 3, 1, 1, 2);
	l->setBuddy(m_comment);

	QCheckBox *cb = new QCheckBox(tr("Automatically select file name e&xtension (.%1)").arg(defaultSuffix()), this);
	cb->setCheckState(Qt::Checked);
	connect(cb, SIGNAL(stateChanged(int)), this, SLOT(autoExtensionChanged(int)));
	grid->addWidget(cb, rowCount, 0, 1, 3);

	QWidget *listView(NULL);
	QList<QWidget *> widgets = this->findChildren<QWidget *>();
	foreach (QWidget *i, widgets)
		if (i->objectName() == "listView")
			listView = i;
	setTabOrder(listView, cb);
	setTabOrder(cb, m_comment);
	setTabOrder(m_comment, w10);
}

QString ConfigFileDialog::comment(void)
{
	QString s = m_comment->text();
	if (s.isEmpty())
		s = "QMesyDAQ configuration file";
	return s;
}

void ConfigFileDialog::autoExtensionChanged(int state)
{
	if (state == Qt::Checked)
		setDefaultSuffix("mcfg");
	else
		setDefaultSuffix("");
}
