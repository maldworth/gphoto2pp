#include "toggle_widget.hpp"

#include "exceptions.hpp"
#include "camera_widget_type_wrapper.hpp"

#include "log.h"

namespace gphoto2pp
{

	ToggleWidget::ToggleWidget(gphoto2::_CameraWidget* cameraWidget)
		: IntWidget(cameraWidget)
	{
		FILE_LOG(logINFO) << "ToggleWidget constructor - widget";
		
		if(this->getType() != CameraWidgetTypeWrapper::Toggle)
		{
			throw exceptions::InvalidWidgetType("The widget type must be a Toggle Widget");
		}
	}
}

