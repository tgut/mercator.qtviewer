#include "qtaxviewer_planetosm.h"
#include "ui_osm_frame_widget.h"
#include <QAxFactory>
#include <QSettings>
#include <QCoreApplication>
#include <QLibraryInfo>
#include <QDebug>
#include <functional>
#include "osmtiles/tilesviewer.h"
#include "osmtiles/layer_tiles.h"
qtaxviewer_planetosm::qtaxviewer_planetosm(QWidget *parent )
	:osm_frame_widget(parent)
{
	m_mutex_proteced.lock();
	QTVOSM_DEBUG("The qtaxviewer_planetosm class is constructing.");
	connect (this,&qtaxviewer_planetosm::_evt_next_pending_evts,this,&qtaxviewer_planetosm::_next_pending_evts,Qt::QueuedConnection);
	ui->widget_mainMap->listen_event("ACTIVEX", std::bind(&qtaxviewer_planetosm::evt_listener,this,std::placeholders::_1));
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

	ui->retranslateUi(this);

	QVector<layer_interface *> vec =  ui->widget_mainMap->layers();
	foreach(layer_interface * p, vec)
	{
		p->load_retranslate_UI();
	}
	QTVOSM_DEBUG("The qtaxviewer_planetosm class constructed.");
	m_mutex_proteced.unlock();
}

qtaxviewer_planetosm::~qtaxviewer_planetosm()
{
	ui->widget_mainMap->unlisten_event("ACTIVEX") ;
}

/*!
 \brief	setTileAddress set the address of the OSM layer.
 a Address is almost like:
	http://192.168.1.127/osm/%1/%2/%3.png
	or
	http://192.168.11.27/new/osm.cgi?z=%1&x=%2&y=%3

 \fn	qtaxviewer_planetosm::setTileAddress
 \param	addr	QString type address.
*/
void qtaxviewer_planetosm::setTileAddress (QString addr)
{
	tilesviewer * pv = this->ui->widget_mainMap ;
	layer_interface * la = pv->layer("OSM");
	if (la)
	{
		layer_tiles * lt = dynamic_cast<layer_tiles *>(la);
		if (lt)
			lt->setServerUrl(addr);
	}
}

QString qtaxviewer_planetosm::tileAddress() const
{
	QString res = "http://c.tile.openstreetmap.org/%1/%2/%3.png";
	tilesviewer * pv = this->ui->widget_mainMap ;
	layer_interface * la = pv->layer("OSM");
	if (la)
	{
		layer_tiles * lt = dynamic_cast<layer_tiles *>(la);
		if (lt)
			res = lt->serverUrl();
	}
	return res;
}

/*!
 \brief This function is equal to check the "auto download" checkbox in UI
	when the tile is not exist in local cache, layer will start a
	task to download it from tileAddress()

 \fn qtaxviewer_planetosm::ConnectToServer
*/
void qtaxviewer_planetosm::ConnectToServer (void)
{
	tilesviewer * pv = this->ui->widget_mainMap ;
	layer_interface * la = pv->layer("OSM");
	if (la)
	{
		layer_tiles * lt = dynamic_cast<layer_tiles *>(la);
		if (lt)
			lt->connectToTilesServer(true);
	}
}

/*!
 \brief return whether  "auto download" checkbox is checked.
 notice that even if this box is checked, if the URL of
 remote tile server is not valid, this function will still return true.

 \fn qtaxviewer_planetosm::IsConnected
 \return int	-1 means connected, 0 mean not.
*/
int qtaxviewer_planetosm::IsConnected(void)
{
	tilesviewer * pv = this->ui->widget_mainMap ;
	layer_interface * la = pv->layer("OSM");
	if (la)
	{
		layer_tiles * lt = dynamic_cast<layer_tiles *>(la);
		if (lt)
			return lt->isConnected()==true?-1:0;
	}
	return 0;
}

int qtaxviewer_planetosm::GetLevel(void)
{
	tilesviewer * pv = this->ui->widget_mainMap ;
	return pv->level();
}

int qtaxviewer_planetosm::SetLevel(int lv)
{
	tilesviewer * pv = this->ui->widget_mainMap ;
	int res =  pv->level();
	pv->setLevel(lv);
	return res;
}

double qtaxviewer_planetosm::GetCenterLatitude()
{
	tilesviewer * pv = this->ui->widget_mainMap ;
	double lat,lon;
	pv->centerLLA(&lat,&lon);
	return lat;
}

double qtaxviewer_planetosm::GetCenterLongitude()
{
	tilesviewer * pv = this->ui->widget_mainMap ;
	double lat,lon;
	pv->centerLLA(&lat,&lon);
	return lon;
}

int qtaxviewer_planetosm::SetCenterPos(double lat,double lon)
{
	tilesviewer * pv = this->ui->widget_mainMap ;
	pv->setCenterLLA(lat,lon);
	return 1;
}

/*!
 \brief this function does the samething as UI button "SaveImg"
 it will call saveToImage() , and save current map view to file.

 \fn qtaxviewer_planetosm::SaveCurrentViewToFile
 \param fm	filename in  QString. ext name can specify the format.
 \return int	1 means OK, 0 means failed.
*/
int qtaxviewer_planetosm::SaveCurrentViewToFile(QString fm)
{
	tilesviewer * pv = this->ui->widget_mainMap ;
	bool ok = pv->saveToImage(fm);
	return ok==true?1:0;
}

/*!
 \brief FrozenMap  mean that
 UI inputs, include MOUOSE, KEY, will take no effect for dragging

 \fn qtaxviewer_planetosm::FrozenMap
 \param status	set status. >0 means frozen it, <0 means re-enable it, 0 means just get status
 \return int	the current status.
*/
int qtaxviewer_planetosm::FrozenMap(int status)
{
	int ret = 0;
	if (status==0)
	{
		QVector <layer_interface *> layers = ui->widget_mainMap->layers();
		for (int i=0;i<layers.size()&& ret == 0;++i)
		{
			//It's exclusive, there should be at most only one layer_tiles active
			if (layers[i]->is_exclusive()==true && layers[i]->is_active()==true)
				ret = -1;
		}
		if (ret==0)
			ret = 1;
	}
	else if (status > 0)
	{
		QVector <layer_interface *> layers = ui->widget_mainMap->layers();
		for (int i=0;i<layers.size();++i)
		{
			//Turn all exclusive layers off( de-actived)
			if (layers[i]->is_exclusive()==true && layers[i]->is_active()==true)
			{
				ret = 1;
				layers[i]->set_active(false);
			}
		}
		this->UpdateLayerTable();
	}
	else
	{
		QVector <layer_interface *> layers = ui->widget_mainMap->layers();
		for (int i=0;i<layers.size() && ret == 0;++i)
		{
			//Turn first exclusive layers on( actived)
			if (layers[i]->is_exclusive()==true && layers[i]->is_active()==true)
			{
				ret = -1;
				layers[i]->set_active(true);
				ui->widget_mainMap->adjust_layers(layers[i]);
			}
		}
		this->UpdateLayerTable();
	}
	return ret;
}


/*!
 \brief this protected function is designed to filter events, and fire OCX events when needed.

 \fn qtaxviewer_planetosm::evt_listener
 \param QMap<QString
 \param e
*/
void qtaxviewer_planetosm::evt_listener(const QMap<QString, QVariant> e)
{
	if (e["destin"]=="ALL" ||  e["destin"]=="OUTER")
	{
		bool needFire = false;
		m_mutex_evts.lock();
		m_list_events.push_back(e);
		if (m_list_events.size()==1)
			needFire = true;
		m_mutex_evts.unlock();
		if (needFire)
			emit _evt_next_pending_evts();
	}
}
/*!
 \brief _next_pending_evts is the internal event loop for interesting events.

 \fn qtaxviewer_planetosm::_next_pending_evts
*/
void qtaxviewer_planetosm::_next_pending_evts()
{
	//!1, In this function, we will first check whether the queue  m_list_events is empty.
	const QMap<QString, QVariant> * e  = 0;
	bool needFire = false;
	m_mutex_evts.lock();
	//!2, If it is not empty, get the earlist item from begin.(FIFO)
	if (m_list_events.empty()==false)
		e = & *m_list_events.constBegin();
	m_mutex_evts.unlock();

	//!3,Extract dsource,destin, lat, lon from map
	QString str_source = e->value("source").toString();
	QString str_name = e->value("name").toString();
	double d_lat = e->value("main_lat").toDouble();
	double d_lon = e->value("main_lon").toDouble();
	QString str_additional_prop;
	//!4, for other key-value paras, just put it into str_additional_prop
	for(QMap<QString, QVariant>::const_iterator p = e->begin();p!=e->end();++p)
	{
		if (p.key()!="source" && p.key()!="name" && p.key()!="main_lat" && p.key()!="main_lon")
		{
			str_additional_prop += p.key();
			str_additional_prop +="=";
			str_additional_prop +=p.value().toString();
			str_additional_prop +=";";
		}
	}
	//!5,Fire the OCX Event
	emit evt_Message(str_source,str_name,d_lat,d_lon,str_additional_prop);

	//pop from queue
	m_mutex_evts.lock();
	e = 0;
	m_list_events.pop_front();
	if (m_list_events.size())
		needFire = true;
	m_mutex_evts.unlock();

	if (needFire)
		emit _evt_next_pending_evts();
}

QAXFACTORY_DEFAULT(qtaxviewer_planetosm,
	   "{8FDF97AD-FEFA-44C9-973B-1B66D4C529DF}",
	   "{4272B226-BAF3-4B7D-87F5-FBFB9D472666}",
	   "{8530D719-A863-4C2A-A1C6-77DC9DC06A5F}",
	   "{38D95322-682B-4CD5-9F28-C24775D70E33}",
	   "{581539E7-58EB-4F90-9B98-B7C4603B7163}")
