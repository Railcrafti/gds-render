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

#include "lib-cell-renderer.h"
#include "../gds-types.h"

G_DEFINE_TYPE(LibCellRenderer, lib_cell_renderer, GTK_TYPE_CELL_RENDERER_TEXT)

enum {
	PROP_LIB = 1,
	PROP_CELL,
	PROP_COUNT
};

void lib_cell_renderer_init(LibCellRenderer *self)
{
	/* Nothing to do */
}

static void lib_cell_renderer_constructed(GObject *obj)
{
	G_OBJECT_CLASS(lib_cell_renderer_parent_class)->constructed(obj);
}

static void lib_cell_renderer_set_property(GObject      *object,
					   guint        param_id,
					   const GValue *value,
					   GParamSpec   *pspec)
{
	GValue val = G_VALUE_INIT;

	g_value_init(&val, G_TYPE_STRING);

	switch (param_id) {
	case PROP_LIB:
		g_value_set_string(&val, ((struct gds_library *)g_value_get_pointer(value))->name);
		g_object_set_property(object, "text", &val);
		break;
	case PROP_CELL:
		g_value_set_string(&val, ((struct gds_cell *)g_value_get_pointer(value))->name);
		g_object_set_property(object, "text", &val);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
		break;
	}
}

static void lib_cell_renderer_get_property(GObject      *object,
					   guint        param_id,
					   GValue	*value,
					   GParamSpec   *pspec)
{
	switch (param_id) {
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
		break;
	}
}

static GParamSpec *properties [PROP_COUNT];

void lib_cell_renderer_class_init(LibCellRendererClass *klass)
{
	GObjectClass *oclass = G_OBJECT_CLASS(klass);
	oclass->constructed = lib_cell_renderer_constructed;
	oclass->set_property = lib_cell_renderer_set_property;
	oclass->get_property = lib_cell_renderer_get_property;

	properties[PROP_LIB] = g_param_spec_pointer("gds-lib", "gds-lib",
							 "Library reference to be displayed",
							 G_PARAM_WRITABLE);
	properties[PROP_CELL] = g_param_spec_pointer("gds-cell", "gds-cell",
							 "Cell reference to be displayed",
							 G_PARAM_WRITABLE);

	g_object_class_install_properties(oclass, PROP_COUNT, properties);
}

GtkCellRenderer *lib_cell_renderer_new()
{
	return GTK_CELL_RENDERER(g_object_new(TYPE_LIB_CELL_RENDERER, NULL));
}