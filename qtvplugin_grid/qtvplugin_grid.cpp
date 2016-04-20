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
#include <QMessageBox>
#include <QSettings>
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
	m_nMarks = 0;
	m_pModelCombo = new QStandardItemModel(this);
	m_pModelCombo->appendRow(new QStandardItem(tr("POINT")));
	m_pModelCombo->appendRow(new QStandardItem(tr("LINE")));
	m_pModelCombo->appendRow(new QStandardItem(tr("POLYGON")));
	ui->combox_type->setModel(m_pModelCombo);
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
		m_SLLibPath = strSLibPath;

		m_pVi = ptrviewer;

		mutex_instances.lock();
		m_nInstance = ++count_instances[m_SLLibName];
		mutex_instances.unlock();

		loadTranslation();
		load_ini();
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



	int x1,y1,x2,y2;
	int sz = m_list_points.size();
	QPen pen_text(QColor(0,0,255));
	for (int i=0;i<sz-1;++i)
	{
		m_pVi->CV_LLA2DP(m_list_points[i].x(),m_list_points[i].y(),&x1,&y1);
		m_pVi->CV_LLA2DP(m_list_points[i+1].x(),m_list_points[i+1].y(),&x2,&y2);
		pImage->setPen(pen_area);
		pImage->drawLine(x1,y1,x2,y2);
		double sita;
		double dis = CalDistance(m_list_points[i].x(),m_list_points[i+1].x(),m_list_points[i].y(),m_list_points[i+1].y(), &sita);
		sprintf(str,"%.2lfkm",dis/1000.0);
		pImage->setPen(pen_text);
		QPointF pos = QLineF(x1,y1,x2,y2).pointAt(i*0.8 / sz+0.1);
		pImage->drawText(pos,str);
	}
	if (bFinished==false && sz>0)
	{
		m_pVi->CV_LLA2DP(m_list_points.last().x(),m_list_points.last().y(),&x1,&y1);
		m_pVi->CV_LLA2DP(m_pt_end.x(),m_pt_end.y(),&x2,&y2);
		pImage->setPen(pen_area);
		pImage->drawLine(x1,y1,x2,y2);
		double sita;
		double dis = CalDistance(m_list_points.last().x(),m_pt_end.x(),m_list_points.last().y(),m_pt_end.y(), &sita);
		sprintf(str,"%.1lfkm",dis/1000.0);
		pImage->setPen(pen_text);
		QPointF pos = QLineF(x1,y1,x2,y2).pointAt(1*0.8 / sz+0.1);
		pImage->drawText(pos,str);
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
	save_ini();
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
	if (m_bVisible==true)
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
	}
	if (m_bActive==false)
		return false;
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
	bool Valid = true;
	strMsg += "Points:\n";
	//Length
	double dTotalDis = 0;
	double dPreLat,dPreLon;
	QString str_Polygon,str_LineString,str_QuickMark;

	str_Polygon = ("ST_Transform(ST_GeomFromText('POLYGON(\n(");
	str_LineString = ("ST_Transform(ST_GeomFromText('LINESTRING(\n(");

	bool bLatFirst = ui->radioButton_latfirst->isChecked();

	for (int i=0;i<Count;i++)
	{
		double dLatCurr = m_list_points[i].x(),dLonCurr = m_list_points[i].y();

		if (bLatFirst)
			str_QuickMark += QString("%1 , %2\n").arg(dLatCurr,0,'f',7).arg(dLonCurr,0,'f',7);
		else
			str_QuickMark += QString("%2 , %1\n").arg(dLatCurr,0,'f',7).arg(dLonCurr,0,'f',7);
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
	ui->plainTextEdit_markcmd->setPlainText(str_QuickMark);
}

/**
 * @brief Classical Polygon Area caculate method
 *
 * @param PointX	the x points
 * @param PointY	the y points
 * @param Count		the points' count
 * @return double	area.
 */
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


/**
 * @brief Calculate distance between  2 positions
 *
 * @param dLatStart	start latitude
 * @param dLatEnd	end latitude
 * @param dLonStart	start longitude
 * @param dLonEnd	end longitude
 * @param psita		the earth center angle in polar
 * @return double	distance.
 */
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

/**
 * function calls avaliable:
 * 	1.function=get_polygon, no other para needed. returns current selected polygon's cornor points, in lat, lon; size=N;lat0=XX;lon0=XX;
 * 	lat1=XX;lon1=XX;lat2=XX;lon2=XX;...;latN-1=XX;lonN-1=XX.
 * 	2.function=get_ruler_status, no other para needed.returns whether ruler tool is active now, status=0 means not active, status=-1 means active.
 * 	3.function=set_ruler_status, with para status, will set ruler status to given value.
 * please notice that the function should be called from the MAIN THREAD ONLY.
 *
 * @param paras	the key-value style paraments.
 * @return QMap<QString, QVariant>	the key-value style return paraments.
 */
QMap<QString, QVariant> qtvplugin_grid::call_func(const  QMap<QString, QVariant> & paras)
{
	QMap<QString, QVariant> res;
	if (paras.contains("function"))
	{
		QString funct = paras["function"].toString();
		if (funct=="get_polygon")
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
		else if (funct=="get_ruler_status")
		{
			res["status"] = m_bActive==false?0:-1;
		}
		else if (funct=="set_ruler_status")
		{
			int status = -1;
			if (paras.contains("status"))
				status = paras["status"].toInt();
			set_active(status==0?false:true);
			res["status"] = m_bActive==false?0:-1;
		}
		else
			res["error"] = QString("unknown function \"%1\".").arg(funct);
	}
	else
		res["error"] = "\"function\" keyword not specified, nothing to do.";
	return std::move(res);
}
void qtvplugin_grid::on_pushButton_add_mark_clicked()
{
	if (!m_pVi)
		return;
	QString strMarkerName = QString("geomarker%1").arg(m_nInstance);
	layer_interface * pif =  m_pVi->layer(strMarkerName);
	save_ini();
	int tp = ui->combox_type->currentIndex();
	QString strAll = ui->plainTextEdit_markcmd->toPlainText();
	QStringList strLines = strAll.split("\n",QString::SkipEmptyParts);
	int c = 0;
	bool bLatFirst = ui->radioButton_latfirst->isChecked();
	QMap<QString, QVariant> map_multi;
	foreach (QString str, strLines)
	{
		QString strRegWest = QString("([%1])+").arg(ui->lineEdit_west_spliter->text());
		QString strRegsout = QString("([%1])+").arg(ui->lineEdit_south_spliter->text());
		int latNG = 1, lonNG = 1;

		if (str.indexOf(QRegExp(strRegWest))>=0)
			lonNG=-1;
		if (str.indexOf(QRegExp(strRegsout))>=0)
			latNG=-1;


		QStringList lst = str.split(QRegExp("([^0123456789.-])+"),QString::SkipEmptyParts);
		double lat = 0, lon = 0;
		if (lst.size()==6)
		{
			lat = lst.first().toDouble();			lst.pop_front();
			lat += lst.first().toDouble() /60.0;	lst.pop_front();
			lat += lst.first().toDouble() /3600.0;	lst.pop_front();
			lon = lst.first().toDouble();			lst.pop_front();
			lon += lst.first().toDouble() /60.0;	lst.pop_front();
			lon += lst.first().toDouble() /3600.0;
		}
		else if (lst.size()==4)
		{
			lat = lst.first().toDouble();			lst.pop_front();
			lat += lst.first().toDouble() /60.0;	lst.pop_front();
			lon = lst.first().toDouble();			lst.pop_front();
			lon += lst.first().toDouble() /60.0;
		}
		else if (lst.size()==2)
		{
			lat = lst.first().toDouble();			lst.pop_front();
			lon = lst.first().toDouble();
		}
		else
		{
			QMessageBox::warning(
						this,
						tr("Error LLA formar"),
						tr("lat lon must have same element nums.")
						);
			break;
		}
		if (bLatFirst==false)
		{
			double tmp = lat;
			lat = lon;
			lon = tmp;
		}
		lat *= latNG;
		lon *= lonNG;


		if (tp==0)
		{
			QMap<QString, QVariant> inPara, outPara;
			inPara["function"] = "update_point";
			inPara["name"] = QString("%1_%2").arg(get_name()).arg(m_nMarks);
			inPara["lat"] = lat;
			inPara["lon"] = lon;
			inPara["color_pen"] = "0,0,255,128";
			inPara["color_brush"] = "0,0,0,64";
			inPara["width"] = "7";
			inPara["height"] = "7";
			inPara["type"] = 1;
			outPara = pif->call_func(inPara);
			inPara.clear();
			inPara["function"] = "update_props";
			inPara["name"] = QString("%1_%2").arg(get_name()).arg(m_nMarks);
			inPara["LABEL"] = QString("%1,%2").arg(lat).arg(lon);
			++m_nMarks;
			outPara = pif->call_func(inPara);
		}
		else
		{
			map_multi[QString("lat%1").arg(c)] = lat;
			map_multi[QString("lon%1").arg(c)] = lon;
			++c;
		}

	}
	if (tp)
	{
		QMap<QString, QVariant> outPara;
		map_multi["function"] = "update_polygon";
		map_multi["name"] = QString("%1_%2").arg(get_name()).arg(m_nMarks);
		map_multi["color_pen"] = "0,0,255,128";
		map_multi["color_brush"] = "0,0,0,64";
		map_multi["type"] = (tp==1)?6:4;
		outPara = pif->call_func(map_multi);
		++m_nMarks;
	}
}

void qtvplugin_grid::on_pushButton_clear_clicked()
{
	if (!m_pVi || m_nMarks<=0)
		return;
	save_ini();
	QString strMarkerName = QString("geomarker%1").arg(m_nInstance);
	layer_interface * pif =  m_pVi->layer(strMarkerName);
	if (pif)
	{
		QMap<QString, QVariant> inPara, outPara;
		inPara["function"] = "delete_marks";
		for (int i=0;i<m_nMarks;++i)
			inPara[QString("name%1").arg(i)] = QString("%1_%2").arg(get_name()).arg(i);
		outPara = pif->call_func(inPara);
	}
	m_nMarks = 0;


}
void qtvplugin_grid::on_pushButton_clear_all_clicked()
{

	if (!m_pVi)
		return;
	save_ini();
	QString strMarkerName = QString("geomarker%1").arg(m_nInstance);
	layer_interface * pif =  m_pVi->layer(strMarkerName);
	if (pif)
	{
		QMap<QString, QVariant> inPara, outPara;
		inPara["function"] = "delete_marks";
		outPara = pif->call_func(inPara);
	}
	m_nMarks = 0;

}
QString qtvplugin_grid::ini_file()
{
	if (m_SLLibPath.size())
		return m_SLLibPath + QString("%1").arg(m_nInstance) + ".ini";
	else
		return QCoreApplication::applicationFilePath() + QString("/grid%1.ini").arg(m_nInstance);
}
void  qtvplugin_grid::load_ini()
{
	QSettings settings(ini_file(),QSettings::IniFormat);
	ui->lineEdit_south_spliter->setText(settings.value("settings/lineEdit_south_spliter","S").toString());
	ui->lineEdit_west_spliter->setText(settings.value("settings/lineEdit_west_spliter","W").toString());
	ui->plainTextEdit_markcmd->setPlainText(settings.value("settings/plainTextEdit_markcmd","").toString());
	ui->combox_type->setCurrentIndex(settings.value("settings/combox_type",0).toInt());
	bool bLatFirst = settings.value("settings/latfirst",true).toBool();
	if (bLatFirst)
		ui->radioButton_latfirst->setChecked(true);
	else
		ui->radioButton_lonfirst->setChecked(true);
}

void  qtvplugin_grid::save_ini()
{
	QSettings settings(ini_file(),QSettings::IniFormat);
	settings.setValue("settings/lineEdit_south_spliter",ui->lineEdit_south_spliter->text());
	settings.setValue("settings/lineEdit_west_spliter",ui->lineEdit_west_spliter->text());
	settings.setValue("settings/plainTextEdit_markcmd",ui->plainTextEdit_markcmd->toPlainText());
	settings.setValue("settings/combox_type",ui->combox_type->currentIndex());
	bool bLatFirst = ui->radioButton_latfirst->isChecked();
	settings.setValue("settings/latfirst",bLatFirst);
}
