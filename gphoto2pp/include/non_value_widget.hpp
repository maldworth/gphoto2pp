#ifndef NONVALUEWIDGET_HPP
#define NONVALUEWIDGET_HPP

#include "camera_widget_wrapper.hpp"

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
		T getChildByName(const std::string& name) const
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
		T getChildByLabel(const std::string& label) const
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
		gphoto2::_CameraWidget* getChildByNameWrapper(const std::string& name) const;
		
		/**
		 * \brief Gets the child widget by the label passed in and returns the widget type that the user expects.
		 * \tparam a type that inherits from CameraWidgetWrapper
		 * \param[in]	label	of the child widget to get
		 * \return the widget
		 * \note Direct wrapper for <tt>gp_widget_get_child_by_label(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		gphoto2::_CameraWidget* getChildByLabelWrapper(const std::string& label) const;
		
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
