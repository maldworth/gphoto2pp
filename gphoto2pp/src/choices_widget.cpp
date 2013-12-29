#include "choices_widget.hpp"

#include "helper_gphoto2.hpp"
#include "camera_widget_type_wrapper.hpp"
#include "exceptions.hpp"

#include "log.h"

#include <algorithm>

namespace gphoto2
{
#include <gphoto2/gphoto2-widget.h>
}

namespace gphoto2pp
{

	ChoicesWidget::ChoicesWidget(gphoto2::_CameraWidget* cameraWidget)
		: StringWidget(cameraWidget)
	{
		FILE_LOG(logINFO) << "ChoicesWidget constructor - widget";	
		
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
		const auto item = std::find(std::begin(choices), std::end(choices), this->getValue());
		
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
		
		const char* temp = nullptr;
		
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

