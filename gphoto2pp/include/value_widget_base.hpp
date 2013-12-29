#ifndef VALUEWIDGETBASE_HPP
#define VALUEWIDGETBASE_HPP

#include "camera_widget_wrapper.hpp"

namespace gphoto2pp
{
	/**
	 * \class ValueWidgetBase
	 * A templated class which must be inherited by any leaf widget type. That means any widget type that is not a Section or a Widget. That way their specific widget implementations can define meaningful Get and Set methods.
	 * \tparam the meaningful value type for the widget
	 */
	template<typename T>
	class ValueWidgetBase : public CameraWidgetWrapper
	{

	public:
		/**
		 * \brief Gets the widget's value in terms of T
		 * \return the widget's value
		 */
		virtual T getValue() const = 0;
		
		/**
		 * \brief Sets the widget's value in terms of T
		 * \param[in]	value	to set for the widget
		 * \return the widget's T value
		 */
		virtual void setValue(const T& value) = 0;
		
	protected:
		ValueWidgetBase(gphoto2::_CameraWidget* cameraWidget)
		: CameraWidgetWrapper(cameraWidget)
		{
			// Empty Constructor
		}
	};

}

#endif // VALUEWIDGETBASE_HPP
