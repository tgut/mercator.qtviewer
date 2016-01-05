#include "qtvplugin_geomarker.h"
#include "ui_qtvplugin_geomarker.h"

/**
 * geomarker supports these function calls.
 *
 *
 *	ATTENTION!THIS FUNCTION IS NOT THREAD SAFE! Calls should be made only in main thread, also called "UI Thread"
 * @param paras	the key-value style paraments.
 * @return QMap<QString, QVariant>	the key-value style return values.
 */
QMap<QString, QVariant> qtvplugin_geomarker::call_func(const  QMap<QString, QVariant> paras)
{
	QMap<QString, QVariant> res;
	if (paras.contains("function"))
	{
		QString funct = paras["function"].toString();
		if (funct=="update_point")
		{

		}
		else if (funct=="update_line")
		{

		}
		else if (funct=="update_polygon")
		{

		}
		else
			res["error"] = QString("unknown function \"%1\".").arg(funct);
	}
	else
		res["error"] = "\"function\" keyword not specified, nothing to do.";
	return std::move(res);
}
