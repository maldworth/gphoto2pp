#ifndef MENUWIDGET_HPP
#define MENUWIDGET_HPP

#include "choices_widget.hpp"

namespace gphoto2pp
{
	/**
	 * \class MenuWidget
	 * A class representing gphoto2 widgets which are of the widget type GPhoto2pp::CameraWidgetTypeWrapper::Menu
	 */
	class MenuWidget: public ChoicesWidget
	{
	friend class NonValueWidget;
	
	protected:
		MenuWidget(gphoto2::_CameraWidget* cameraWidget);
	};

}

#endif // MENUWIDGET_HPP
