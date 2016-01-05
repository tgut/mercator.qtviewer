#include "qtvplugin_geomarker.h"
#include "ui_qtvplugin_geomarker.h"
#include <QPainter>
#include <QBrush>
#include <QCoreApplication>
#include <QPen>
#include <QDebug>
#include <QLibraryInfo>
#include <QFileInfo>
#include <QMutex>
#include <QGraphicsSceneMouseEvent>
#include <QMap>
#include <assert.h>
#include "geographicsellipseitem.h"
#include "geographicsrectitem.h"
#include "geographicslineitem.h"
#include "geographicspolygonitem.h"
QMutex mutex_instances;
QMap<viewer_interface *,  qtvplugin_geomarker * > map_instances;
QMap<QString,  int > count_instances;
qtvplugin_geomarker::qtvplugin_geomarker(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::qtvplugin_geomarker)
{
	m_nInstance = 0;
	ui->setupUi(this);
	m_pVi = 0;
	m_bVisible = true;
	m_pScene = new QTVP_GEOMARKER::geoGraphicsScene(this);
	m_pScene->setBackgroundBrush(Qt::NoBrush);
	//senece is 256 * 2^level
	m_pScene->setSceneRect(0,0,256,256);

	m_pGeoItemModel = new QStandardItemModel(this);
	m_pGeoItemModel->setColumnCount(3);
	m_pGeoItemModel->setHeaderData(0,Qt::Horizontal,tr("Name"));
	m_pGeoItemModel->setHeaderData(1,Qt::Horizontal,tr("Type"));
	m_pGeoItemModel->setHeaderData(2,Qt::Horizontal,tr("Props"));
	ui->tableView_marks->setModel(m_pGeoItemModel);

	m_pGeoPropModel = new QStandardItemModel(this);
	m_pGeoPropModel->setColumnCount(2);
	m_pGeoPropModel->setHeaderData(0,Qt::Horizontal,tr("Name"));
	m_pGeoPropModel->setHeaderData(1,Qt::Horizontal,tr("Value"));
	ui->tableView_props->setModel(m_pGeoPropModel);

	m_pLineStyleModel = new QStandardItemModel(this);
	m_pLineStyleModel->appendRow(new QStandardItem("NoPen"));
	m_pLineStyleModel->appendRow(new QStandardItem("SolidLine"));
	m_pLineStyleModel->appendRow(new QStandardItem("DashLine"));
	m_pLineStyleModel->appendRow(new QStandardItem("DotLine"));
	m_pLineStyleModel->appendRow(new QStandardItem("DashDotLine"));
	m_pLineStyleModel->appendRow(new QStandardItem("DashDotDotLine"));
	m_pLineStyleModel->appendRow(new QStandardItem("CustomDashLine"));
	ui->comboBox_linePad->setModel(m_pLineStyleModel);

	m_pFillStyleModel = new QStandardItemModel(this);
	m_pFillStyleModel->appendRow(new QStandardItem("NoBrush"));
	m_pFillStyleModel->appendRow(new QStandardItem("SolidPattern"));
	m_pFillStyleModel->appendRow(new QStandardItem("Dense1Pattern"));
	m_pFillStyleModel->appendRow(new QStandardItem("Dense2Pattern"));
	m_pFillStyleModel->appendRow(new QStandardItem("Dense3Pattern"));
	m_pFillStyleModel->appendRow(new QStandardItem("Dense4Pattern"));
	m_pFillStyleModel->appendRow(new QStandardItem("Dense5Pattern"));
	m_pFillStyleModel->appendRow(new QStandardItem("Dense6Pattern"));
	m_pFillStyleModel->appendRow(new QStandardItem("Dense7Pattern"));
	m_pFillStyleModel->appendRow(new QStandardItem("HorPattern"));
	m_pFillStyleModel->appendRow(new QStandardItem("VerPattern"));
	m_pFillStyleModel->appendRow(new QStandardItem("CrossPattern"));
	m_pFillStyleModel->appendRow(new QStandardItem("BDiagPattern"));
	m_pFillStyleModel->appendRow(new QStandardItem("FDiagPattern"));
	m_pFillStyleModel->appendRow(new QStandardItem("DiagCrossPattern"));
	ui->comboBox_fillPad->setModel(m_pFillStyleModel);

	m_bNeedRefresh = false;
	m_nTimerID_refreshUI = startTimer(2000);
}

qtvplugin_geomarker::~qtvplugin_geomarker()
{
	delete ui;
}
void qtvplugin_geomarker::load_retranslate_UI()
{
	ui->retranslateUi(this);
}

layer_interface * qtvplugin_geomarker::load_initial_plugin(QString strSLibPath,viewer_interface  * ptrviewer)
{
	qtvplugin_geomarker * instance = 0;
	mutex_instances.lock();
	if (map_instances.empty()==true)
	{
		map_instances[ptrviewer] = this;
		instance = this;
	}
	else if (map_instances.contains(ptrviewer)==false)
	{
		instance = new qtvplugin_geomarker;
		map_instances[ptrviewer] = instance;
	}
	else
		instance = map_instances[ptrviewer];
	mutex_instances.unlock();
	if (instance==this)
	{
		QFileInfo info(strSLibPath);
		m_SLLibPath = strSLibPath;
		m_SLLibName = info.completeBaseName();
		m_pVi = ptrviewer;

		mutex_instances.lock();
		m_nInstance = ++count_instances[m_SLLibName];
		mutex_instances.unlock();

		loadTranslations();
		loadSettingsFromIni();
	}
	else
	{
		 layer_interface * ret = instance->load_initial_plugin(strSLibPath,ptrviewer);
		 assert(ret==instance);
		 return ret;
	}

	return instance;
}
void qtvplugin_geomarker::loadTranslations()
{
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
		}
		else
			QTVOSM_WARNING("Load translationfile")<<"\n\t"<<strTransLocalFile<<" Not Found.";

	}
}

QWidget * qtvplugin_geomarker::load_prop_window()
{
	return this;
}

void qtvplugin_geomarker::cb_paintEvent( QPainter * pImage )
{
	if (!m_pVi || m_bVisible==false)
		return ;
	QRect rect = m_pVi->windowRect();
	// Calc current viewport in world
	double leftcenx, topceny, rightcenx, bottomceny;
	m_pVi->CV_DP2World(0,0,&leftcenx,&topceny);
	m_pVi->CV_DP2World(rect.width()-1,rect.height()-1,&rightcenx,&bottomceny);
	QRectF source(
				leftcenx ,
				topceny,
				(rightcenx - leftcenx),
				(bottomceny - topceny)
				);
	int winsz = 256 * (1<<m_pVi->level());
	//Warpping 180, -180
	for (int t = -1; t <=1;++t)
	{
		QRectF destin(
					0 + winsz * t ,
					0,
					rect.width(),
					rect.height()
					);
		if (destin.right() < 0 || destin.left() >= rect.width())
			continue;
		m_pScene->render(pImage,destin,source);
	}
}

void qtvplugin_geomarker::cb_levelChanged(int level)
{
	if (!m_pVi)
		return ;
	//Adjust new Scene rect
	QRectF rect(0,0,256*(1<<level),256*(1<<level));
	m_pScene->adjust_item_coords(level);
	m_pScene->setSceneRect(rect);
}

bool qtvplugin_geomarker::is_visible()
{
	return m_bVisible;
}

void qtvplugin_geomarker::set_visible(bool vb)
{
	m_bVisible = vb;
}

QString qtvplugin_geomarker::get_name()
{
	QString strName = m_SLLibName.mid(10);
	if (m_SLLibName.left(3)=="lib")
		strName = m_SLLibName.mid(13);
	if (strName.length())
		return strName + QString("%1").arg(m_nInstance);
	else
		return "geomarker";
}

void qtvplugin_geomarker::set_name(QString /*vb*/)
{
	if (!m_pVi)
		return ;

}

bool qtvplugin_geomarker::cb_event(const QMap<QString, QVariant> para)
{
	return false;
}
bool		qtvplugin_geomarker::cb_mouseReleaseEvent(QMouseEvent * e)
{
	if (!m_pVi)
		return false;

	QPoint mouse_view_pt = e->pos();
	int winsz = 256 * (1<<m_pVi->level());
	double wx,wy;
	m_pVi->CV_DP2World(mouse_view_pt.x(),mouse_view_pt.y(),&wx,&wy);
	//Warp
	while (wx < 0) wx += winsz;
	while (wx > winsz-1) wx -= winsz;

	QPointF mouse_scene_pt(wx,wy);
	QPoint mouse_screen_pt = e->globalPos();
	Qt::MouseButton mouse_button = e->button();
	QWidget * pwig = dynamic_cast<QWidget *> (m_pVi);
	if (m_bVisible && pwig)
	{
		// Convert and deliver the mouse event to the scene.
		QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseRelease);
		mouseEvent.setWidget(pwig);
		mouseEvent.setButtonDownScenePos(mouse_button, mouse_scene_pt);
		mouseEvent.setButtonDownScreenPos(mouse_button, mouse_screen_pt);
		mouseEvent.setScenePos(mouse_scene_pt);
		mouseEvent.setScreenPos(mouse_screen_pt);
		mouseEvent.setLastScenePos(mouse_scene_pt);
		mouseEvent.setLastScreenPos(mouse_screen_pt);
		mouseEvent.setButtons(e->buttons());
		mouseEvent.setButton(e->button());
		mouseEvent.setModifiers(e->modifiers());
		mouseEvent.setAccepted(false);
		QApplication::sendEvent(m_pScene, &mouseEvent);
		bool isAccepted = mouseEvent.isAccepted();
		e->setAccepted(isAccepted);
		return isAccepted;
	}
	return false;
}

bool		qtvplugin_geomarker::cb_mouseDoubleClickEvent(QMouseEvent * e)
{
	if (!m_pVi)
		return false;

	QPoint mouse_view_pt = e->pos();
	int winsz = 256 * (1<<m_pVi->level());
	double wx,wy;
	double mlat, mlon;
	m_pVi->CV_DP2World(mouse_view_pt.x(),mouse_view_pt.y(),&wx,&wy);
	m_pVi->CV_DP2LLA(mouse_view_pt.x(),mouse_view_pt.y(),&mlat,&mlon);
	ui->lineEdit_point_lat->setText(QString("%1").arg(mlat,0,'f',14));
	ui->lineEdit_point_lon->setText(QString("%1").arg(mlon,0,'f',14));
	//Warp
	while (wx < 0) wx += winsz;
	while (wx > winsz-1) wx -= winsz;

	QPointF mouse_scene_pt(wx,wy);
	QPoint mouse_screen_pt = e->globalPos();
	Qt::MouseButton mouse_button = e->button();
	QWidget * pwig = dynamic_cast<QWidget *> (m_pVi);
	if (m_bVisible && pwig)
	{
		// Convert and deliver the mouse event to the scene.
		QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseDoubleClick);
		mouseEvent.setWidget(pwig);
		mouseEvent.setButtonDownScenePos(mouse_button, mouse_scene_pt);
		mouseEvent.setButtonDownScreenPos(mouse_button, mouse_screen_pt);
		mouseEvent.setScenePos(mouse_scene_pt);
		mouseEvent.setScreenPos(mouse_screen_pt);
		mouseEvent.setLastScenePos(mouse_scene_pt);
		mouseEvent.setLastScreenPos(mouse_screen_pt);
		mouseEvent.setButtons(e->buttons());
		mouseEvent.setButton(e->button());
		mouseEvent.setModifiers(e->modifiers());
		mouseEvent.setAccepted(false);
		QApplication::sendEvent(m_pScene, &mouseEvent);
		bool isAccepted = mouseEvent.isAccepted();
		e->setAccepted(isAccepted);
		return isAccepted;
	}
	return false;
}

bool qtvplugin_geomarker::cb_mousePressEvent(QMouseEvent * e)
{
	if (!m_pVi)
		return false;

	QPoint mouse_view_pt = e->pos();
	int winsz = 256 * (1<<m_pVi->level());
	double wx,wy;
	m_pVi->CV_DP2World(mouse_view_pt.x(),mouse_view_pt.y(),&wx,&wy);
	//Warp
	while (wx < 0) wx += winsz;
	while (wx > winsz-1) wx -= winsz;

	QPointF mouse_scene_pt(wx,wy);
	QPoint mouse_screen_pt = e->globalPos();
	Qt::MouseButton mouse_button = e->button();
	QWidget * pwig = dynamic_cast<QWidget *> (m_pVi);
	if (m_bVisible && pwig)
	{
		// Convert and deliver the mouse event to the scene.
		QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMousePress);
		mouseEvent.setWidget(pwig);
		mouseEvent.setButtonDownScenePos(mouse_button, mouse_scene_pt);
		mouseEvent.setButtonDownScreenPos(mouse_button, mouse_screen_pt);
		mouseEvent.setScenePos(mouse_scene_pt);
		mouseEvent.setScreenPos(mouse_screen_pt);
		mouseEvent.setLastScenePos(mouse_scene_pt);
		mouseEvent.setLastScreenPos(mouse_screen_pt);
		mouseEvent.setButtons(e->buttons());
		mouseEvent.setButton(e->button());
		mouseEvent.setModifiers(e->modifiers());
		mouseEvent.setAccepted(false);
		QApplication::sendEvent(m_pScene, &mouseEvent);
		bool isAccepted = mouseEvent.isAccepted();
		e->setAccepted(isAccepted);
		//return isAccepted;
		return true;
	}
	return false;

}

QColor qtvplugin_geomarker::string2color(const QString & s)
{
	QStringList lst = s.split(",",QString::SkipEmptyParts);
	int r = 255,g = 255, b = 255, a= 128;
	if (lst.empty()==false) {r = lst.first().toInt(); lst.pop_front();}
	if (lst.empty()==false) {g = lst.first().toInt(); lst.pop_front();}
	if (lst.empty()==false) {b = lst.first().toInt(); lst.pop_front();}
	if (lst.empty()==false) {a = lst.first().toInt(); lst.pop_front();}
	return QColor(r,g,b,a);
}
QString qtvplugin_geomarker::color2string(const QColor & col)
{
	quint32 cv =(col.alpha()<<24) +(col.blue()<<16) + (col.green()<<8) + (col.red());
	QString str = QString("%1,%2,%3,%4,%5").arg(col.red()).arg(col.green()).arg(col.blue()).arg(col.alpha()).arg(cv);
	return str;
}

QString qtvplugin_geomarker::inifile()
{
	if (m_SLLibPath.size())
		return m_SLLibPath + QString("%1").arg(m_nInstance) + ".ini";
	else
		return QCoreApplication::applicationFilePath() + QString("/geomarker%1.ini").arg(m_nInstance);
}
void qtvplugin_geomarker::refreshMarks()
{
	if (!m_pVi || !m_pScene)
		return;
	m_bNeedRefresh = true;
}

void qtvplugin_geomarker::update_line(const QString & name,double lat1, double lon1,double lat2, double lon2, QPen pen)
{
	//Get raw Item by name
	QTVP_GEOMARKER::geoItemBase * base = m_pScene->geoitem_by_name(name);
	//Get Props
	QStringList propNames;
	QVariantList propValues;
	if (base)
	{
		propNames = base->prop_names();
		propValues = base->prop_values();
	}
	//type convertion to T
	QTVP_GEOMARKER::geoGraphicsLineItem * pitem = base?dynamic_cast<QTVP_GEOMARKER::geoGraphicsLineItem  *>(base):0;
	if (!pitem)
		pitem	= new QTVP_GEOMARKER::geoGraphicsLineItem(name,
						this->m_pVi,
						lat1,lon1,lat2,lon2);

	pitem->setPen(pen);

	if (false==this->m_pScene->addItem(pitem,0))
	{
		if (base != pitem)
			delete pitem;
	}
	else
	{
		int cs = propNames.size();
		for (int i=0;i<cs && base != pitem;++i)
		{
			pitem->set_prop_data(propNames.first(), propValues.first());
			propNames.pop_front();
			propValues.pop_front();
		}
	}

}
void qtvplugin_geomarker::update_region		(const QString & name,const QPolygonF latlons, QPen pen, QBrush brush)
{
	//Get raw Item by name
	QTVP_GEOMARKER::geoItemBase * base = m_pScene->geoitem_by_name(name);
	//Get Props
	QStringList propNames;
	QVariantList propValues;
	if (base)
	{
		propNames = base->prop_names();
		propValues = base->prop_values();
	}
	//type convertion to T
	QTVP_GEOMARKER::geoGraphicsPolygonItem * pitem = base?dynamic_cast<QTVP_GEOMARKER::geoGraphicsPolygonItem  *>(base):0;
	if (!pitem)
		pitem	= new QTVP_GEOMARKER::geoGraphicsPolygonItem(name,
						this->m_pVi,
						latlons);

	pitem->setPen(pen);
	pitem->setBrush(brush);
	if (false==this->m_pScene->addItem(pitem,0))
	{
		if (base != pitem)
			delete pitem;
	}
	else
	{
		int cs = propNames.size();
		for (int i=0;i<cs && base != pitem;++i)
		{
			pitem->set_prop_data(propNames.first(), propValues.first());
			propNames.pop_front();
			propValues.pop_front();
		}
	}
}
