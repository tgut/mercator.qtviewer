#include <QThread>
#include <QCoreApplication>
#include <QPainter>
#include <QSettings>
#include <QFileInfo>
#include "layer_tiles.h"
#include "tilesviewer.h"
namespace QTVOSM{
	/*!
	 \brief in this constructor, some initial approach will be token.

	 \fn layer_tiles::layer_tiles
	 \param parent	parent objects
	*/
	layer_tiles::layer_tiles(QObject *parent) :
		QObject(parent),
		m_nCacheExpireDays(30)
	{
		m_bActive = false;
		m_bVisible = false;
		m_pViewer = 0;
		//!1,Create thread object for download. download will be hold in different threads/
		m_downloader = new urlDownloader(0);
		m_downloadThread = new QThread(this);
		m_downloader->moveToThread(m_downloadThread);
		m_downloadThread->start();
		//!2,init basic cache and server site.if user does not give address, default set to "http://localhost/osm/%1/%2/%3.png"
		this->m_strLocalCache = "/OSMCache";
		this->m_strServerURL = "http://c.tile.openstreetmap.org/%1/%2/%3.png";
		m_bconnected = false;
		m_propPage = 0;
		//!3, init the Drag position, for mouse drag and explore.
		this->m_nStartPosX = this->m_nStartPosY = -1;
		m_name = QString("%1").arg((quint64)this);
	}
	void layer_tiles::setServerUrl(QString url)
	{
		m_strServerURL = url;
		QSettings settings(QCoreApplication::applicationFilePath()+".ini",QSettings::IniFormat);
		settings.setValue(QString("settings/ServerURL_%1").arg(get_name()),m_strServerURL);
		emit svrurlChanged(url);
	}

	void layer_tiles::setLocalCache(QString cache)
	{
		m_strLocalCache = cache;
		QSettings settings(QCoreApplication::applicationFilePath()+".ini",QSettings::IniFormat);
		settings.setValue(QString("settings/LocalCache_%1").arg(get_name()), m_strLocalCache);
		emit cacheChanged(cache);
	}
	void layer_tiles::setCacheExpireDays(int nCacheExpireDays)
	{
		m_nCacheExpireDays = nCacheExpireDays;
		QSettings settings(QCoreApplication::applicationFilePath()+".ini",QSettings::IniFormat);
		settings.setValue(QString("settings/CacheExpireDays_%1").arg(get_name()), m_nCacheExpireDays);
	}

	/*!
	 \brief When the tileviewer enter its paint_event function, this callback will be called.

	 \fn layer_tiles::cb_paintEvent
	 \param pImage	the In-mem image for paint .
	*/
	void layer_tiles::cb_paintEvent( QPainter * pPainter )
	{
		if (!m_pViewer || m_bVisible==false) return;
		//!1,We should first calculate current windows' position, centerx,centery, in pixcel.
		double nCenter_X ,nCenter_Y;
		//!2,if the CV_PercentageToPixel returns true, painting will begin.
		if (true==m_pViewer->CV_Pct2World(
					m_pViewer->centerX(),
					m_pViewer->centerY(),
					&nCenter_X,&nCenter_Y))
		{
			int sz_whole_idx = 1<<m_pViewer->level();
			//!2.1 get current center tile idx, in tile count.(tile is 256x256)
			int nCenX = nCenter_X/256;
			int nCenY = nCenter_Y/256;
			//!2.2 calculate current left top tile idx
			int nCurrLeftX = floor((nCenter_X-m_pViewer->width()/2)/256.0);
			int nCurrTopY = floor((nCenter_Y-m_pViewer->height()/2)/256.0);
			//!2.3 calculate current right bottom idx
			int nCurrRightX = ceil((nCenter_X+m_pViewer->width()/2)/256.0);
			int nCurrBottomY = ceil((nCenter_Y+m_pViewer->height()/2)/256.0);

			//!2.4 a repeat from tileindx left to right.
			for (int col = nCurrLeftX;col<=nCurrRightX;col++)
			{
				//!2.4.1 a repeat from tileindx top to bottom.
				for (int row = nCurrTopY;row<=nCurrBottomY;row++)
				{
					QImage image_source;
					int req_row = row, req_col = col;
					if (row<0 || row>=sz_whole_idx)
						continue;
					if (col>=sz_whole_idx)
						req_col = col % sz_whole_idx;
					if (col<0)
						req_col = (col + (1-col/sz_whole_idx)*sz_whole_idx) % sz_whole_idx;
					//!2.4.2 call getTileImage to query the image .
					if (true==this->getTileImage(m_pViewer->level(),req_col,req_row,image_source))
					{
						//bitblt
						int nTileOffX = (col-nCenX)*256;
						int nTileOffY = (row-nCenY)*256;
						//0,0 lefttop offset
						int zero_offX = int(nCenter_X+0.5) % 256;
						int zero_offY = int(nCenter_Y+0.5) % 256;
						//bitblt cood
						int tar_x = m_pViewer->width()/2-zero_offX+nTileOffX;
						int tar_y = m_pViewer->height()/2-zero_offY+nTileOffY;
						//bitblt
						pPainter->drawImage(tar_x,tar_y,image_source);
					}
				}
			}
		}

	}
	layer_tiles::~layer_tiles()
	{
		m_downloadThread->quit();
		m_downloadThread->wait();
		m_downloader->deleteLater();
		if (!m_propPage)
			m_propPage->deleteLater();
	}

	bool layer_tiles::cb_mousePressEvent ( QMouseEvent * event )
	{
		if (!m_pViewer || m_bVisible==false || m_bActive==false) return false;
		bool res = false;
		if (event->button()==Qt::LeftButton)
		{
			this->m_nStartPosX = event->pos().x();
			this->m_nStartPosY = event->pos().y();
		}
		else if (event->button()==Qt::RightButton)
		{
			int nOffsetX = event->pos().x()-m_pViewer->width()/2;
			int nOffsetY = event->pos().y()-m_pViewer->height()/2;
			m_pViewer->DragView(-nOffsetX,-nOffsetY);
			res = true;
		}
		return res;
	}

	bool layer_tiles::cb_mouseReleaseEvent ( QMouseEvent * event )
	{
		if (!m_pViewer || m_bVisible==false || m_bActive==false) return false;
		bool res = false;
		if (event->button()==Qt::LeftButton)
		{
			int nOffsetX = event->pos().x()-this->m_nStartPosX;
			int nOffsetY = event->pos().y()-this->m_nStartPosY;
			if (!(nOffsetX ==0 && nOffsetY==0))
			{
				m_pViewer->DragView(nOffsetX,nOffsetY);
				this->m_nStartPosX = this->m_nStartPosY = -1;
				res = true;
			}
		}
		return res;
	}

	bool layer_tiles::cb_mouseMoveEvent(QMouseEvent * /*event*/)
	{
		return false;
	}

	bool layer_tiles::cb_wheelEvent ( QWheelEvent * event )
	{
		if (!m_pViewer || m_bVisible==false || m_bActive==false) return false;
		int nLevel = m_pViewer->level();
		if (event->delta()<0)
		{
			nLevel++;
			if (nLevel>18)
				nLevel=18;
		}
		else if (event->delta()>0)
		{
			nLevel--;
			if (nLevel<0)
				nLevel=0;
		}
		if (nLevel != m_pViewer->level())
			m_pViewer->setLevel(nLevel);
		return true;
	}

	void layer_tiles::cb_resizeEvent ( QResizeEvent * /*event*/)
	{
		if (!m_pViewer || m_bVisible==false) return;
	}

	layer_interface * layer_tiles::load_initial_plugin(QString /*strSLibPath*/,viewer_interface  * viewer)
	{
		m_pViewer = dynamic_cast<tilesviewer *>(viewer);
		if (!m_pViewer)
			return 0;
		connect(m_downloader,SIGNAL(evt_all_taskFinished()),m_pViewer,SLOT(UpdateWindow()));
		//Get Cache Address
		QSettings settings(QCoreApplication::applicationFilePath()+".ini",QSettings::IniFormat);
		m_strServerURL = settings.value(QString("settings/ServerURL_%1").arg(m_name),"http://c.tile.openstreetmap.org/%1/%2/%3.png").toString();
		m_strLocalCache = settings.value(QString("settings/LocalCache_%1").arg(m_name), QCoreApplication::applicationDirPath() +"/OSMCache").toString();
		m_nCacheExpireDays = settings.value(QString("settings/CacheExpireDays_%1").arg(m_name), 30).toInt();
		return this;
	}

	QWidget * layer_tiles::load_prop_window()
	{
		if (!m_propPage)
		{
			m_propPage = new layer_tiles_page(this,0);
			connect (m_downloader,&urlDownloader::evt_message,m_propPage,&layer_tiles_page::slot_message,Qt::QueuedConnection);
		}
		return m_propPage;
	}

	void layer_tiles::cb_levelChanged(int /*nLevel*/)
	{
		if (!m_pViewer || m_bVisible==false) return;
	}

	/*!
	 \brief Get a single tile PNG from web service. return false if error occurred.

	 \fn layer_tiles::getTileImage
	 \param nLevel current level
	 \param nX	current col
	 \param nY	current row
	 \param image	the image object to hold the result data
	 \return bool	succeed when ok.
	*/
	bool layer_tiles::getTileImage(int nLevel,int nX,int nY,QImage & image)
	{
		if (!m_pViewer || m_bVisible==false) return true;
		QString strVal = m_strLocalCache;
		strVal += '/';
		strVal += m_name;
		strVal += '/';
		strVal += QString::number(nLevel,10);
		strVal += '/';
		strVal += QString::number(nX,10);
		strVal += '/';
		strVal += QString::number(nY,10);
		strVal += ".png";

		bool res = true;
		bool needReg = false;
		if (res)
		{
			QByteArray array_out;
			QFile file(strVal);
			QFileInfo info(strVal);
			if (file.open(QIODevice::ReadOnly)==true)
			{
				array_out = file.readAll();
				image = QImage::fromData(array_out);
				if (image.isNull()==true)
					res = false, needReg = true;
				else if (m_nCacheExpireDays > 0 && info.lastModified().secsTo(QDateTime::currentDateTime()) > this->m_nCacheExpireDays * 3600 * 24 )
					needReg = true;
			}
			else
				res = false, needReg = true;
		}
		if (needReg)
			RegImages(nX,nY,nLevel);
		return res;
	}

	void layer_tiles::UpdateLayer()
	{
		m_pViewer->update();
	}


	/*!
	 \brief connectToTilesServer will set connect flag to true.
	 notice that a connected tile means that it will try to doanload new tile images from
	 remote server, with no swearing for succeed doanload.
	 ther will be one event fired in this method.

	 \fn layer_tiles::connectToTilesServer
	 \param bconnected	return current status
	*/
	void layer_tiles::connectToTilesServer(bool bconnected)
	{
		this->m_bconnected = bconnected;
		if (!m_pViewer) return;
		m_pViewer->update();
		emit connected(m_bconnected);
		//! 1. source=MAIN_MAP,  destin = ALL, msg = CONNECTION
		if (this->is_active())
		{
			QMap<QString, QVariant> map_evt;
			map_evt["source"] = get_name();
			map_evt["destin"] = "ALL";
			map_evt["name"] = "CONNECTION";
			map_evt["status"] =m_bconnected?"CONNECTED":"CLOSED";
			m_pViewer->post_event(map_evt);
		}
	}


	/*!
	 \brief RegImages will download images from tile address.

	 \fn layer_tiles::RegImages
	 \param nX	col (x) tile id og this level nLevel
	 \param nY	row (y) tile id og this level nLevel
	 \param nLevel	current level. In osm, nlevel often take 0~18
	 \return bool	succeeded.
	*/
	bool layer_tiles::RegImages(int nX, int nY,int nLevel)
	{
		if (!m_pViewer || m_bVisible==false) return true;
		if (m_bconnected==false)
			return true;
		QString LFix;
		QString strSourceUrl, strDestinDir, strFileName;
		LFix += '/';
		LFix += QString::number(nLevel,10);
		LFix += '/';
		LFix += QString::number(nX,10);
		strDestinDir = m_strLocalCache + "/" + m_name + "/" + LFix;
		LFix += '/';
		LFix += QString::number(nY,10);
		LFix += ".png";
		strFileName = QString::number(nY,10);
		strFileName += ".png";
		strSourceUrl = m_strServerURL.arg(nLevel).arg(nX).arg(nY);

		this->m_downloader->addDownloadUrl(strSourceUrl,strDestinDir,strFileName);
		return true;
	}

}
