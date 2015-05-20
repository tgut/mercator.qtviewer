#include <QThread>
#include <QCoreApplication>
#include <QPainter>
#include <QSettings>
#include "layer_tiles.h"
#include "tilesviewer.h"
layer_tiles::layer_tiles(QObject *parent) :
	QObject(parent)
{
	m_bActive = false;
	m_bVisible = false;
	m_pViewer = 0;
	//Create thread object for download
	m_downloader = new urlDownloader(0);
	m_downloadThread = new QThread(this);
	m_downloader->moveToThread(m_downloadThread);
	m_downloadThread->start();
	//init basic cache and server site
	this->m_strLocalCache = QCoreApplication::applicationDirPath() +"/Cache";
	this->m_strServerURL = "http://localhost/osm_tiles2/%1/%2/%3.png";
	m_bconnected = false;
	m_propPage = 0;
	//Drag
	this->m_nStartPosX = this->m_nStartPosY = -1;
	m_name = QString("%1").arg((quint64)this);
}
void layer_tiles::setServerUrl(QString url)
{
	m_strServerURL = url;
	QSettings settings(QCoreApplication::applicationFilePath()+".ini",QSettings::IniFormat);
	settings.setValue(QString("settings/ServerURL_%1").arg(cb_name()),m_strServerURL);
}

void layer_tiles::setLocalCache(QString cache)
{
	m_strLocalCache = cache;
	QSettings settings(QCoreApplication::applicationFilePath()+".ini",QSettings::IniFormat);
	settings.setValue(QString("settings/LocalCache_%1").arg(cb_name()), m_strLocalCache);
}

void layer_tiles::cb_paintEvent( QPainter * pImage )
{
	if (!m_pViewer || m_bVisible==false) return;
	//calculate current position
	int nCenter_X ,nCenter_Y;
	if (true==m_pViewer->CV_PercentageToPixel(
				m_pViewer->level(),
				m_pViewer->centerX(),
				m_pViewer->centerY(),
				&nCenter_X,&nCenter_Y))
	{
		int sz_whole_idx = 1<<m_pViewer->level();
		//current center
		int nCenX = nCenter_X/256;
		int nCenY = nCenter_Y/256;
		//current left top tile idx
		int nCurrLeftX = floor((nCenter_X-m_pViewer->width()/2)/256.0);
		int nCurrTopY = floor((nCenter_Y-m_pViewer->height()/2)/256.0);
		//current right btm
		int nCurrRightX = ceil((nCenter_X+m_pViewer->width()/2)/256.0);
		int nCurrBottomY = ceil((nCenter_Y+m_pViewer->height()/2)/256.0);

		//draw images

		bool needreg = false;
		int reg_left = sz_whole_idx,reg_right = -1,reg_bottom = -1,reg_top = sz_whole_idx;

		for (int col = nCurrLeftX;col<=nCurrRightX;col++)
		{
			for (int row = nCurrTopY;row<=nCurrBottomY;row++)
			{
				//generate a image
				QImage image_source;
				int req_row = row, req_col = col;
				if (row<0 || row>=sz_whole_idx)
					continue;
				if (col>=sz_whole_idx)
					req_col = col % sz_whole_idx;
				if (col<0)
					req_col = (col + (1-col/sz_whole_idx)*sz_whole_idx) % sz_whole_idx;
				//query
				if (false==this->getTileImage(m_pViewer->level(),req_col,req_row,image_source))
				{
					needreg = true;
					if (reg_left>=req_col) reg_left = req_col;
					if (reg_right<=req_col) reg_right = req_col;
					if (reg_top>=req_row) reg_top = req_row;
					if (reg_bottom<=req_row) reg_bottom = req_row;
				}
				else
				{
					//bitblt
					int nTileOffX = (col-nCenX)*256;
					int nTileOffY = (row-nCenY)*256;
					//0,0 lefttop offset
					int zero_offX = nCenter_X % 256;
					int zero_offY = nCenter_Y % 256;
					//bitblt cood
					int tar_x = m_pViewer->width()/2-zero_offX+nTileOffX;
					int tar_y = m_pViewer->height()/2-zero_offY+nTileOffY;
					//bitblt
					pImage->drawImage(tar_x,tar_y,image_source);
				}
			}
		}

		if (needreg==true)
			RegImages(reg_left,reg_right,reg_top,reg_bottom,m_pViewer->level());
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
		res = true;
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
		m_pViewer->DragView(nOffsetX,nOffsetY);
		this->m_nStartPosX = this->m_nStartPosY = -1;
		res = true;
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

bool layer_tiles::cb_initlayer(QObject  * viewer)
{
	m_pViewer = qobject_cast<tilesviewer *>(viewer);
	if (!m_pViewer)
		return false;
	connect(m_downloader,SIGNAL(evt_all_taskFinished()),m_pViewer,SLOT(UpdateWindow()));
	//Get Cache Address
	QSettings settings(QCoreApplication::applicationFilePath()+".ini",QSettings::IniFormat);
	m_strServerURL = settings.value(QString("settings/ServerURL_%1").arg(m_name),"http://localhost/osm_tiles2/%1/%2/%3.png").toString();
	m_strLocalCache = settings.value(QString("settings/LocalCache_%1").arg(m_name), QCoreApplication::applicationDirPath() +"/Cache").toString();
	return true;
}
void layer_tiles::cb_finilayer(void * /*viewer*/)
{
	if (m_pViewer)
		disconnect(m_downloader,SIGNAL(evt_all_taskFinished()),m_pViewer,SLOT(UpdateWindow()));
	m_pViewer = 0;
}
QWidget * layer_tiles::cb_create_propWindow()
{
	if (!m_propPage)
		m_propPage = new layer_tiles_page(this,0);
	return m_propPage;
}

void layer_tiles::cb_levelChanged(int /*nLevel*/)
{
	if (!m_pViewer || m_bVisible==false) return;
}

void layer_tiles::cb_update()
{
	if (!m_pViewer || m_bVisible==false) return;
}
//Get a single tile PNG from web service. return false if error occurred.
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
	if (res)
	{
		QByteArray array_out;
		QFile file(strVal);
		if (file.open(QIODevice::ReadOnly)==true)
		{
			array_out = file.readAll();
			image = QImage::fromData(array_out);
			if (image.isNull()==true)
				res = false;
		}
		else
			res = false;
	}
	return res;

}
//public slots for enable/disable web service
void layer_tiles::UpdateLayer()
{
	m_pViewer->update();
}
//public slots for enable/disable web service
void layer_tiles::connectToTilesServer(bool bconnected)
{
	this->m_bconnected = bconnected;
	if (!m_pViewer) return;
		m_pViewer->update();
}
bool layer_tiles::RegImages(int nLeft,int nRight,int nTop,int nBottom,int nLevel)
{
	if (!m_pViewer || m_bVisible==false) return true;
	if (m_bconnected==false)
		return true;
	for (int nX = nLeft; nX <=nRight ; ++nX)
	{
		for (int nY = nTop; nY <= nBottom ; ++nY)
		{
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
		}
	}
	return true;
}
