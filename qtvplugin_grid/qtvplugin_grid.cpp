#include "qtvplugin_grid.h"
#include "ui_qtvplugin_grid.h"
#include <QPainter>
#include <QBrush>
#include <QPen>
qtvplugin_grid::qtvplugin_grid(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::qtvplugin_grid)
{
	ui->setupUi(this);
	m_pVi = 0;
	m_bVisible = true;
}

qtvplugin_grid::~qtvplugin_grid()
{
	delete ui;
}
bool qtvplugin_grid::cb_initlayer(QObject  * ptrviewer)
{
	m_pVi = qobject_cast<viewer_interface *>(ptrviewer);
	if (!m_pVi)
		return false;
	return true;
}

QWidget * qtvplugin_grid::cb_create_propWindow()
{
	return this;
}

void qtvplugin_grid::cb_finilayer(void * ptrviewer)
{

}

void qtvplugin_grid::cb_update()
{

}

void qtvplugin_grid::cb_paintEvent( QPainter * pImage )
{
	int step_level[10] = {20,20,10,10,5,5,2,1,1,1};
	double dstep_level[10] = {0.5,0.2,0.1,0.05,0.02,0.01,0.005,0.002,0.001,0.0005};
	if (!m_pVi || m_bVisible==false)
		return ;
	QRect rect = m_pVi->windowRect();
	//首先确定当前视点的LLA
	double lat_top,lon_left;
	double lat_bottom,lon_right;
	m_pVi->oTVP_DP2LLA(
		rect.left(),
		rect.top(),
		&lat_top,&lon_left
		);
	if (lat_top >=85) lat_top = 85;
	m_pVi->oTVP_DP2LLA(
		rect.right(),
		rect.bottom(),
		&lat_bottom,&lon_right
		);
	if (lat_bottom <=-85) lat_bottom = -85;

	QPen pen_line(QColor(0,0,0,64));
	QPen oldpen = pImage->pen();
	pImage->setPen(pen_line);
	char str[1024];

	if (m_pVi->level()<9)
	{
		int step = step_level[m_pVi->level()];
		for (int lat=int(lat_bottom/10.0-1.5)*10;lat<=int(lat_top/10.0+1.5)*10&&m_pVi->level()>0;lat+=step)
		{
			if (lat>=85 || lat<=-85)
				continue;
			int DP_left,DP_top,DP_right,DP_bottom;
			if (false==m_pVi->oTVP_LLA2DP(lat,lon_left,&DP_left,&DP_top))
				continue;
			if (false==m_pVi->oTVP_LLA2DP(lat,lon_right,&DP_right,&DP_bottom))
				continue;
			pImage->drawLine(DP_left,DP_top,DP_right,DP_bottom);
			sprintf(str,"%-03d",lat);
			pImage->drawText(DP_left,DP_top-8,str);
		}
		for (int lon=int(lon_left/10.0-1.5)*10;lon<=(lon_right/10.0+1.5)*10&&m_pVi->level()>0;lon+=step)
		{

			int DP_left,DP_top,DP_right,DP_bottom;
			if (false==m_pVi->oTVP_LLA2DP(lat_top,lon,&DP_left,&DP_top))
				continue;
			if (false==m_pVi->oTVP_LLA2DP(lat_bottom,lon,&DP_right,&DP_bottom))
				continue;
			pImage->drawLine(DP_left,DP_top,DP_right,DP_bottom);
			//回环
			int nLon = lon;
			while (nLon<-180)
				nLon+=360;
			while (nLon>180)
				nLon-=360;
			sprintf(str,"%-04d",nLon);
			pImage->drawText(DP_left,DP_bottom-16,str);
		}
	}
	else
	{
		double step = dstep_level[m_pVi->level()-9];
		for (double lat=int(lat_bottom*(1/step)-1.5)*step;
			lat<=int(lat_top*(1/step)+1.5)*step&&m_pVi->level()>0;lat+=step)
		{
			if (lat>=85 || lat<=-85)
				continue;
			int DP_left,DP_top,DP_right,DP_bottom;
			if (false==m_pVi->oTVP_LLA2DP(lat,lon_left,&DP_left,&DP_top))
				continue;
			if (false==m_pVi->oTVP_LLA2DP(lat,lon_right,&DP_right,&DP_bottom))
				continue;
			pImage->drawLine(DP_left,DP_top,DP_right,DP_bottom);
			sprintf(str,"%-03d",lat);
			pImage->drawText(DP_left,DP_top-8,str);
		}
		for (double lon=int(lon_left*(1/step)-1.5)*step;lon<=int(lon_right*(1/step)+1.5)*step&&m_pVi->level()>0;lon+=step)
		{
			//if (lon>=180 || lon<=-180)
			//	continue;
			int DP_left,DP_top,DP_right,DP_bottom;
			if (false==m_pVi->oTVP_LLA2DP(lat_top,lon,&DP_left,&DP_top))
				continue;
			if (false==m_pVi->oTVP_LLA2DP(lat_bottom,lon,&DP_right,&DP_bottom))
				continue;
			pImage->drawLine(DP_left,DP_top,DP_right,DP_bottom);
			//回环
			float dLon = lon;
			while (dLon<-180)
				dLon+=360;
			while (dLon>180)
				dLon-=360;
			sprintf(str,"%-08.3f",(float)dLon);
			pImage->drawText(DP_left,DP_bottom-16,str);
		}

	}
	pImage->setPen(oldpen);
}

bool qtvplugin_grid::cb_mousePressEvent ( QMouseEvent * event )
{
	return false;
}

bool qtvplugin_grid::cb_mouseReleaseEvent ( QMouseEvent * event )
{
	return false;
}

bool qtvplugin_grid::cb_mouseMoveEvent(QMouseEvent * event)
{
	return false;
}

bool qtvplugin_grid::cb_wheelEvent ( QWheelEvent * event )
{
	return false;
}

void qtvplugin_grid::cb_resizeEvent ( QResizeEvent * event )
{

}

void qtvplugin_grid::cb_levelChanged(int nLevel)
{

}

bool qtvplugin_grid::cb_isActive()
{
	return false;
}

void qtvplugin_grid::cb_setActive(bool at)
{
	m_bVisible = at;
}

bool qtvplugin_grid::cb_isVisible()
{
	return m_bVisible;
}

void qtvplugin_grid::cb_setVisible(bool vb)
{

}

QString qtvplugin_grid::cb_name()
{
	return "Grid";
}

void qtvplugin_grid::cb_setName(QString vb)
{

}
