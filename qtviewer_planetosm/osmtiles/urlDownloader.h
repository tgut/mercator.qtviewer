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
#include <QSet>
namespace QTVOSM{
	struct tag_download_tasks{
		QString str_url;
		QString str_destinDir;
		QString str_destinFile;
		bool operator == (const tag_download_tasks & t1)
		{
			return t1.str_url==str_url && t1.str_destinDir == str_destinDir && t1.str_destinFile == str_destinFile;
		}
	};

	/*!
	 \brief	urlDownloader is a toolclass for tile download.

	 \class		urlDownloader urlDownloader.h "qtviewer_planetosm/osmtiles/urlDownloader.h"
	*/
	class urlDownloader : public QObject
	{
		Q_OBJECT
	public:
		urlDownloader(QObject * pParent, int nMaxAsynThread = 5);
		~urlDownloader();
		void addDownloadUrl(const QString &sourceUrl,const QString & DestinDir, const QString & filename,bool newerFirst = true);
		//view CurrentTasks
		QVector<tag_download_tasks> current_tasks();
	protected:
		//the QSet to avoid repeatedly download same tile.
		QSet<QString> m_set_tileAddress;
		QNetworkAccessManager * m_pNetMge;
		QList<tag_download_tasks> m_listTask;
		QMap <QNetworkReply * ,tag_download_tasks >m_map_pendingTasks;
		QMutex m_mutex_protect;
		int m_nMaxAsynThread;
	protected slots:
		void newTaskAdded();
		void replyFinished(QNetworkReply*);
	signals:
		void evt_all_taskFinished();
		void evt_doNextJob();
	};
}
#endif // URLDOWNLOADER_H
