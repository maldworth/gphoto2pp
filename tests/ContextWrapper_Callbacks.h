#include <cxxtest/TestSuite.h>

#include <gphoto2pp/helper_context.hpp>
#include <gphoto2pp/exceptions.hpp>

#include <stdexcept>
#include <type_traits>

static_assert(!std::is_copy_constructible<
    gphoto2pp::ContextWrapper>::value,
    "Contexts must not be copied");
static_assert(!std::is_copy_assignable<
    gphoto2pp::ContextWrapper>::value,
    "Contexts must not be copied");

namespace gphoto2 {
#include <gphoto2/gphoto2-context.h>

struct _GPContext {
    GPContextIdleFunc idle;
    void *idle_data;
    GPContextErrorFunc error;
    void *error_data;
    GPContextStatusFunc status;
    void *status_data;
    GPContextMessageFunc message;
    void *message_data;
    GPContextQuestionFunc question;
    void *question_data;
    GPContextCancelFunc cancel;
    void *cancel_data;
    GPContextProgressStartFunc progress_start;
    GPContextProgressUpdateFunc progress_update;
    GPContextProgressStopFunc progress_stop;
    void *progress_data;
};
}

namespace {
gphoto2::GPContext fake_context{};
bool return_null_context = false;
int context_new_calls = 0;
int context_unref_calls = 0;
}

namespace gphoto2 {
extern "C" GPContext *gp_context_new() {
    ++context_new_calls;
    return return_null_context ? nullptr : &fake_context;
}

extern "C" void gp_context_unref(GPContext *) {
    ++context_unref_calls;
}

extern "C" void gp_context_ref(GPContext *) {}

extern "C" void gp_context_set_idle_func(
    GPContext *context, GPContextIdleFunc func, void *data) {
    context->idle = func;
    context->idle_data = data;
}

extern "C" void gp_context_set_error_func(
    GPContext *context, GPContextErrorFunc func, void *data) {
    context->error = func;
    context->error_data = data;
}

extern "C" void gp_context_set_status_func(
    GPContext *context, GPContextStatusFunc func, void *data) {
    context->status = func;
    context->status_data = data;
}

extern "C" void gp_context_set_message_func(
    GPContext *context, GPContextMessageFunc func, void *data) {
    context->message = func;
    context->message_data = data;
}

extern "C" void gp_context_set_question_func(
    GPContext *context, GPContextQuestionFunc func, void *data) {
    context->question = func;
    context->question_data = data;
}

extern "C" void gp_context_set_cancel_func(
    GPContext *context, GPContextCancelFunc func, void *data) {
    context->cancel = func;
    context->cancel_data = data;
}

extern "C" void gp_context_set_progress_funcs(
    GPContext *context, GPContextProgressStartFunc start,
    GPContextProgressUpdateFunc update, GPContextProgressStopFunc stop,
    void *data) {
    context->progress_start = start;
    context->progress_update = update;
    context->progress_stop = stop;
    context->progress_data = data;
}

extern "C" void gp_context_idle(GPContext *) {}

extern "C" void gp_context_error(GPContext *, const char *format, ...) {
    (void)format;
}

extern "C" void gp_context_status(GPContext *, const char *format, ...) {
    (void)format;
}

extern "C" void gp_context_message(GPContext *, const char *format, ...) {
    (void)format;
}

extern "C" GPContextFeedback gp_context_question(
    GPContext *, const char *, ...) {
    return GP_CONTEXT_FEEDBACK_CANCEL;
}

extern "C" GPContextFeedback gp_context_cancel(GPContext *) {
    return GP_CONTEXT_FEEDBACK_OK;
}

extern "C" unsigned int gp_context_progress_start(
    GPContext *, float, const char *, ...) {
    return 77U;
}

extern "C" void gp_context_progress_update(
    GPContext *, unsigned int, float) {}

extern "C" void gp_context_progress_stop(GPContext *, unsigned int) {}
}

class ContextWrapper_Callbacks : public CxxTest::TestSuite {
public:
    void setUp() {
        fake_context = {};
        return_null_context = false;
        context_new_calls = 0;
        context_unref_calls = 0;
    }

    void testContextCallbacksDispatchAndReceiveText() {
        gphoto2pp::ContextWrapper context;
        int idle_calls = 0;
        std::string error_text;
        std::string status_text;
        std::string message_text;
        std::string question_text;
        context.setIdleCallback([&idle_calls]() { ++idle_calls; });
        context.setErrorCallback(
            [&error_text](std::string const &text) { error_text = text; });
        context.setStatusCallback(
            [&status_text](std::string const &text) { status_text = text; });
        context.setMessageCallback(
            [&message_text](std::string const &text) { message_text = text; });
        context.setQuestionCallback(
            [&question_text](std::string const &text) {
                question_text = text;
                return gphoto2pp::ContextFeedback::Cancel;
            });
        context.setCancelCallback(
            []() { return gphoto2pp::ContextFeedback::Ok; });

        fake_context.idle(fake_context.idle ? &fake_context : nullptr,
            fake_context.idle_data);
        fake_context.error(&fake_context, "IO error",
            fake_context.error_data);
        fake_context.status(&fake_context, "Downloading",
            fake_context.status_data);
        fake_context.message(&fake_context, "Ready",
            fake_context.message_data);
        const auto question =
            fake_context.question(&fake_context, "Overwrite?",
                fake_context.question_data);
        const auto cancel =
            fake_context.cancel(&fake_context, fake_context.cancel_data);

        TS_ASSERT_EQUALS(idle_calls, 1);
        TS_ASSERT_EQUALS(error_text, "IO error");
        TS_ASSERT_EQUALS(status_text, "Downloading");
        TS_ASSERT_EQUALS(message_text, "Ready");
        TS_ASSERT_EQUALS(question_text, "Overwrite?");
        TS_ASSERT_EQUALS(question, gphoto2::GP_CONTEXT_FEEDBACK_CANCEL);
        TS_ASSERT_EQUALS(cancel, gphoto2::GP_CONTEXT_FEEDBACK_OK);
    }

    void testProgressCallbacksAndMoveOwnership() {
        gphoto2pp::ContextWrapper source;
        unsigned int progress_id = 0;
        float progress_value = 0;
        unsigned int stopped_id = 0;
        source.setProgressCallbacks(
            [](float, std::string const &) { return 42U; },
            [&progress_value, &progress_id](unsigned int id, float value) {
                progress_id = id;
                progress_value = value;
            },
            [&stopped_id](unsigned int id) { stopped_id = id; });
        gphoto2pp::ContextWrapper moved(std::move(source));

        TS_ASSERT_EQUALS(source.getPtr(), nullptr);
        TS_ASSERT_THROWS(source.setIdleCallback([]() {}),
            const std::logic_error &);
        TS_ASSERT_EQUALS(fake_context.progress_start(
            &fake_context, 100.0f, "Copying",
            fake_context.progress_data), 42U);
        fake_context.progress_update(
            &fake_context, 42U, 75.0f, fake_context.progress_data);
        fake_context.progress_stop(
            &fake_context, 42U, fake_context.progress_data);
        TS_ASSERT_EQUALS(progress_id, 42U);
        TS_ASSERT_EQUALS(progress_value, 75.0f);
        TS_ASSERT_EQUALS(stopped_id, 42U);

        gphoto2pp::ContextWrapper destination;
        destination = std::move(moved);
        TS_ASSERT_EQUALS(moved.getPtr(), nullptr);
        TS_ASSERT_EQUALS(destination.getPtr(), &fake_context);
    }

    void testThrowingAndClearedCallbacksAreSafe() {
        gphoto2pp::ContextWrapper context;
        context.setIdleCallback([]() {
            throw std::runtime_error("synthetic callback failure");
        });
        fake_context.idle(&fake_context, fake_context.idle_data);

        context.setQuestionCallback(
            [](std::string const &) -> gphoto2pp::ContextFeedback {
                throw std::runtime_error("synthetic question failure");
            });
        TS_ASSERT_EQUALS(fake_context.question(
            &fake_context, "Question", fake_context.question_data),
            gphoto2::GP_CONTEXT_FEEDBACK_OK);

        context.setProgressCallbacks(
            [](float, std::string const &) -> unsigned int {
                throw std::runtime_error("synthetic progress failure");
            },
            {}, {});
        TS_ASSERT_EQUALS(fake_context.progress_start(
            &fake_context, 100.0f, "Progress",
            fake_context.progress_data), 0U);

        context.setIdleCallback({});
        fake_context.idle(&fake_context, fake_context.idle_data);
    }

    void testDirectNotificationMethods() {
        gphoto2pp::ContextWrapper context;
        context.idle();
        context.reportError("Error");
        context.reportStatus("Status");
        context.reportMessage("Message");
        TS_ASSERT_EQUALS(context.ask("Question"),
            gphoto2pp::ContextFeedback::Cancel);
        TS_ASSERT_EQUALS(context.cancelRequested(),
            gphoto2pp::ContextFeedback::Ok);
        TS_ASSERT_EQUALS(context.startProgress(100.0f, "Copy"), 77U);
        context.updateProgress(77U, 50.0f);
        context.stopProgress(77U);

        const std::string invalid_error("bad\0error", 9);
        TS_ASSERT_THROWS(context.reportError(invalid_error),
            const gphoto2pp::exceptions::ArgumentException &);
        TS_ASSERT_THROWS(context.reportStatus(
            std::string("bad\0status", 10)),
            const gphoto2pp::exceptions::ArgumentException &);
        TS_ASSERT_THROWS(context.reportMessage(
            std::string("bad\0message", 11)),
            const gphoto2pp::exceptions::ArgumentException &);
        TS_ASSERT_THROWS(context.ask(std::string("bad\0question", 12)),
            const gphoto2pp::exceptions::ArgumentException &);
        TS_ASSERT_THROWS(context.startProgress(
            100.0f, std::string("bad\0progress", 12)),
            const gphoto2pp::exceptions::ArgumentException &);
    }

    void testNullAndMovedFromContexts() {
        return_null_context = true;
        TS_ASSERT_THROWS(gphoto2pp::ContextWrapper(),
            const std::runtime_error &);

        return_null_context = false;
        gphoto2pp::ContextWrapper context;
        gphoto2pp::ContextWrapper moved(std::move(context));
        TS_ASSERT_EQUALS(context.getPtr(), nullptr);
        TS_ASSERT_THROWS(context.setIdleCallback([]() {}),
            const std::logic_error &);
        TS_ASSERT_EQUALS(context_new_calls, 2);
        TS_ASSERT_EQUALS(context_unref_calls, 0);
    }
};
