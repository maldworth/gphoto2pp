#ifndef STRINGWIDGET_HPP
#define STRINGWIDGET_HPP

#include "value_widget_base.hpp"

#include <string>

namespace gphoto2pp
{
	/**
	 * \class StringWidget
	 * A class representing gphoto2 widgets which have a value that is meaningfully represented by a string
	 */
	class StringWidget : public ValueWidgetBase<std::string>
	{
	friend class NonValueWidget;

	public:
		/**
		 * \brief Gets the widget's value in terms of std::string
		 * \return the widget's std::string value
		 * \note Direct wrapper for gp_widget_get_value(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string getValue() const override;
		
		/**
		 * \brief Sets the widget's value in terms of std::string
		 * \param[in]	value	to set for the widget
		 * \note Direct wrapper for <tt>gp_widget_set_value(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setValue(const std::string& value) override;
		
	protected:
		StringWidget(gphoto2::_CameraWidget* cameraWidget);
	};

}

#endif // STRINGWIDGET_HPP
