#pragma once
#include <math.h>
namespace QTVOSM
{
	/*! cProjectionMercator is a toolkit class that turns coordinates between LLA(4326) and Mercator(900913)
	 cProjectionMercator cProjectionMercator.h "qtviewer_planetosm/osmtiles/cProjectionMercator.h"
	  \author	goldenhawking	\date	2015-12-11
	*/
	class cProjectionMercator
	{
	public:
		double m_lat,m_lon;
		double m_x,m_y;
		static const double R;
		static const double pi;

		cProjectionMercator(double v_cood=0,double h_cood=0)
			:m_lat(v_cood),
			  m_lon(h_cood),
			  m_x(h_cood),
			  m_y(v_cood)
		{
		}

		virtual ~cProjectionMercator(void)
		{
		}

		cProjectionMercator & ToLatLon()
		{
			m_lon = 180.0 * (m_x / cProjectionMercator::R) /cProjectionMercator::pi;
			m_lat = (atan(exp(m_y / cProjectionMercator::R))-cProjectionMercator::pi/4)*2.0/cProjectionMercator::pi*180.0;
			return *this;
		}

		cProjectionMercator & ToMercator()
		{
			m_x = cProjectionMercator::R * m_lon* cProjectionMercator::pi /180.0;
			m_y =
					cProjectionMercator::R *
					log
					(
						tan
						(
							m_lat/180.0* cProjectionMercator::pi/2.0 + cProjectionMercator::pi/4
							)
						);
			return *this;
		}

	};
}
