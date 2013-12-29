#ifndef GPPORTINFOLISTWRAPPER_HPP
#define GPPORTINFOLISTWRAPPER_HPP

#include <string>

namespace gphoto2
{
	struct _GPPortInfoList;
}

namespace gphoto2pp
{
	
	/**
	 * \class GPPortInfoListWrapper
	 * A wrapper around the gphoto2 GPPortInfoList struct.
	 */
	class GPPortInfoListWrapper
	{
	public:
		GPPortInfoListWrapper();
		~GPPortInfoListWrapper();
		
		gphoto2::_GPPortInfoList* getPtr() const;
		
		/**
		 * \brief Finds the list index of the specified port path
		 * \return the port index
		 * \note Direct wrapper for <tt>gp_port_info_list_lookup_path(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int lookupPath(const std::string& path) const;
		
		/**
		 * \brief Finds the list index of the specified model name
		 * \return the port index
		 * \note Direct wrapper for <tt>gp_port_info_list_lookup_name(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int lookupName(const std::string& name) const;
		
		/**
		 * \brief Gets the number of items in the list
		 * \return the count of items
		 * \note Direct wrapper for <tt>gp_port_info_list_count(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int count() const;
		
	private:
		gphoto2::_GPPortInfoList* m_portInfoList;
	};
}

#endif // GPPORTINFOLISTWRAPPER_HPP
