#include <cxxtest/TestSuite.h>

#include <gphoto2pp/choices_widget.hpp>
#include <gphoto2pp/camera_widget_wrapper.hpp>
#include <gphoto2pp/camera_widget_type_wrapper.hpp>
#include <gphoto2pp/menu_widget.hpp>
#include <gphoto2pp/range_widget.hpp>
#include <gphoto2pp/text_widget.hpp>
#include <gphoto2pp/window_widget.hpp>
#include <gphoto2pp/exceptions.hpp>

namespace gphoto2 {
#include <gphoto2/gphoto2-widget.h>
}

#include <memory>
#include <stdexcept>

class TestChoicesWidget : public gphoto2pp::ChoicesWidget {
public:
    explicit TestChoicesWidget(gphoto2::_CameraWidget *widget)
        : gphoto2pp::ChoicesWidget(widget) {
    }
};

class ChoicesWidget_Bounds : public CxxTest::TestSuite {
    gphoto2::_CameraWidget *m_widget = nullptr;
    std::unique_ptr<TestChoicesWidget> m_choices;

    bool choiceToStringThrowsOutOfRange(int index) {
        try {
            m_choices->choiceToString(index);
        } catch (const gphoto2pp::exceptions::IndexOutOfRange &) {
            return true;
        }
        return false;
    }

    bool setChoiceThrowsOutOfRange(int index) {
        try {
            m_choices->setChoice(index);
        } catch (const gphoto2pp::exceptions::IndexOutOfRange &) {
            return true;
        }
        return false;
    }

public:
    void setUp() {
        if (gphoto2::gp_widget_new(
                gphoto2::GP_WIDGET_MENU, "Choices", &m_widget) != GP_OK) {
            throw std::runtime_error("Unable to create synthetic choices widget");
        }
        if (gphoto2::gp_widget_add_choice(m_widget, "Auto") != GP_OK ||
            gphoto2::gp_widget_add_choice(m_widget, "Manual") != GP_OK ||
            gphoto2::gp_widget_set_value(m_widget, "Auto") != GP_OK) {
            gphoto2::gp_widget_unref(m_widget);
            m_widget = nullptr;
            throw std::runtime_error("Unable to configure synthetic choices widget");
        }
        m_choices.reset(new TestChoicesWidget(m_widget));
    }

    void tearDown() {
        m_choices.reset();
        if (m_widget != nullptr) {
            gphoto2::gp_widget_unref(m_widget);
            m_widget = nullptr;
        }
    }

    void testValidChoiceIndexes() {
        TS_ASSERT_EQUALS(m_choices->choiceToString(0), "Auto");
        TS_ASSERT_EQUALS(m_choices->choiceToString(1), "Manual");
        TS_ASSERT_THROWS_NOTHING(m_choices->setChoice(1));
        TS_ASSERT_EQUALS(m_choices->getChoice(), 1);
    }

    void testNegativeAndPastEndIndexesThrowRangeException() {
        TS_ASSERT(choiceToStringThrowsOutOfRange(-1));
        TS_ASSERT(choiceToStringThrowsOutOfRange(2));
        TS_ASSERT(setChoiceThrowsOutOfRange(-1));
        TS_ASSERT(setChoiceThrowsOutOfRange(2));
    }

    void testMetadataAndChangedStateMutations() {
        m_choices->setName("UpdatedChoices");
        m_choices->setInfo("Synthetic menu information");
        TS_ASSERT_EQUALS(m_choices->getName(), "UpdatedChoices");
        TS_ASSERT_EQUALS(m_choices->getInfo(),
            "Synthetic menu information");

        m_choices->setReadOnly(1);
        TS_ASSERT_EQUALS(m_choices->getReadOnly(), 1);
        m_choices->setChanged(1);
        TS_ASSERT(m_choices->isChanged());
        m_choices->setChanged(0);
        TS_ASSERT(!m_choices->isChanged());

        TS_ASSERT_THROWS(m_choices->setName(
            std::string("bad\0name", 8)),
            const gphoto2pp::exceptions::ArgumentException &);
        TS_ASSERT_THROWS(m_choices->setInfo(
            std::string("bad\0info", 9)),
            const gphoto2pp::exceptions::ArgumentException &);
        TS_ASSERT_THROWS(m_choices->setReadOnly(2),
            const gphoto2pp::exceptions::ArgumentException &);
        TS_ASSERT_THROWS(m_choices->setChanged(-1),
            const gphoto2pp::exceptions::ArgumentException &);
    }

    void testStandaloneCreationAndContainerInsertion() {
        auto root = gphoto2pp::CameraWidgetWrapper::create(
            gphoto2pp::CameraWidgetTypeWrapper::Window, "Root");
        auto *window =
            dynamic_cast<gphoto2pp::WindowWidget *>(root.get());
        TS_ASSERT(window != nullptr);

        auto menu = gphoto2pp::CameraWidgetWrapper::create(
            gphoto2pp::CameraWidgetTypeWrapper::Menu, "Mode");
        auto *typedMenu =
            dynamic_cast<gphoto2pp::MenuWidget *>(menu.get());
        TS_ASSERT(typedMenu != nullptr);
        typedMenu->addChoice("Auto");
        typedMenu->addChoice("Manual");
        TS_ASSERT_EQUALS(typedMenu->countChoices(), 2);
        TS_ASSERT_THROWS(typedMenu->addChoice(
            std::string("bad\0choice", 11)),
            const gphoto2pp::exceptions::ArgumentException &);

        auto text = gphoto2pp::CameraWidgetWrapper::create(
            gphoto2pp::CameraWidgetTypeWrapper::Text, "Name");
        window->append(*menu);
        window->prepend(*text);
        TS_ASSERT_EQUALS(window->countChildren(), 2);

        auto range = gphoto2pp::CameraWidgetWrapper::create(
            gphoto2pp::CameraWidgetTypeWrapper::Range, "Exposure");
        auto *typedRange =
            dynamic_cast<gphoto2pp::RangeWidget *>(range.get());
        TS_ASSERT(typedRange != nullptr);
        gphoto2pp::RangeWidgetRange limits;
        limits.Min = -2.0f;
        limits.Max = 2.0f;
        limits.Step = 0.5f;
        typedRange->setRange(limits);
        const auto readback = typedRange->getRange();
        TS_ASSERT_EQUALS(readback.Min, -2.0f);
        TS_ASSERT_EQUALS(readback.Max, 2.0f);
        TS_ASSERT_EQUALS(readback.Step, 0.5f);

        limits.Step = 0.0f;
        TS_ASSERT_THROWS(typedRange->setRange(limits),
            const gphoto2pp::exceptions::ArgumentException &);
    }
};
