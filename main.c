/*
 * GDSII-Converter
 * Copyright (C) 2018  Mario Hüttel <mario.huettel@gmx.net>
 *
 * This file is part of GDSII-Converter.
 *
 * GDSII-Converter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * GDSII-Converter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GDSII-Converter.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <gtk/gtk.h>
#include <glib.h>
#include "main-window.h"
#include "command-line.h"

struct application_data {
	GtkApplication *app;
	GtkWindow *main_window;
};


static void app_quit(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	struct application_data *appdata = (struct application_data *)user_data;
	gtk_widget_destroy(GTK_WIDGET(appdata->main_window));
}

static void app_about(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkBuilder *builder;
	GtkDialog *dialog;
	struct application_data *appdata = (struct application_data *)user_data;

	builder = gtk_builder_new_from_resource("/about.glade");
	dialog = GTK_DIALOG(gtk_builder_get_object(builder, "about-dialog"));
	gtk_window_set_transient_for(GTK_WINDOW(dialog), appdata->main_window);
	gtk_dialog_run(dialog);

	gtk_widget_destroy(GTK_WIDGET(dialog));
	g_object_unref(builder);
}

const GActionEntry app_actions[] = {
	{ "quit", app_quit },
	{ "about", app_about }
};

static void gapp_activate(GApplication *app, gpointer user_data)
{
	GtkWindow *main_window;
	struct application_data *appdata = (struct application_data *)user_data;

	main_window = create_main_window();
	appdata->main_window = main_window;
	gtk_application_add_window(GTK_APPLICATION(app), main_window);
	gtk_widget_show(GTK_WIDGET(main_window));
}

static int start_gui(int argc, char **argv)
{

	GtkApplication *gapp;
	int app_status;
	static struct application_data appdata;
	GMenu *menu;
	GMenu *m_quit;
	GMenu *m_about;

	gapp = gtk_application_new("de.shimatta.gds-render", G_APPLICATION_FLAGS_NONE);
	g_application_register(G_APPLICATION(gapp), NULL, NULL);
	//g_action_map_add_action_entries(G_ACTION_MAP(gapp), app_actions, G_N_ELEMENTS(app_actions), &appdata);
	g_signal_connect (gapp, "activate", G_CALLBACK(gapp_activate), &appdata);



	menu = g_menu_new();
	m_quit = g_menu_new();
	m_about = g_menu_new();
	g_menu_append(m_quit, "Quit", "app.quit");
	g_menu_append(m_about, "About", "app.about");
	g_menu_append_section(menu, NULL, G_MENU_MODEL(m_about));
	g_menu_append_section(menu, NULL, G_MENU_MODEL(m_quit));
	g_action_map_add_action_entries(G_ACTION_MAP(gapp), app_actions, G_N_ELEMENTS(app_actions), &appdata);
	gtk_application_set_app_menu(GTK_APPLICATION(gapp), G_MENU_MODEL(menu));

	g_object_unref(m_quit);
	g_object_unref(m_about);
	g_object_unref(menu);


	app_status = g_application_run (G_APPLICATION(gapp), argc, argv);
	g_object_unref (gapp);

	return app_status;
}

int main(int argc, char **argv)
{
	GError *error = NULL;
	GOptionContext *context;
	gchar *gds_name;
	gchar *basename;
	gchar *pdfname = NULL, *texname = NULL, *mappingname = NULL, *cellname = NULL;
	gboolean tikz = FALSE, pdf = FALSE, pdf_layers = FALSE, pdf_standalone = FALSE;
	int scale = 1000;
	int app_status;


	GOptionEntry entries[] =
	{
	  { "tikz", 't', 0, G_OPTION_ARG_NONE, &tikz, "Output TikZ code", NULL },
	  { "pdf", 'p', 0, G_OPTION_ARG_NONE, &pdf, "Output PDF document", NULL },
	  { "scale", 's', 0, G_OPTION_ARG_INT, &scale, "Divide putput coordinates by <SCALE>", "<SCALE>" },
	  { "tex-output", 'o', 0, G_OPTION_ARG_FILENAME, &texname, "Optional path for TeX file", "PATH" },
	  { "pdf-output", 'O', 0, G_OPTION_ARG_FILENAME, &pdfname, "Optional path for PDF file", "PATH" },
	  { "mapping", 'm', 0, G_OPTION_ARG_FILENAME, &mappingname, "Path for Layer Mapping File", "PATH" },
	  { "cell", 'c', 0, G_OPTION_ARG_STRING, &cellname, "Cell to render", "NAME" },
	  { "tex-standalone", 'a', 0, G_OPTION_ARG_NONE, &pdf_standalone, "Create standalone PDF", NULL },
	  { "tex-layers", 'l', 0, G_OPTION_ARG_NONE, &pdf_layers, "Create standalone PDF", NULL },
	  { NULL }
	};

	context = g_option_context_new(" FILE - Convert GDS file <FILE> to graphic");
	g_option_context_add_main_entries(context, entries, NULL);
	g_option_context_add_group(context, gtk_get_option_group(TRUE));
	if (!g_option_context_parse (context, &argc, &argv, &error))
	    {
	      g_print ("Option parsing failed: %s\n", error->message);
	      exit (1);
	    }

	if (argc >= 2) {
		if (scale < 1) {
			printf("Scale < 1 not allowed. Setting to 1\n");
			scale = 1;
		}

		/* No format selected */
		if (!(tikz || pdf)) {
			tikz = TRUE;
		}

		/* Get gds name */
		gds_name = argv[1];

		/* Check if PDF/TeX names are supplied. if not generate */
		basename = g_path_get_basename(gds_name);

		if (!texname) {
			texname = g_strdup_printf("./%s.tex", basename);
		}

		if (!pdfname) {
			pdfname = g_strdup_printf("./%s.pdf", basename);
		}

		command_line_convert_gds(gds_name, pdfname, texname, pdf, tikz, mappingname, cellname,
					 (double)scale, pdf_layers, pdf_standalone);
		/* Clean up */
		g_free(pdfname);
		g_free(texname);
		if (mappingname)
			g_free(mappingname);
		if (cellname)
			g_free(cellname);
		app_status = 0;
	} else {
		app_status = start_gui(argc, argv);
	}


	return app_status;
}
