#ifndef VIEWER_INTERFACE_H
#define VIEWER_INTERFACE_H
#define OSMViewInterface_iid "org.goldenhawkingStudio.OSMViewer_iid.ViewerInterface"
class viewer_interface{
public:
	virtual ~viewer_interface(){}
	//Convert LLA and DP Coords
	virtual bool oTVP_LLA2DP(double lat,double lon,qint32 * pX,qint32 *pY) = 0;
	virtual bool oTVP_DP2LLA(qint32 X,qint32 Y,double  * plat,double * plon) = 0;
	virtual bool CV_PercentageToPixel(int nLevel,double px,double py,int * nx,int * ny) = 0;
	//return current level
	virtual int level() = 0;
	//center
	virtual double centerX() = 0;
	virtual double centerY() = 0;
	//return current center view lat, lon
	virtual void centerLLA(double * lat, double * lon) = 0;
	//drag image for special pixes
	virtual void DragView(int nOffsetX,int nOffsetY) = 0;
	//set current level
	virtual void setLevel(int n) = 0;
	//force update
	virtual void UpdateWindow() = 0;
	//set center LLA
	virtual void setCenterLLA(double lat, double lon) = 0;
	//set center LLA
	virtual QRect windowRect() = 0;
};
Q_DECLARE_INTERFACE(viewer_interface, OSMViewInterface_iid)
#endif // VIEWER_INTERFACE_H
