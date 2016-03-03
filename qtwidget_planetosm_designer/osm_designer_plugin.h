#ifndef OSM_DESIGNER_PLUGIN_H
#define OSM_DESIGNER_PLUGIN_H

#include <QObject>
#include <QDesignerCustomWidgetCollectionInterface>
#include "qtwidget_planetosm_designer_global.h"
class QTWIDGET_PLANETOSM_DESIGNERSHARED_EXPORT osm_designer_plugin : public QObject, public QDesignerCustomWidgetInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetInterface")
	Q_INTERFACES(QDesignerCustomWidgetInterface)
public:

	explicit osm_designer_plugin(QObject *parent = 0);
	bool isContainer() const;
	bool isInitialized() const;
	QIcon icon() const;
	QString domXml() const;
	QString group() const;
	QString includeFile() const;
	QString name() const;
	QString toolTip() const;
	QString whatsThis() const;
	QWidget *createWidget(QWidget *parent);
	void initialize(QDesignerFormEditorInterface *core);

private:
	bool initialized;
};

#endif // OSM_DESIGNER_PLUGIN_H

