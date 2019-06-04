#include <QApplication>
#include <QDebug>
#include <SoftwareUpdateDialog.hpp>
#include <QTimer>
int main(int ac , char **av){
	if(ac < 4){
		qDebug() << "Usage: " << av[0] << " [APP NAME] [OLD VERSION] [NEW VERSION] [RELEASE NOTES]";
		return 0;
	}

	QApplication app(ac , av);
	QString appName(av[1]),
		oldVersion(av[2]),
		newVersion(av[3]),
		releaseNotes(av[4]);

	SoftwareUpdateDialog m_Dialog;
	
	QObject::connect(&m_Dialog , &SoftwareUpdateDialog::accepted , &app , &QApplication::quit);
	QObject::connect(&m_Dialog , &SoftwareUpdateDialog::rejected , &app , &QApplication::quit);
	
	QTimer::singleShot(1000 , [&](){
	m_Dialog.init(appName , oldVersion , newVersion , releaseNotes);
	});
	return app.exec();
}
