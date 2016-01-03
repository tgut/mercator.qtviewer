#ifndef LAYER_BASE_H
#define LAYER_BASE_H
#include <QObject>
#include <QWidget>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QVariant>
#include <QMap>
#define OSMLayerInterface_iid "org.goldenhawkingStudio.OSMViewer_iid.LayerInterface"

#define QTVOSM_DEBUG(MSG) qDebug()<<"QTVOSM Debug>"<< MSG <<"\n\t"<<__FUNCTION__<<":"<<__FILE__<<__LINE__
#define QTVOSM_WARNING(MSG) qWarning()<<"QTVOSM Debug>"<< MSG <<"\n\t"<<__FUNCTION__<<":"<<__FILE__<<__LINE__
#define QTVOSM_FATAL(MSG) qFatal()<<"QTVOSM Debug>"<< MSG <<"\n\t"<<__FUNCTION__<<":"<<__FILE__<<__LINE__

class osm_frame_widget;
class qtaxviewer_planetosm;

namespace QTVOSM
{
	class viewer_interface;

	/*! the interface for layers. cb_ means 'CALLBACK'
	 \author	goldenhawking	\date	2015-12-11
	*/
	class layer_interface
	{
		friend class tilesviewer;
		friend class ::osm_frame_widget;
		friend class ::qtaxviewer_planetosm;
	public:
		virtual ~layer_interface(){}
		/*!	get_propWindow should return the pointer to prop window.
		 * 	this funtion is called by MAINWND or other layers when needed.
		 * \return	QWidget *	the window pointer.
		*/
		//initlize functions is protected, only viewer_interface can call it when start up
	protected:
		/*! load_InitialBind Initializer, plugin should return a instance belongs to ptrviewer in this callback.
		 * Very Important! Note that Even If there're more than one viewer_interface in an process, the Plugin-Shared lib will only be loaded once,
		 * Plugin designer has a responsblility to maintain a map between viewer_interface and layer_interface, return a brave new instance of layer_interface
		 * for each viewer_interface.
		 *
		 * \param	strSLibPath	QString	strSLibPath the fullpath of this plugin descovered by Main Prg.
		 * \param	ptrviewer	viewer_interface * stands for the main view.
		 * \return	layer_interface *	return this plugin interface when succeeded, otherwise return 0;
		 */
		virtual layer_interface *	load_initial_plugin(QString strSLibPath,viewer_interface  * ptrviewer) = 0;


		/*! MainWND will call this method when plugin loading.
		 * Plugin should create a new widget in this funtion, and return the pointer.
		 * \return  QWidget * means the prop window just being created
		*/
		virtual QWidget *			load_prop_window() = 0;


		/*! load_retranslate_UI is called when layer is being inited.
		 * 	User should inher this method, when you want to include international local support.
		 */
		virtual void				load_retranslate_UI(){}

		//callback functions is protected, only viewer_interface can call it when start up
	protected:
		//Callback Systems
		virtual void cb_paintEvent( QPainter * /*pImage*/ ) = 0;
		virtual bool cb_mousePressEvent ( QMouseEvent * /*event*/ ){return false;}
		virtual bool cb_mouseReleaseEvent ( QMouseEvent * /*event*/ ){return false;}
		virtual bool cb_mouseMoveEvent(QMouseEvent *){return false;}
		virtual bool cb_mouseDoubleClickEvent(QMouseEvent *){return false;}
		virtual bool cb_wheelEvent ( QWheelEvent * ){return false;}
		virtual void cb_resizeEvent ( QResizeEvent *){}
		virtual void cb_levelChanged(int /*nLevel*/){}
		//user-def event callback
		virtual bool cb_event(const QMap<QString, QVariant> /*event*/){return false;}

	public:
		/*! is_exclusive returns whether this layer is Exclusive
		 * "Exclusive" means that at same time, there should be only one Exclusive plugins is active.
		 * Layer who want itself be Exclusive often has a demand of interaction with mouse, keyborad.
		 * Durning these interactions, it is unwilling to be disturbed by other layers.
		 *
		 * \return	bool Exclusive Method
		*/
		virtual bool		is_exclusive(){return false;}
		virtual QWidget *	get_propWindow() = 0;
		virtual QString		get_name() = 0;
		virtual void		set_name(QString)  =0;
		/*! return whether this layer is active.
		 *
		 * active layer will process mouse, key events, otherwise it should omit these messages.
		 */
		virtual bool is_active(){return false;}
		virtual void set_active(bool /*at*/){}
		virtual bool is_visible(){return false;}
		virtual void set_visible(bool /*vb*/) {}
	public:
		//user-def direct function calls
		virtual QMap<QString, QVariant> call_func(const  QMap<QString, QVariant> /*paras*/){return  std::move( QMap<QString, QVariant>());}

	};

}
Q_DECLARE_INTERFACE(QTVOSM::layer_interface, OSMLayerInterface_iid)
#endif // LAYER_BASE_H
