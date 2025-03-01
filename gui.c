#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>

// Structure to hold user settings
typedef struct {
    gboolean dark_mode;
    gchar *theme;
    gboolean notifications_enabled;
    gchar *language;
} UserSettings;

// Save the settings to a file
void save_user_settings(UserSettings *settings) {
    FILE *file = fopen("user_settings.conf", "w");
    if (file != NULL) {
        fprintf(file, "dark_mode=%d\n", settings->dark_mode);
        fprintf(file, "theme=%s\n", settings->theme);
        fprintf(file, "notifications_enabled=%d\n", settings->notifications_enabled);
        fprintf(file, "language=%s\n", settings->language);
        fclose(file);
    }
}

// Load the settings from a file
UserSettings* load_user_settings() {
    UserSettings *settings = malloc(sizeof(UserSettings));
    FILE *file = fopen("user_settings.conf", "r");

    if (file != NULL) {
        fscanf(file, "dark_mode=%d\n", &settings->dark_mode);
        settings->theme = malloc(50);
        fscanf(file, "theme=%s\n", settings->theme);
        fscanf(file, "notifications_enabled=%d\n", &settings->notifications_enabled);
        settings->language = malloc(50);
        fscanf(file, "language=%s\n", settings->language);
        fclose(file);
    } else {
        settings->dark_mode = FALSE;
        settings->theme = "Light";
        settings->notifications_enabled = TRUE;
        settings->language = "English";
    }

    return settings;
}

// Callback to close the window
static void on_quit_clicked(GtkWidget *widget, gpointer data) {
    gtk_window_close(GTK_WINDOW(data));
}

// Show notification window
void show_notification(const gchar *message) {
    GtkWidget *notification_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(notification_window), "Notification");
    gtk_window_set_default_size(GTK_WINDOW(notification_window), 250, 150);

    GtkWidget *label = gtk_label_new(message);
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 10);

    gtk_container_add(GTK_CONTAINER(notification_window), vbox);
    gtk_widget_show_all(notification_window);
}

// Show a notification when the button is clicked
static void on_notify_button_clicked(GtkWidget *widget, gpointer data) {
    show_notification("This is a test notification from the application!");
}

// Create settings window
GtkWidget* create_settings_window(UserSettings *settings) {
    GtkWidget *settings_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(settings_window), "Settings");
    gtk_window_set_default_size(GTK_WINDOW(settings_window), 400, 300);

    GtkWidget *label = gtk_label_new("Here you can customize the application's settings.");

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 10);

    // Dark mode toggle
    GtkWidget *dark_mode_switch = gtk_switch_new();
    gtk_switch_set_active(GTK_SWITCH(dark_mode_switch), settings->dark_mode);
    gtk_box_pack_start(GTK_BOX(vbox), dark_mode_switch, FALSE, FALSE, 10);

    // Language selection
    GtkWidget *language_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(language_combo), "English");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(language_combo), "Arabic");
    gtk_combo_box_set_active(GTK_COMBO_BOX(language_combo), g_strcmp0(settings->language, "English") == 0 ? 0 : 1);
    gtk_box_pack_start(GTK_BOX(vbox), language_combo, FALSE, FALSE, 10);

    // Save button
    GtkWidget *save_button = gtk_button_new_with_label("Save Settings");
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_quit_clicked), settings_window);
    gtk_box_pack_start(GTK_BOX(vbox), save_button, TRUE, TRUE, 10);

    gtk_container_add(GTK_CONTAINER(settings_window), vbox);
    return settings_window;
}

// Settings button callback
static void on_settings_clicked(GtkWidget *widget, gpointer data) {
    UserSettings *settings = load_user_settings();
    GtkWidget *settings_window = create_settings_window(settings);
    gtk_widget_show_all(settings_window);
}

// Create a sidebar menu
GtkWidget* create_sidebar() {
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *home_button = gtk_button_new_with_label("Home");
    GtkWidget *settings_button = gtk_button_new_with_label("Settings");
    GtkWidget *about_button = gtk_button_new_with_label("About");
    GtkWidget *file_button = gtk_button_new_with_label("Open File");

    gtk_box_pack_start(GTK_BOX(sidebar), home_button, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(sidebar), settings_button, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(sidebar), about_button, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(sidebar), file_button, FALSE, FALSE, 5);

    return sidebar;
}

// Create main window layout
static void on_activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "XFCE-like GTK Application");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    UserSettings *settings = load_user_settings();
    
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    // Create sidebar
    GtkWidget *sidebar = create_sidebar();
    gtk_box_pack_start(GTK_BOX(vbox), sidebar, FALSE, FALSE, 0);

    // Create main content area
    GtkWidget *content_area = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), content_area, TRUE, TRUE, 10);

    // Create buttons
    GtkWidget *notify_button = gtk_button_new_with_label("Test Notification");
    g_signal_connect(notify_button, "clicked", G_CALLBACK(on_notify_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(content_area), notify_button, TRUE, TRUE, 10);

    GtkWidget *settings_button = gtk_button_new_with_label("Settings");
    g_signal_connect(settings_button, "clicked", G_CALLBACK(on_settings_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(content_area), settings_button, TRUE, TRUE, 10);

    // File selection button
    GtkWidget *file_button = gtk_button_new_with_label("Open File");
    g_signal_connect(file_button, "clicked", G_CALLBACK(on_notify_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(content_area), file_button, TRUE, TRUE, 10);

    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show_all(window);
}

// Create About window
GtkWidget* create_about_window() {
    GtkWidget *about_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(about_window), "About");
    gtk_window_set_default_size(GTK_WINDOW(about_window), 300, 200);

    GtkWidget *label = gtk_label_new("XFCE-like Application\nVersion 1.0\nDeveloped by YourName");

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 10);

    gtk_container_add(GTK_CONTAINER(about_window), vbox);
    return about_window;
}

// About button callback
static void on_about_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *about_window = create_about_window();
    gtk_widget_show_all(about_window);
}

// Main function to initialize and run the application
int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.xfce_app", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    g_signal_connect(app, "quit", G_CALLBACK(on_quit_clicked), NULL);
    
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}

