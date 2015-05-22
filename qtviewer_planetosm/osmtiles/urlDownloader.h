#ifndef URLDOWNLOADER_H
#define URLDOWNLOADER_H
#include <QObject>
#include <QVector>
#include <QThread>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMutex>
#include <QMap>

class urlDownloader : public QObject
{
	Q_OBJECT
public:
	urlDownloader(QObject * pParent, int nMaxAsynThread = 1);
	~urlDownloader();
	void addDownloadUrl(QString sourceUrl,QString DestinDir, QString filename);
protected:
	QNetworkAccessManager * m_pNetMge;
	QList<QString> m_list_source;
	QList<QString> m_list_destinDir;
	QList<QString> m_list_destinFile;
	QMap <QNetworkReply * , QVector<QString> >m_map_pendingTasks;
	QMutex m_mutex_protect;
	int m_nMaxAsynThread;
protected slots:
	void newTaskAdded();
	void replyFinished(QNetworkReply*);
signals:
	void evt_all_taskFinished();
	void evt_doNextJob();
};

#endif // URLDOWNLOADER_H
