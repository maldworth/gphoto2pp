#ifndef TOGGLEWIDGET_HPP
#define TOGGLEWIDGET_HPP

#include "int_widget.hpp"

namespace gphoto2pp
{
	/**
	 * \class ToggleWidget
	 * A class representing gphoto2 widgets which are of the widget type GPhoto2pp::CameraWidgetTypeWrapper::Toggle
	 */
	class ToggleWidget: public IntWidget
	{
	friend class NonValueWidget;
	
	protected:
		ToggleWidget(gphoto2::_CameraWidget* cameraWidget);
	};
}

#endif // TOGGLEWIDGET_HPP
