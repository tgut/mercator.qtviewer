#ifndef QTVPLUGIN_GRID_H
#define QTVPLUGIN_GRID_H

#include <QWidget>
#include <QTranslator>
#include <QVector>
#include <QStandardItemModel>
#include "../qtviewer_planetosm/osmtiles/layer_interface.h"
#include "../qtviewer_planetosm/osmtiles/viewer_interface.h"
namespace Ui {
	class qtvplugin_grid;
}
using namespace QTVOSM;
/*!	qtvplugin_grid support distance and area measuring. function calls avaliable:
	1.function=get_polygon, no other para needed. returns current selected polygon's cornor points, in lat, lon; size=N;lat0=XX;lon0=XX;
	lat1=XX;lon1=XX;lat2=XX;lon2=XX;...;latN-1=XX;lonN-1=XX.
	2.function=get_ruler_status, no other para needed.returns whether ruler tool is active now, status=0 means not active, status=-1 means active.
	3.function=set_ruler_status, with para status, will set ruler status to given value.
 \class qtvplugin_grid qtvplugin_grid.h "qtvplugin_grid/qtvplugin_grid.h"
*/
class qtvplugin_grid : public QWidget,public layer_interface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID OSMLayerInterface_iid )
	Q_INTERFACES(QTVOSM::layer_interface)
public:
	qtvplugin_grid(QWidget *parent = 0);
	~qtvplugin_grid();
	//Properties
	bool		is_active();
	void		set_active(bool ab);
	bool		is_visible();
	void		set_visible(bool vb);
	QString		get_name();
	void		set_name(QString vb);
	QWidget *	get_propWindow() {return this;}
	QMap<QString, QVariant> call_func(const  QMap<QString, QVariant> & /*paras*/);
private:
	int m_nInstance;
	//International Translator
	QTranslator				pluginTranslator;
	Ui::qtvplugin_grid *	ui;
	//the pointer to main MAP functions
	viewer_interface *		m_pVi;
	//Flags
	bool					m_bVisible;
	bool					m_bActive;
	//Lib Name
	QString					m_SLLibName;
	QString					m_SLLibPath;
	//Measure
	QPointF					m_pt_end;
	QPoint					m_mousePos;
	QVector<QPointF>		m_list_points;
	bool					bFinished;
	//simple mark
	int						m_nMarks;
	QStandardItemModel *	m_pModelCombo;
	//measure method
	void	CalArea();
	double	GetArea(double * PointX,double * PointY,int Count);
	double	CalDistance(double,double,double,double,double * psita);
	bool	loadTranslation();
protected:
	layer_interface *	load_initial_plugin(QString strSLibPath,viewer_interface  * ptrviewer);
	QWidget *			load_prop_window();
	void				load_retranslate_UI();
	void				load_ini();
	void				save_ini();
	QString				ini_file();
protected:
	//Events
	void cb_paintEvent( QPainter * pImage );
	bool cb_mouseMoveEvent(QMouseEvent *);
	bool cb_mousePressEvent(QMouseEvent *);
	//Event filters
	bool cb_event(const QMap<QString, QVariant>);
protected slots:
	void on_checkBox_measure_clicked(bool);
	void on_pushButton_add_mark_clicked();
	void on_pushButton_clear_clicked();
	void on_pushButton_clear_all_clicked();
};

#endif // QTVPLUGIN_GRID_H


