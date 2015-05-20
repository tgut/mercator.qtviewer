#include <QThread>
#include <QCoreApplication>
#include <QPainter>
#include <QSettings>
#include "layer_browser.h"
#include "tilesviewer.h"
layer_browser::layer_browser(QObject *parent) :
	QObject(parent)
{
	m_name = QString("%1").arg((quint64)this);
}

void layer_browser::cb_paintEvent( QPainter * pImage  )
{
	if (!m_pViewer) return;
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
				if (this->getTileImage(m_pViewer->level(),req_col,req_row,image_source))
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

	}
}
layer_browser::~layer_browser()
{

}
QWidget * layer_browser::cb_create_propWindow()
{
	return 0;
}

bool layer_browser::cb_mousePressEvent ( QMouseEvent * event )
{
	if (!m_pViewer) return false;
	bool res = false;
	if (event->button()==Qt::LeftButton)
	{
		int nOffsetX = event->pos().x()-m_pViewer->width()/2;
		int nOffsetY = event->pos().y()-m_pViewer->height()/2;
		m_pViewer->DragView(-nOffsetX,-nOffsetY);
		res = true;
	}
	return res;
}

bool layer_browser::cb_mouseReleaseEvent ( QMouseEvent * event )
{
	return false;
}

bool layer_browser::cb_mouseMoveEvent(QMouseEvent * /*event*/)
{
	return false;
}

bool layer_browser::cb_wheelEvent ( QWheelEvent * /*event*/ )
{
	if (!m_pViewer) return false;
	return true;
}

void layer_browser::cb_resizeEvent ( QResizeEvent * /*event*/)
{

}

bool layer_browser::cb_initlayer(QObject  * viewer)
{
	m_pViewer =qobject_cast<tilesviewer *>(viewer);
	if (m_pViewer==0)
		return false;
	return true;
}
void layer_browser::cb_finilayer(void * /*viewer*/)
{
	m_pViewer = 0;
}

void layer_browser::cb_levelChanged(int /*nLevel*/)
{

}

void layer_browser::cb_update()
{

}
//Get a single tile PNG from web service. return false if error occurred.
bool layer_browser::getTileImage(int nLevel,int nX,int nY,QImage & image)
{
	if (!m_pViewer) return true;

	QSettings settings(QCoreApplication::applicationFilePath()+".ini",QSettings::IniFormat);
	QString strLocalCache = settings.value(QString("settings/LocalCache_%1").arg(cb_name()), QCoreApplication::applicationDirPath() +"/Cache").toString();

	QString strVal = strLocalCache;
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
		if (res==false)
			image = QImage(256,256,QImage::Format_ARGB32);
	}
	return res;

}
