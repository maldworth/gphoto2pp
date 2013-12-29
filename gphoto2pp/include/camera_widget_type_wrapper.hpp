#ifndef WIDGETTYPE_HPP
#define WIDGETTYPE_HPP

namespace gphoto2pp
{
	/**
	 * Provides a C++ enum for the gphoto2 CameraWidgetType enum
	 */
	enum class CameraWidgetTypeWrapper : int
	{
		Window = 0,		///< Maps to GP_WIDGET_WINDOW
		Section = 1,	///< Maps to GP_WIDGET_SECTION
		Text = 2,		///< Maps to GP_WIDGET_TEXT
		Range = 3,		///< Maps to GP_WIDGET_RANGE
		Toggle = 4,		///< Maps to GP_WIDGET_TOGGLE
		Radio = 5,		///< Maps to GP_WIDGET_RADIO
		Menu = 6,		///< Maps to GP_WIDGET_MENU
		Button = 7,		///< Maps to GP_WIDGET_BUTTON
		Date = 8,		///< Maps to GP_WIDGET_DATE
	};
}


#endif // WIDGETTYPE_HPP
