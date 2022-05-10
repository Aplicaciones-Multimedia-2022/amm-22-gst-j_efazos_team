//gst-launch-1.0 filesrc location=Hulk.mp4 ! qtdemux name=demux demux.audio_0 !  queue ! decodebin ! audioconvert ! audioresample! autoaudiosink demux.video_0
//! queue ! decodebin ! videoconvert ! theoraenc ! queue ! m. oggmux name=m ! queue ! tcpserversink host=127.0.0.1 port=9090



#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>

#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct _CustomData {

  GstElement *pipeline;
  GstElement *source;
  GstElement *mpegaudioparse;
  GstElement *decoder;
  GstElement *tee;

  //Elementos de la primera cola

  GstElement *queue1;
  GstElement *decodebin;
  GstElement *audioconvert;
  GstElement *audioresample;
  GstElement *autoaudiosink;
  GstElement *demux;

  //Elementos de la segunda cola

  GstElement *queue2;
  GstElement *decodebin2;
  GstElement *videoconvert;
  GstElement *theoraenc;
  GstElement *oggmux;
  GstElement *tcpserversink;

}CustomData;

static void pad_added_handler (GstElement *src, GstPad *pad, CustomData *data);
static gboolean bus_call (GstBus *bus,GstMessage *msg, gpointer databus);

 GMainLoop *loop = (GMainLoop *) data;
 char *src = GST_MESSAGE_SRC_NAME(msg);

 switch (GST_MESSAGE_TYPE (msg)) {
 case GST_MESSAGE_EOS:
 g_main_loop_quit (loop);
 break;
 case GST_MESSAGE_ERROR: {
 res = 1;
 #ifdef _DEBUG
 gchar *debug;
 GError *error;

 gst_message_parse_error (msg, &error, &debug);
 g_free (debug);

 g_printerr ("Error: %s\n", error−>message);
 g_error_free (error);
 #endif
 g_main_loop_quit (loop);
 break;
 }
 default: {
 break;
 }
 }


static void on_pad_added (GstElement *element,
                          GstPad     *pad,
                          gpointer    data)
{
  GstPad *sinkpad;
  GstElement *decoder = (GstElement *) decoder;

  /* We can now link this pad with the vorbis-decoder sink pad */
  g_print ("Dynamic pad created, linking demuxer/decoder\n");

  sinkpad = gst_element_get_static_pad (decoder, "sink");

  gst_pad_link (pad, sinkpad);

  gst_object_unref (sinkpad);

}

int main(int argc, char *argv[]) {



  GMainLoop * loop = g_main_loop_new ( NULL , FALSE ); //Creamos un objeto bucle de espera

  /*Initialize GStreamer */

    gst_init (&argc, &argv); //Inicializamos la librería de GStreamer

    //parametros: (nombre por el que lo va a reconocer gstreamer, como lo llamo yo)
    data.source = gst_element_factory_make ("filesrc", "source");
    data.mpegaudioparse = gst_element_factory_make ("mpegaudioparse", "mpegaudioparse");
    data.decoder = gst_element_factory_make ("mpg123audiodec", "mpg123audiodecoder");
    data.tee = gst_element_factory_make ("tee", "tee");
    data.pipeline = gst_pipeline_new ("nuestro_pipeline"); //Creamos un nuevo pipeline

    //Elementos primera cola

    data.queue1 = gst_element_factory_make ("queue", "queue1");
    data.decodebin = gst_element_factory_make ("decodebin", "decodebin1");
    data.audioconvert = gst_element_factory_make ("audioconvert", "audioconvert");
    data.audioresample= gst_element_factory_make ("audioresample", "audioresample");
    data.autoaudiosink = gst_element_factory_make ("autoaudiosink", "autoaudiosink");
    data.demux = gst_element_factory_make ("demux", "demux");

      //Elementos segunda cola

      data.queue2 = gst_element_factory_make ("queue", "queue2");
      data.decodebin2 = gst_element_factory_make ("decodebin", "decodebin2");
      data.videoconvert = gst_element_factory_make ("videoconvert", "videoconvert");
      data.theoraenc= gst_element_factory_make ("theoraenc", "theoraenc");
      data.oggmux = gst_element_factory_make ("oggmux", "oggmux");
      data.tcpserversink= gst_element_factory_make ("tcpserversink", "tcpserversink");


      loop = g_main_loop_new (NULL, FALSE);

      /* Create the empty pipeline */
      data.pipeline = gst_pipeline_new ("pipeline_test");

    //COMPROBACIÓN DE ERRORES: Conviene comprobar que no ha habido erroes al obtener el elemento o la factoría

        if (! data.pipeline  || ! data.source || !   data.mpegaudioparse || ! data.decoder  || ! data.tee  || ! data.queue1  || ! data.decodebin
	         || ! data.audioconvert ||! data.audioresample || !data.autoaudiosink || ! data.demux || ! data.queue2  || ! data.decodebin2
           || ! data.videoconvert ||! data.theoraenc || !data.oggmux || ! data.tcpserversink) {
            // Error
            g_printerr (" Error : could not create element . Exit .\n") ;
            return -1;
        }


     /* set elements properties */

    g_object_set (G_OBJECT (video_sink), "location", url, NULL); // web server


    /* add all elements into the pipeline */

    gst_bin_add_many (GST_BIN (pipeline),source,mpegaudioparse, mpg123audiodecoder,queue1, decodebin1, audioconvert, audioresample, autoaudiosink, demux,
    queue2, decodebin2, videoconvert, theoraenc, oggmux, tcpserversink);

    gst_element_link (source, mpegaudioparse, mpg123audiodecoder, tee, );

    gst_element_link_many (tee, queue1,decodebin1, audioconvert,audioresample, audio_sink,demux, NULL);
    gst_element_link_many (tee, queue2, decodebin2, videoconvert, theoraenc, oggmux, tcpserversink, NULL);

    /* Connect to the pad-added signal */
    g_signal_connect (data.source, "pad-added", G_CALLBACK (pad_added_handler), &data);

        /* Set the pipeline to "playing" state*/
    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    /* bus message handler */
    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));

    //añade un reloj al contexto principal que permite controlar el tiempo
    gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref (bus);
    /* Run... */
    g_main_loop_run (loop);
    /* Out of the main loop, clean up nicely */
    gst_object_unref (bus);
    g_print ("Returned, stopping playback\n");
    gst_element_set_state (pipeline, GST_STATE_NULL);
    g_print ("Deleting pipeline\n");
    gst_object_unref (GST_OBJECT (pipeline));
    return 0;


  return 0;
}

}
/* This function will be called by the pad-added signal */
static void pad_added_handler (GstElement *src, GstPad *new_pad, CustomData *data) {
  //gst_element_get_static_pad (elemnto del que quiero obtener el pad, nombre del elemento), lo guardamos en sink pad_added_handler
  //si no se encuentra devuelve null
  GstPad *sink_pad = gst_element_get_static_pad (data->mpegaudioparse, "sink");
  GstPadLinkReturn ret;
  GstCaps *new_pad_caps = NULL;
  GstStructure *new_pad_struct = NULL;

  const gchar *new_pad_type = NULL;

  g_print ("Received new pad '%s' from '%s':\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src));

  /* If our converter is already linked, we have nothing to do here */
  //Comprueba si un pad está vinculado a otro pad o no
  //devuelve true si está bien linkeado
  if (gst_pad_is_linked (sink_pad)) {

    g_print ("We are already linked. Ignoring.\n");
    goto exit;
  }

  /* Check the new pad's type */
  //no sabemos que es esto  (info sobre el flujo???)
  new_pad_caps = gst_pad_get_current_caps (new_pad);
  new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
  new_pad_type = gst_structure_get_name (new_pad_struct);

//comprueba si el archivo contiene ese prefijo------- nos puede venir bien para comprobar que es un mp3
  if (!g_str_has_prefix (new_pad_type, "audio/x-raw")) {

    g_print ("It has type '%s' which is not raw audio. Ignoring.\n", new_pad_type);
    goto exit;
  }

  /* Attempt the link */
  //linkea el pad de la fuente con el del sumidero
  ret = gst_pad_link (new_pad, sink_pad);
  if (GST_PAD_LINK_FAILED (ret)) {

    g_print ("Type is '%s' but link failed.\n", new_pad_type);

  } else {

    g_print ("Link succeeded (type '%s').\n", new_pad_type);
  }

exit:

  /* Unreference the new pad's caps, if we got them */
  if (new_pad_caps != NULL){

    gst_caps_unref (new_pad_caps);
  /* Unreference the sink pad */
  }
  gst_object_unref (sink_pad);

}

/* --------------------------------------------------------------------- *

 *  Bus message handler

 * --------------------------------------------------------------------- */

//funcion de callback
static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer databus){

    GMainLoop *loop = (GMainLoop *) databus;
    char *src = GST_MESSAGE_SRC_NAME(msg);
    GError *err;
    gchar *debug_info;

      //comprueba el tipo de mensaje
      switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_ERROR:

// gst_message_parse_error( mensaje de estado, donde guardo el error, donde guardo la cadena de depuracion(info del error))
          gst_message_parse_error (msg, &err, &debug_info);
          g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
          g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
          g_clear_error (&err);
          g_free (debug_info);
          g_main_loop_quit (loop);

          break;

        case GST_MESSAGE_EOS:

          g_print ("End-Of-Stream reached.\n");
          g_main_loop_quit (loop);

          break;

        case GST_MESSAGE_STATE_CHANGED:
          /* We are only interested in state-changed messages from the pipeline */
          if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data.pipeline)) {

            GstState old_state, new_state, pending_state;
            //gst_message_parse_state_changed(de que queremos saber el estado, el estado antiguo, estado nuevo)
            gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
            g_print ("Pipeline state changed from %s to %s:\n",
            gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
          }
          break;
        default:
          /* We should not reach here */
        //    g_print ("..[bus].. %15s :: %-15s\n", src, GST_MESSAGE_TYPE_NAME(msg));
      //    g_printerr ("Unexpected message received.\n");
          break;
      }
    //  gst_message_unref (msg);
      return TRUE;
  }

 // return TRUE;
//}
