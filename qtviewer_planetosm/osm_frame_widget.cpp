#include "osm_frame_widget.h"
#include "ui_osm_frame_widget.h"
#include <QDir>
#include <QtPlugin>
#include "osmtiles/layer_tiles.h"
#include "osmtiles/layer_browser.h"
#include <QModelIndexList>
#include <QModelIndex>
#include <QPluginLoader>
osm_frame_widget::osm_frame_widget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::osm_frame_widget)
{
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
	layer_tiles * pOSMTile = new layer_tiles(ui->widget_mainMap);
	pOSMTile->cb_setName("BackgroundOSM");
	pOSMTile->cb_setActive(true);
	pOSMTile->cb_setVisible(true);
	pOSMTile->connectToTilesServer(true);
	AppendLayer(pOSMTile);


	//add an sat layer
	layer_tiles * pSatTile = new layer_tiles(ui->widget_mainMap);
	pSatTile->cb_setName("BackgroundSAT");
	pSatTile->connectToTilesServer(true);
	AppendLayer(pSatTile);

	//add single layer to browser
	layer_browser * pOSMTileBr = new layer_browser(ui->browserView);
	pOSMTileBr->cb_setName("BackgroundOSM");
	ui->browserView->addLayer(pOSMTileBr);

	//connect center change event
	connect (ui->widget_mainMap,&tilesviewer::evt_center_changed,ui->browserView,&tilesviewer::setBrCenterLLA);
	connect (ui->browserView,&tilesviewer::evt_center_changed,ui->widget_mainMap,&tilesviewer::setCenterLLA);
	connect (ui->widget_mainMap,&tilesviewer::evt_level_changed,ui->browserView,&tilesviewer::setBrLevel);

	EnumPlugins();
	UpdateLayerTable();
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
void osm_frame_widget::AppendLayer(layer_interface * interface)
{
	if (false==ui->widget_mainMap->addLayer(interface))
		return;
	QWidget * wig = interface->cb_create_propWindow();
	if (wig)
	{
		m_layerPropPages[interface] = wig;
		ui->tabWidget_main->addTab(wig,interface->cb_name());
	}
}

osm_frame_widget::~osm_frame_widget()
{
	delete ui;
}

void osm_frame_widget::EnumPlugins()
{
	QDir pluginsDir(QApplication::applicationDirPath());
	QStringList filters;
	filters << "*.dll";
	pluginsDir.setNameFilters(filters);
	//查找文件
	foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
		QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
		QObject *plugin = pluginLoader.instance();//尝试调入Plugin
		if (plugin) {
			layer_interface * pPlugin= qobject_cast<layer_interface *>(plugin);//动态类型转换
			if (pPlugin)//具有这个接口
			{
				//加入图层
				AppendLayer(pPlugin);
			}
		}
	}
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
			layers[nItems - 1 -row]->cb_setVisible(true);
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
			layers[nItems - 1 -row]->cb_setVisible(false);
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
				layers[i]->cb_setActive((i==(nItems - 1 -row))?true:false);
			UpdateLayerTable();

		}
	}
}

