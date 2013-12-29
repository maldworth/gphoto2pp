#ifndef DATEWIDGET_HPP
#define DATEWIDGET_HPP

#include "value_widget_base.hpp"

#include <ctime>

namespace gphoto2pp
{
	/**
	 * \class DateWidget
	 * A class representing gphoto2 widgets which are of the widget type GPhoto2pp::CameraWidgetTypeWrapper::Date
	 */
	class DateWidget: public ValueWidgetBase<std::time_t>
	{
	friend class NonValueWidget;

	public:
		/**
		 * \brief Gets the widget's value in terms of std::time_t
		 * \return the widget's std::time_t value
		 * \note Direct wrapper for <tt>gp_widget_get_value(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::time_t getValue() const override;
		
		/**
		 * \brief Sets the widget's value in terms of std::time_t
		 * \param[in]	date	to set for the widget
		 * \note Direct wrapper for gp_widget_set_value(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setValue(const std::time_t& date) override;

	protected:
		DateWidget(gphoto2::_CameraWidget* cameraWidget);
	};
}

#endif // DATEWIDGET_HPP
