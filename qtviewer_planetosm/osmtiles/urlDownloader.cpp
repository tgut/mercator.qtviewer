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
		bool succeeded = false;
		bool redirect = false;
		tag_download_tasks redirect_tk;
		m_mutex_protect.lock();
		QString errMsg;
		QString sourceUrl;
		if (m_map_pendingTasks.contains(rply)==true)
		{
			const tag_download_tasks & tk = m_map_pendingTasks[rply];
			if (rply->error()==QNetworkReply::NoError)
			{
				if (rply->bytesAvailable())
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
						succeeded = true;
						sourceUrl = m_map_pendingTasks[rply].str_url;
					}
				}
				else
				{
					QVariant vaurl = rply->attribute(QNetworkRequest::RedirectionTargetAttribute);
					if (vaurl.type()==QVariant::Url)
					{
						QUrl urlRd = vaurl.toUrl();
						if (urlRd.isRelative())
							urlRd = rply->request().url().resolved(urlRd);

						redirect_tk = tk;
						redirect_tk.str_url = urlRd.toString();
						errMsg = tr("task redirected: %1").arg(redirect_tk.str_url);
						redirect = true;
						succeeded = true;
					}
					else
					{
						errMsg = tr("reply is empty: %1").arg(tk.str_url);
					}
				}
			}
			else
			{
				qCritical()<<(errMsg = rply->errorString());
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

		if (succeeded)
		{
			QString strMsg = tr("task succeeded: %1;%2").arg(sourceUrl).arg(errMsg);
			emit evt_message(strMsg);
		}
		else
		{
			QString strMsg = tr("task failed: %1,msg %2").arg(sourceUrl).arg(errMsg);
			emit evt_message(strMsg);
		}
		if (redirect)
		{
			addDownloadUrl(redirect_tk.str_url,redirect_tk.str_destinDir,redirect_tk.str_destinFile,true);
		}

		if (needDo)
			emit evt_doNextJob();
		if (allFinished == true && succeeded)
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
			QNetworkRequest req;
			tag_download_tasks tk = m_listTask.first();
			req.setUrl(QUrl(tk.str_url));
			req.setRawHeader("User-Agent", "qplanetosm 1.0");
			QNetworkReply * ptply = m_pNetMge->get(req);
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
		{
			if (newerFirst)
			{
				m_listTask.removeAll(tk);
				m_listTask.push_front(tk);
				QString strMsg = tr("Bring task %1 to front.").arg(sourceUrl);
				emit evt_message(strMsg);
			}
		}
		else
		{
			m_set_tileAddress.insert(uniqueKey);
			if (m_map_pendingTasks.size()<m_nMaxAsynThread)
				bNeedEmit = true;
			if (newerFirst)
				m_listTask.push_front(tk);
			else
				m_listTask.push_back(tk);
			QString strMsg = tr("Add task %1").arg(sourceUrl);
			emit evt_message(strMsg);
		}
		m_mutex_protect.unlock();
		if (bNeedEmit)
			emit evt_doNextJob();

	}
}
