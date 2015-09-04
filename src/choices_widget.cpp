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

#include <gphoto2pp/choices_widget.hpp>

#include <gphoto2pp/helper_gphoto2.hpp>
#include <gphoto2pp/camera_widget_type_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>

#include <algorithm>
#include <sstream>

namespace gphoto2
{
#include <gphoto2/gphoto2-widget.h>
}

namespace gphoto2pp
{

	ChoicesWidget::ChoicesWidget(gphoto2::_CameraWidget* cameraWidget)
		: StringWidget{cameraWidget}
	{
		switch(this->getType())
		{
			case CameraWidgetTypeWrapper::Menu:
			case CameraWidgetTypeWrapper::Radio:
				break;
			default:
				throw exceptions::InvalidWidgetType("A Choice Widget can only be of type Menu or Radio");
				break;
		}
	}
	
	int ChoicesWidget::countChoices() const
	{
		return gphoto2pp::checkResponse(gphoto2::gp_widget_count_choices(m_cameraWidget),"gp_widget_count_choices");
	}

	std::vector<std::string> ChoicesWidget::getChoices() const
	{
		int choiceCount = countChoices();
		
		std::vector<std::string> choices(choiceCount);
		for(int i = 0; i < choiceCount; ++i)
		{
			choices[i] = choiceToString(i);
		}
		
		return choices;
	}
	
	int ChoicesWidget::getChoice() const
	{
		// Gets all the choices in the structure
		auto choices = getChoices();
		
		// Finds the iterator over the choice that matches the currently set one
		auto const item = std::find(std::begin(choices), std::end(choices), this->getValue());
		
		// If the index is at the end
		if(item == std::end(choices))
		{
			throw exceptions::ValueOutOfLimits("For some strange reason, the current value set on the camera didn't match to a value from the choices");
		}
		
		return std::distance(std::begin(choices), item);
	}
	
	void ChoicesWidget::setChoice(int index)
	{
		if (index >= countChoices())
		{
			throw exceptions::IndexOutOfRange("You are trying to set a choice index which is greater than the maximum choice index.");
		}
		
		this->setValue(choiceToString(index));
	}
	
	std::string ChoicesWidget::choiceToString(int index) const
	{
		if (index >= countChoices())
		{
			throw exceptions::IndexOutOfRange("You are trying to get a choice index which is greater than the maximum choice index.");
		}
		
		char const * temp = nullptr;
		
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_choice(m_cameraWidget, index, &temp),"gp_widget_get_choice");
		
		return std::string(temp);
	}
	
	std::string ChoicesWidget::choicesToString(std::string&& separator /* = " " */) const
	{
		// We don't allow an empty separator
		if(separator.empty())
		{
			throw exceptions::ArgumentException("You are not allowed to have an empty separator");
		}
		std::stringstream temp;
		for(auto choice : getChoices())
		{
			temp << "\"" << choice << "\"" << separator;
		}
		
		return temp.str().substr(0, temp.str().length() - separator.length()); // erases the last separator
	}

}

