#include "window_widget.hpp"

#include "exceptions.hpp"
#include "camera_widget_type_wrapper.hpp"

#include "log.h"

namespace gphoto2pp
{

	WindowWidget::WindowWidget(gphoto2::_CameraWidget* cameraWidget)
		: NonValueWidget(cameraWidget)
	{
		if(this->getType() != CameraWidgetTypeWrapper::Window)
		{
			throw exceptions::InvalidWidgetType("The widget type must be a Window Widget");
		}
	}
}

