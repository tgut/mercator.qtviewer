#ifndef VIEWER_INTERFACE_H
#define VIEWER_INTERFACE_H
#include <QMap>
#include <QVariant>
#include <QVector>
#include <QString>


#define OSMViewInterface_iid "org.goldenhawkingStudio.OSMViewer_iid.ViewerInterface"
namespace QTVOSM{
	class layer_interface;
	/*!	viewer_interface is the basic interface of views.
	 * views is the canvas holds all layers. Actually, views is the MAINWND of a MAP.
	 * It contains a user interactive system, a coordinates system, it alse maintains the
	 * layers approaches, such as loading, ordering, setName, messages pumping
	 *
	 * The coordinates system in this framework listed below is very IMPORTANT:
	 * 1.Mercator projection: Project WGS 84 (LLA) to conformal cylindrical projection, which is wildy used by goole map, OSM, baidu map.
	 *   Mercator coords unit is "Meter", the worldmap is square, means X,Y range are both from -pi * R to pi *R, R is the radius of earth.
	 *   X's direction is from left,west (-) to right, east (+), Y's direction is from bottom,south(-) to top, north(+)
	 *   the center (0,0) takes the same point at LLA (0,0,0), the Y axis is the prime meridian, X axis is the equator woof.
	 *
	 * 2.LLA, latitude, longitude and altitude
	 *
	 * 3.World Coordinate: uint:Pixel.
	 *     Map system have different zoom level, from 0 to 18.
	 *    At level 0, the Mercator Square is mapped onto a 256x256 tile.
	 *    At level 1, the Mercator Square is mapped onto a 512x512 tile.
	 *	  At level 18,the Mercator Square is mapped onto a 67108864 x 67108864 tile. (size is 256 * 2 ^18)
	 *    the coord direction of .World Coordinate is some different from Mercator projection.
	 *    top-left is 0,0, right-bottom is  256 * 2 ^level-1,256 * 2 ^level-1
	 *
	 * 4.Percentage Coordinate: unit: 1%
	 *    it simply stands for Mercator projection, as percentage, -0.5~0.5
	 *
	 * 5.Device Coords
	 *	  it corresponds to  the widget coords.
	 *
	 * \class	viewer_interface viewer_interface.h "qtviewer_planetosm/osmtiles/viewer_interface.h"
	 * \author	goldenhawking	\date	2015-12-11
	*/
	class viewer_interface{
	public:
		virtual ~viewer_interface(){}
	public:
		//!	Convert LLA and DP Coords. from LLA to view port pixels
		virtual bool CV_LLA2DP(double lat,double lon,qint32 * pX,qint32 *pY) = 0;
		//!	Convert LLA and DP Coords. from  view port pixels to LLA
		virtual bool CV_DP2LLA(qint32 X,qint32 Y,double  * plat,double * plon) = 0;
		//Convert Merkator and LLA
		virtual bool CV_MK2LLA(double mx, double my, double * plat, double * plon) = 0;
		virtual bool CV_LLA2MK(double lat,double lon, double * pmx, double * pmy) = 0;
		//Convert Merkator and WorldPixel
		virtual bool CV_MK2World(double mx, double my, double * px, double * py) = 0;
		virtual bool CV_World2MK(double x,double y, double * pmx, double * pmy) = 0;
		//Convert LLA and World
		virtual bool CV_LLA2World(double lat, double lon, double * px, double * py) = 0;
		virtual bool CV_World2LLA(double x,double y, double * plat, double * plon) = 0;
		//Convert World and DP
		virtual bool CV_DP2World(qint32 dX, qint32 dY, double * px, double * py) = 0;
		virtual bool CV_World2DP(double x,double y, qint32 * dX, qint32 * dY) = 0;
		//!	Convert Global Percentage coods (-0.5% ~ 0.5%) to pixel coods {( 1<<nLevel ) * 256}
		virtual bool CV_Pct2World(double px,double py,double * nx,double * ny) = 0;
		virtual bool CV_World2Pct(double nx,double ny,double * px,double * py) = 0;
	public:
		//!	return current level
		virtual int		level		() = 0;
		//!	set current level, will re-paint the map when needed
		virtual void	setLevel	(int n) = 0;
		//!	get centerX,percentage, -0.5~0.5
		virtual double	centerX	() = 0;
		//!	get centerY,percentage, -0.5~0.5
		virtual double	centerY	() = 0;
		//!	return current center view lat, lon
		virtual void	centerLLA		(double * lat, double * lon) = 0;
		//!	set center LLA, will re-paint the map
		virtual void	setCenterLLA	(double lat, double lon) = 0;
	public:
		//!	make exclusive layrs being de-activated,except for target
		virtual bool adjust_layers(layer_interface * target ) = 0;
		virtual QVector<layer_interface * > layers() = 0;
		virtual QVector<QString> layerNames() = 0;
		virtual layer_interface * layer(const QString &) = 0;
		virtual bool addLayer(layer_interface * ) = 0;
		virtual void removeLayer(layer_interface * ) = 0;
		virtual void moveLayerUp(layer_interface *)= 0;
		virtual void moveLayerDown(layer_interface *)= 0;
		virtual void moveLayerTop(layer_interface *)= 0;
		virtual void moveLayerBottom(layer_interface *)= 0;
		virtual void updateLayerGridView()= 0;
		//!	get the windows device size in pixels
		virtual QRect windowRect() = 0;
		virtual int windowWidth() = 0;
		virtual int windowHeight() = 0;
		//!	force update
		virtual void UpdateWindow() = 0;
		//!	drag image for special pixes
		virtual void DragView(int nOffsetX,int nOffsetY) = 0;
		virtual bool saveToImage(const QString & strFm) = 0;
	public:
		/*! post_event posts events to mainPRG and return immediately.
		 * the event system is formed by key-value maps. A message CAB at lease should contain 2
		 * pairs: destin and source
		 * destin means to whom this message is sent to. source means from whom this message is sent.
		 * MAIN_MAP means the main framework, it can be used both in source and destin
		 * OUTER	means the plugins loaded from dynamic libraries and OCX containers. it can only be used in  destin
		 * ALL  	means Every possible listenes. it can only be used in  destin
		 * except these 3 names above, other names is freely used for convenience.
		 *
		 * \param	QMap<QString.QVariant >
		 * \return	bool Succeed = true.
		*/
		virtual bool post_event(const QMap<QString, QVariant> ) = 0;
		/*! send events to mainPRG and return when all progress finished(Blocking).
		 * the event system is formed by key-value maps. A message CAB at lease should contain 2
		 * pairs: destin and source
		 * destin means to whom this message is sent to. source means from whom this message is sent.
		 * MAIN_MAP means the main framework, it can be used both in source and destin
		 * OUTER	means the plugins loaded from dynamic libraries and OCX containers. it can only be used in  destin
		 * ALL  	means Every possible listenes. it can only be used in  destin
		 * except these 3 names above, other names is freely used for convenience.
		 *
		 * \param	QMap<QString.QVariant >
		 * \return	bool Succeed = true.
		*/
		virtual bool send_event(const QMap<QString, QVariant> ) = 0;

	};

}
Q_DECLARE_INTERFACE(QTVOSM::viewer_interface, OSMViewInterface_iid)
#endif // VIEWER_INTERFACE_H
