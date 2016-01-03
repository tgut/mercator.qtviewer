#include "urlDownloader.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QDir>
#include <assert.h>
namespace QTVOSM{
	urlDownloader::urlDownloader(QObject * pParent, int nMaxAsynThread)
		:QObject(pParent)
		,m_nMaxAsynThread(nMaxAsynThread)
	{
		m_pNetMge = 0;
		connect(this,SIGNAL(evt_doNextJob()),
				this,SLOT(newTaskAdded()),Qt::QueuedConnection
				);
	}
	urlDownloader::~urlDownloader()
	{
		if (m_pNetMge)
			m_pNetMge->deleteLater();
	}
	void urlDownloader::replyFinished(QNetworkReply* rply)
	{
		bool needDo = false;
		bool allFinished = false;
		m_mutex_protect.lock();
		if (m_map_pendingTasks.contains(rply)==true)
		{
			const tag_download_tasks & tk = m_map_pendingTasks[rply];
			if (rply->error()==QNetworkReply::NoError)
			{
				QString strDir = tk.str_destinDir;
				QString strFile = tk.str_destinFile;
				//mkdir
				QDir dir(strDir);
				dir.mkpath(strDir);
				//saveFile
				QFile file(strDir+"/"+strFile);
				if (file.open(QIODevice::WriteOnly)==true)
				{
					file.write(rply->readAll());
					file.close();
				}
			}
			QString uniqueKey = tk.str_url + ":" +  tk.str_destinDir +":" + tk.str_destinFile;
			m_set_tileAddress.remove(uniqueKey);
			m_map_pendingTasks.remove(rply);
			rply->deleteLater();
		}
		else
			assert(false);
		if (m_map_pendingTasks.size()<m_nMaxAsynThread && m_listTask.empty()==false)
			needDo = true;
		if (m_listTask.empty()==true && m_map_pendingTasks.empty()==true)
			allFinished = true;
		m_mutex_protect.unlock();
		if (needDo)
			emit evt_doNextJob();
		if (allFinished == true)
			emit evt_all_taskFinished();
	}

	void urlDownloader::newTaskAdded()
	{
		if (!m_pNetMge)
		{
			m_pNetMge = new QNetworkAccessManager(this);
			connect(m_pNetMge, SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)),Qt::QueuedConnection);
		}
		m_mutex_protect.lock();
		while (m_map_pendingTasks.size()<m_nMaxAsynThread && m_listTask.empty()==false)
		{
			tag_download_tasks tk = m_listTask.first();
			QNetworkReply * ptply = m_pNetMge->get(QNetworkRequest(QUrl(tk.str_url)));
			m_map_pendingTasks[ptply]=tk;
			m_listTask.pop_front();
		}
		m_mutex_protect.unlock();
	}
	void urlDownloader::addDownloadUrl(const QString & sourceUrl, const QString & DestinDir, const QString & filename,bool newerFirst )
	{
		bool bNeedEmit = false;
		tag_download_tasks tk;
		tk.str_url = sourceUrl;
		tk.str_destinDir = DestinDir;
		tk.str_destinFile = filename;
		QString uniqueKey = sourceUrl + ":" +  DestinDir +":" + filename;
		m_mutex_protect.lock();
		if (m_set_tileAddress.contains(uniqueKey)==true)
			m_listTask.removeAll(tk);
		else
			m_set_tileAddress.insert(uniqueKey);
		if (m_map_pendingTasks.size()<m_nMaxAsynThread)
			bNeedEmit = true;
		if (newerFirst)
			m_listTask.push_front(tk);
		else
			m_listTask.push_back(tk);

		m_mutex_protect.unlock();
		if (bNeedEmit)
			emit evt_doNextJob();
	}
	QVector<tag_download_tasks> urlDownloader::current_tasks()
	{
		QVector<tag_download_tasks> ret;
		m_mutex_protect.lock();
		foreach (tag_download_tasks t, m_listTask)
			ret.push_back(t);
		m_mutex_protect.unlock();
		return ret;
	}
}
