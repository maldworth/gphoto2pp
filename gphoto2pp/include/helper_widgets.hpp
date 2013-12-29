#ifndef HELPERWIDGETS_HPP
#define HELPERWIDGETS_HPP

#include <string>
#include <vector>

//g++ -std=c++11 main.cpp -o main -I. -I/usr/include/boost -I../Library ../Library/logog/liblogog.a ./Debug/libGPhoto2pp.a -lgphoto2
//g++ -std=c++11 main.cpp -o main -I. -I/usr/include/boost -I../Library ../Library/logog/liblogog.a ./Debug/libGPhoto2pp.a -lgphoto2 -DLOGOG_LEVEL=0
//g++ -std=c++11 main.cpp -o main -I. -I/usr/include/boost -I../Library ./Debug/libGPhoto2pp.a -lgphoto2

namespace gphoto2pp
{
	class NonValueWidget;
	enum class CameraWidgetTypeWrapper : int;
	
	namespace helper
	{	
		/**
		 * \brief Traverses the widget tree (depth first pre-order) and returns all widgets
		 * \param[in]	parentWidget	is the starting node, and this will only traverse children of this node (it will not visit ancestors, if present)
		 * \param[in]	showFullName	indicates whether to show the full widget path (eg... true = /main/imgsettings/iso vs. false = iso)
		 * \return the compiled list of all widgets found
		 */
		std::vector<std::string> getAllWidgetsNames(const NonValueWidget& parentWidget, bool showFullName = false); // Need to ask Marcus if these names are unique? So far they appear to be (I've not encountered two leaf nodes with the same name

		/**
		 * \brief Traverses the widget tree (depth first pre-order) and returns all widgets matching the provided type
		 * \param[in]	parentWidget	is the starting node, and this will only traverse children of this node (it will not visit ancestors, if present)
		 * \param[in]	filterByWidgetType	will only show results that match this widget type
		 * \param[in]	showFullName	indicates whether to show the full widget path (eg... true = /main/imgsettings/iso vs. false = iso)
		 */
		std::vector<std::string> getAllWidgetsNamesOfType(const NonValueWidget& parentWidget, const CameraWidgetTypeWrapper& filterByWidgetType, bool showFullName = false);
	}
}

#endif // HELPERWIDGETS_HPP
