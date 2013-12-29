#ifndef TEXTWIDGET_HPP
#define TEXTWIDGET_HPP

#include "string_widget.hpp"

namespace gphoto2pp
{
	/**
	 * \class TextWidget
	 * A class representing gphoto2 widgets which are of the widget type GPhoto2pp::CameraWidgetTypeWrapper::Text
	 */
	class TextWidget: public StringWidget
	{
	friend class NonValueWidget;
	
	protected:
		TextWidget(gphoto2::_CameraWidget* cameraWidget);
	};

}

#endif // TEXTWIDGET_HPP
