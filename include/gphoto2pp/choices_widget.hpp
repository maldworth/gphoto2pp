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

#ifndef CHOICESWIDGET_HPP
#define CHOICESWIDGET_HPP

#include <gphoto2pp/string_widget.hpp>

#include <vector>

namespace gphoto2pp
{
	/**
	 * \class ChoicesWidget
	 * A class representing gphoto2 widgets which are of the widget type GPhoto2pp::CameraWidgetTypeWrapper::Menu or GPhoto2pp::CameraWidgetTypeWrapper::Radio
	 */
	class ChoicesWidget : public StringWidget
	{
	friend class NonValueWidget;

	public:
		/**
		 * \brief Counts the number of choices/options to set for this widget
		 * \return the number of choices
		 * \note Direct wrapper for <tt>gp_widget_count_choices(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int countChoices() const;
		
		/**
		 * \brief Gets the currently set choice at the specified index
		 * \return the choices index
		 * \note Direct wrapper for <tt>gp_widget_get_choice(...)</tt>
		 * \throw GPhoto2pp::exceptions::IndexOutOfRange if the index is greater than countChoices of choices
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int getChoice() const;
		
		/**
		 * \brief Sets the choice at the specified index
		 * \param[in]	index	of the choice to set
		 * \note Helper which calls getChoice with the provided index and then setValue with the response
		 * \throw GPhoto2pp::exceptions::IndexOutOfRange if the index is greater than countChoices
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setChoice(int index);
		
		/**
		 * \brief Gets all the possible choices
		 * \return the choices
		 * \note Helper which iterates through all choices compiling them into a vector
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::vector<std::string> getChoices() const;
		
		/**
		 * \brief Gets the string representation of the choice at the specified index
		 * \param[in]	index	of the choice to get
		 * \return the choices value
		 * \note Direct wrapper for <tt>gp_widget_get_choice(...)</tt>
		 * \throw GPhoto2pp::exceptions::IndexOutOfRange if the index is greater than countChoices of choices
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string choiceToString(int index) const;
		
		/**
		 * \brief Formats the choices into a string with optional separator
		 * \param[in]	separator	used to insert inbetween all the choices for concatenation
		 * \return the string of choices
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string choicesToString(std::string&& separator = " ") const;
		
	protected:
		ChoicesWidget(gphoto2::_CameraWidget* cameraWidget);
	};
}

#endif // CHOICESWIDGET_HPP
