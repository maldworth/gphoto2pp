/**
 * \file button_widget.hpp
 *
 * \author Copyright (c) 2013 maldworth <https://github.com/maldworth>
 *
 * \author Copyright (c) 2026 Vitor Fonseca <https://github.com/VSF1>
 *
 * \note This file is part of gphoto2pp
 *
 * \note gphoto2pp is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * \note gphoto2pp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * \note You should have received a copy of the GNU Lesser General Public
 * License along with gphoto2pp. If not, see http://www.gnu.org/licenses
 */

#ifndef BUTTONWIDGET_HPP
#define BUTTONWIDGET_HPP

#include <gphoto2pp/camera_widget_wrapper.hpp>

namespace gphoto2pp
{
	/**
	 * \class ButtonWidget
	 * A typed wrapper for libgphoto2 button widgets. Button callbacks are
	 * invoked through @ref CameraWrapper::pressButton.
	 */
	class ButtonWidget : public CameraWidgetWrapper
	{
		friend class CameraWrapper;
		friend class NonValueWidget;
		friend class CameraWidgetWrapper;

	public:
		/**
		 * \brief Reports whether libgphoto2 assigned a button callback.
		 * \return true when gp_widget_get_value contains a non-null callback
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		bool hasCallback() const;

		/**
		 * \brief Assigns a libgphoto2-compatible callback pointer.
		 * \param[in] callback function pointer cast to const void*
		 * \throw GPhoto2pp::exceptions::NullWidget for a null callback
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setCallback(void const * callback);

	protected:
		explicit ButtonWidget(gphoto2::_CameraWidget *cameraWidget);
	};
}

#endif // BUTTONWIDGET_HPP
