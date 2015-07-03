
#ifndef CONFIG_FILE_DIALOG_H
#define CONFIG_FILE_DIALOG_H

#include <QFileDialog>

class QLineEdit;

class ConfigFileDialog : public QFileDialog
{
Q_OBJECT

public:
	ConfigFileDialog(QWidget *parent, Qt::WindowFlags flags);

	ConfigFileDialog(QWidget *parent = 0, const QString &caption = QString(), const QString &directory = QString(), const QString &filter = QString(), const QString &comment = QString());

	virtual ~ConfigFileDialog();

	QString comment(void);

private slots:
	void autoExtensionChanged(int state);

private:
	void init(void);

private:
	QLineEdit *m_comment;
};

#endif
