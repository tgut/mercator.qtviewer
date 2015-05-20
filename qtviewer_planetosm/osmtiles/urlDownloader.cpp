#include "urlDownloader.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QDir>

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
		if (rply->error()==QNetworkReply::NoError)
		{
			QString strDir = m_map_pendingTasks[rply][0];
			QString strFile = m_map_pendingTasks[rply][1];
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
		m_map_pendingTasks.remove(rply);
		rply->deleteLater();
	}
	if (m_map_pendingTasks.size()<m_nMaxAsynThread && m_list_source.empty()==false)
		needDo = true;
	if (m_list_source.empty()==true && m_map_pendingTasks.empty()==true)
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
	while (m_map_pendingTasks.size()<m_nMaxAsynThread && m_list_source.empty()==false)
	{
		QString strSource = m_list_source.first();
		QNetworkReply * ptply = m_pNetMge->get(QNetworkRequest(QUrl(strSource)));
		m_map_pendingTasks[ptply].push_back(m_list_destinDir.first());
		m_map_pendingTasks[ptply].push_back(m_list_destinFile.first());
		m_list_source.pop_front();
		m_list_destinFile.pop_front();
		m_list_destinDir.pop_front();
	}
	m_mutex_protect.unlock();
}
void urlDownloader::addDownloadUrl(QString sourceUrl, QString DestinDir, QString filename)
{
	bool bNeedEmit = false;
	m_mutex_protect.lock();
	if (m_map_pendingTasks.size()<m_nMaxAsynThread)
		bNeedEmit = true;
	m_list_source.push_back(sourceUrl);
	m_list_destinDir.push_back(DestinDir);
	m_list_destinFile.push_back(filename);
	m_mutex_protect.unlock();
	if (bNeedEmit)
		emit evt_doNextJob();
}


