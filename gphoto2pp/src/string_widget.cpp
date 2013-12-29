#include "string_widget.hpp"

#include "exceptions.hpp"
#include "camera_widget_type_wrapper.hpp"

#include "log.h"

namespace gphoto2pp
{

	StringWidget::StringWidget(gphoto2::_CameraWidget* cameraWidget)
		: ValueWidgetBase(cameraWidget)
	{
		FILE_LOG(logINFO) << "StringWidget constructor - widget";
	}

	std::string StringWidget::getValue() const
	{
		void* temp;
			
		temp = this->getValueDefault();
		
		return std::string(static_cast<char*>(temp));
	}
	
	void StringWidget::setValue(const std::string& value)
	{
		const void* tempValue = static_cast<const void*>(value.c_str());
		
		this->setValueDefault(tempValue);
	}

}

