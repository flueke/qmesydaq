#include <QDialog>
#include <QUrl>
#include <QWebView>
#include <QCloseEvent>

#include "ui_website.h"

class WebsiteTool : public QDialog, public Ui_WebsiteTool
{
	Q_OBJECT
public:
    	WebsiteTool(const QString &site, QWidget *parent = NULL)
		: QDialog(parent)
	{
		setupUi(this);
//		site = settings.get('url', '');
		if (!site.isEmpty())
			webView->load(QUrl(site));
	}

protected:
	void closeEvent(QCloseEvent * /* event */)
	{
        	deleteLater();
        	accept();
	}
};
