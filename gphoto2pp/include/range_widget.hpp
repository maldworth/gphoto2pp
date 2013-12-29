#ifndef RANGEWIDGET_HPP
#define RANGEWIDGET_HPP

#include "float_widget.hpp"

namespace gphoto2pp
{
	struct RangeWidgetRange;
	
	/**
	 * \class RangeWidget
	 * A class representing gphoto2 widgets which are of the widget type GPhoto2pp::CameraWidgetTypeWrapper::Range
	 */
	class RangeWidget: public FloatWidget
	{
	friend class NonValueWidget;

	public:
		/**
		 * \brief Gets the widget's range.
		 * \return the widget's range
		 * \note Direct wrapper for <tt>gp_widget_get_range(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		RangeWidgetRange getRange() const;
		
		/**
		 * \brief Gets the widget's range and returns their values in a user friendly string.
		 * \return the widget's range
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string ToString() const;

	protected:
		RangeWidget(gphoto2::_CameraWidget* cameraWidget);
	};

}

#endif // RANGEWIDGET_HPP
