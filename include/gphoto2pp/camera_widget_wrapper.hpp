/** 
 * \file camera_widget_wrapper.hpp
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

#ifndef CAMERAWIDGETWRAPPER_HPP
#define CAMERAWIDGETWRAPPER_HPP

#include <string>
#include <cstddef>
#include <memory>

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
	 * This class provides a RAII wrapper around the gphoto2 CameraWidget struct.
     * In the gphoto2 the camera's abilities are represented by an N-ary tree.
	 * 
	 * Gphoto2 has 9 node types. 7 of them compose the leaf nodes of the tree
     * and contian read, or read/write values. 2 of them (window and section)
     * are non value widgets, and represent the internal nodes with 1 or more
     * children.
	 * 
	 * A complete camera abilities tree has a Window root, typically with
     * Section children. A single-setting query can return a standalone widget.
	 * 
	 * Please review the above graphical tree to look at the inheritance
     * hierarchy of the widget types to understand the underlying value they
     * each represent.
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
	friend class CameraWrapper;
	friend class NonValueWidget;
	public:
		virtual ~CameraWidgetWrapper();

		/**
		 * \brief Creates a standalone widget of the requested type.
		 * \param[in] type libgphoto2 widget type
		 * \param[in] label initial label/name, without embedded NUL bytes
		 * \return Owning polymorphic widget
		 * \throw GPhoto2pp::exceptions::ArgumentException for embedded NUL
		 * \throw GPhoto2pp::exceptions::InvalidWidgetType for unsupported types
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		static std::unique_ptr<CameraWidgetWrapper> create(
			CameraWidgetTypeWrapper type, std::string const & label);
		
		// Move constructor and move assignment
		CameraWidgetWrapper(CameraWidgetWrapper&& other);
		CameraWidgetWrapper& operator=(CameraWidgetWrapper&& other);
		
		// Copy constructor and copy assignment
		CameraWidgetWrapper(CameraWidgetWrapper const & other);
		CameraWidgetWrapper& operator=(CameraWidgetWrapper const & other);
		
		/**
		 * \brief Gets the raw resource
		 * RAII indicates we still should allow our users access to the RAW
         * Resource and it is applicable.
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
		 * \brief Gets the widget name, enforcing a maximum byte length.
		 * \param[in]	maxLength	maximum accepted length
		 * \return the widget name
		 * \throw std::length_error if the name exceeds maxLength
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string getName(std::size_t maxLength) const;

		/**
		 * \brief Sets the widget name.
		 * \param[in] name new name, without embedded NUL bytes
		 * \throw GPhoto2pp::exceptions::ArgumentException for embedded NUL
		 * or a name exceeding libgphoto2's 255-byte field
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setName(std::string const & name);
		
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
		 * \brief Gets the widget label, enforcing a maximum byte length.
		 * \param[in]	maxLength	maximum accepted length
		 * \return the widget label
		 * \throw std::length_error if the label exceeds maxLength
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string getLabel(std::size_t maxLength) const;
		
		/**
		 * \brief Gets the widget's info
		 * \return the widget info
		 * \note Direct wrapper for gp_widget_get_info(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string getInfo() const;

		/**
		 * \brief Sets the widget information text.
		 * \param[in] info new info text, without embedded NUL bytes
		 * \throw GPhoto2pp::exceptions::ArgumentException for embedded NUL
		 * or text exceeding libgphoto2's 1023-byte field
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setInfo(std::string const & info);
		
		/**
		 * \brief Gets the widget's unique id
		 * \return the widget's id
		 * \note Direct wrapper for gp_widget_get_id(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int getId() const;

		/**
		 * \brief Gets the widget's read-only flag.
		 * \return the value returned by libgphoto2, normally 0 or 1
		 * \note Direct wrapper for gp_widget_get_readonly(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int getReadOnly() const;

		/**
		 * \brief Sets the widget read-only flag.
		 * \param[in] readOnly 0 or 1
		 * \throw GPhoto2pp::exceptions::ArgumentException for another value
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setReadOnly(int readOnly);

		/**
		 * \brief Gets whether the widget reports a changed state.
		 * \return true if libgphoto2 reports changed, false otherwise
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		bool isChanged() const;

		/**
		 * \brief Sets the widget changed state.
		 * \param[in] changed 0 or 1
		 * \throw GPhoto2pp::exceptions::ArgumentException for another value
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setChanged(int changed);
		
		/**
		 * \brief Gets the widget's Root.
		 * \return the root widget, which may be this widget
		 * \note Direct wrapper for gp_widget_get_root(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		CameraWidgetWrapper getRoot() const;
		
		/**
		 * \brief Gets the widget's parent.
		 * This is the immediate parent of the widget. If the current widget is
         * a leaf node, then this is likely a Section Widget, but could also be
         * a Window Widget.
		 * \return the parent widget
		 * \note Direct wrapper for gp_widget_get_parent(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		CameraWidgetWrapper getParent() const;
		
	protected:
		CameraWidgetWrapper(gphoto2::_CameraWidget* cameraWidget);
		void releaseOwnership();
		
		/**
		 * \brief Adds a reference count to the internal gphoto2::CameraWidget 
         * struct
		 * \note Direct wrapper for gp_widget_ref(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void ref();
		
		/**
		 * \brief Subtracts a reference count to the internal 
         * gphoto2::CameraWidget struct
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
