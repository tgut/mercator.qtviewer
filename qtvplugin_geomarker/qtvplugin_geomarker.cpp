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
#include "geographicsmultilineitem.h"
QMutex mutex_instances;
QMap<viewer_interface *,  qtvplugin_geomarker * > map_instances;
QMap<QString,  int > count_instances;
qtvplugin_geomarker::qtvplugin_geomarker(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::qtvplugin_geomarker),
	m_currentTools(qtvplugin_geomarker::TOOLS_DISPLAY_ONLY),
	m_sel_ptStart_World(0,0),
	m_sel_ptEnd_World(0,0)
{
	m_nInstance = 0;
	ui->setupUi(this);
	m_pVi = 0;
	m_nDivideTimer = 0;
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

	m_pSelItemNameModel = new QStandardItemModel(this);
	m_pSelItemNameModel->setColumnCount(1);
	m_pSelItemNameModel->setHeaderData(0,Qt::Horizontal,tr("Name"));
	ui->tableView_marks_sel->setModel(m_pSelItemNameModel);


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

	ui->radioButton_display->setChecked(true);
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
		int reduce_limit = (1<<currentLevel) * 4096;
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
	else
	{
		QPen pen(QColor(255,0,0,128));
		pen.setWidth(m_pVi->level()/2+1);
		pImage->setPen(pen);
		QList<QTVP_GEOMARKER::geoItemBase *> items
				= m_pScene->geo_items();
		foreach (QTVP_GEOMARKER::geoItemBase * pitem, items)
		{
			QPointF pt = pitem->center_pos();
			qint32 x,y;
			m_pVi->CV_World2DP(pt.x(),pt.y(),&x,&y);
			pImage->drawPoint(x,y);
		}
	}
	//draw progres
	double pc = m_pScene->progress_queue();
	if (pc<0.999)
	{
		QBrush br1(QColor(255,0,0,128));
		QBrush br2(QColor(0,255,0,128));
		pImage->setBrush(br1);
		int left = rect.center().x()-128;
		int top = rect.center().y()-16;
		pImage->drawRect(left,top,256,32);
		pImage->setBrush(br2);
		pImage->drawRect(left,top,256*pc,32);
		QString strmsg= tr("Level Re-Coord %1 %%").arg(pc*100);
		pImage->drawText(left+32,top+16,strmsg);
	}
	//draw current tools
	switch (m_currentTools)
	{
	case qtvplugin_geomarker::TOOLS_RECT_SELECTION:
	{
		QPen pen_sel(QColor(128,64,0,128));
		pen_sel.setWidth(3);
		pen_sel.setStyle(Qt::DashLine);
		pImage->setPen(pen_sel);
		pImage->drawText(32,32,"GEO MARKER Rect-Selection Tools Actived.");
		QRectF wrct = current_sel_RectWorld();
		if (wrct.isValid())
		{
			double x1 = wrct.left();
			double y1 = wrct.top();
			double x2 = wrct.right();
			double y2 = wrct.bottom();
			qint32 nx1,ny1,nx2,ny2;
			m_pVi->CV_World2DP(x1,y1,&nx1,&ny1);
			m_pVi->CV_World2DP(x2,y2,&nx2,&ny2);
			for (int i = -1;i<=1;++i)
			{
				pImage->drawLine(nx1 + i * winsz,ny1,nx1 + i * winsz,ny2);
				pImage->drawLine(nx1 + i * winsz,ny2,nx2 + i * winsz,ny2);
				pImage->drawLine(nx2 + i * winsz,ny2,nx2 + i * winsz,ny1);
				pImage->drawLine(nx2 + i * winsz,ny1,nx1 + i * winsz,ny1);
			}
		}
	}
		break;
	default:
		break;
	}
	if (m_sel_rects_merkator.empty()==false)
	{
		QPen pen_sel(QColor(0,64,255,128));
		pen_sel.setWidth(2);
		pen_sel.setStyle(Qt::DotLine);
		pImage->setPen(pen_sel);
		//draw select regions
		foreach (QRectF wr, m_sel_rects_merkator)
		{
			double wx1,wy1,wx2,wy2;
			qint32 nx1,ny1,nx2,ny2;
			m_pVi->CV_MK2World(wr.left(),wr.top(),&wx1,&wy1);
			m_pVi->CV_MK2World(wr.right(),wr.bottom(),&wx2,&wy2);
			m_pVi->CV_World2DP(wx1,wy1,&nx1,&ny1);
			m_pVi->CV_World2DP(wx2,wy2,&nx2,&ny2);
			for (int i = -1;i<=1;++i)
				pImage->drawRect(QRectF(QPointF(nx1 + i * winsz,ny1),QPointF(nx2 + i * winsz,ny2)));
		}
	}
}

void qtvplugin_geomarker::cb_levelChanged(int level)
{
	if (!m_pVi)
		return ;
	m_sel_ptStart_World = m_sel_ptEnd_World = QPointF();
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
void qtvplugin_geomarker::set_active(bool ab)
{
	if (ab==true)
	{
		if (m_currentTools==qtvplugin_geomarker::TOOLS_DISPLAY_ONLY)
		{
			ui->radioButton_rect_selection->setChecked(true);
			m_currentTools = qtvplugin_geomarker::TOOLS_RECT_SELECTION;
		}
	}
	else
	{
		m_currentTools = qtvplugin_geomarker::TOOLS_DISPLAY_ONLY;
		ui->radioButton_display->setChecked(true);
	}

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
QRectF qtvplugin_geomarker::CV_RectWrold2Mkt(QRectF world)
{
	if (!m_pVi)	return QRectF();
	double mx1,my1,mx2,my2;
	m_pVi->CV_World2MK(world.left(),world.top(),&mx1,&my1);
	m_pVi->CV_World2MK(world.right(),world.bottom(),&mx2,&my2);
	return QRectF(QPointF(mx1,my1),QPointF(mx2,my2));
}
QRectF qtvplugin_geomarker::current_sel_RectWorld()
{
	if (!m_pVi)	return QRectF();
	if (m_currentTools!=qtvplugin_geomarker::TOOLS_RECT_SELECTION)
		return QRectF();
	if (m_sel_ptEnd_World.isNull() || m_sel_ptStart_World.isNull())
		return QRectF();
	qint32 wsz = 256*(1<<m_pVi->level());
	int wx1 = m_sel_ptStart_World.x(),wx2 = m_sel_ptEnd_World.x(),
			wy1 = m_sel_ptStart_World.y(), wy2 = m_sel_ptEnd_World.y();
	while (wx2 - wx1 > wsz/2)
		wx1+=wsz;
	while (wx2 - wx1 < -wsz/2)
		wx2+=wsz;
	if (wx1 > wx2)
	{
		float tp = wx1;
		wx1 = wx2;
		wx2 = tp;
	}
	if (wy1 > wy2)
	{
		float tp = wy1;
		wy1 = wy2;
		wy2 = tp;
	}

	return QRectF(QPointF(wx1,wy1),QPointF(wx2,wy2));
}

void qtvplugin_geomarker::clearSelection()
{
	m_set_itemNameSelected.clear();
	m_sel_rects_merkator.clear();
	refresh_selection_listview();
	m_pVi->UpdateWindow();
}
void qtvplugin_geomarker::addSelection(QRectF rectWorld)
{
	qint32 wsz = 256*(1<<m_pVi->level());
	int oldsz = m_set_itemNameSelected.size();
	for (int i=-1;i<=1;++i)
	{
		double	x1 = rectWorld.left()+i * wsz,
				y1 = rectWorld.top(),
				x2 = rectWorld.right()+i * wsz,
				y2 = rectWorld.bottom();

		QList<QGraphicsItem *> itmesel = m_pScene->items(QRectF(QPointF(x1,y1),QPointF(x2,y2)));
		foreach (QGraphicsItem * it, itmesel)
		{
			QTVP_GEOMARKER::geoItemBase *
					gi = dynamic_cast<QTVP_GEOMARKER::geoItemBase *>(it);
			if (gi)
			{
				m_set_itemNameSelected.insert(gi->item_name());
			}
		}
	}
	int newsz = m_set_itemNameSelected.size();
	if (newsz>oldsz)
	{
		m_sel_rects_merkator.push_back(CV_RectWrold2Mkt(rectWorld));
		refresh_selection_listview();
		scheduleUpdateMap();
	}

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
		QGraphicsSceneMouseEvent * pmouseEvent = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMouseDoubleClick);
		QGraphicsSceneMouseEvent & mouseEvent = * pmouseEvent;
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
		QApplication::postEvent(m_pScene, &mouseEvent);
		scheduleUpdateMap();
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
		QGraphicsSceneMouseEvent * pmouseEvent = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMousePress);
		QGraphicsSceneMouseEvent & mouseEvent = * pmouseEvent;
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
		QApplication::postEvent(m_pScene, &mouseEvent);
		scheduleUpdateMap();
	}
	//tools
	switch (m_currentTools)
	{
	case qtvplugin_geomarker::TOOLS_RECT_SELECTION:
	{
		if (e->button()==Qt::LeftButton)
		{
			m_sel_ptStart_World = m_sel_ptEnd_World = QPointF(wx,wy);
		}
	}
		break;
	default:
		break;
	}


	return false;

}
bool qtvplugin_geomarker::cb_mouseMoveEvent ( QMouseEvent * e )
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

	//tools
	switch (m_currentTools)
	{
	case qtvplugin_geomarker::TOOLS_RECT_SELECTION:
	{
		if (e->buttons()==Qt::LeftButton)
		{
			m_sel_ptEnd_World = QPointF(wx,wy);
			scheduleUpdateMap();
		}
	}
		break;
	default:
		break;
	}


	return false;

}

bool qtvplugin_geomarker::cb_mouseReleaseEvent ( QMouseEvent * e )
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

	//tools
	switch (m_currentTools)
	{
	case qtvplugin_geomarker::TOOLS_RECT_SELECTION:
	{
		if (e->button()==Qt::LeftButton)
		{
			m_sel_ptEnd_World = QPointF(wx,wy);
			QRectF rectSel = current_sel_RectWorld();
			m_sel_ptStart_World = m_sel_ptEnd_World = QPointF();
			addSelection(rectSel);
			scheduleUpdateMap();
		}
	}
		break;
	default:
		break;
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
	m_items_to_insert.clear();
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
void qtvplugin_geomarker::refresh_selection_listview()
{
	if (!m_pVi || !m_pScene)
		return;
	//refersh
	this->m_pSelItemNameModel->removeRows(0,this->m_pSelItemNameModel->rowCount());
	foreach (QString name, m_set_itemNameSelected)
	{
		m_pSelItemNameModel->appendRow(new QStandardItem(name));
	}
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


QTVP_GEOMARKER::geoItemBase *   qtvplugin_geomarker::update_polygon		(const QString & name,const QPolygonF latlons, QPen pen, QBrush brush, bool tp)
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
	if (tp==false)
	{
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
	}
	else
	{
		//type convertion to T
		QTVP_GEOMARKER::geoGraphicsMultilineItem * pitem = base?dynamic_cast<QTVP_GEOMARKER::geoGraphicsMultilineItem  *>(base):0;
		if (!pitem)
			pitem	= new QTVP_GEOMARKER::geoGraphicsMultilineItem(name,
																 this->m_pVi,
																 latlons);
		pitem->setPen(pen);
		//pitem->setBrush(QBrush(Qt::NoBrush));
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
