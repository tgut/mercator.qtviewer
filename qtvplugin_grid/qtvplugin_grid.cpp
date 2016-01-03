#include "qtvplugin_grid.h"
#include "ui_qtvplugin_grid.h"
#include <QPainter>
#include <QBrush>
#include <QCoreApplication>
#include <QPen>
#include <QDebug>
#include <QLibraryInfo>
#include <QFileInfo>
#include <QMutex>
#include <QMap>
#include <assert.h>
/*!
 * The plugin dynamic library (.dll in windows or .so in linux) will be loaded into memory only once.
 * for example, a windows app like test_container will contain 2 qtaxviewer_planetosm OCX  ctrls ,
 * each OCX ctrl will load plugins when initializing. We need a mechanism to handle this situation,
 * that maintains a connection between plugin instances and their parents(always be viewer_interface).
 * For the reason above, these paraments are introduced:
*/
QMutex mutex_instances;										//!This QMutex protect map_instances and count_instances
QMap<viewer_interface *,  qtvplugin_grid * > map_instances;	//!Mapping viewer_interface to qtvplugins
QMap<QString,  int > count_instances;						//!a counter for instances numbering


qtvplugin_grid::qtvplugin_grid(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::qtvplugin_grid),
	m_nInstance(0)
{
	ui->setupUi(this);
	m_pVi = 0;
	m_bVisible = true;
	m_pt_end = QPointF(-1,-1);
	bFinished = true;
	m_bActive = false;
}

qtvplugin_grid::~qtvplugin_grid()
{
	delete ui;
}
void qtvplugin_grid::load_retranslate_UI()
{
	ui->retranslateUi(this);
}

layer_interface * qtvplugin_grid::load_initial_plugin(QString strSLibPath,viewer_interface  * ptrviewer)
{
	//!In this instance, we will see how to create a new instance for each ptrviewer
	qtvplugin_grid * instance = 0;

	//!1.Check whether there is already a instance for ptrviewer( viewer_interface)
	mutex_instances.lock();
	if (map_instances.empty()==true)
	{
		map_instances[ptrviewer] = this;
		instance = this;
	}
	else if (map_instances.contains(ptrviewer)==false)
	{
		//Create a new instance for ptrviewer
		instance = new qtvplugin_grid;
		map_instances[ptrviewer] = instance;
	}
	else
		instance = map_instances[ptrviewer];
	mutex_instances.unlock();

	//!2.if the instance correspones to this, do init operations.
	if (instance==this)
	{
		QFileInfo info(strSLibPath);
		m_SLLibName = info.completeBaseName();
		m_pVi = ptrviewer;

		mutex_instances.lock();
		m_nInstance = ++count_instances[m_SLLibName];
		mutex_instances.unlock();

		loadTranslation();
	}
	//!3.if the instance not correspones to this, call the instances' load_initial_plugin instead.
	else
	{
		 layer_interface * ret = instance->load_initial_plugin(strSLibPath,ptrviewer);
		 assert(ret==instance);
	}

	return instance;
}

bool	qtvplugin_grid::loadTranslation()
{
	bool res = false;
	//Trans
	QCoreApplication * app =  QCoreApplication::instance();
	if (app && m_nInstance==1)
	{
		QString strTransLocalFile =
				QCoreApplication::applicationDirPath()+"/" +
				m_SLLibName+ "_" +
				QLocale::system().name()+".qm";
		if (true==pluginTranslator.load(strTransLocalFile ))
		{
			QTVOSM_DEBUG("Load translationfile")<<"\n\t"<<strTransLocalFile<<" Succeeded.";
			app->installTranslator(&pluginTranslator);
			ui->retranslateUi(this);
			res = true;
		}
		else
			QTVOSM_WARNING("Load translationfile")<<"\n\t"<<strTransLocalFile<<" Not Found.";
	}
	return res;
}

QWidget * qtvplugin_grid::load_prop_window()
{
	return this;
}

void qtvplugin_grid::cb_paintEvent( QPainter * pImage )
{
	int step_level[10] = {20,20,10,10,5,5,2,1,1,1};
	double dstep_level[10] = {0.5,0.2,0.1,0.05,0.02,0.01,0.005,0.002,0.001,0.0005};
	if (!m_pVi || m_bVisible==false)
		return ;
	QRect rect = m_pVi->windowRect();
	//Get current viewport pos, in LLA
	double lat_top,lon_left;
	double lat_bottom,lon_right;
	m_pVi->CV_DP2LLA(
		rect.left(),
		rect.top(),
		&lat_top,&lon_left
		);
	if (lat_top >=85) lat_top = 85;
	m_pVi->CV_DP2LLA(
		rect.right(),
		rect.bottom(),
		&lat_bottom,&lon_right
		);
	if (lat_bottom <=-85) lat_bottom = -85;

	QPen pen_line(QColor(0,0,0,96)), pen_area(QColor(255,0,0,96));
	pen_area.setWidth(4);
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
			if (false==m_pVi->CV_LLA2DP(lat,lon_left,&DP_left,&DP_top))
				continue;
			if (false==m_pVi->CV_LLA2DP(lat,lon_right,&DP_right,&DP_bottom))
				continue;
			pImage->drawLine(DP_left,DP_top,DP_right,DP_bottom);
			sprintf(str,"%-3d",lat);
			pImage->drawText(DP_left,DP_top-8,str);
		}
		for (int lon=int(lon_left/10.0-1.5)*10;lon<=(lon_right/10.0+1.5)*10&&m_pVi->level()>0;lon+=step)
		{

			int DP_left,DP_top,DP_right,DP_bottom;
			if (false==m_pVi->CV_LLA2DP(lat_top,lon,&DP_left,&DP_top))
				continue;
			if (false==m_pVi->CV_LLA2DP(lat_bottom,lon,&DP_right,&DP_bottom))
				continue;
			pImage->drawLine(DP_left,DP_top,DP_right,DP_bottom);
			//warpping
			int nLon = lon;
			while (nLon<-180)
				nLon+=360;
			while (nLon>180)
				nLon-=360;
			sprintf(str,"%-4d",nLon);
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
			if (false==m_pVi->CV_LLA2DP(lat,lon_left,&DP_left,&DP_top))
				continue;
			if (false==m_pVi->CV_LLA2DP(lat,lon_right,&DP_right,&DP_bottom))
				continue;
			pImage->drawLine(DP_left,DP_top,DP_right,DP_bottom);
			sprintf(str,"%-8.3f",lat);
			pImage->drawText(DP_left,DP_top-8,str);
		}
		for (double lon=int(lon_left*(1/step)-1.5)*step;lon<=int(lon_right*(1/step)+1.5)*step&&m_pVi->level()>0;lon+=step)
		{
			//if (lon>=180 || lon<=-180)
			//	continue;
			int DP_left,DP_top,DP_right,DP_bottom;
			if (false==m_pVi->CV_LLA2DP(lat_top,lon,&DP_left,&DP_top))
				continue;
			if (false==m_pVi->CV_LLA2DP(lat_bottom,lon,&DP_right,&DP_bottom))
				continue;
			pImage->drawLine(DP_left,DP_top,DP_right,DP_bottom);
			//Warpping
			float dLon = lon;
			while (dLon<-180)
				dLon+=360;
			while (dLon>180)
				dLon-=360;
			sprintf(str,"%-8.3f",(float)dLon);
			pImage->drawText(DP_left,DP_bottom-16,str);
		}

	}


	pImage->setPen(pen_area);
	int x1,y1,x2,y2;
	int sz = m_list_points.size();
	for (int i=0;i<sz-1;++i)
	{
		m_pVi->CV_LLA2DP(m_list_points[i].x(),m_list_points[i].y(),&x1,&y1);
		m_pVi->CV_LLA2DP(m_list_points[i+1].x(),m_list_points[i+1].y(),&x2,&y2);
		pImage->drawLine(x1,y1,x2,y2);
	}
	if (bFinished==false && sz>0)
	{
		m_pVi->CV_LLA2DP(m_list_points.last().x(),m_list_points.last().y(),&x1,&y1);
		m_pVi->CV_LLA2DP(m_pt_end.x(),m_pt_end.y(),&x2,&y2);
		pImage->drawLine(x1,y1,x2,y2);

	}
	pImage->setPen(oldpen);
}

bool qtvplugin_grid::is_visible()
{
	return m_bVisible;
}

void qtvplugin_grid::set_visible(bool vb)
{
	m_bVisible = vb;
}

QString qtvplugin_grid::get_name()
{
	QString strName = m_SLLibName.mid(10);
	if (m_SLLibName.left(3)=="lib")
		strName = m_SLLibName.mid(13);
	if (strName.length())
		return strName + QString("%1").arg(m_nInstance);
	else
		return "grid";
}
bool qtvplugin_grid::is_active()
{
	return m_bActive;
}

void qtvplugin_grid::set_active(bool ab)
{
	m_bActive = ab;
	ui->checkBox_measure->setChecked(m_bActive);
}

void qtvplugin_grid::set_name(QString /*vb*/)
{

}
void qtvplugin_grid::on_checkBox_measure_clicked(bool acti)
{
	m_bActive = acti;
	m_pVi->updateLayerGridView();
}
bool qtvplugin_grid::cb_event(const QMap<QString, QVariant> para)
{
	if (m_pVi==0)
		return false;
	return false;
}
bool qtvplugin_grid::cb_mouseMoveEvent(QMouseEvent * e )
{
	if (m_pVi==0)
		return false;
	m_mousePos = e->pos();
	if (m_bActive==false)
	{
		double clat,clon;
		char buftmp[256];

		m_pVi->CV_DP2LLA(m_mousePos.x(),m_mousePos.y(),&clat,&clon);
		sprintf (buftmp,"Mouse LAT=%14.9lf, LON=%14.9lf\n",clat,clon);
		QString strMsg = buftmp;
		m_pVi->centerLLA(&clat,&clon);
		sprintf (buftmp,"Center LAT=%14.9lf, LON=%14.9lf",clat,clon);
		strMsg += buftmp;
		ui->plainTextEdit_cursor->setPlainText(strMsg);
		return false;
	}
	if (m_list_points.size()>0&&bFinished==false)
	{
		QPoint pt = e->pos();
		double lat,lon;
		m_pVi->CV_DP2LLA(pt.x(),pt.y(),&lat,&lon);
		QPointF point(lat,lon);
		m_pt_end = point;
		return true;
	}
	return false;
}

bool qtvplugin_grid::cb_mousePressEvent(QMouseEvent *e )
{
	if (m_pVi==0)
		return false;
	if (m_bActive==false)
		return false;
	if (e->buttons() & Qt::LeftButton)
	{
		QPoint pt = e->pos();
		double lat,lon;
		m_pVi->CV_DP2LLA(pt.x(),pt.y(),&lat,&lon);
		QPointF point(lat,lon);
		if (bFinished==true)
		{
			m_list_points.clear();
			bFinished = false;
		}
		m_list_points.push_back(point);
		m_pt_end = point;
		CalArea();
		return true;
	}
	else if (e->buttons() & Qt::RightButton)
	{
		if (bFinished==true)
		{
			m_list_points.clear();
			return true;
		}
		if (m_list_points.empty()==false)
			m_list_points.push_back(m_list_points.first());
		bFinished = true;
		CalArea();
		return true;
	}
	return false;
}

void qtvplugin_grid::CalArea()
{
	QString strMsg;
	int Count = m_list_points.size();
	if (Count<1)
		return;
	double * PointX = new double [Count];
	double * PointY = new double [Count];
	char  * buffertmp = new char[2048];
	//转换为经纬度
	bool Valid = true;
	strMsg += "Points:\n";
	//Length
	double dTotalDis = 0;
	double dPreLat,dPreLon;
	QString str_Polygon,str_LineString;

	str_Polygon = ("ST_Transform(ST_GeomFromText('POLYGON(\n(");
	str_LineString = ("ST_Transform(ST_GeomFromText('LINESTRING(\n(");

	for (int i=0;i<Count;i++)
	{
		double dLatCurr = m_list_points[i].x(),dLonCurr = m_list_points[i].y();
		if (i>0)
		{
			double sita;
			double dis = CalDistance(dPreLat,dLatCurr,dPreLon,dLonCurr, &sita);

			sprintf(buffertmp,"(%11.6lf,%11.6lf)->(%11.6lf,%11.6lf),%11.2lf(m),%11.2lf(km),SITA=%.12lf (degree)\n",
				dPreLat,dPreLon,dLatCurr,dLonCurr,dis,dis/1000.0,sita);
			strMsg += buffertmp;
			dTotalDis += dis;
		}
		sprintf(buffertmp,("\t%14.9lf %14.9lf"),dLonCurr,dLatCurr);
		strMsg += buffertmp;
		if (i!=Count-1)
			strcat(buffertmp,",");
		strcat(buffertmp,"\n");
		str_Polygon += buffertmp;
		str_LineString += buffertmp;

		dPreLat = dLatCurr;
		dPreLon = dLonCurr;


		PointX [i] = dLonCurr;
		PointY [i] = dLatCurr;
	}
	str_Polygon += (")\n)',4326),900913)\n");
	str_LineString +=  (")',4326),900913)\n");

	if (Valid==true)
	{
		double Area = GetArea(PointX,PointY,Count);
		sprintf(buffertmp,"\n\nArea:\n%lf\tm^2\n%lf\tkm^2\n\nTotalLength:\n%lf\t(m)\n%lf\t(km)\n\nPostGIS String:\n",
			Area,Area/1000000.0,
			dTotalDis,dTotalDis/1000.0);
		strMsg += buffertmp;
		strMsg += str_Polygon;
		strMsg += str_LineString;
	}


	delete [] PointX;
	delete [] PointY;
	delete [] buffertmp;

	ui->plainTextEdit_RES->setPlainText(strMsg);
}

double qtvplugin_grid::GetArea(double * PointX,double * PointY,int Count)
{
	const double pi=3.1415926535897932384626433832795;
	const double R=6378137;
	try
	{
		if (Count>3)
		{
			double mtotalArea = 0;
			if((PointX[0]!=PointX[Count-1])||(PointY[0]!=PointY[Count-1]))
				return 0;
			Count--;
			double LowX=0.0;
			double LowY=0.0;
			double MiddleX=0.0;
			double MiddleY=0.0;
			double HighX=0.0;
			double HighY=0.0;

			double AM = 0.0;
			double BM = 0.0;
			double CM = 0.0;

			double AL = 0.0;
			double BL = 0.0;
			double CL = 0.0;

			double AH = 0.0;
			double BH = 0.0;
			double CH = 0.0;

			double CoefficientL = 0.0;
			double CoefficientH = 0.0;

			double ALtangent = 0.0;
			double BLtangent = 0.0;
			double CLtangent = 0.0;

			double AHtangent = 0.0;
			double BHtangent = 0.0;
			double CHtangent = 0.0;

			double ANormalLine = 0.0;
			double BNormalLine = 0.0;
			double CNormalLine = 0.0;

			double OrientationValue = 0.0;

			double AngleCos = 0.0;

			double Sum1 = 0.0;
			double Sum2 = 0.0;
			double Count2 = 0;
			double Count1 = 0;


			double Sum = 0.0;
			double Radius = R;
			double Math_PI = pi;
			for(int i=0;i<Count;i++)
			{
				if(i==0)
				{
					LowX = PointX[Count-1] * Math_PI / 180;
					LowY = PointY[Count-1] * Math_PI / 180;
					MiddleX = PointX[0] * Math_PI / 180;
					MiddleY = PointY[0] * Math_PI / 180;
					HighX = PointX[1] * Math_PI / 180;
					HighY = PointY[1] * Math_PI / 180;
				}
				else if(i==Count-1)
				{
					LowX = PointX[Count-2] * Math_PI / 180;
					LowY = PointY[Count-2] * Math_PI / 180;
					MiddleX = PointX[Count-1] * Math_PI / 180;
					MiddleY = PointY[Count-1] * Math_PI / 180;
					HighX = PointX[0] * Math_PI / 180;
					HighY = PointY[0] * Math_PI / 180;
				}
				else
				{
					LowX = PointX[i-1] * Math_PI / 180;
					LowY = PointY[i-1] * Math_PI / 180;
					MiddleX = PointX[i] * Math_PI / 180;
					MiddleY = PointY[i] * Math_PI / 180;
					HighX = PointX[i+1] * Math_PI / 180;
					HighY = PointY[i+1] * Math_PI / 180;
				}

				AM = cos(MiddleY) * cos(MiddleX);
				BM = cos(MiddleY) * sin(MiddleX);
				CM = sin(MiddleY);
				AL = cos(LowY) * cos(LowX);
				BL = cos(LowY) * sin(LowX);
				CL = sin(LowY);
				AH = cos(HighY) * cos(HighX);
				BH = cos(HighY) * sin(HighX);
				CH = sin(HighY);


				CoefficientL = (AM*AM + BM*BM + CM*CM)/(AM*AL + BM*BL + CM*CL);
				CoefficientH = (AM*AM + BM*BM + CM*CM)/(AM*AH + BM*BH + CM*CH);

				ALtangent = CoefficientL * AL - AM;
				BLtangent = CoefficientL * BL - BM;
				CLtangent = CoefficientL * CL - CM;
				AHtangent = CoefficientH * AH - AM;
				BHtangent = CoefficientH * BH - BM;
				CHtangent = CoefficientH * CH - CM;


				AngleCos = (AHtangent * ALtangent + BHtangent * BLtangent + CHtangent * CLtangent)/
					(sqrt(AHtangent * AHtangent + BHtangent * BHtangent +CHtangent * CHtangent) *
					sqrt(ALtangent * ALtangent + BLtangent * BLtangent +CLtangent * CLtangent));

				AngleCos = acos(AngleCos);

				ANormalLine = BHtangent * CLtangent - CHtangent * BLtangent;
				BNormalLine = 0 - (AHtangent * CLtangent - CHtangent * ALtangent);
				CNormalLine = AHtangent * BLtangent - BHtangent * ALtangent;

				if(AM!=0)
					OrientationValue = ANormalLine/AM;
				else if(BM!=0)
					OrientationValue = BNormalLine/BM;
				else
					OrientationValue = CNormalLine/CM;

				if(OrientationValue>0)
				{
					Sum1 += AngleCos;
					Count1 ++;

				}
				else
				{
					Sum2 += AngleCos;
					Count2 ++;
				}

			}

			if(Sum1>Sum2){
				Sum = Sum1+(2*Math_PI*Count2-Sum2);
			}
			else{
				Sum = (2*Math_PI*Count1-Sum1)+Sum2;
			}

			//m^2
			mtotalArea = (Sum-(Count-2)*Math_PI)*Radius*Radius;
			return mtotalArea;
		}
	}
	catch(...)
	{
		return 0;
	}
	return 0;
}


double qtvplugin_grid::CalDistance(double dLatStart,double dLatEnd,double dLonStart,double dLonEnd,double * psita)
{
	//deg to rad
	const double pi=3.1415926535897932384626433832795;
	const double R=6378137;
	double drLatStart = dLatStart /180.0 * pi;
	double drLatEnd = dLatEnd /180.0 * pi;
	double drLonStart = dLonStart /180.0 * pi;
	double drLonEnd = dLonEnd /180.0 * pi;

	//Calc the ECEF coords
	double x1,y1,z1,x2,y2,z2;
	x1 = cos(drLatStart)*cos(drLonStart);
	y1 = sin(drLatStart);
	z1 = cos(drLatStart)*sin(drLonStart);

	x2 = cos(drLatEnd)*cos(drLonEnd);
	y2 = sin(drLatEnd);
	z2 = cos(drLatEnd)*sin(drLonEnd);


	//COS Angle
	double dot = x1*x2+y1*y2+z1*z2;
	double abs1 = sqrt(x1*x1+y1*y1+z1*z1);
	double abs2 = sqrt(x2*x2+y2*y2+z2*z2);
	double cossita = dot/(abs1*abs2);
	double sita = acos(cossita);

	double dist = R * sita;

	*psita = sita / pi * 180;

	return dist;
}

QMap<QString, QVariant> qtvplugin_grid::call_func(const  QMap<QString, QVariant> paras)
{
	QMap<QString, QVariant> res;
	if (paras.contains("function"))
	{
		QString funct = paras["function"].toString();
		if (funct=="get_region")
		{
			int Count = m_list_points.size();
			res["size"] = Count;
			for (int i=0;i<Count;++i)
			{
				QString latkey = QString("lat%1").arg(i);
				QString lonkey = QString("lon%1").arg(i);
				res[latkey] = m_list_points[i].x();
				res[lonkey] = m_list_points[i].y();
			}
		}
	}
	return std::move(res);
}
