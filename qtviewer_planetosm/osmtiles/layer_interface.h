#ifndef LAYER_BASE_H
#define LAYER_BASE_H
#include <QObject>
#include <QWidget>
#include <QMouseEvent>
#include <QResizeEvent>
#define OSMLayerInterface_iid "org.goldenhawkingStudio.OSMViewer_iid.LayerInterface"
class layer_interface
{
public:
	virtual ~layer_interface(){}
	virtual bool cb_initlayer(QObject  * ptrviewer) = 0;
	virtual QWidget * cb_create_propWindow() = 0;
	virtual void cb_finilayer(void * ptrviewer) = 0;
	virtual void cb_update() = 0;

	virtual void cb_paintEvent( QPainter * pImage ) = 0;
	virtual bool cb_mousePressEvent ( QMouseEvent * event ) = 0;
	virtual bool cb_mouseReleaseEvent ( QMouseEvent * event ) = 0;
	virtual bool cb_mouseMoveEvent(QMouseEvent * event) = 0;
	virtual bool cb_wheelEvent ( QWheelEvent * event ) = 0;
	virtual void cb_resizeEvent ( QResizeEvent * event ) = 0;
	virtual void cb_levelChanged(int nLevel) = 0;

	virtual bool cb_isActive() =0;
	virtual void cb_setActive(bool at) =0;
	virtual bool cb_isVisible() =0;
	virtual void cb_setVisible(bool vb) =0;
	virtual QString cb_name() =0;
	virtual void cb_setName(QString vb)  =0;

};
Q_DECLARE_INTERFACE(layer_interface, OSMLayerInterface_iid)
#endif // LAYER_BASE_H
