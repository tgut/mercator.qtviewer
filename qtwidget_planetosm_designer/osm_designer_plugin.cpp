#include "osm_designer_plugin.h"
#include "../qtviewer_planetosm/qtwidget_planetosm.h"

osm_designer_plugin::osm_designer_plugin(QObject *parent) :
	QObject(parent),
	initialized(false)
{
}
bool osm_designer_plugin::isContainer() const
{
	return false;
}
bool osm_designer_plugin::isInitialized() const
{
	return initialized;
}
QIcon osm_designer_plugin::icon() const
{
	return QIcon(":/ui/icons/Blizzard17.png");
}
QString osm_designer_plugin::domXml() const
{
	return "<ui language=\"c++\">\n"
		   " <widget class=\"qtwidget_planetosm\" name=\"osmmap\">\n"
		   "  <property name=\"geometry\">\n"
		   "   <rect>\n"
		   "    <x>0</x>\n"
		   "    <y>0</y>\n"
		   "    <width>640</width>\n"
		   "    <height>480</height>\n"
		   "   </rect>\n"
		   "  </property>\n"
		   " </widget>\n"
		   "</ui>";

}
QString osm_designer_plugin::group() const
{
	return "Display Widgets";
}
QString osm_designer_plugin::includeFile() const
{
	return "qtwidget_planetosm.h";
}
QString osm_designer_plugin::name() const
{
	return "qtwidget_planetosm";
}
QString osm_designer_plugin::toolTip() const
{
	return "A widget that provides full access to OSM tiles of planetosm.";
}
QString osm_designer_plugin::whatsThis() const
{
	return "A widget that provides full access to OSM tiles of planetosm.";
}
QWidget *osm_designer_plugin::createWidget(QWidget *parent)
{
	return new qtwidget_planetosm(parent);
}

void osm_designer_plugin::initialize(QDesignerFormEditorInterface *core)
{
	if (initialized)
		return;

	initialized = true;
}
