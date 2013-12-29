#include "float_widget.hpp"

#include "helper_widgets.hpp"
#include "exceptions.hpp"
#include "camera_widget_type_wrapper.hpp"

#include "log.h"

namespace gphoto2pp
{

	FloatWidget::FloatWidget(gphoto2::_CameraWidget* cameraWidget)
		: ValueWidgetBase(cameraWidget)
	{
		FILE_LOG(logINFO) << "FloatWidget constructor - widget";	
	}
	
	float FloatWidget::getValue() const
	{
		void* temp = nullptr;
		
		temp = this->getValueDefault();
		
		return *reinterpret_cast<float*>(&temp);
	}
	void FloatWidget::setValue(const float& value)
	{
		void* temp = nullptr;
		
		temp = const_cast<float*>(&value);
		
		this->setValueDefault(temp);
	}
}

