#ifndef WINDOWWIDGET_HPP
#define WINDOWWIDGET_HPP

#include "non_value_widget.hpp"

namespace gphoto2pp
{
	/**
	 * \class WindowWidget
	 * A class representing gphoto2 widgets which are of the widget type GPhoto2pp::CameraWidgetTypeWrapper::Window
	 */
	class WindowWidget : public NonValueWidget
	{
	friend class CameraWrapper;

	protected:
		WindowWidget(gphoto2::_CameraWidget* cameraWidget);
	};
}

#endif // WINDOWWIDGET_HPP
