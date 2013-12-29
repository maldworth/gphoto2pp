#ifndef FLOATWIDGET_HPP
#define FLOATWIDGET_HPP

#include "value_widget_base.hpp"

namespace gphoto2pp
{
	/**
	 * \class FloatWidget
	 * A class representing gphoto2 widgets which have a value that is meaningfully represented by a float
	 */
	class FloatWidget: public ValueWidgetBase<float>
	{
	friend class NonValueWidget;

	public:
		/**
		 * \brief Gets the widget's value in terms of float
		 * \return the widget's float value
		 * \note Direct wrapper for gp_widget_get_value(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		float getValue() const override;
		
		/**
		 * \brief Sets the widget's value in terms of float
		 * \param[in]	value	to set for widget
		 * \note Direct wrapper for gp_widget_set_value(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setValue(const float& value) override;
		
	protected:
		FloatWidget(gphoto2::_CameraWidget* cameraWidget);
	};

}

#endif // FLOATWIDGET_HPP
