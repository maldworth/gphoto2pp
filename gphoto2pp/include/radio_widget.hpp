#ifndef RADIOWIDGET_HPP
#define RADIOWIDGET_HPP

#include "choices_widget.hpp"

namespace gphoto2pp
{
	/**
	 * \class RadioWidget
	 * A class representing gphoto2 widgets which are of the widget type GPhoto2pp::CameraWidgetTypeWrapper::Radio
	 */
	class RadioWidget: public ChoicesWidget
	{
	friend class NonValueWidget;
		
	protected:
		RadioWidget(gphoto2::_CameraWidget* cameraWidget);
	};

}

#endif // RADIOWIDGET_HPP
