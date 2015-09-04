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

#ifndef CAMERAWIDGETWRAPPER_HPP
#define CAMERAWIDGETWRAPPER_HPP

#include <string>

namespace gphoto2
{
	// Forward declare the gphoto2 struct
	struct _CameraWidget;
}

namespace gphoto2pp
{
	enum class CameraWidgetTypeWrapper : int;
	
	/**
	 * \class CameraWidgetWrapper
	 * This class provides a RAII wrapper around the gphoto2 CameraWidget struct. In the gphoto2 the camera's abilities are represented by an N-ary tree.
	 * 
	 * Gphoto2 has 9 node types. 7 of them compose the leaf nodes of the tree and contian read, or read/write values. 2 of them (window and section) are non value widgets, and represent the internal nodes with 1 or more children.
	 * 
	 * Every camera abilities tree has Only 1 root node, which is always of type *Window*. Then this can have n children, typically they are *Section* widgets.
	 * 
	 * Please review the above graphical tree to look at the inheritance hierarchy of the widget types to understand the underlying value they each represent.
	 * 
	 * | Type    | Value  | Node Type |
	 * | ------- |:------:| --------- |
	 * | Window  | N/A    | Root      |
	 * | Section | N/A    | Non Leaf  |
	 * | Text    | string | Leaf      |
	 * | Range   | float  | Leaf      |
	 * | Toggle  | int    | Leaf      |
	 * | Radio   | string | Leaf      |
	 * | Menu    | string | Leaf      |
	 * | Button  | N/A    | Leaf      |
	 * | Date    | time_t | Leaf      |
	 * 
	 */
	class CameraWidgetWrapper
	{
	public:
		virtual ~CameraWidgetWrapper();
		
		// Move constructor and move assignment
		CameraWidgetWrapper(CameraWidgetWrapper&& other);
		CameraWidgetWrapper& operator=(CameraWidgetWrapper&& other);
		
		// Copy constructor and copy assignment
		CameraWidgetWrapper(CameraWidgetWrapper const & other);
		CameraWidgetWrapper& operator=(CameraWidgetWrapper const & other);
		
		/**
		 * \brief Gets the raw resource
		 * RAII indicates we still should allow our users access to the RAW Resource and it is applicable.
		 * \return the gphoto2 CameraWidget struct
		 */
		gphoto2::_CameraWidget* getPtr() const;
		
		/**
		 * \brief Gets the widget's name
		 * \return the widget name
		 * \note Direct wrapper for gp_widget_get_name(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string getName() const;
		
		/**
		 * \brief Gets the widget's type
		 * \return the widget type
		 * \note Direct wrapper for gp_widget_get_type(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		CameraWidgetTypeWrapper getType() const;
		
		/**
		 * \brief Gets the widget's label
		 * \return the widget label
		 * \note Direct wrapper for gp_widget_get_label(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string getLabel() const;
		
		/**
		 * \brief Gets the widget's info
		 * \return the widget info
		 * \note Direct wrapper for gp_widget_get_info(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string getInfo() const;
		
		/**
		 * \brief Gets the widget's unique id
		 * \return the widget's id
		 * \note Direct wrapper for gp_widget_get_id(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int getId() const;
		
		/**
		 * \brief Gets the widget's Root.
		 * This is likely going to be the Window Widget (same widget from ICameraWrapper::getConfig()).
		 * \return the root widget
		 * \note Direct wrapper for gp_widget_get_root(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		CameraWidgetWrapper getRoot() const;
		
		/**
		 * \brief Gets the widget's parent.
		 * This is the immediate parent of the widget. If the current widget is a leaf node, then this is likely a Section Widget, but could also be a Window Widget.
		 * \return the parent widget
		 * \note Direct wrapper for gp_widget_get_parent(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		CameraWidgetWrapper getParent() const;
		
	protected:
		CameraWidgetWrapper(gphoto2::_CameraWidget* cameraWidget);
		
		/**
		 * \brief Adds a reference count to the internal gphoto2::CameraWidget struct
		 * \note Direct wrapper for gp_widget_ref(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void ref();
		
		/**
		 * \brief Subtracts a reference count to the internal gphoto2::CameraWidget struct
		 * \note Direct wrapper for gp_widget_unref(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void unref();
		
		/**
		 * \brief Gets the unwrapped CameraWidget struct pointer of the root
		 * \note Direct wrapper for gp_widget_get_root(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		gphoto2::_CameraWidget* getRootDefault() const;
		
		/**
		 * \brief Gets the unwrapped CameraWidget struct pointer of the parent
		 * \note Direct wrapper for gp_widget_get_root(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		gphoto2::_CameraWidget* getParentDefault() const;
		
		gphoto2::_CameraWidget* m_cameraWidget = nullptr;
	};

}

#endif // CAMERAWIDGETWRAPPER_HPP
