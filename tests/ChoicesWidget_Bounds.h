#include <cxxtest/TestSuite.h>

#include <gphoto2pp/choices_widget.hpp>
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
};
