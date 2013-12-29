#include "date_widget.hpp"

#include "exceptions.hpp"
#include "camera_widget_type_wrapper.hpp"

#include "log.h"

namespace gphoto2pp
{

	DateWidget::DateWidget(gphoto2::_CameraWidget* cameraWidget)
		: ValueWidgetBase(cameraWidget)
	{
		FILE_LOG(logINFO) << "DateWidget constructor - widget";	
		
		if(this->getType() != CameraWidgetTypeWrapper::Date)
		{
			throw exceptions::InvalidWidgetType("The widget type must be a Date Widget");
		}
	}
	
	std::time_t DateWidget::getValue() const
	{
		void* temp = this->getValueDefault();
		
		int temp2 = *reinterpret_cast<int*>(&temp);
		
		return static_cast<std::time_t>(temp2);
	}
	
	void DateWidget::setValue(const std::time_t& datetime)
	{
		const int temp = static_cast<int>(datetime);
		
		void* temp2 = const_cast<int*>(&temp);
		
		this->setValueDefault(temp2);
		
		//this->setValue(static_cast<int>(datetime));
	}
}

