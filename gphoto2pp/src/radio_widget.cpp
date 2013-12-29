#include "radio_widget.hpp"

#include "exceptions.hpp"
#include "camera_widget_type_wrapper.hpp"

#include "log.h"

namespace gphoto2pp
{

	RadioWidget::RadioWidget(gphoto2::_CameraWidget* cameraWidget)
		: ChoicesWidget(cameraWidget)
	{
		FILE_LOG(logINFO) << "RadioWidget constructor - widget";	
		
		if(this->getType() != CameraWidgetTypeWrapper::Radio)
		{
			throw exceptions::InvalidWidgetType("The widget type must be a Radio Widget");
		}
	}
}

