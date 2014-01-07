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

#include "helper_widgets.hpp"

#include "log.h"

#include "exceptions.hpp"

#include "camera_widget_type_wrapper.hpp"
#include "non_value_widget.hpp"

namespace gphoto2pp
{
	namespace helper
	{
		void getWidgetSummary(const NonValueWidget& currentWidget, const std::string& parentWidgetPath, std::vector<std::string>& allWidgetNames, bool showFullName, bool onlySpecificWidgetType, const CameraWidgetTypeWrapper& filterByWidgetType)
		{
			std::string currentWidgetName(currentWidget.getName());
			
			if(showFullName)
			{
				currentWidgetName = parentWidgetPath + "/" + currentWidgetName;
			}
			
			// This means we are logging all widget types, and so we can ignore the filterByWidgetType argument
			if(onlySpecificWidgetType == false)
			{
				allWidgetNames.push_back(currentWidgetName);
			}
			else if(filterByWidgetType == currentWidget.getType())
			{
				allWidgetNames.push_back(currentWidgetName);
			}
			
			int numOfChildren = currentWidget.countChildren();
								
			for(int i = 0; i < numOfChildren; ++i)
			{
				auto childWidget = currentWidget.getChild<NonValueWidget>(i);
				getWidgetSummary(childWidget, currentWidgetName, allWidgetNames, showFullName, onlySpecificWidgetType, filterByWidgetType);
			}
		}
		
		std::vector<std::string> getAllWidgetsNames(const NonValueWidget& parentWidget, bool showFullName /* = false */)
		{
			std::vector<std::string> allWidgetNames{};
			
			getWidgetSummary(parentWidget, std::string(""), allWidgetNames, showFullName, false, CameraWidgetTypeWrapper::Window); // we could have passed in anything for the widget type because it will be ignored, we just chose Window
			
			return std::move(allWidgetNames);
			
			//~ return getAllWidgetsNamesOfType(parentWidget, CameraWidgetTypeWrapper::Unassigned, showFullName);
		}
		
		std::vector<std::string> getAllWidgetsNamesOfType(const NonValueWidget& parentWidget, const CameraWidgetTypeWrapper& filterByWidgetType, bool showFullName /* = false */)
		{
			std::vector<std::string> allWidgetNames{};
			
			getWidgetSummary(parentWidget, std::string(""), allWidgetNames, showFullName, true, filterByWidgetType);
			
			return std::move(allWidgetNames);
		}
	}
}
