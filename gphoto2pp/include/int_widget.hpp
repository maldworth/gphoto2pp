#ifndef INTWIDGET_HPP
#define INTWIDGET_HPP

#include "value_widget_base.hpp"

namespace gphoto2pp
{
	/**
	 * \class IntWidget
	 * A class representing gphoto2 widgets which have a value that is meaningfully represented by an int
	 */
	class IntWidget: public ValueWidgetBase<int>
	{
	friend class NonValueWidget;

	public:
		/**
		 * \brief Gets the widget's value in terms of int
		 * \return the widget's int value
		 * \note Direct wrapper for <tt>gp_widget_get_value(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int getValue() const override;
		
		/**
		 * \brief Sets the widget's value in terms of int
		 * \param[in]	value	to set the widget to
		 * \note Direct wrapper for <tt>gp_widget_set_value(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setValue(const int& value) override;
		
	protected:
		IntWidget(gphoto2::_CameraWidget* cameraWidget);
	};

}

#endif // INTWIDGET_HPP
