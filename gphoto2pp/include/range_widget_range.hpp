#ifndef RANGEWIDGETRANGE_HPP
#define RANGEWIDGETRANGE_HPP

#include <string>

namespace gphoto2pp
{
	/** struct RangeWidgetRange
	 * Provides a POD for the RangeWidget's getRange method.
	 */
	struct RangeWidgetRange
	{
		float Min;
		float Max;
		float Step;
	};
}

#endif // RANGEWIDGETRANGE_HPP
