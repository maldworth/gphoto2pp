#include "range_widget.hpp"

#include "helper_gphoto2.hpp"
#include "exceptions.hpp"
#include "camera_widget_type_wrapper.hpp"
#include "range_widget_range.hpp"

#include "log.h"

namespace gphoto2
{
#include <gphoto2/gphoto2-widget.h>
}

namespace gphoto2pp
{

	RangeWidget::RangeWidget(gphoto2::_CameraWidget* cameraWidget)
		: FloatWidget(cameraWidget)
	{
		FILE_LOG(logINFO) << "RangeWidget constructor - widget";	
		
		if(this->getType() != CameraWidgetTypeWrapper::Range)
		{
			throw exceptions::InvalidWidgetType("The widget type must be a Range Widget");
		}
	}
	
	RangeWidgetRange RangeWidget::getRange() const
	{
		auto range = RangeWidgetRange{0,0,0};
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_range(m_cameraWidget, &range.Min, &range.Max, &range.Step),"gp_widget_get_range");
		return range;
	}
	
	std::string RangeWidget::ToString() const
	{
		float m_min = 0, m_max = 0, m_step = 0;
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_range(m_cameraWidget, &m_min, &m_max, &m_step),"gp_widget_get_range");
		
		std::stringstream temp;
		temp << "[min: " << m_min << " max: " << m_max << " step: " << m_step << "]";
		return temp.str();
	}

}

