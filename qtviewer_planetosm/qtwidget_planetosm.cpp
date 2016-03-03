#include "qtwidget_planetosm.h"
#include "osm_frame_widget.h"
#include <QSettings>
#include <QCoreApplication>
#include <QLibraryInfo>
#include <QDebug>
#include <functional>
#include <assert.h>
#include "osmtiles/tilesviewer.h"
#include "osmtiles/layer_tiles.h"

qtwidget_planetosm::qtwidget_planetosm(QWidget *parent )
	:QWidget(parent)
	,m_map_widget(0)
{
	QCoreApplication * app =  QCoreApplication::instance();
	if (app)
	{
		qtTranslator.load("qt_" + QLocale::system().name(),
						  QLibraryInfo::location(QLibraryInfo::TranslationsPath));
		app->installTranslator(&qtTranslator);

		QString strTransLocalFile =
				QCoreApplication::applicationDirPath()+"/" +
				"qtviewer_planetosm_"+
				QLocale::system().name()+".qm";
		if (true==appTranslator.load(strTransLocalFile ))
			QTVOSM_DEBUG("Load translationfile")<<"\n\t"<<strTransLocalFile<<" Succeeded.";
		else
			QTVOSM_WARNING("Load translationfile")<<"\n\t"<<strTransLocalFile<<" Not Found.";
		app->installTranslator(&appTranslator);
	}
	//create main widget
	m_map_widget =new osm_frame_widget(this);
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);
	assert(mp!=NULL);
	mp->viewer()->listen_event("ACTIVEX", std::bind(&qtwidget_planetosm::evt_listener,this,std::placeholders::_1));

	QVector<layer_interface *> vec =  mp->viewer()->layers();
	foreach(layer_interface * p, vec)
	{
		p->load_retranslate_UI();
	}
	QTVOSM_DEBUG("The qtwidget_planetosm class constructed.");
}

qtwidget_planetosm::~qtwidget_planetosm()
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);
	assert(mp!=NULL);
	mp->viewer()->unlisten_event("ACTIVEX") ;
}

void qtwidget_planetosm::resizeEvent(QResizeEvent * e)
{
	if (!m_map_widget)
		return;

	if (e->size().width()>0)
	{
		osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);
		assert(mp!=NULL);
		mp->setGeometry(0,0,e->size().width(),e->size().height());
	}
	QWidget::resizeEvent(e);
}

viewer_interface * qtwidget_planetosm::viewer()
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);
	assert(mp!=NULL);
	return mp->viewer();
}

/*!
 \brief	osm_set_remote_address set the address of the OSM layer.
 a Address is almost like:
	http://192.168.1.127/osm/%1/%2/%3.png
	or
	http://192.168.11.27/new/osm.cgi?z=%1&x=%2&y=%3

 \fn	qtwidget_planetosm::setTileAddress
 \param	addr	QString type address.
*/
void qtwidget_planetosm::osm_set_remote_address (QString layerName, QString addr)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	layer_interface * la = pv->layer(layerName);
	if (la)
	{
		layer_tiles * lt = dynamic_cast<layer_tiles *>(la);
		if (lt)
			lt->setServerUrl(addr);
	}
}

QString qtwidget_planetosm::osm_get_remote_address(QString layerName) const
{
	QString res = "http://c.tile.openstreetmap.org/%1/%2/%3.png";
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	layer_interface * la = pv->layer(layerName);
	if (la)
	{
		layer_tiles * lt = dynamic_cast<layer_tiles *>(la);
		if (lt)
			res = lt->serverUrl();
	}
	return res;
}

QString qtwidget_planetosm::osm_get_local_cache(QString layerName) const
{
	QString res = "./OSMCache";
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	layer_interface * la = pv->layer(layerName);
	if (la)
	{
		layer_tiles * lt = dynamic_cast<layer_tiles *>(la);
		if (lt)
			res = lt->localCache();
	}
	return res;
}
void	qtwidget_planetosm::osm_set_local_cache (QString layerName, QString addr)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	layer_interface * la = pv->layer(layerName);
	if (la)
	{
		layer_tiles * lt = dynamic_cast<layer_tiles *>(la);
		if (lt)
			lt->setLocalCache(addr);
	}
}
int		qtwidget_planetosm::osm_get_cache_expire_days(QString layerName)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	layer_interface * la = pv->layer(layerName);
	if (la)
	{
		layer_tiles * lt = dynamic_cast<layer_tiles *>(la);
		if (lt)
			return lt->cacheExpireDays();
	}
	return 0;
}

int		qtwidget_planetosm::osm_set_cache_expire_days(QString layerName,int days)
{
	int res = 0;
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	layer_interface * la = pv->layer(layerName);
	if (la)
	{
		layer_tiles * lt = dynamic_cast<layer_tiles *>(la);
		if (lt)
		{
			lt->setCacheExpireDays(days);
			res = days;
		}
	}
	return res;
}

/*!
 \brief This function is equal to check the "auto download" checkbox in UI
	when the tile is not exist in local cache, layer will start a
	task to download it from tileAddress()

 \fn qtwidget_planetosm::ConnectToServer
*/
void qtwidget_planetosm::osm_set_auto_download (QString LayerName, int v)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	layer_interface * la = pv->layer(LayerName);
	if (la)
	{
		layer_tiles * lt = dynamic_cast<layer_tiles *>(la);
		if (lt)
			lt->connectToTilesServer(v==0?false:true);
	}
}

/*!
 \brief return whether  "auto download" checkbox is checked.
 notice that even if this box is checked, if the URL of
 remote tile server is not valid, this function will still return true.

 \fn qtwidget_planetosm::IsConnected
 \return int	-1 means connected, 0 mean not.
*/
int qtwidget_planetosm::osm_get_auto_download(QString LayerName)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	layer_interface * la = pv->layer(LayerName);
	if (la)
	{
		layer_tiles * lt = dynamic_cast<layer_tiles *>(la);
		if (lt)
			return lt->isConnected()==true?-1:0;
	}
	return 0;
}

int qtwidget_planetosm::osm_get_level(void)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	return pv->level();
}

int qtwidget_planetosm::osm_set_level(int lv)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	int res =  pv->level();
	pv->setLevel(lv);
	return res;
}

double qtwidget_planetosm::osm_get_center_lat()
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	double lat,lon;
	pv->centerLLA(&lat,&lon);
	return lat;
}

double qtwidget_planetosm::osm_get_center_lon()
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	double lat,lon;
	pv->centerLLA(&lat,&lon);
	return lon;
}

int qtwidget_planetosm::osm_set_center_pos(double lat,double lon)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	pv->setCenterLLA(lat,lon);
	return 1;
}

/*!
 \brief this function does the samething as UI button "SaveImg"
 it will call saveToImage() , and save current map view to file.

 \fn qtwidget_planetosm::SaveCurrentViewToFile
 \param fm	filename in  QString. ext name can specify the format.
 \return int	1 means OK, 0 means failed.
*/
int qtwidget_planetosm::osm_save_view(QString fm)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	bool ok = pv->saveToImage(fm);
	return ok==true?1:0;
}


/*!
 \brief this protected function is designed to filter events, and fire OCX events when needed.

 \fn qtwidget_planetosm::evt_listener
 \param QMap<QString
 \param e
*/
void qtwidget_planetosm::evt_listener(const QMap<QString, QVariant> e)
{
	if (e["destin"]=="ALL" ||  e["destin"]=="OUTER")
	{
		emit map_event(e);
	}
}

int qtwidget_planetosm::osm_layer_get_count()
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);
	return mp->viewer()->layerNames().size();
}

QString qtwidget_planetosm::osm_layer_get_name(int n)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);
	QVector<QString> names = mp->viewer()->layerNames();
	if (n>=0 && n<names.size())
		return names[n];
	return "";
}

int qtwidget_planetosm::osm_layer_set_visiable(QString layerName, int v)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	layer_interface * la = pv->layer(layerName);
	if (la)
	{
		la->set_visible(v==0?false:true);
		pv->updateLayerGridView();
		return la->is_visible()==false?0:-1;
	}
	return 0;
}

int qtwidget_planetosm::osm_layer_get_visiable(QString layerName)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	layer_interface * la = pv->layer(layerName);
	if (la)
		return la->is_visible()==false?0:-1;
	return 0;
}

int qtwidget_planetosm::osm_layer_set_active(QString layerName, int v)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	layer_interface * la = pv->layer(layerName);
	if (la)
	{
		la->set_active(v==0?false:true);
		if (v!=0)
			pv->adjust_layers(la);
		pv->updateLayerGridView();
		return la->is_active()==false?0:-1;
	}
	return 0;
}

int qtwidget_planetosm::osm_layer_get_active(QString layerName)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	layer_interface * la = pv->layer(layerName);
	if (la)
		return la->is_active()==false?0:-1;
	return 0;
}
int		qtwidget_planetosm::osm_layer_move_up(QString layerName)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	layer_interface * la = pv->layer(layerName);
	if (la)
	{
		pv->moveLayerUp(la);
		pv->updateLayerGridView();
		return -1;
	}
	return 0;
}

int		qtwidget_planetosm::osm_layer_move_down(QString layerName)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	layer_interface * la = pv->layer(layerName);
	if (la)
	{
		pv->moveLayerDown(la);
		pv->updateLayerGridView();
		return -1;
	}
	return 0;
}

int		qtwidget_planetosm::osm_layer_move_top(QString layerName)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	layer_interface * la = pv->layer(layerName);
	if (la)
	{
		pv->moveLayerTop(la);
		pv->updateLayerGridView();
		return -1;
	}
	return 0;
}

int		qtwidget_planetosm::osm_layer_move_bottom(QString layerName)
{
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);tilesviewer * pv=mp->viewer();
	layer_interface * la = pv->layer(layerName);
	if (la)
	{
		pv->moveLayerBottom(la);
		pv->updateLayerGridView();
		return -1;
	}
	return 0;
}

/**
 * @brief	osm_layer_call_function call layers' call_func method from
 * outside the ocx ctrl. Please MAKE SURE that this function is called from UI thread,
 * which means the same thread that OCX ctrl stays. Calling "call_func" from another thread is
 * NOT SUPPORTED, and will cause strange problems.
 *
 * @param layerName	the layer name to whom this function call will be sent
 * @param args	args stored in key, value strings,
 * key, value is connected with "=", and each pairs splitted by ";"
 * eg, function=get_polygon;x=38.43834784;y=16.3834754;
 * @return QString	the result string is also formatted with key-vaslue para strings.
 */
QMap<QString, QVariant> qtwidget_planetosm::osm_layer_call_function(QString layerName, QMap<QString, QVariant> args)
{
	QMap<QString, QVariant> p_out;
	osm_frame_widget * mp = qobject_cast<osm_frame_widget *>(m_map_widget);
	tilesviewer * pv = mp->viewer();
	layer_interface * la = pv->layer(layerName);
	if (la)
		p_out = la->call_func(args);
	else
		p_out["error"] = QString("Layer name \"%1\" does not exist.").arg(layerName);
	return p_out;
}



