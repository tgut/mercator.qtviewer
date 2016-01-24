#include "osm_frame_widget.h"
#include "ui_osm_frame_widget.h"
#include <QDir>
#include <QtPlugin>
#include "osmtiles/layer_tiles.h"
#include "osmtiles/layer_browser.h"
#include <QModelIndexList>
#include <QModelIndex>
#include <QPluginLoader>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include "osmtiles/viewer_interface.h"

 QMutex osm_frame_widget::m_mutex_proteced;

/*!
 \brief osm_frame_widget is the main widget of this control.
 in this constructor, 2 OUTER message will be fired.

 \fn osm_frame_widget::osm_frame_widget
 \param parent
*/
osm_frame_widget::osm_frame_widget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::osm_frame_widget)
{
	m_mutex_proteced.lock();
	QTVOSM_DEBUG("The osm_frame_widget class constructing...");
	ui->setupUi(this);
	m_pLayerDispMod = new QStandardItemModel(this);
	m_pLayerDispMod->setColumnCount(3);
	m_pLayerDispMod->setHeaderData(0,Qt::Horizontal,QString(tr("name")));
	m_pLayerDispMod->setHeaderData(1,Qt::Horizontal,QString(tr("active")));
	m_pLayerDispMod->setHeaderData(2,Qt::Horizontal,QString(tr("visible")));
	ui->tableView_layers->setModel(m_pLayerDispMod);
	connect(ui->widget_mainMap,&tilesviewer::evt_level_changed,ui->dial_zoom,&QDial::setValue);
	connect(ui->dial_zoom,&QDial::valueChanged,ui->widget_mainMap,&tilesviewer::setLevel);


	//add an osm layer
	layer_tiles * pOSMTile =  new layer_tiles(ui->widget_mainMap);
	pOSMTile->set_name("OSM");
	pOSMTile->set_active(true);
	pOSMTile->set_visible(true);
	//pOSMTile->connectToTilesServer(true);
	AppendLayer(QCoreApplication::applicationFilePath(),pOSMTile);

/*
	//add an sat layer
	layer_tiles * pSatTile = new layer_tiles(ui->widget_mainMap);
	pSatTile->set_name("SAT");
	//pSatTile->connectToTilesServer(true);
	AppendLayer(QCoreApplication::applicationFilePath(),pSatTile);
*/
	//add single layer to browser
	layer_browser * pOSMTileBr = new layer_browser(ui->browserView);
	pOSMTileBr->set_name("OSM");
	pOSMTileBr->load_initial_plugin(QCoreApplication::applicationFilePath(),ui->browserView);
	ui->browserView->addLayer(pOSMTileBr);

	//connect center change event
	connect (ui->widget_mainMap,&tilesviewer::evt_center_changed,ui->browserView,&tilesviewer::setBrCenterLLA);
	connect (ui->browserView,&tilesviewer::evt_center_changed,ui->widget_mainMap,&tilesviewer::setCenterLLA);
	connect (ui->widget_mainMap,&tilesviewer::evt_level_changed,ui->browserView,&tilesviewer::setBrLevel);
	connect (ui->widget_mainMap,&tilesviewer::cmd_update_layer_box,this,&osm_frame_widget::delacmd_refresh_layer_view,Qt::QueuedConnection);
	//send messages
	//! 1. source=MAIN_MAP,  destin = ALL, msg = WINDOW_CREATE
	if (this->isEnabled())
	{
		QMap<QString, QVariant> map_evt;
		map_evt["source"] = "MAIN_MAP";
		map_evt["destin"] = "ALL";
		map_evt["name"] = "WINDOW_CREATE";
		ui->widget_mainMap->post_event(map_evt);
	}

	ui->tab_map->installEventFilter(this);
	//adjust layers, make exclusive layrs being de-activated.
	ui->widget_mainMap->adjust_layers(pOSMTile);


	//! 2. source=MAIN_MAP,  destin = ALL, msg = MAP_INITED
	if ( this->isEnabled())
	{
		QMap<QString, QVariant> map_evt;
		map_evt["source"] = "MAIN_MAP";
		map_evt["destin"] = "ALL";
		map_evt["name"] = "MAP_INITED";
		map_evt["nLevel"] = ui->widget_mainMap->level();
		ui->widget_mainMap->post_event(map_evt);
	}
	QTVOSM_DEBUG("The osm_frame_widget class constructed.");
	EnumPlugins();
	UpdateLayerTable();
	//Dock is closable
	ui->dockWidget_side->installEventFilter(this);
	m_mutex_proteced.unlock();
}
void osm_frame_widget::UpdateLayerTable()
{
	QVector<QString> names = ui->widget_mainMap->layerNames();
	QVector<bool> activities = ui->widget_mainMap->layerActivities();
	QVector<bool> visibles = ui->widget_mainMap->layerVisibilities();
	int nItems = names.size();
	if (m_pLayerDispMod->rowCount()>0)
		m_pLayerDispMod->removeRows(0,m_pLayerDispMod->rowCount());
	for (int i=0;i<nItems;++i)
	{
		m_pLayerDispMod->appendRow(new QStandardItem(names[nItems-1-i]));
		m_pLayerDispMod->setData(m_pLayerDispMod->index(i,1),activities[nItems-1-i]);
		m_pLayerDispMod->setData(m_pLayerDispMod->index(i,2),visibles[nItems-1-i]);
	}
}
tilesviewer * osm_frame_widget::viewer()
{
	return ui->widget_mainMap;
}

bool osm_frame_widget::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::Close)
	{
		if (obj == ui->tab_map)
		{
			QWidget * wig = qobject_cast<QWidget *>(obj);
			if (wig)
			{
				Qt::WindowFlags flg = wig->windowFlags();
				flg &= ~(Qt::WindowMinMaxButtonsHint|Qt::WindowStaysOnTopHint|Qt::Window );
				wig->setWindowFlags(flg);
				ui->tabWidget_main->addTab(
							wig,
							"Map"
							);
				return true;
			}
		}
		else if (obj == ui->dockWidget_side)
		{
			ui->dockWidget_side->hide();
			QMargins m = this->layout()->contentsMargins();
			this->layout()->setContentsMargins(m.left(),m.top(),12,m.bottom());
			return true;
		}
		else if (m_PropPageslayer.contains(obj))
		{
			QWidget * wig = qobject_cast<QWidget *>(obj);
			if (wig)
			{
				Qt::WindowFlags flg = wig->windowFlags();
				flg &= ~(Qt::WindowMinMaxButtonsHint|Qt::WindowStaysOnTopHint|Qt::Window );
				wig->setWindowFlags(flg);
				ui->tabWidget_main->addTab(
							wig,
							m_PropPageslayer[obj]->get_name()
							);
			}
			return true;
		}
		else
		{

		}

	}
	// standard event processing
	return QObject::eventFilter(obj, event);
}

bool osm_frame_widget::AppendLayer(QString SLName,layer_interface * interface)
{
	layer_interface * ci = interface->load_initial_plugin(SLName,ui->widget_mainMap);
	if (0==ci)
		return false;
	if (false==ui->widget_mainMap->addLayer(ci))
		return false;
	QWidget * wig = ci->load_prop_window();
	if (wig)
	{
		m_layerPropPages[ci] = wig;
		m_PropPageslayer[wig] = ci;
		ui->tabWidget_main->addTab(wig,ci->get_name());
		wig->installEventFilter(this);
	}
	return true;
}

osm_frame_widget::~osm_frame_widget()
{
	delete ui;
}
void osm_frame_widget::mousePressEvent(QMouseEvent * e)
{
	if (e->pos().x() >= this->rect().right()-12)
	{
		if (ui->dockWidget_side->isVisible()==false)
		{
			ui->dockWidget_side->show();

			QMargins m = this->layout()->contentsMargins();
			this->layout()->setContentsMargins(m.left(),m.top(),m.left(),m.bottom());
		}
	}

	QWidget::mousePressEvent(e);
}
void osm_frame_widget::EnumPlugins()
{
	QTVOSM_DEBUG("The osm_frame_widget is enuming plugins.");
	QString strAppDir = QCoreApplication::applicationDirPath();
	QDir pluginsDir(strAppDir);
	QStringList filters;
	filters << "qtvplugin_*.dll" << "libqtvplugin_*.so";
	pluginsDir.setNameFilters(filters);
	//Enum files
	foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
		QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
		QObject *plugin = pluginLoader.instance();//try to load Plugins
		if (plugin) {
			layer_interface * pPlugin= qobject_cast<layer_interface *>(plugin);
			if (pPlugin)
			{
				if (false==AppendLayer(fileName,pPlugin))
				{

				}
			}
		}
	}
	QTVOSM_DEBUG("The osm_frame_widget loaded all plugins.");
	return ;
}

void osm_frame_widget::on_pushButton_visible_clicked()
{
	QVector <layer_interface *> layers = ui->widget_mainMap->layers();
	int nItems = layers.size();
	QModelIndexList lstSel = ui->tableView_layers->selectionModel()->selectedIndexes();
	if (lstSel.size())
	{
		int row = lstSel.first().row();
		if (row >=0 && row < layers.size())
		{
			layers[nItems - 1 -row]->set_visible(true);
			UpdateLayerTable();
			ui->widget_mainMap->UpdateWindow();

		}
	}
}

void osm_frame_widget::on_pushButton_hide_clicked()
{
	QVector <layer_interface *> layers = ui->widget_mainMap->layers();
	QModelIndexList lstSel = ui->tableView_layers->selectionModel()->selectedIndexes();
	int nItems = layers.size();
	if (lstSel.size())
	{
		int row = lstSel.first().row();
		if (row >=0 && row < layers.size())
		{
			layers[nItems - 1 -row]->set_visible(false);
			UpdateLayerTable();
			ui->widget_mainMap->UpdateWindow();
		}
	}
}

void osm_frame_widget::on_pushButton_moveDown_clicked()
{
	QVector <layer_interface *> layers = ui->widget_mainMap->layers();
	QModelIndexList lstSel = ui->tableView_layers->selectionModel()->selectedIndexes();
	int nItems = layers.size();
	if (lstSel.size())
	{
		int row = lstSel.first().row();
		if (row >=0 && row < layers.size())
		{
			ui->widget_mainMap->moveLayerUp(layers[nItems - 1 -row]);
			UpdateLayerTable();
			ui->widget_mainMap->UpdateWindow();
		}
	}
}

void osm_frame_widget::on_pushButton_moveBtm_clicked()
{
	QVector <layer_interface *> layers = ui->widget_mainMap->layers();
	QModelIndexList lstSel = ui->tableView_layers->selectionModel()->selectedIndexes();
	int nItems = layers.size();
	if (lstSel.size())
	{
		int row = lstSel.first().row();
		if (row >=0 && row < layers.size())
		{
			ui->widget_mainMap->moveLayerTop(layers[nItems - 1 -row]);
			UpdateLayerTable();
			ui->widget_mainMap->UpdateWindow();
		}
	}
}

void osm_frame_widget::on_pushButton_moveUp_clicked()
{
	QVector <layer_interface *> layers = ui->widget_mainMap->layers();
	QModelIndexList lstSel = ui->tableView_layers->selectionModel()->selectedIndexes();
	int nItems = layers.size();
	if (lstSel.size())
	{
		int row = lstSel.first().row();
		if (row >=0 && row < layers.size())
		{
			ui->widget_mainMap->moveLayerDown(layers[nItems - 1 -row]);
			UpdateLayerTable();
			ui->widget_mainMap->UpdateWindow();
		}
	}
}

void osm_frame_widget::on_pushButton_moveTop_clicked()
{
	QVector <layer_interface *> layers = ui->widget_mainMap->layers();
	QModelIndexList lstSel = ui->tableView_layers->selectionModel()->selectedIndexes();
	int nItems = layers.size();
	if (lstSel.size())
	{
		int row = lstSel.first().row();
		if (row >=0 && row < layers.size())
		{
			ui->widget_mainMap->moveLayerBottom(layers[nItems - 1 -row]);
			UpdateLayerTable();
			ui->widget_mainMap->UpdateWindow();
		}
	}
}
void osm_frame_widget::delacmd_refresh_layer_view()
{
	UpdateLayerTable();
	ui->widget_mainMap->UpdateWindow();
}

void osm_frame_widget::on_pushButton_active_clicked()
{
	QVector <layer_interface *> layers = ui->widget_mainMap->layers();
	QModelIndexList lstSel = ui->tableView_layers->selectionModel()->selectedIndexes();
	int nItems = layers.size();
	if (lstSel.size())
	{
		int row = lstSel.first().row();
		if (row >=0 && row < layers.size())
		{
			for (int i=0;i<layers.size();++i)
			{
				//It's exclusive, there should be at most only one layer_tiles active
				if (i==(nItems - 1 -row))
				{
					layers[i]->set_active(true);
					ui->widget_mainMap->adjust_layers(layers[i]);
				}
			}
			UpdateLayerTable();

		}
	}
}

void osm_frame_widget::on_pushButton_deactive_clicked()
{
	QVector <layer_interface *> layers = ui->widget_mainMap->layers();
	QModelIndexList lstSel = ui->tableView_layers->selectionModel()->selectedIndexes();
	int nItems = layers.size();
	if (lstSel.size())
	{
		int row = lstSel.first().row();
		if (row >=0 && row < layers.size())
		{
			for (int i=0;i<layers.size();++i)
			{
				if (i==(nItems - 1 -row))
					layers[i]->set_active(false);
			}
			UpdateLayerTable();

		}
	}
}

void osm_frame_widget::on_tabWidget_main_tabCloseRequested(int index)
{
	QWidget * wig = ui->tabWidget_main->widget(index);
	this->ui->tabWidget_main->removeTab(index);

	Qt::WindowFlags flg = wig->windowFlags();
	flg |= (Qt::WindowMinMaxButtonsHint|Qt::WindowStaysOnTopHint|Qt::Window );
	wig->setWindowFlags(flg);
	wig->show();
	wig->move(100,100);

}

void osm_frame_widget::on_pushButton_saveToFile_clicked()
{
	QSettings settings(QCoreApplication::applicationFilePath()+".ini",QSettings::IniFormat);
	QString strLastSaveImgDir = settings.value("history/last_save_img_dir","./").toString();
	QString newfm = QFileDialog::getSaveFileName(this,tr("save to image"),strLastSaveImgDir,
								 "Images (*.png *.bmp *.jpg);;All files(*.*)"
								 );
	if (newfm.size()>2)
	{
		if (true == ui->widget_mainMap->saveToImage(newfm))
			 settings.setValue("history/last_save_img_dir",newfm);
	}
}
