/** \file 
 * \author Copyright (c) 2013 maldworth <https://github.com/maldworth>
 *
 * \note
 * This file is part of gphoto2pp
 * 
 * \note
 * gphoto2pp is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * \note
 * gphoto2pp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * \note
 * You should have received a copy of the GNU Lesser General Public
 * License along with gphoto2pp.
 * If not, see http://www.gnu.org/licenses
 */

#ifndef NONVALUEWIDGET_HPP
#define NONVALUEWIDGET_HPP

#include <gphoto2pp/camera_widget_wrapper.hpp>

namespace gphoto2pp
{
	/**
	 * \class NonValueWidget
	 * A class representing gphoto2 widgets which are not leaf nodes (have children), which is a Window or Section widget
	 */
	class NonValueWidget : public CameraWidgetWrapper
	{
	public:
		/**
		 * \brief Gets the number of children to this current widget
		 * \return the number of children
		 * \note Direct wrapper for <tt>gp_widget_count_children(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int countChildren() const;
		
		/**
		 * \brief Gets the child widget that matches the name.
		 * \tparam T type that inherits from CameraWidgetWrapper
		 * \param[in]	name	of the child widget to get
		 * \return the widget
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		template<typename T>
		T getChildByName(std::string const & name) const
		{
			auto cameraWidget = getChildByNameWrapper(name);
			return T(cameraWidget);
		}
		
		/**
		 * \brief Gets the child widget that matches the label.
		 * \tparam T type that inherits from CameraWidgetWrapper
		 * \param[in]	label	of the child widget to get
		 * \return the widget
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		template<typename T>
		T getChildByLabel(std::string const & label) const
		{
			auto cameraWidget = getChildByLabelWrapper(label);
			return T(cameraWidget);
		}
		
		/**
		 * \brief Gets the child widget at the specified index.
		 * \tparam a type that inherits from CameraWidgetWrapper
		 * \param[in]	index	of the child widget to get
		 * \return the widget
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		template<typename T>
		T getChild(int index) const
		{
			auto cameraWidget = getChildWrapper(index);
			return T(cameraWidget);
		}
		
		/**
		 * \brief Gets the child widget that matches the unique id.
		 * \tparam a type that inherits from CameraWidgetWrapper
		 * \param[in]	id	of the child widget to get
		 * \return the widget
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		template<typename T>
		T getChildById(int id) const
		{
			auto cameraWidget = getChildByIdWrapper(id);
			return T(cameraWidget);
		}
		
	protected:
		NonValueWidget(gphoto2::_CameraWidget* cameraWidget);
		
		/**
		 * \brief Gets the child widget by the name passed in and returns the widget type that the user expects.
		 * \tparam a type that inherits from CameraWidgetWrapper
		 * \param[in]	name	of the child widget to get
		 * \return the widget
		 * \note Direct wrapper for <tt>gp_widget_get_child_by_name(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		gphoto2::_CameraWidget* getChildByNameWrapper(std::string const & name) const;
		
		/**
		 * \brief Gets the child widget by the label passed in and returns the widget type that the user expects.
		 * \tparam a type that inherits from CameraWidgetWrapper
		 * \param[in]	label	of the child widget to get
		 * \return the widget
		 * \note Direct wrapper for <tt>gp_widget_get_child_by_label(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		gphoto2::_CameraWidget* getChildByLabelWrapper(std::string const & label) const;
		
		/**
		 * \brief Gets the child widget at the specified index and returns the widget type that the user expects.
		 * \tparam a type that inherits from CameraWidgetWrapper
		 * \param[in]	index	of the child widget to get
		 * \return the widget
		 * \note Direct wrapper for <tt>gp_widget_get_child(...)</tt>
		 * \throw GPhoto2pp::exceptions::IndexOutOfRange
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		gphoto2::_CameraWidget* getChildWrapper(int index) const;
		
		/**
		 * \brief Gets the child widget that matches the unique id.
		 * \tparam a type that inherits from CameraWidgetWrapper
		 * \param[in]	id	of the child widget to get
		 * \return the widget
		 * \note Direct wrapper for <tt>gp_widget_get_child_by_id(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		gphoto2::_CameraWidget* getChildByIdWrapper(int id) const;
	};

}

#endif // NONVALUEWIDGET_HPP
