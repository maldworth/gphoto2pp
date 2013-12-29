#ifndef SECTIONWIDGET_HPP
#define SECTIONWIDGET_HPP

#include "non_value_widget.hpp"

namespace gphoto2pp
{
	/**
	 * \class SectionWidget
	 * A class representing gphoto2 widgets which are of the widget type GPhoto2pp::CameraWidgetTypeWrapper::Section
	 */
	class SectionWidget: public NonValueWidget
	{
	protected:
		SectionWidget(gphoto2::_CameraWidget* cameraWidget);
	};

}

#endif // SECTIONWIDGET_HPP
