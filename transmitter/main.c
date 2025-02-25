#include <gst/gst.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Global variables for the pipeline and main loop
static GstElement *pipeline = NULL;
static GMainLoop *loop = NULL;

// Signal handler function
static void handle_signal(int signum) {
    g_print("Caught signal %d, stopping pipeline...\n", signum);

    if (loop) {
        g_main_loop_quit(loop); // Quit the main loop
    }

    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL); // Stop the pipeline
        gst_object_unref(pipeline); // Clean up the pipeline
    }

    exit(0); // Exit the program
}

// Bus message handler function
static gboolean bus_callback(GstBus *bus, GstMessage *msg, gpointer data) {
    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR: {
            GError *err;
            gchar *debug_info;

            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);

            g_main_loop_quit(loop); // Quit the main loop on error
            break;
        }
        case GST_MESSAGE_EOS:
            g_print("End-Of-Stream reached.\n");
            g_main_loop_quit(loop); // Quit the main loop on EOS
            break;
        default:
            // Ignore other messages
            break;
    }

    return TRUE; // Keep listening for more messages
}

int main(int argc, char *argv[]) {
    GstElement *source, *sink;
    GstBus *bus;
    GstStateChangeReturn ret;
    const gchar *host;
    GstPad *src_pad;
    GstCaps *caps;

    // Get the host IP from the environment variable
    host = g_getenv("UDP_HOST");
    if (!host) {
        g_printerr("Environment variable UDP_HOST is not set.\n");
        return -1;
    }

    // Initialize GStreamer
    gst_init(&argc, &argv);

    // Create elements
    source = gst_element_factory_make("alsasrc", "source");
    sink = gst_element_factory_make("udpsink", "sink");

    // Create the pipeline
    pipeline = gst_pipeline_new("audio-sender-pipeline");

    if (!pipeline || !source || !sink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    // Build the pipeline
    gst_bin_add_many(GST_BIN(pipeline), source, sink, NULL);
    if (!gst_element_link_many(source, sink, NULL)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Set properties
    g_object_set(source, "device", "hw:0,0", NULL);
    g_object_set(sink, "host", host, NULL);
    g_object_set(sink, "port", 5000, NULL);

    // Define the capabilities
    caps = gst_caps_new_simple("audio/x-raw",
        "format", G_TYPE_STRING, "S24LE",
        "channels", G_TYPE_INT, 2,
        "rate", G_TYPE_INT, 48000,
        NULL);

    // Get the source pad of the alsasrc element
    src_pad = gst_element_get_static_pad(source, "src");
    if (!src_pad) {
        g_printerr("Could not get source pad from alsasrc.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Set the capabilities on the source pad
    if (!gst_pad_set_caps(src_pad, caps)) {
        g_printerr("Could not set caps on source pad.\n");
        gst_object_unref(pipeline);
        gst_object_unref(src_pad);
        return -1;
    }

    // Release the pad and caps references
    gst_object_unref(src_pad);
    gst_caps_unref(caps);

    // Set up a bus watch to listen for messages
    bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, bus_callback, NULL); // Add a callback for bus messages
    gst_object_unref(bus); // Release the bus reference

    // Start playing
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    // Set up signal handling
    signal(SIGINT, handle_signal);  // Handle Ctrl+C
    signal(SIGTERM, handle_signal); // Handle systemd stop

    // Create a GLib main loop
    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop); // Run the main loop

    // Clean up
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);

    return 0;
}
