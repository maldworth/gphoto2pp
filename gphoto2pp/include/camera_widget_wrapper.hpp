#ifndef CAMERAWIDGETWRAPPER_HPP
#define CAMERAWIDGETWRAPPER_HPP

#include <string>

namespace gphoto2
{
	// Forward declare the gphoto2 struct
	struct _CameraWidget;
}

namespace gphoto2pp
{
	enum class CameraWidgetTypeWrapper : int;
	
	/**
	 * \class CameraWidgetWrapper
	 * A wrapper around the gphoto2 CameraWidget struct.
	 */
	class CameraWidgetWrapper
	{
	public:
		virtual ~CameraWidgetWrapper();
		
		// Move constructor and move assignment
		CameraWidgetWrapper(CameraWidgetWrapper&& other);
		CameraWidgetWrapper& operator=(CameraWidgetWrapper&& other);
		
		// Copy constructor and copy assignment
		CameraWidgetWrapper(const CameraWidgetWrapper& other);
		CameraWidgetWrapper& operator=( const CameraWidgetWrapper& other);
		
		/**
		 * \brief Gets the raw resource
		 * RAII indicates we still should allow our users access to the RAW Resource and it is applicable.
		 * \return the gphoto2 CameraWidget struct
		 */
		gphoto2::_CameraWidget* getPtr() const;
		
		/**
		 * \brief Gets the widget's name
		 * \return the widget name
		 * \note Direct wrapper for gp_widget_get_name(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string getName() const;
		
		/**
		 * \brief Gets the widget's type
		 * \return the widget type
		 * \note Direct wrapper for gp_widget_get_type(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		CameraWidgetTypeWrapper getType() const;
		
		/**
		 * \brief Gets the widget's label
		 * \return the widget label
		 * \note Direct wrapper for gp_widget_get_label(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string getLabel() const;
		
		/**
		 * \brief Gets the widget's info
		 * \return the widget info
		 * \note Direct wrapper for gp_widget_get_info(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string getInfo() const;
		
		/**
		 * \brief Gets the widget's unique id
		 * \return the widget's id
		 * \note Direct wrapper for gp_widget_get_id(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int getId() const;
		
		/**
		 * \brief Gets the widget's Root.
		 * This is likely going to be the Window Widget (same widget from ICameraWrapper::getConfig()).
		 * \return the root widget
		 * \note Direct wrapper for gp_widget_get_root(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		CameraWidgetWrapper getRoot() const;
		
		/**
		 * \brief Gets the widget's parent.
		 * This is the immediate parent of the widget. If the current widget is a leaf node, then this is likely a Section Widget, but could also be a Window Widget.
		 * \return the parent widget
		 * \note Direct wrapper for gp_widget_get_parent(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		CameraWidgetWrapper getParent() const;
		
	protected:
		CameraWidgetWrapper(gphoto2::_CameraWidget* cameraWidget);
		
		/**
		 * \brief Adds a reference count to the internal gphoto2::CameraWidget struct
		 * \note Direct wrapper for gp_widget_ref(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void ref();
		
		/**
		 * \brief Subtracts a reference count to the internal gphoto2::CameraWidget struct
		 * \note Direct wrapper for gp_widget_unref(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void unref();
		
		/**
		 * \brief Gets the unwrapped CameraWidget struct pointer of the root
		 * \note Direct wrapper for gp_widget_get_root(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		gphoto2::_CameraWidget* getRootDefault() const;
		
		/**
		 * \brief Gets the unwrapped CameraWidget struct pointer of the parent
		 * \note Direct wrapper for gp_widget_get_root(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		gphoto2::_CameraWidget* getParentDefault() const;
		
		/**
		 * \brief Gets the value of the current widget
		 * This is protected because we don't want api users having to guess the conversions from void* to a more user friendly data structure.
		 * \note Direct wrapper for gp_widget_get_value(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void* getValueDefault() const;
		
		/**
		 * \brief Sets the value of the current widget
		 * This is protected because we don't want api users having to use an unfriendly void*.
		 * \param[in]	value	to set the widget to
		 * \note Direct wrapper for gp_widget_set_value(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setValueDefault(const void* value);
		
		gphoto2::_CameraWidget* m_cameraWidget = nullptr;
	};

}

#endif // CAMERAWIDGETWRAPPER_HPP
