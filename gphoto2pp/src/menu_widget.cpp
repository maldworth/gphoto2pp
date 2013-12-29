#include "menu_widget.hpp"

#include "exceptions.hpp"
#include "camera_widget_type_wrapper.hpp"

#include "log.h"

namespace gphoto2pp
{

	MenuWidget::MenuWidget(gphoto2::_CameraWidget* cameraWidget)
		: ChoicesWidget(cameraWidget)
	{
		FILE_LOG(logINFO) << "MenuWidget constructor - widget";	
		
		if(this->getType() != CameraWidgetTypeWrapper::Menu)
		{
			throw exceptions::InvalidWidgetType("The widget type must be a Menu Widget");
		}
	}
}

