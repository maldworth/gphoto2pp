/** \file
 * \author Copyright (c) 2026 Vitor Fonseca <https://github.com/VSF1>
 *
 * \note This file is part of gphoto2pp.
 */

#include <gphoto2pp/button_widget.hpp>

#include <gphoto2pp/camera_widget_type_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>

namespace gphoto2pp
{
	ButtonWidget::ButtonWidget(gphoto2::_CameraWidget *cameraWidget)
		: CameraWidgetWrapper{cameraWidget}
	{
		if (this->getType() != CameraWidgetTypeWrapper::Button)
		{
			throw exceptions::InvalidWidgetType(
				"A ButtonWidget can only wrap a button widget");
		}
	}
}
