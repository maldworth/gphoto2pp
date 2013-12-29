#include "text_widget.hpp"

#include "exceptions.hpp"
#include "camera_widget_type_wrapper.hpp"

#include "log.h"

namespace gphoto2pp
{

	TextWidget::TextWidget(gphoto2::_CameraWidget* cameraWidget)
		: StringWidget(cameraWidget)
	{
		FILE_LOG(logINFO) << "TextWidget constructor - widget";	
		
		if(getType() != CameraWidgetTypeWrapper::Text)
		{
			throw exceptions::InvalidWidgetType("The widget type must be a Text Widget");
		}
	}
}

