#include <QMainWindow>

class Plot;

class MainWindow: public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget * = NULL);

private slots:
	void showSpectrogram(bool);

private:
	Plot *m_plot;
};

