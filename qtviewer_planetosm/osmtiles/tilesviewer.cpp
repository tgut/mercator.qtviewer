#include<QGraphicsView>
#include "tilesviewer.h"
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>
#include <math.h>
#include "layer_interface.h"
tilesviewer::tilesviewer(QWidget *parent) :
	QWidget(parent)
{

	this->m_dCenterX = this->m_dCenterY = 0;
	this->m_nLevel = 0;
	this->setMouseTracking(true);
}

tilesviewer::~tilesviewer()
{
}

bool tilesviewer::addLayer(layer_interface * ba)
{
	if (m_setLayers.contains(ba)==true)
		return false;
	if (ba->cb_initlayer(this)==false)
		return false;
	m_listLayers.push_back(ba);
	m_setLayers.insert(ba);
	return true;
}
void tilesviewer::removeLayer(layer_interface * ba )
{
	if (m_setLayers.contains(ba)==false)
		return;
	ba->cb_finilayer(this);
	m_listLayers.removeOne(ba);
	m_setLayers.remove(ba);
}
QVector<layer_interface * > tilesviewer::layers()
{
	QVector<layer_interface * > vec_layers;
	//Call layers to draw images
	foreach (layer_interface * pItem, m_listLayers)
		vec_layers.push_back(pItem);
	return vec_layers;
}

QVector<QString> tilesviewer::layerNames()
{
	QVector<QString> vec_layers;
	//Call layers to draw images
	foreach (layer_interface * pItem, m_listLayers)
		vec_layers.push_back(pItem->cb_name());
	return vec_layers;
}

QVector<bool> tilesviewer::layerVisibilities()
{
	QVector<bool> vec_layers;
	//Call layers to draw images
	foreach (layer_interface * pItem, m_listLayers)
		vec_layers.push_back(pItem->cb_isVisible());
	return vec_layers;
}

QVector<bool> tilesviewer::layerActivities()
{
	QVector<bool> vec_layers;
	//Call layers to draw images
	foreach (layer_interface * pItem, m_listLayers)
		vec_layers.push_back(pItem->cb_isActive());
	return vec_layers;
}

void tilesviewer::moveLayerUp(layer_interface * ly)
{
	int n = m_listLayers.indexOf(ly);
	if (n>0 && n<m_listLayers.size())
	{
		m_listLayers.removeAt(n);
		m_listLayers.insert(n-1,ly);
	}
}

void tilesviewer::moveLayerDown(layer_interface * ly)
{
	int n = m_listLayers.indexOf(ly);
	if (n>=0 && n + 1 < m_listLayers.size())
	{
		m_listLayers.removeAt(n);
		m_listLayers.insert(n+1,ly);
	}

}

void tilesviewer::moveLayerTop(layer_interface * ly)
{
	int n = m_listLayers.indexOf(ly);
	if (n>0 && n<m_listLayers.size())
	{
		m_listLayers.removeAt(n);
		m_listLayers.push_front(ly);
	}
}

void tilesviewer::moveLayerBottom(layer_interface *ly)
{
	int n = m_listLayers.indexOf(ly);
	if (n>=0 && n+1<m_listLayers.size())
	{
		m_listLayers.removeAt(n);
		m_listLayers.push_back(ly);
	}
}

void tilesviewer::paintEvent( QPaintEvent * /*event*/ )
{
	QPainter painter(this);
	QBrush br_back(QColor(192,192,192),Qt::Dense5Pattern);
	painter.setBackground(br_back);
	painter.eraseRect(this->rect());

	//Call layers to draw images
	foreach (layer_interface * pItem, m_listLayers)
		pItem->cb_paintEvent(&painter);

	//Draw center mark
	QPen pen(Qt::DotLine);
	pen.setColor(QColor(0,0,255,128));
	painter.setPen(pen);
	painter.drawLine(
				width()/2+.5,height()/2+.5-32,
				width()/2+.5,height()/2+.5+32
				);
	painter.drawLine(
				width()/2+.5-32,height()/2+.5,
				width()/2+.5+32,height()/2+.5
				);


}


//public slots for resolution changed events
void tilesviewer::setLevel(int n)
{
	this->m_nLevel = n;
	//Call layers
	foreach (layer_interface * pItem, m_listLayers)
		pItem->cb_levelChanged(n);
	update();
	emit evt_level_changed(m_nLevel);
}


//public slots for resolution changed events
void tilesviewer::setBrLevel(int n)
{
	this->m_nLevel = n - 8;
	if (m_nLevel < 0)
		m_nLevel = 0;
	else
	{
		//Call layers
		foreach (layer_interface * pItem, m_listLayers)
			pItem->cb_levelChanged(n);
		update();
	}
}

void tilesviewer::UpdateWindow()
{
	//Call layers
	foreach (layer_interface * pItem, m_listLayers)
		pItem->cb_update();
	update();
}
bool tilesviewer::CV_PercentageToPixel(int nLevel,double px,double py,int * nx,int * ny)
{
	if (!nx || !ny || nLevel<0 || nLevel>18)
		return false;
	if (px<-0.5 || px>0.5 || py<-0.5 || py>0.5)
		return false;
	//calculate the region we need
	//first, determine whole map size in current level
	int sz_whole_idx = 1<<nLevel;
	int sz_whole_size = sz_whole_idx*256;

	//calculate pix coodinats
	int nCenter_X = px * sz_whole_size+sz_whole_size/2+.5;
	if (nCenter_X<0)
		nCenter_X = 0;
	if (nCenter_X>=sz_whole_size)
		nCenter_X = sz_whole_size-1;

	int nCenter_Y = py * sz_whole_size+sz_whole_size/2+.5;
	if (nCenter_Y<0)
		nCenter_Y = 0;
	if (nCenter_Y>=sz_whole_size)
		nCenter_Y = sz_whole_size-1;

	*nx = nCenter_X;
	*ny = nCenter_Y;
	return true;
}

void tilesviewer::mousePressEvent ( QMouseEvent * event )
{
	//Call layers
	bool needUpdate = false;
	foreach (layer_interface * pItem, m_listLayers)
		needUpdate = needUpdate || pItem->cb_mousePressEvent(event);
	if (needUpdate)
	{
		foreach (layer_interface * pItem, m_listLayers)
			pItem->cb_update();
		this->update();
	}
	QWidget::mousePressEvent(event);

}
void tilesviewer::resizeEvent ( QResizeEvent * event )
{
	//Call layers
	foreach (layer_interface * pItem, m_listLayers)
		pItem->cb_resizeEvent(event);
	update();
	QWidget::resizeEvent(event);
}

void tilesviewer::mouseReleaseEvent ( QMouseEvent * event )
{
	//Call layers
	bool needUpdate = false;
	foreach (layer_interface * pItem, m_listLayers)
		needUpdate = needUpdate || pItem->cb_mouseReleaseEvent(event);
	if (needUpdate)
	{
		foreach (layer_interface * pItem, m_listLayers)
			pItem->cb_update();
		this->update();
	}
	QWidget::mouseReleaseEvent(event);
}
void tilesviewer::mouseMoveEvent(QMouseEvent * event)
{
	//Call layers
	bool needUpdate = false;
	foreach (layer_interface * pItem, m_listLayers)
		needUpdate = needUpdate || pItem->cb_mouseMoveEvent(event);
	if (needUpdate)
	{
		foreach (layer_interface * pItem, m_listLayers)
			pItem->cb_update();
		this->update();
	}
	double lat,lon;
	oTVP_DP2LLA(event->x(),event->y(),&lat,&lon);
	emit evt_mouse_moved(lat,lon);
	QWidget::mouseMoveEvent(event);
}

void tilesviewer::wheelEvent ( QWheelEvent * event )
{
	//Call layers
	bool needUpdate = false;
	foreach (layer_interface * pItem, m_listLayers)
		needUpdate = needUpdate || pItem->cb_wheelEvent(event);
	if (needUpdate)
	{
		foreach (layer_interface * pItem, m_listLayers)
			pItem->cb_update();
		this->update();
	}
	QWidget::wheelEvent(event);
}

void tilesviewer::DragView(int nOffsetX,int nOffsetY)
{
	if (nOffsetX==0 && nOffsetY == 0)
		return;

	int sz_whole_idx = 1<<m_nLevel;
	int sz_whole_size = sz_whole_idx*256;

	double dx = nOffsetX*1.0/sz_whole_size;
	double dy = nOffsetY*1.0/sz_whole_size;

	this->m_dCenterX -= dx;
	this->m_dCenterY -= dy;

	while (m_dCenterX<-0.5)
		m_dCenterX += 1;
	while (m_dCenterX>0.5)
		m_dCenterX -= 1;
	if (m_dCenterY<-0.5)
		m_dCenterY = -0.5;
	if (m_dCenterY>0.5)
		m_dCenterY = 0.5;

	double lat,lon;
	oTVP_DP2LLA(width()/2,height()/2,&lat,&lon);
	emit evt_center_changed(lat,lon);

}
//set center LLA
void tilesviewer::setCenterLLA(double lat, double lon)
{
	//到墨卡托投影
	double dMx = cProjectionMercator(lat,lon).ToMercator().m_x;
	double dMy = cProjectionMercator(lat,lon).ToMercator().m_y;

	//计算巨幅图片内的百分比
	double dCtX = dMx/(cProjectionMercator::pi*cProjectionMercator::R*2);
	double dCtY =  -dMy/(cProjectionMercator::pi*cProjectionMercator::R*2);


	while (dCtX<-0.5)
		dCtX += 1;
	while (dCtX>0.5)
		dCtX -= 1;
	if (dCtY<-0.5)
		dCtY = -0.5;
	if (dCtY>0.5)
		dCtY = 0.5;

	if (m_dCenterX != dCtX || m_dCenterY != dCtY)
	{
		this->m_dCenterX = dCtX;
		this->m_dCenterY = dCtY;

		//Call layers
		foreach (layer_interface * pItem, m_listLayers)
			pItem->cb_update();
		update();

		emit evt_center_changed(lat,lon);

	}
}
//set center LLA, not emit centerChanged
void tilesviewer::setBrCenterLLA(double lat, double lon)
{
	//到墨卡托投影
	double dMx = cProjectionMercator(lat,lon).ToMercator().m_x;
	double dMy = cProjectionMercator(lat,lon).ToMercator().m_y;

	//计算巨幅图片内的百分比
	double dCtX = dMx/(cProjectionMercator::pi*cProjectionMercator::R*2);
	double dCtY =  -dMy/(cProjectionMercator::pi*cProjectionMercator::R*2);


	while (dCtX<-0.5)
		dCtX += 1;
	while (dCtX>0.5)
		dCtX -= 1;
	if (dCtY<-0.5)
		dCtY = -0.5;
	if (dCtY>0.5)
		dCtY = 0.5;

	if (m_dCenterX != dCtX || m_dCenterY != dCtY)
	{
		this->m_dCenterX = dCtX;
		this->m_dCenterY = dCtY;

		//Call layers
		foreach (layer_interface * pItem, m_listLayers)
			pItem->cb_update();
		update();
	}
}

bool tilesviewer::oTVP_LLA2DP(double lat,double lon,qint32 * pX,qint32 *pY)
{
	QRect rect = this->rect();
	if (!pX||!pY)
		return false;

	//到墨卡托投影
	double dMx = cProjectionMercator(lat,lon).ToMercator().m_x;
	double dMy = cProjectionMercator(lat,lon).ToMercator().m_y;

	//计算巨幅图片内的百分比
	double dperx = dMx/(cProjectionMercator::pi*cProjectionMercator::R*2);
	double dpery = -dMy/(cProjectionMercator::pi*cProjectionMercator::R*2);

	double dCurrImgSize = pow(2.0,m_nLevel)*256;
	//计算要转换的点的巨幅图像坐标
	double dTarX = dperx * dCurrImgSize + dCurrImgSize/2;
	double dTarY = dpery * dCurrImgSize + dCurrImgSize/2;
	//计算当前中心点的巨幅图像坐标
	double dCurrX = dCurrImgSize*m_dCenterX+dCurrImgSize/2;
	double dCurrY = dCurrImgSize*m_dCenterY+dCurrImgSize/2;
	//计算当前中心的全局坐标
	double nOffsetLT_x = (dCurrX-width()/2.0);
	double nOffsetLT_y = (dCurrY-height()/2.0);

	//判断是否在视点内
	*pX = dTarX - nOffsetLT_x+.5;
	*pY = dTarY - nOffsetLT_y+.5;

	//if (*pX>=rect.left() && *pX<=rect.right()&&*pY>=rect.top()&&*pY<=rect.bottom())
		return true;

	//return false;
}

bool tilesviewer::oTVP_DP2LLA(qint32 X,qint32 Y,double  * plat,double * plon)
{
	if (!plat||!plon)
		return false;
	//显示经纬度
	//当前缩放图幅的像素数
	double dCurrImgSize = pow(2.0,m_nLevel)*256;
	int dx = X-(width()/2+.5);
	int dy = Y-(height()/2+.5);
	double dImgX = dx/dCurrImgSize+m_dCenterX;
	double dImgY = dy/dCurrImgSize+m_dCenterY;
	double Mercator_x = cProjectionMercator::pi*cProjectionMercator::R*2*dImgX;
	double Mercator_y = -cProjectionMercator::pi*cProjectionMercator::R*2*dImgY;
	double dLat = cProjectionMercator(Mercator_y,Mercator_x).ToLatLon().m_lat;
	double dLon = cProjectionMercator(Mercator_y,Mercator_x).ToLatLon().m_lon;
	//if (dLat>=-90 && dLat<=90 && dLon>=-180 && dLon<=180)
	{
		*plat = dLat;
		*plon = dLon;
		return true;
	}
	//return false;
}

