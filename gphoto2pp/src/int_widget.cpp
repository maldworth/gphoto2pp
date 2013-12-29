#include "int_widget.hpp"

#include "helper_widgets.hpp"
#include "exceptions.hpp"
#include "camera_widget_type_wrapper.hpp"

#include "log.h"

namespace gphoto2pp
{

	IntWidget::IntWidget(gphoto2::_CameraWidget* cameraWidget)
		: ValueWidgetBase(cameraWidget)
	{
		FILE_LOG(logINFO) << "IntWidget constructor";	
	}
	
	int IntWidget::getValue() const
	{
		void* temp = nullptr;
		
		temp = this->getValueDefault();
		
		return *reinterpret_cast<int*>(&temp);
		
		//return *static_cast<int*>(&temp);
	}
	void IntWidget::setValue(const int& value)
	{
		void* temp = nullptr;
		
		temp = const_cast<int*>(&value);
		
		this->setValueDefault(temp);
	}

}

