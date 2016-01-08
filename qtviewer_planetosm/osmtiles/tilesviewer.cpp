#include "tilesviewer.h"
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>
#include <math.h>
#include <QPixmap>
#include "layer_interface.h"
namespace QTVOSM{
	tilesviewer::tilesviewer(QWidget *parent) :
		QWidget(parent)
	{
		QTVOSM_DEBUG("The tilesviewer class is constructing.");
		this->m_dCenterX = this->m_dCenterY = 0;
		this->m_nLevel = 0;
		this->setMouseTracking(true);
		connect(this,&tilesviewer::_inside_do_next_evts,this,&tilesviewer::_do_next_evts,Qt::QueuedConnection);
		QTVOSM_DEBUG("The tilesviewer class constructed.");
	}

	tilesviewer::~tilesviewer()
	{
		//Call layers to draw images
		QVector<layer_interface * > slayers = layers();
		foreach (layer_interface * pItem, slayers)
			removeLayer(pItem);
	}

	bool tilesviewer::addLayer(layer_interface * ba)
	{
		if (m_setLayers.contains(ba)==true)
			return false;
		m_listLayers.push_back(ba);
		m_setLayers.insert(ba);
		return true;
	}
	void tilesviewer::removeLayer(layer_interface * ba )
	{
		if (m_setLayers.contains(ba)==false)
			return;
		m_listLayers.removeOne(ba);
		m_setLayers.remove(ba);
	}
	layer_interface * tilesviewer::layer(const QString & name)
	{
		layer_interface * res = 0;
		foreach (layer_interface * pItem, m_listLayers)
		{
			if (name ==pItem->get_name())
			{
				res = pItem;
				break;
			}
		}
		return res;
	}
	bool tilesviewer::adjust_layers(layer_interface * la )
	{
		QVector <layer_interface *> vec_layers = layers();
		bool activeb = la->is_active();
		bool tarExclusive = false;
		for (int i=0;i<vec_layers.size();++i)
		{
			//It's exclusive, there should be at most only one layer_tiles active
			if (vec_layers[i] == la )
				tarExclusive = vec_layers[i]->is_exclusive();
		}
		if (tarExclusive && activeb)
		{
			for (int i=0;i<vec_layers.size();++i)
			{
				//It's exclusive, there should be only one layer_tiles active
				if (vec_layers[i] != la  && vec_layers[i]->is_exclusive()==true)
					vec_layers[i]->set_active(false);
			}
		}

		return true;
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
			vec_layers.push_back(pItem->get_name());
		return vec_layers;
	}

	QVector<bool> tilesviewer::layerVisibilities()
	{
		QVector<bool> vec_layers;
		//Call layers to draw images
		foreach (layer_interface * pItem, m_listLayers)
			vec_layers.push_back(pItem->is_visible());
		return vec_layers;
	}

	QVector<bool> tilesviewer::layerActivities()
	{
		QVector<bool> vec_layers;
		//Call layers to draw images
		foreach (layer_interface * pItem, m_listLayers)
			vec_layers.push_back(pItem->is_active());
		return vec_layers;
	}

	void tilesviewer::moveLayerUp(layer_interface * ly)
	{
		int n = m_listLayers.indexOf(ly);
		if (n>0 && n<m_listLayers.size())
		{
			m_listLayers.removeAt(n);
			m_listLayers.insert(n-1,ly);
			//! 1. source=MAIN_MAP,  destin = ALL, msg = LAYER_MOVED_UP
			if (this->isEnabled())
			{
				QMap<QString, QVariant> map_evt;
				map_evt["source"] = "MAIN_MAP";
				map_evt["destin"] = "ALL";
				map_evt["name"] = "LAYER_MOVED_UP";
				map_evt["layerName"] = ly->get_name();
				post_event(map_evt);
			}
		}

	}

	void tilesviewer::moveLayerDown(layer_interface * ly)
	{
		int n = m_listLayers.indexOf(ly);
		if (n>=0 && n + 1 < m_listLayers.size())
		{
			m_listLayers.removeAt(n);
			m_listLayers.insert(n+1,ly);
			//! 1. source=MAIN_MAP,  destin = ALL, msg = LAYER_MOVED_DOWN
			if (this->isEnabled())
			{
				QMap<QString, QVariant> map_evt;
				map_evt["source"] = "MAIN_MAP";
				map_evt["destin"] = "ALL";
				map_evt["name"] = "LAYER_MOVED_DOWN";
				map_evt["layerName"] = ly->get_name();
				post_event(map_evt);
			}
		}

	}

	void tilesviewer::moveLayerTop(layer_interface * ly)
	{
		int n = m_listLayers.indexOf(ly);
		if (n>0 && n<m_listLayers.size())
		{
			m_listLayers.removeAt(n);
			m_listLayers.push_front(ly);
			//! 1. source=MAIN_MAP,  destin = ALL, msg = LAYER_MOVED_TOP
			if (this->isEnabled())
			{
				QMap<QString, QVariant> map_evt;
				map_evt["source"] = "MAIN_MAP";
				map_evt["destin"] = "ALL";
				map_evt["name"] = "LAYER_MOVED_TOP";
				map_evt["layerName"] = ly->get_name();

				post_event(map_evt);
			}
		}
	}

	void tilesviewer::moveLayerBottom(layer_interface *ly)
	{
		int n = m_listLayers.indexOf(ly);
		if (n>=0 && n+1<m_listLayers.size())
		{
			m_listLayers.removeAt(n);
			m_listLayers.push_back(ly);
			//! 1. source=MAIN_MAP,  destin = ALL, msg = LAYER_MOVED_BOTTOM
			if (this->isEnabled())
			{
				QMap<QString, QVariant> map_evt;
				map_evt["source"] = "MAIN_MAP";
				map_evt["destin"] = "ALL";
				map_evt["name"] = "LAYER_MOVED_BOTTOM";
				map_evt["layerName"] = ly->get_name();
				post_event(map_evt);
			}
		}
	}
	void tilesviewer::updateLayerGridView()
	{
		emit cmd_update_layer_box();
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
		bool needUpdate = m_nLevel==n?false:true;
		if (needUpdate==false)
			return;

		this->m_nLevel = n;

		//Call layers
		foreach (layer_interface * pItem, m_listLayers)
			pItem->cb_levelChanged(n);
		update();
		emit evt_level_changed(m_nLevel);

		//! 1. source=MAIN_MAP,  destin = ALL, msg = LEVEL_CHANGED
		if (this->isEnabled())
		{
			QMap<QString, QVariant> map_evt;
			map_evt["source"] = "MAIN_MAP";
			map_evt["destin"] = "ALL";
			map_evt["name"] = "LEVEL_CHANGED";
			map_evt["nLevel"] = n;
			post_event(map_evt);
		}
	}


	//public slots for resolution changed events
	void tilesviewer::setBrLevel(int n)
	{
		this->m_nLevel = n - 6;
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
		update();
	}

	void tilesviewer::mousePressEvent ( QMouseEvent * event )
	{
		//Call layers
		int needUpdate = 0;
		foreach (layer_interface * pItem, m_listLayers)
			needUpdate +=  pItem->cb_mousePressEvent(event)==true?1:0;
		if (needUpdate>0)
			this->update();

		//! 1. source=MAIN_MAP,  destin = ALL, msg = MOUSE_LBUTTON_DOWN
		//! 2. source=MAIN_MAP,  destin = ALL, msg = MOUSE_RBUTTON_DOWN
		//! 3. source=MAIN_MAP,  destin = ALL, msg = MOUSE_MBUTTON_DOWN
		//! 4. source=MAIN_MAP,  destin = ALL, msg = MOUSE_BUTTON_DOWN
		if (this->isEnabled())
		{
			QMap<QString, QVariant> map_evt;
			map_evt["source"] = "MAIN_MAP";
			map_evt["destin"] = "OUTER";
			if (event->buttons() & Qt::LeftButton)
				map_evt["name"] = "MOUSE_LBUTTON_DOWN";
			else if (event->buttons() & Qt::RightButton)
				map_evt["name"] = "MOUSE_RBUTTON_DOWN";
			else if (event->buttons() & Qt::MidButton)
				map_evt["name"] = "MOUSE_MBUTTON_DOWN";
			else
				map_evt["name"] = "MOUSE_BUTTON_DOWN";
			double tlat, tlon;
			CV_DP2LLA(event->pos().x(),event->pos().y(),&tlat,&tlon);
			map_evt["lat"] = tlat;
			map_evt["lon"] = tlon;
			map_evt["nLevel"] = m_nLevel;
			post_event(map_evt);

			//test coord cnt
			/**/
			if (false)
			{
				QString strMsg;
				int dpx = event->pos().x(), dpy = event->pos().y();
				strMsg += QString("DP(%1,%2)").arg(dpx).arg(dpy);
				double wdx,wdy;
				CV_DP2World(dpx,dpy,&wdx,&wdy);
				strMsg += QString("->WD(%1,%2)").arg(wdx).arg(wdy);
				CV_World2DP(wdx,wdy,&dpx,&dpy);
				strMsg += QString("->DP(%1,%2)").arg(dpx).arg(dpy);
				double lat,lon;
				CV_World2LLA(wdx,wdy,&lat,&lon);
				strMsg += QString("->LLA(%1,%2)").arg(lat).arg(lon);
				CV_LLA2World(lat,lon,&wdx,&wdy);
				strMsg += QString("->WD(%1,%2)").arg(wdx).arg(wdy);

				CV_DP2LLA(dpx,dpy,&lat,&lon);
				strMsg += QString("->LLA(%1,%2)").arg(lat).arg(lon);
				CV_LLA2DP(lat,lon,&dpx,&dpy);
				strMsg += QString("->DP(%1,%2)").arg(dpx).arg(dpy);
				double mkx,mky;
				CV_LLA2MK(lat,lon,&mkx,&mky);
				strMsg += QString("->MK(%1,%2)").arg(mkx).arg(mky);
				CV_MK2LLA(mkx,mky,&lat,&lon);
				strMsg += QString("->LLA(%1,%2)").arg(lat).arg(lon);

				CV_MK2World(mkx,mky,&wdx,&wdy);
				strMsg += QString("->WD(%1,%2)").arg(wdx).arg(wdy);
				CV_World2MK(wdx,wdy,&mkx,&mky);
				strMsg += QString("->MK(%1,%2)").arg(mkx).arg(mky);

				double pctx,pcty;
				CV_World2Pct(wdx,wdy,&pctx,&pcty);
				strMsg += QString("->PCT(%1,%2)").arg(pctx).arg(pcty);
				CV_Pct2World(pctx,pcty,&wdx,&wdy);
				strMsg += QString("->WD(%1,%2)").arg(wdx).arg(wdy);

				qDebug()<<strMsg;
			}
		}

		QWidget::mousePressEvent(event);

	}
	void tilesviewer::resizeEvent ( QResizeEvent * event )
	{
		//Call layers
		foreach (layer_interface * pItem, m_listLayers)
			pItem->cb_resizeEvent(event);
		update();

		//! 1. source=MAIN_MAP,  destin = ALL, msg = MAP_RESIZED
		if (this->isEnabled())
		{
			QMap<QString, QVariant> map_evt;
			map_evt["source"] = "MAIN_MAP";
			map_evt["destin"] = "OUTER";
			map_evt["name"] = "MAP_RESIZED";
			map_evt["width"] = event->size().width();
			map_evt["height"] = event->size().height();
			post_event(map_evt);
		}

		QWidget::resizeEvent(event);
	}

	void tilesviewer::mouseReleaseEvent ( QMouseEvent * event )
	{
		//Call layers
		int needUpdate = 0;
		foreach (layer_interface * pItem, m_listLayers)
			needUpdate += pItem->cb_mouseReleaseEvent(event)==true?1:0;
		if (needUpdate>0)
			this->update();

		//! 1. source=MAIN_MAP,  destin = ALL, msg = MOUSE_LBUTTON_UP
		//! 2. source=MAIN_MAP,  destin = ALL, msg = MOUSE_RBUTTON_UP
		//! 3. source=MAIN_MAP,  destin = ALL, msg = MOUSE_MBUTTON_UP
		//! 4. source=MAIN_MAP,  destin = ALL, msg = MOUSE_BUTTON_UP
		if (this->isEnabled())
		{
			QMap<QString, QVariant> map_evt;
			map_evt["source"] = "MAIN_MAP";
			map_evt["destin"] = "OUTER";
			if (event->buttons() & Qt::LeftButton)
				map_evt["name"] = "MOUSE_LBUTTON_UP";
			else if (event->buttons() & Qt::RightButton)
				map_evt["name"] = "MOUSE_RBUTTON_UP";
			else if (event->buttons() & Qt::MidButton)
				map_evt["name"] = "MOUSE_MBUTTON_UP";
			else
				map_evt["name"] = "MOUSE_BUTTON_UP";
			double tlat, tlon;
			CV_DP2LLA(event->pos().x(),event->pos().y(),&tlat,&tlon);
			map_evt["lat"] = tlat;
			map_evt["lon"] = tlon;
			map_evt["nLevel"] = m_nLevel;
			post_event(map_evt);
		}


		QWidget::mouseReleaseEvent(event);
	}

	void tilesviewer::mouseDoubleClickEvent(QMouseEvent * event)
	{
		//Call layers
		int needUpdate = false;
		foreach (layer_interface * pItem, m_listLayers)
			needUpdate += pItem->cb_mouseDoubleClickEvent(event)==true?1:0;
		if (needUpdate>0)
			this->update();

		//! 1. source=MAIN_MAP,  destin = ALL, msg = MOUSE_LBUTTON_DBLCLK
		//! 2. source=MAIN_MAP,  destin = ALL, msg = MOUSE_RBUTTON_BLCLK
		//! 3. source=MAIN_MAP,  destin = ALL, msg = MOUSE_MBUTTON_BLCLK
		//! 4. source=MAIN_MAP,  destin = ALL, msg = MOUSE_BUTTON_BLCLK
		if (this->isEnabled())
		{
			QMap<QString, QVariant> map_evt;
			map_evt["source"] = "MAIN_MAP";
			map_evt["destin"] = "OUTER";
			if (event->buttons() & Qt::LeftButton)
				map_evt["name"] = "MOUSE_LBUTTON_DBLCLK";
			else if (event->buttons() & Qt::RightButton)
				map_evt["name"] = "MOUSE_RBUTTON_DBLCLK";
			else if (event->buttons() & Qt::MidButton)
				map_evt["name"] = "MOUSE_MBUTTON_DBLCLK";
			else
				map_evt["name"] = "MOUSE_BUTTON_DBLCLK";
			double tlat, tlon;
			CV_DP2LLA(event->pos().x(),event->pos().y(),&tlat,&tlon);
			map_evt["lat"] = tlat;
			map_evt["lon"] = tlon;
			map_evt["nLevel"] = m_nLevel;
			post_event(map_evt);
		}


		QWidget::mouseDoubleClickEvent(event);
	}

	void tilesviewer::mouseMoveEvent(QMouseEvent * event)
	{
		//Call layers
		int needUpdate = 0;
		foreach (layer_interface * pItem, m_listLayers)
			needUpdate += pItem->cb_mouseMoveEvent(event)==true?1:0;
		if (needUpdate)
			this->update();
		double lat,lon;
		CV_DP2LLA(event->x(),event->y(),&lat,&lon);

		//! 1. source=MAIN_MAP,  destin = ALL, msg = MOUSE_MOVE
		if (this->isEnabled())
		{
			QMap<QString, QVariant> map_evt;
			map_evt["source"] = "MAIN_MAP";
			map_evt["destin"] = "OUTER";
			map_evt["name"] = "MOUSE_MOVE";
			double tlat, tlon;
			CV_DP2LLA(event->pos().x(),event->pos().y(),&tlat,&tlon);
			map_evt["lat"] = tlat;
			map_evt["lon"] = tlon;
			map_evt["nLevel"] = m_nLevel;
			map_evt["mask"] = (quint32)event->buttons();
			post_event(map_evt);
		}

		QWidget::mouseMoveEvent(event);
	}

	void tilesviewer::wheelEvent ( QWheelEvent * event )
	{
		//Call layers
		int needUpdate = 0;
		foreach (layer_interface * pItem, m_listLayers)
			needUpdate += pItem->cb_wheelEvent(event);
		if (needUpdate)
			this->update();

		//! 1. source=MAIN_MAP,  destin = ALL, msg = MOUSE_WHELL
		if (this->isEnabled())
		{
			QMap<QString, QVariant> map_evt;
			map_evt["source"] = "MAIN_MAP";
			map_evt["destin"] = "OUTER";
			map_evt["name"] = "MOUSE_WHELL";
			double tlat, tlon;
			CV_DP2LLA(event->pos().x(),event->pos().y(),&tlat,&tlon);
			map_evt["lat"] = tlat;
			map_evt["lon"] = tlon;
			map_evt["nLevel"] = m_nLevel;
			map_evt["pDeltaX"] = event->pixelDelta().x();
			map_evt["pDeltaY"] = event->pixelDelta().y();
			map_evt["aDeltaX"] = event->angleDelta().x();
			map_evt["aDeltaY"] = event->angleDelta().y();
			post_event(map_evt);
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
		CV_DP2LLA(width()/2,height()/2,&lat,&lon);
		emit evt_center_changed(lat,lon);

		//! 1. source=MAIN_MAP,  destin = ALL, msg = CENTER_CHANGED
		if (this->isEnabled())
		{
			QMap<QString, QVariant> map_evt;
			map_evt["source"] = "MAIN_MAP";
			map_evt["destin"] = "OUTER";
			map_evt["name"] = "CENTER_CHANGED";
			map_evt["lat"] = lat;
			map_evt["lon"] = lon;
			map_evt["nLevel"] = m_nLevel;
			post_event(map_evt);
		}


	}
	//set center LLA
	void tilesviewer::setCenterLLA(double lat, double lon)
	{
		//To Mercator Projection
		double dMx = cProjectionMercator(lat,lon).ToMercator().m_x;
		double dMy = cProjectionMercator(lat,lon).ToMercator().m_y;

		//Calculat the percentage pos (-0.5-0.5) of the Mercator point
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
			update();
			emit evt_center_changed(lat,lon);
			//! 1. source=MAIN_MAP,  destin = ALL, msg = CENTER_CHANGED
			if (this->isEnabled())
			{
				QMap<QString, QVariant> map_evt;
				map_evt["source"] = "MAIN_MAP";
				map_evt["destin"] = "OUTER";
				map_evt["name"] = "CENTER_CHANGED";
				centerLLA(&lat,&lon);
				map_evt["lat"] = lat;
				map_evt["lon"] = lon;
				map_evt["nLevel"] = m_nLevel;
				post_event(map_evt);
			}
		}
	}
	//set center LLA, not emit centerChanged
	void tilesviewer::setBrCenterLLA(double lat, double lon)
	{
		//To Mercator Projection
		double dMx = cProjectionMercator(lat,lon).ToMercator().m_x;
		double dMy = cProjectionMercator(lat,lon).ToMercator().m_y;

		//Calculat the percentage pos (-0.5-0.5) of the Mercator point
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
			update();
		}
	}

	/*!
	 \brief convert LLA to Device Points. Device Points is according to current viewport,
	 point(0,0) stay at the top-left, point (width-1,height-1) in bottom-right
	 This approach is devided into several steps, and it is LEVEL RELATED!

	 \fn tilesviewer::CV_LLA2DP
	 \param lat	latitude in degree
	 \param lon	longitude in degree
	 \param pX	the pointer to hold X cood of DP
	 \param pY	the pointer to hold Y cood of DP
	 \return bool	will always return true except for the pointer is NULL
	*/
	bool tilesviewer::CV_LLA2DP(double lat,double lon,qint32 * pX,qint32 *pY)
	{
		if (!pX||!pY)			return false;

		//!1.To Mercator Projection
		double dMx = cProjectionMercator(lat,lon).ToMercator().m_x;
		double dMy = cProjectionMercator(lat,lon).ToMercator().m_y;

		//!2.Calculat the percentage pos (-0.5-0.5) of the Mercator point
		double dperx = dMx/(cProjectionMercator::pi*cProjectionMercator::R*2);
		double dpery = -dMy/(cProjectionMercator::pi*cProjectionMercator::R*2);

		int nCurrImgSize = (1<<m_nLevel)*256;
		//!3.Calculat the World pixel coordinats
		double dTarX = dperx * nCurrImgSize + nCurrImgSize/2;
		double dTarY = dpery * nCurrImgSize + nCurrImgSize/2;
		//!4.Calculat the World pixel coordinats of current view-center point
		double dCurrX = nCurrImgSize*m_dCenterX+nCurrImgSize/2;
		double dCurrY = nCurrImgSize*m_dCenterY+nCurrImgSize/2;
		//!5.Calculat the World pixel coordinats of current view-left-top point
		double nOffsetLT_x = (dCurrX-width()/2.0);
		double nOffsetLT_y = (dCurrY-height()/2.0);

		//!6.Turn world coord to DP coord
		*pX = dTarX - nOffsetLT_x+.5;
		*pY = dTarY - nOffsetLT_y+.5;
		return true;
	}
	/*!
	 \brief convert  Device Points to LLA. Device Points is according to current viewport,
	 point(0,0) stay at the top-left, point (width-1,height-1) in bottom-right
	 This approach is devided into several steps, and it is LEVEL RELATED!

	 \fn tilesviewer::CV_DP2LLA
	 \param X	 X cood of DP
	 \param Y	 Y cood of DP
	 \param plat	the pointer to hold lat in degree
	 \param plon	the pointer to hold lon in degree
	 \return bool	will always return true except for the pointer is NULL
	*/
	bool tilesviewer::CV_DP2LLA(qint32 X,qint32 Y,double  * plat,double * plon)
	{
		if (!plat||!plon)
			return false;

		//!1.Current World Pixel Size, connected to nLevel
		int nCurrImgSize = (1<<m_nLevel)*256;
		//!2.current DP according to center
		double dx = X-(width()/2.0);
		double dy = Y-(height()/2.0);
		//!3.Percentage -0.5 ~ 0.5 coord
		double dImgX = dx/nCurrImgSize+m_dCenterX;
		double dImgY = dy/nCurrImgSize+m_dCenterY;
		//!4.to Mercator
		double Mercator_x = cProjectionMercator::pi*cProjectionMercator::R*2*dImgX;
		double Mercator_y = -cProjectionMercator::pi*cProjectionMercator::R*2*dImgY;
		*plat = cProjectionMercator(Mercator_y,Mercator_x).ToLatLon().m_lat;
		*plon = cProjectionMercator(Mercator_y,Mercator_x).ToLatLon().m_lon;
		return true;
	}

	/*!
	 \brief convert Mercator to LLA.Mercator coord is a projection with ID 900913.
	 this method is NOT Level releated.

	 \fn tilesviewer::CV_MK2LLA
	 \param mx	Mercator x coord in meters
	 \param my	Mercator y coord in meters
	 \param plat	the pointer to hold lat in degree
	 \param plon	the pointer to hold lon in degree
	 \return bool	will always return true except for the pointer is NULL
	*/
	bool tilesviewer::CV_MK2LLA(double mx, double my, double * plat, double * plon)
	{
		if (!plat||!plon)			return false;
		*plat  = cProjectionMercator(my,mx).ToLatLon().m_lat;
		*plon  = cProjectionMercator(my,mx).ToLatLon().m_lon;
		return true;
	}

	/*!
	 \brief convert LLA to Mercator .Mercator coord is a projection with ID 900913.
	 this method is NOT Level releated.

	 \fn tilesviewer::CV_LLA2MK
	 \param lat	latitude in degree
	 \param lon	longitude in degree
	 \param pmx	the pointer to hold Mercator x coord in meters
	 \param pmy	the pointer to hold Mercator y coord in meters
	 \return bool	will always return true except for the pointer is NULL
	*/
	bool tilesviewer::CV_LLA2MK(double lat,double lon, double * pmx, double * pmy)
	{
		if (!pmx||!pmy)			return false;
		//To Mercator Projection
		*pmx = cProjectionMercator(lat,lon).ToMercator().m_x;
		*pmy = cProjectionMercator(lat,lon).ToMercator().m_y;
		return true;
	}


	/*!
	 \brief convert Mercator to World. World Points is according to current level,
	 point(0,0) stay at the left-top, point (SZ,SZ) in bottom-right
	 the pixel size is 2^m_nLevel*256, m_nLevel between 0 and 18
	 This approach is devided into several steps, and it is LEVEL RELATED!

	 \fn tilesviewer::CV_MK2World
	 \param mx	Mercator x in meters
	 \param my	Mercator y in meters
	 \param px	pointer to hold world x in pixel
	 \param py	pointer to hold world y in pixel
	 \return bool will always return true except for the pointer is NULL
	*/
	bool tilesviewer::CV_MK2World(double mx, double my, double * px, double * py)
	{
		if (!px||!py)			return false;
		//!1.Calculat the percentage pos (-0.5-0.5) of the Mercator point
		double dperx = mx/(cProjectionMercator::pi*cProjectionMercator::R*2);
		double dpery = -my/(cProjectionMercator::pi*cProjectionMercator::R*2);

		int nCurrImgSize = (1<<m_nLevel)*256;

		//!2.Calculat the World pixel coordinats
		*px = dperx * nCurrImgSize + nCurrImgSize/2;
		*py = dpery * nCurrImgSize + nCurrImgSize/2;

		return true;
	}

	/*!
	 \brief convert World to  Mercator. World Points is according to current level,
	 point(0,0) stay at the left-top, point (SZ,SZ) in bottom-right
	 the pixel size is 2^m_nLevel*256, m_nLevel between 0 and 18
	 This approach is devided into several steps, and it is LEVEL RELATED!

	 \fn tilesviewer::CV_World2MK
	 \param x world x in pixel
	 \param y world y in pixel
	 \param pmx pointer to hold Mercator x in meters
	 \param pmy pointer to hold Mercator y in meters
	 \return bool  will always return true except for the pointer is NULL
	*/
	bool tilesviewer::CV_World2MK(double x,double y, double * pmx, double * pmy)
	{
		if (!pmx||!pmy)			return false;
		//!1.Current World Pixel Size, connected to nLevel
		int nCurrImgSize = (1<<m_nLevel)*256;
		//!2.Percentage -0.5 ~ 0.5 coord
		double dImgX = x/nCurrImgSize - .5;
		double dImgY = y/nCurrImgSize - .5;
		//!3.to Mercator
		*pmx = cProjectionMercator::pi*cProjectionMercator::R*2*dImgX;
		*pmy = -cProjectionMercator::pi*cProjectionMercator::R*2*dImgY;
		return true;
	}


	/*!
	 \brief  convert LLA to world. World Points is according to current level,
	 point(0,0) stay at the left-top, point (SZ,SZ) in bottom-right
	 the pixel size is 2^m_nLevel*256, m_nLevel between 0 and 18
	 This approach is devided into several steps, and it is LEVEL RELATED!

	 \fn tilesviewer::CV_LLA2World
	 \param lat		the latitude in meter
	 \param lon		the longitude in meter
	 \param px	pointer to hold world x in pixel
	 \param py	pointer to hold world y in pixel
	 \return bool	 will always return true except for the pointer is NULL
	*/
	bool tilesviewer::CV_LLA2World(double lat, double lon, double * px, double * py)
	{
		if (!px||!py)			return false;
		//!1.To Mercator Projection
		double dMx = cProjectionMercator(lat,lon).ToMercator().m_x;
		double dMy = cProjectionMercator(lat,lon).ToMercator().m_y;

		//!2.Calculat the percentage pos (-0.5-0.5) of the Mercator point
		double dperx = dMx/(cProjectionMercator::pi*cProjectionMercator::R*2);
		double dpery = -dMy/(cProjectionMercator::pi*cProjectionMercator::R*2);

		int nCurrImgSize = (1<<m_nLevel)*256;
		//!3.Calculat the World pixel coordinats
		*px = dperx * nCurrImgSize + nCurrImgSize/2;
		*py = dpery * nCurrImgSize + nCurrImgSize/2;
		return true;
	}

	/*!
	 \brief  convert world to LLA. World Points is according to current level,
	 point(0,0) stay at the left-top, point (SZ,SZ) in bottom-right
	 the pixel size is 2^m_nLevel*256, m_nLevel between 0 and 18
	 This approach is devided into several steps, and it is LEVEL RELATED!

	 \fn tilesviewer::CV_World2LLA
	 \param x	world x coord in pixels
	 \param y	world y coord in pixels
	 \param plat	pointer to hold latitude in degree
	 \param plon	pointer to hold longitude in degree
	 \return bool	will always return true except for the pointer is NULL
	*/
	bool tilesviewer::CV_World2LLA(double x,double y, double * plat, double * plon)
	{
		if (!plat||!plon)			return false;
		//!1.Current World Pixel Size, connected to nLevel
		int nCurrImgSize = (1<<m_nLevel)*256;
		//!2.Percentage -0.5 ~ 0.5 coord
		double dImgX = x/nCurrImgSize - .5;
		double dImgY = y/nCurrImgSize - .5;
		//!3.to Mercator
		double mkx = cProjectionMercator::pi*cProjectionMercator::R*2*dImgX;
		double mky = -cProjectionMercator::pi*cProjectionMercator::R*2*dImgY;
		//!4.to LLA
		*plat = cProjectionMercator(mky,mkx).ToLatLon().m_lat;
		*plon = cProjectionMercator(mky,mkx).ToLatLon().m_lon;

		return true;
	}

	/*!
	 \brief	 convert  Device Points to World. Device Points is according to current viewport,
	 point(0,0) stay at the top-left, point (width-1,height-1) in bottom-right. World Points is according
	 to current level,	 point(0,0) stay at the left-top, point (SZ,SZ) in bottom-right,
	 the pixel size is 2^m_nLevel*256, m_nLevel between 0 and 18
	 This approach is devided into several steps, and it is LEVEL RELATED!

	 \fn tilesviewer::CV_DP2World
	 \param dX	Device point X in pixel
	 \param dY	Device point Y in pixel
	 \param px	pointer to hold world x in pixel
	 \param py	pointer to hold world y in pixel
	 \return bool	will always return true except for the pointer is NULL
	*/
	bool tilesviewer::CV_DP2World(qint32 dX, qint32 dY, double * px, double * py)
	{
		if (!px||!py)			return false;
		//!1.Current World Pixel Size, connected to nLevel
		int nCurrImgSize = (1<<m_nLevel)*256;
		//!2.current DP according to center
		double dx = dX-(width()/2.0);
		double dy = dY-(height()/2.0);
		//!3.Percentage -0.5 ~ 0.5 coord
		double dImgX = dx/nCurrImgSize+m_dCenterX;
		double dImgY = dy/nCurrImgSize+m_dCenterY;
		//!4.Calculat the World pixel coordinats
		*px = dImgX * nCurrImgSize + nCurrImgSize/2;
		*py = dImgY * nCurrImgSize + nCurrImgSize/2;
		return true;

	}

	/*!
	 \brief	 convert  World to Device Points. Device Points is according to current viewport,
	 point(0,0) stay at the top-left, point (width-1,height-1) in bottom-right. World Points is according
	 to current level,	 point(0,0) stay at the left-top, point (SZ,SZ) in bottom-right,
	 the pixel size is 2^m_nLevel*256, m_nLevel between 0 and 18
	 This approach is devided into several steps, and it is LEVEL RELATED!

	 \fn tilesviewer::CV_World2DP
	 \param x	world x in pixel
	 \param y	world y in pixel
	 \param pdX	pointer to hold Device point X in pixel
	 \param pdY	pointer to hold Device point Y in pixel
	 \return bool	will always return true except for the pointer is NULL
	*/
	bool tilesviewer::CV_World2DP(double x,double y, qint32 * pdX, qint32 * pdY)
	{
		if (!pdX||!pdY)			return false;

		//!1.Current World Pixel Size, connected to nLevel
		int nCurrImgSize = (1<<m_nLevel)*256;

		//!2.Calculat the World pixel coordinats of current view-center point
		double dCurrX = nCurrImgSize*m_dCenterX+nCurrImgSize/2;
		double dCurrY = nCurrImgSize*m_dCenterY+nCurrImgSize/2;
		//!3.Calculat the World pixel coordinats of current view-left-top point
		double nOffsetLT_x = (dCurrX-width()/2.0);
		double nOffsetLT_y = (dCurrY-height()/2.0);

		//!4.Turn world coord to DP coord
		*pdX = x - nOffsetLT_x+.5;
		*pdY = y - nOffsetLT_y+.5;
		return true;

	}
	/*!
	 \brief	 convert  percentage coord to world. World Points is according
	 to current level,	 point(0,0) stay at the left-top, point (SZ,SZ) in bottom-right,
	 the pixel size is 2^m_nLevel*256, m_nLevel between 0 and 18. Percentage coord is a
	 level-unretated coord, take a range -0.5~0.5, the world center in 0,0, -0.5.-0.5
	 at top-left, 0.5,0.5 at bottom-right
	 This approach is LEVEL RELATED!

	 \fn tilesviewer::CV_Pct2World
	 \param px	percentage coord x
	 \param py	percentage coord y
	 \param nx	pointer to hold world x in pixel
	 \param ny	pointer to hold world y in pixel
	 \return bool	will always return true except for the pointer is NULL
	*/
	bool tilesviewer::CV_Pct2World(double px,double py,double * nx,double * ny)
	{
		if (!nx || !ny)
			return false;
		//first, determine whole map size in current level
		int sz_whole_idx = 1<<m_nLevel;
		int sz_whole_size = sz_whole_idx*256;
		//calculate pix coodinats
		*nx = px * sz_whole_size+sz_whole_size/2;
		*ny = py * sz_whole_size+sz_whole_size/2;
		return true;
	}
	/*!
	 \brief	 convert world to   percentage coord. World Points is according
	 to current level,	 point(0,0) stay at the left-top, point (SZ,SZ) in bottom-right,
	 the pixel size is 2^m_nLevel*256, m_nLevel between 0 and 18. Percentage coord is a
	 level-unretated coord, take a range -0.5~0.5, the world center in 0,0, -0.5.-0.5
	 at top-left, 0.5,0.5 at bottom-right
	 This approach is LEVEL RELATED!

	 \fn tilesviewer::CV_World2Pct
	 \param nx	world x in pixel
	 \param ny	world y in pixel
	 \param px	pointer to hold percentage coord x
	 \param py	pointer to hold percentage coord y
	 \return bool	will always return true except for the pointer is NULL
	*/
	bool  tilesviewer::CV_World2Pct(double nx,double ny,double * px,double * py)
	{
		if (!px || !py)
			return false;
		//Current World Pixel Size, connected to nLevel
		int nCurrImgSize = (1<<m_nLevel)*256;
		//Percentage -0.5 ~ 0.5 coord
		*px = nx/nCurrImgSize - .5;
		*py = ny/nCurrImgSize - .5;

		return true;
	}

	bool tilesviewer::saveToImage(const QString & strFm)
	{
		QPixmap m = this->grab();
		return m.save(strFm);
	}

	void tilesviewer::_do_next_evts()
	{
		QMap<QString, QVariant> event;
		bool empty = true;
		m_mutex_events.lock();
		if ( m_list_pendingEvts.empty()==false)
		{
			event = m_list_pendingEvts.first();
			m_list_pendingEvts.pop_front();
			empty =  (m_list_pendingEvts.empty());
		}
		m_mutex_events.unlock();

		int st = 0;
		QString strDestin = event["destin"].toString();
		/*!
		 * In event/Message system,  Send or Post messages between different parts are enabled.the messages will be sent/posted from source to destin.
		 * The destin property setted to OUTER Message means this message/event is sent for OCX containers outer this instance.
		 * The destin property setted to MAIN_MAP Message means this message/event is sent for this MAP instance.
		 * The destin property setted to ALL Message means this message/event is sent for ALL possible recievers.
		 * Other destin will be treated as plugin DLL names ret by get_name()
		 */
		if (strDestin!=QString("OUTER") && strDestin!=QString("MAIN_MAP"))
		{
			int needUpdate = 0;
			foreach (layer_interface * pItem, m_listLayers)
			{
				if (pItem->get_name()==strDestin || strDestin==QString("ALL"))
				{
					needUpdate += pItem->cb_event(event)==true?1:0;
					++st;
				}
			}
			if (needUpdate>0)
				this->update();
		}
		/*! There are 3 ways dealing with messages.
		 * 1.cb_event callback mechanism is designed for DLL plugin system, that is Public for Qtvplugin extentions, and only take effect when destin=ALL, get_name()
		 * 2.A C++11 functinal mechanism called "send-listen-recievt" is built for OCX internal usage, and only take effect when destin=ALL, OUTER
		 * 3.All messages with destin setted to "MAINMAP" will be dealing in main map directly.
		*/
		if (strDestin==QString("OUTER") || strDestin==QString("ALL"))
		{
			m_mutex_listeners.lock();
			foreach (auto p, m_map_listeners)
				p(event);
			st += m_map_listeners.size();
			if (strDestin==QString("OUTER"))
				++st;
			m_mutex_listeners.unlock();
		}
		//This message/event is for this MAP itself
		else if (strDestin==QString("MAIN_MAP"))
		{
			++st;
		}

		if (st==0)
		{
			qWarning()<<"EVENT/MESSAGE "<<event["name"].toString() <<"'s' DESTIN "<<strDestin << " does not hit any recievers.";
			QString warmsg;
			foreach (layer_interface * pItem, m_listLayers)
				warmsg += pItem->get_name() + ";";
			qWarning()<<"Valid recievers:ALL;OUTER;MAIN_MAP;"<<warmsg;
		}

		if (empty==false)
			emit _inside_do_next_evts();
	}

	bool tilesviewer::post_event(const QMap<QString, QVariant> event)
	{
		if (event.contains("destin")==false ||event.contains("source")==false)
			return false;
		bool needEmit = false;
		m_mutex_events.lock();
		m_list_pendingEvts.append(event);
		if (m_list_pendingEvts.size()==1)
			needEmit = true;
		m_mutex_events.unlock();
		if (needEmit)
			emit _inside_do_next_evts();
		return true;
	}

	bool tilesviewer::send_event(const QMap<QString, QVariant> event)
	{
		if (event.contains("destin")==false ||event.contains("source")==false)
			return false;
		QString strDestin = event["destin"].toString();
		int st = 0;
		/*!
		 * In event/Message system,  Send or Post messages between different parts are enabled.The messages will be sent/posted from source to destin.
		 * The destin property setted to OUTER Message means this message/event is sent for OCX containers outer this instance.
		 * The destin property setted to MAIN_MAP Message means this message/event is sent for this MAP instance.
		 * The destin property setted to ALL Message means this message/event is sent for ALL possible recievers.
		 * Other destin will be treated as plugin DLL names ret by get_name()
		 */

		if (strDestin!=QString("OUTER") && strDestin!=QString("MAIN_MAP"))
		{
			int needUpdate = 0;
			foreach (layer_interface * pItem, m_listLayers)
			{
				if (pItem->get_name()==strDestin||strDestin==QString("ALL"))
				{
					++st;
					needUpdate += pItem->cb_event(event)==true?1:0;
				}
			}
			if (needUpdate>0)
				this->update();
		}
		/*! There are 3 ways dealing with messages.
		 * 1.cb_event callback mechanism is designed for DLL plugin system, that is Public for Qtvplugin extentions, and only take effect when destin=ALL, get_name()
		 * 2.A C++11 functinal mechanism called "send-listen-recievt" is built for OCX internal usage, and only take effect when destin=ALL, OUTER
		 * 3.All messages with destin setted to "MAINMAP" will be dealing in main map directly.
		*/

		if (strDestin==QString("OUTER") || strDestin==QString("ALL"))
		{
			foreach (auto p, m_map_listeners)
				p(event);
			st += m_map_listeners.size();
			if (strDestin==QString("OUTER"))
				++st;
		}
		//This message/event is for this MAP itself
		else if (strDestin==QString("MAIN_MAP"))
		{
			++st;
		}

		if (st==0)
		{
			qWarning()<<"EVENT/MESSAGE DESTIN "<<strDestin << " does not hit any recievers.";
			QString warmsg;
			foreach (layer_interface * pItem, m_listLayers)
				warmsg += pItem->get_name() + ";";
			qWarning()<<"Valid recievers:ALL;OUTER;MAIN_MAP;"<<warmsg;
		}
		return true;
	}
	void tilesviewer::listen_event(QString name,std::function<void (const QMap<QString, QVariant>)> functor)
	{
		m_mutex_listeners.lock();
		m_map_listeners[name] = (functor);
		m_mutex_listeners.unlock();
	}

	void tilesviewer::unlisten_event(QString name)
	{
		m_mutex_listeners.lock();
		m_map_listeners.remove(name);
		m_mutex_listeners.unlock();

	}
}
