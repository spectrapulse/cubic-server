#ifndef LOGSINTERFACE_HPP
#define LOGSINTERFACE_HPP

#include <gtkmm.h>

class LogsInterface : public Gtk::Box {
    public:
        LogsInterface();
        virtual ~LogsInterface();

    private:
        bool on_log_to_display();
        bool on_key_press_event(GdkEventKey* event) override;
        void on_button_clicked();

    protected:
        Gtk::ScrolledWindow m_Logs_container;
        Gtk::TextView m_Logs_view;
        Glib::RefPtr<Gtk::TextBuffer> m_Logs;
        Glib::RefPtr<Gtk::TextBuffer::Mark> m_endMark;
        Gtk::Entry m_Entry;
        Gtk::Box m_VBox_main, m_VBox_logs;
        Gtk::Frame m_Frame_filters;
        Gtk::ButtonBox m_HBox_filters;
        Gtk::Button m_Info, m_Warn, m_Error, m_Fatal, m_Debug;
};

#endif /* !LOGSINTERFACE_HPP */
