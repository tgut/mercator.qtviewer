#ifndef QTVPLUGIN_GRID_H
#define QTVPLUGIN_GRID_H

#include <QWidget>
#include "../qtviewer_planetosm/osmtiles/layer_interface.h"
#include "../qtviewer_planetosm/osmtiles/viewer_interface.h"
namespace Ui {
	class qtvplugin_grid;
}

class qtvplugin_grid : public QWidget, layer_interface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID OSMLayerInterface_iid )
	Q_INTERFACES(layer_interface)
public:
	qtvplugin_grid(QWidget *parent = 0);
	~qtvplugin_grid();

private:
	Ui::qtvplugin_grid *ui;
	viewer_interface * m_pVi;
	bool m_bVisible;
public:
	virtual bool cb_initlayer(QObject  * ptrviewer);
	virtual QWidget * cb_create_propWindow();
	virtual void cb_finilayer(void * ptrviewer);
	virtual void cb_update();

	virtual void cb_paintEvent( QPainter * pImage );
	virtual bool cb_mousePressEvent ( QMouseEvent * event );
	virtual bool cb_mouseReleaseEvent ( QMouseEvent * event );
	virtual bool cb_mouseMoveEvent(QMouseEvent * event);
	virtual bool cb_wheelEvent ( QWheelEvent * event );
	virtual void cb_resizeEvent ( QResizeEvent * event );
	virtual void cb_levelChanged(int nLevel);

	virtual bool cb_isActive();
	virtual void cb_setActive(bool at);
	virtual bool cb_isVisible();
	virtual void cb_setVisible(bool vb);
	virtual QString cb_name();
	virtual void cb_setName(QString vb);

};

#endif // QTVPLUGIN_GRID_H


