/** \file 
 * \author Copyright (c) 2013 maldworth <https://github.com/maldworth>
 *
 * \note
 * This file is part of gphoto2pp
 * 
 * \note
 * gphoto2pp is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * \note
 * gphoto2pp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * \note
 * You should have received a copy of the GNU Lesser General Public
 * License along with gphoto2pp.
 * If not, see http://www.gnu.org/licenses
 */

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
		std::vector<std::string> getAllWidgetsNames(NonValueWidget const & parentWidget, bool showFullName = false); // Need to ask Marcus if these names are unique? So far they appear to be (I've not encountered two leaf nodes with the same name

		/**
		 * \brief Traverses the widget tree (depth first pre-order) and returns all widgets matching the provided type
		 * \param[in]	parentWidget	is the starting node, and this will only traverse children of this node (it will not visit ancestors, if present)
		 * \param[in]	filterByWidgetType	will only show results that match this widget type
		 * \param[in]	showFullName	indicates whether to show the full widget path (eg... true = /main/imgsettings/iso vs. false = iso)
		 */
		std::vector<std::string> getAllWidgetsNamesOfType(NonValueWidget const & parentWidget, CameraWidgetTypeWrapper const & filterByWidgetType, bool showFullName = false);
	}
}

#endif // HELPERWIDGETS_HPP
