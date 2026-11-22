/** \file
 * \author Copyright (c) 2026 Vitor Fonseca <https://github.com/VSF1>
 *
 * \note This file is part of gphoto2pp.
 */

#include <gphoto2pp/button_widget.hpp>

#include <gphoto2pp/camera_widget_type_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/helper_gphoto2.hpp>

namespace gphoto2
{
#include <gphoto2/gphoto2-widget.h>
}

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

	bool ButtonWidget::hasCallback() const
	{
		gphoto2::CameraWidgetCallback callback = nullptr;
		gphoto2pp::checkResponse(
			gphoto2::gp_widget_get_value(m_cameraWidget, &callback),
			"gp_widget_get_value");
		return callback != nullptr;
	}

	void ButtonWidget::setCallback(void const * callback)
	{
		if (callback == nullptr)
		{
			throw exceptions::NullWidget(
				"Button widget callback must not be null");
		}
		gphoto2pp::checkResponse(
			gphoto2::gp_widget_set_value(m_cameraWidget, callback),
			"gp_widget_set_value");
	}

}
