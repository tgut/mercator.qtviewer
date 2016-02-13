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
#include "geographicspixmapitem.h"
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

	//insert 1 icons
	QTVP_GEOMARKER::tag_icon icon;
	icon.name = "default";
	icon.filename = "://icons/default.png";
	icon.centerx = 8;
	icon.centery = 8;
	if (icon.icon.load(icon.filename))
		m_map_icons[icon.name] = icon;


	m_pIconsModel = new QStandardItemModel(this);
	refreshIconModel();
	ui->comboBox_icons->setModel(m_pIconsModel);

	m_bNeedRefresh = false;
	m_bNeedUpdateView = false;
	m_nTimerID_refreshUI = startTimer(2000);
	m_nTimerID_refreshMap = startTimer(100);
	m_nTimerID_levelQueue = startTimer(100);
}

qtvplugin_geomarker::~qtvplugin_geomarker()
{
	delete ui;
}
void qtvplugin_geomarker::load_retranslate_UI()
{
	ui->retranslateUi(this);
}
/*! load_initial_plugin will be called when this plug in being loaded.
 * Please notice that a global parament "map_instances" is introduced, for multi-instance situation
 * in windows OCX usage. A Brave new instance of "qtvplugin_geomarker" will be created for each different viewer_interface.
 *
 * @param strSLibPath the absolute path of this dll.
 * @param ptrviewer the pointer to main view.
 * @return	return the instance pointer to the instance belong to ptrviewer
*/
layer_interface * qtvplugin_geomarker::load_initial_plugin(QString strSLibPath,viewer_interface  * ptrviewer)
{
	//!In this instance, we will see how to create a new instance for each ptrviewer
	qtvplugin_geomarker * instance = 0;
	//!1.Check whether there is already a instance for ptrviewer( viewer_interface)
	mutex_instances.lock();
	//!1.1 situation 1: map_instances is empty, which means no instance exists. We just save this pointer to map_instances
	if (map_instances.empty()==true)
	{
		map_instances[ptrviewer] = this;
		instance = this;
	}
	/*! 1.2 situation 2: map_instances dose not contain ptrviewer, which is the normal situation when a second ocx ctrl is initializing.
	 * we just allocate a new  qtvplugin_geomarker, and save key-value in map_instances.
	*/
	else if (map_instances.contains(ptrviewer)==false)
	{
		instance = new qtvplugin_geomarker;
		map_instances[ptrviewer] = instance;
	}
	//! 1.3 situation 3: a ABNORMAL situation. load_initial_plugin is called again.
	else
		instance = map_instances[ptrviewer];
	mutex_instances.unlock();
	//2. if the instance is just this object, we do real init code.
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
		ini_load();
		initialBindPluginFuntions();
	}
	//3. elseif, we call the instance's load_initial_plugin method instead
	else
	{
		 layer_interface * ret = instance->load_initial_plugin(strSLibPath,ptrviewer);
		 assert(ret==instance);
		 return ret;
	}
	qDebug()<<QFont::substitutions();
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
bool qtvplugin_geomarker::too_many_items()
{
	bool res = false;
	if (!m_pVi || m_bVisible==false)
		return res;
	int currentLevel = m_pVi->level();
	if (currentLevel <=7)
	{
		//skip painting when there are too many marks on map
		int reduce_limit = (1<<currentLevel) * 2048;
		if (this->m_pScene->total_items()>=reduce_limit)
			res = true;
	}
	return res;
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

	int winsz = 256 * (1<<m_pVi->level());

	QRectF destin(
				0,
				0,
				rect.width(),
				rect.height()
				);
	if (too_many_items()==false)
	{
		//Warpping 180, -180. because longitude +180 and -180 is the same point,
		// but the map is plat, -180 and + 180 is quite different positions, we
		// should draw 3 times, to slove cross 180 drawing problems.
		for (int p = -1; p<=1 ;++p)
		{
			QRectF source(
						leftcenx + p * winsz,
						topceny,
						(rightcenx - leftcenx),
						(bottomceny - topceny)
						);

			m_pScene->render(pImage,destin,source);

		}
	}
}

void qtvplugin_geomarker::cb_levelChanged(int level)
{
	if (!m_pVi || m_bVisible==false)
		return ;
	//Adjust new Scene rect
	QRectF rect(0,0,256*(1<<level),256*(1<<level));
	this->set_visible(false);
	m_pScene->adjust_item_coords(level);
	m_pScene->setSceneRect(rect);
	this->set_visible(true);
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

/*! qtvplugin_geomarker::cb_mouseXXXEvent tranfer mouse events from main view to
 * QGraphicsItem based classes, so that these items will recieve mouse events.
 * For qt's graphics-view framework, this approach is done inside QGraphicsView class.
 * however, our main view is a simple widget, which means mouse events should be dealed manually.
 *
 * @fn qtvplugin_geomarker::cb_mouseDoubleClickEvent(QMouseEvent * e)
 * @param e the mouse event.
 * @return bool event acception.
*/
bool		qtvplugin_geomarker::cb_mouseDoubleClickEvent(QMouseEvent * e)
{
	if (!m_pVi)
		return false;
	if (m_bVisible==false)
		return false;
	QPoint mouse_view_pt = e->pos();
	int winsz = 256 * (1<<m_pVi->level());
	double wx,wy;
	double mlat, mlon;
	m_pVi->CV_DP2World(mouse_view_pt.x(),mouse_view_pt.y(),&wx,&wy);
	m_pVi->CV_DP2LLA(mouse_view_pt.x(),mouse_view_pt.y(),&mlat,&mlon);
	if (e->button()==Qt::RightButton)
	{
		ui->lineEdit_point_lat->setText(QString("%1").arg(mlat,0,'f',7));
		ui->lineEdit_point_lon->setText(QString("%1").arg(mlon,0,'f',7));
		ui->lineEdit_icon_lat->setText(QString("%1").arg(mlat,0,'f',7));
		ui->lineEdit_icon_lon->setText(QString("%1").arg(mlon,0,'f',7));
	}
	//Warp
	while (wx < 0) wx += winsz;
	while (wx > winsz-1) wx -= winsz;

	QPointF mouse_scene_pt(wx,wy);
	QPoint mouse_screen_pt = e->globalPos();
	Qt::MouseButton mouse_button = e->button();
	QWidget * pwig = dynamic_cast<QWidget *> (m_pVi);
	if (m_bVisible && pwig && too_many_items()==false)
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
/*! qtvplugin_geomarker::cb_mouseXXXEvent tranfer mouse events from main view to
 * QGraphicsItem based classes, so that these items will recieve mouse events.
 * For qt's graphics-view framework, this approach is done inside QGraphicsView class.
 * however, our main view is a simple widget, which means mouse events should be dealed manually.
 *
 * @fn qtvplugin_geomarker::cb_mousePressEvent(QMouseEvent * e)
 * @param e the mouse event.
 * @return bool event acception.
*/
bool qtvplugin_geomarker::cb_mousePressEvent(QMouseEvent * e)
{
	if (!m_pVi)
		return false;
	if (m_bVisible==false)
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
	if (m_bVisible && pwig  && too_many_items()==false)
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
/*! for convenience, color is stored in plain text in XML and UI.
 * the plain text color is 4 sub value , which stands for r,g,b,alpha.
 *
 * @fn qtvplugin_geomarker::string2color(const QString & s)
 * @param s the string color.
 * @return QColor is the color object.
*/
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
/*! for convenience, color is stored in plain text in XML and UI.
 * the plain text color is 4 sub value , which stands for r,g,b,alpha.
 *
 * @fn qtvplugin_geomarker::color2string(const QColor & col)
 * @param col the  color object.
 * @return QString is the color string.
*/
QString qtvplugin_geomarker::color2string(const QColor & col)
{
	QString str = QString("%1,%2,%3,%4").arg(col.red()).arg(col.green()).arg(col.blue()).arg(col.alpha());
	return str;
}

QString qtvplugin_geomarker::ini_file()
{
	if (m_SLLibPath.size())
		return m_SLLibPath + QString("%1").arg(m_nInstance) + ".ini";
	else
		return QCoreApplication::applicationFilePath() + QString("/geomarker%1.ini").arg(m_nInstance);
}

void qtvplugin_geomarker::scheduleRefreshMarks()
{
	if (!m_pVi || !m_pScene)
		return;
	//We do not refresh UI immediately after each mark-insert, for these inserts is very dense ,
	//BAD performence will arise if so.
	//We will set a flag and refresh the ui in timerEvent Instead.
	m_bNeedRefresh = true;
}
void qtvplugin_geomarker::scheduleUpdateMap()
{
	if (!m_pVi || !m_pScene)
		return;
	//We do not refresh MAP immediately after each mark-insert, for these inserts is very dense ,
	//BAD performence will arise if so.
	//We will set a flag and refresh the ui in timerEvent Instead.
	m_bNeedUpdateView = true;
}

QTVP_GEOMARKER::geoItemBase *  qtvplugin_geomarker::update_line(const QString & name,double lat1, double lon1,double lat2, double lon2, QPen pen)
{
	QTVP_GEOMARKER::geoItemBase *  res = 0;
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

	if (base == pitem)
	{
		pitem->setGeo(lat1,lon1,lat2,lon2);
		res = pitem;
	}
	else if (false==this->m_pScene->addItem(pitem,0))
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
		res = pitem;
	}
	return res;
}


QTVP_GEOMARKER::geoItemBase *   qtvplugin_geomarker::update_polygon		(const QString & name,const QPolygonF latlons, QPen pen, QBrush brush)
{
	QTVP_GEOMARKER::geoItemBase *  res = 0;
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
	if (base == pitem)
	{
		pitem->setGeo(latlons);
		res = pitem;
	}
	else if (false==this->m_pScene->addItem(pitem,0))
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
		res = pitem;
	}
	return res;
}
QTVP_GEOMARKER::geoItemBase *	qtvplugin_geomarker::update_icon(const QString & name,double lat, double lon,qreal scale, qreal rotate,int smooth, QString id)
{
	QTVP_GEOMARKER::geoItemBase *  res = 0;
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
	QTVP_GEOMARKER::geoGraphicsPixmapItem * pitem = base?dynamic_cast<QTVP_GEOMARKER::geoGraphicsPixmapItem  *>(base):0;
	if (!pitem)
	{
		const QTVP_GEOMARKER::tag_icon * iconp = 0;
		if (m_map_icons.contains(id))
			iconp = &m_map_icons[id];
		else
			iconp = &m_map_icons["default"];

		pitem	= new QTVP_GEOMARKER::geoGraphicsPixmapItem(name,this->m_pVi,
															iconp,
															lat,lon
															);
	}
	if (base == pitem)
	{
		pitem->setGeo(lat,lon);
		if (m_map_icons.contains(id))
			pitem->setPixmap(m_map_icons[id]);
		else
			pitem->setPixmap(m_map_icons["default"]);
		res = pitem;
	}
	else if (false==this->m_pScene->addItem(pitem,0))
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
		res = pitem;
	}
	if (pitem)
	{
		pitem->setScale(scale);
		pitem->setRotation(rotate);
		if (smooth)
			pitem->setTransformationMode(Qt::SmoothTransformation);
		else
			pitem->setTransformationMode(Qt::FastTransformation);
		pitem->adjustLabelPos();
	}
	return res;
}
