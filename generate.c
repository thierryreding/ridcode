#include <cairo.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include <librsvg/rsvg.h>

/* each module is represented by an arc of 9 degrees */
#define NUM_MODULES 40

struct faction {
	const char *name;
	const char *template;
	struct {
		double red;
		double green;
		double blue;
	} color;
};

static const struct faction autobot = {
	.name = "Autobot",
	.template = "autobot.svg",
	.color = {
		0.78125, 0.0, 0.0
	},
};

struct shield {
	const char *name;
	const struct faction *faction;
	uint8_t code[NUM_MODULES];
};

static const struct shield shields[] = {
	{
		.name = "Mega Optimus",
		.faction = &autobot,
		.code = {
			0, 1, 0, 1, 1, 0, 0, 1,
			0, 0, 1, 1, 0, 0, 1, 0,
			1, 0, 1, 0, 0, 1, 1, 1,
			0, 0, 0, 0, 0, 1, 0, 1,
			0, 1, 1, 0, 0, 1, 1, 0,
		},
	}, {
		.name = "Warrior Class Night Strike Bumblebee",
		.faction = &autobot,
		.code = {
			0, 0, 1, 0, 0, 1, 1, 1,
			0, 1, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 1, 1, 1, 0, 0,
			1, 0, 1, 1, 0, 0, 1, 0,
			0, 0, 0, 1, 1, 0, 0, 0,
		},
	}, {
		.name = "Legion Class Night Strike Bumblebee",
		.faction = &autobot,
		.code = {
			1, 0, 0, 0, 1, 0, 1, 1,
			0, 0, 1, 1, 0, 0, 1, 0,
			0, 0, 1, 0, 0, 1, 1, 0,
			1, 0, 0, 0, 1, 0, 0, 0,
			1, 0, 0, 1, 0, 0, 1, 1,
		},
	}, {
		.name = "Warrior Class Grimlock",
		.faction = &autobot,
		.code = {
			1, 1, 1, 0, 0, 1, 1, 0,
			0, 1, 1, 1, 0, 0, 0, 0,
			1, 1, 1, 1, 0, 0, 0, 0,
			1, 1, 1, 0, 1, 0, 1, 1,
			0, 1, 1, 0, 0, 1, 0, 1,
		},
	}, {
		.name = "One-Step Patrol Strongarm",
		.faction = &autobot,
		.code = {
			1, 0, 0, 1, 1, 1, 1, 0,
			0, 1, 0, 0, 1, 0, 1, 1,
			1, 0, 1, 1, 1, 1, 0, 0,
			0, 1, 0, 1, 0, 1, 0, 0,
			1, 0, 0, 1, 1, 1, 1, 0,
		},
	}, {
		.name = "Energon",
		.faction = &autobot,
		.code = {
			0, 1, 0, 1, 0, 0, 1, 1,
			0, 0, 1, 1, 0, 0, 1, 1,
			1, 0, 0, 1, 0, 1, 0, 0,
			1, 1, 0, 0, 0, 1, 0, 0,
			0, 0, 0, 1, 0, 1, 1, 1,
		},
	},
};

/* XXX half a module */
static const double offset = -4.5 * M_PI / 180.0;

int main(int argc, char *argv[])
{
	const unsigned int width = 512, height = 512;
	const struct shield *shield = NULL;
	const struct faction *faction;
	RsvgDimensionData dimensions;
	cairo_surface_t *surface;
	GError *error = NULL;
	RsvgHandle *rsvg;
	cairo_t *cairo;
	unsigned int i;

	for (i = 0; i < G_N_ELEMENTS(shields); i++) {
		if (strcmp(argv[1], shields[i].name) == 0) {
			shield = &shields[i];
			break;
		}
	}

	if (!shield) {
		fprintf(stderr, "shield %s not found\n", argv[1]);
		return 1;
	}

	faction = shield->faction;

	rsvg = rsvg_handle_new_from_file(faction->template, &error);
	if (!rsvg) {
		fprintf(stderr, "failed to load %s\n", faction->template);
		return 1;
	}

	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
					     width, height);
	cairo = cairo_create(surface);

	cairo_set_source_rgb(cairo, 0, 0, 0);

	cairo_scale(cairo, width, height);
	cairo_translate(cairo, 0.5, 0.5);

	cairo_set_source_rgb(cairo, 1, 1, 1);
	cairo_arc(cairo, 0, 0, 0.50, 0.0, 2 * M_PI);
	cairo_fill(cairo);

	cairo_set_line_width(cairo, 0.02);
	cairo_set_source_rgb(cairo, faction->color.red, faction->color.green,
			     faction->color.blue);

	cairo_arc(cairo, 0, 0, 0.330, 0.0, 2 * M_PI);
	cairo_stroke(cairo);

	cairo_arc(cairo, 0, 0, 0.415, 0.0, 2 * M_PI);
	cairo_stroke(cairo);

	cairo_set_line_width(cairo, 0.10);

	for (i = 0; i < NUM_MODULES; i++) {
		double module = 2 * M_PI / NUM_MODULES;
		double start = offset + i * module, angle;
		unsigned int j;

		if (shield->code[i] != 0) {
			for (j = 0; i + j < NUM_MODULES; j++)
				if (shield->code[i + j] == 0)
					break;

			angle = j * module;

			cairo_arc(cairo, 0, 0, 0.3725, start, start + angle);
			cairo_stroke(cairo);

			i += j;
		}
	}

	rsvg_handle_get_dimensions(rsvg, &dimensions);
	cairo_translate(cairo, -0.25, -0.220);
	cairo_scale(cairo, 0.5 / dimensions.width, 0.5 / dimensions.height);
	rsvg_handle_render_cairo(rsvg, cairo);

	cairo_surface_write_to_png(surface, argv[2]);

	cairo_destroy(cairo);
	cairo_surface_destroy(surface);
	g_object_unref(rsvg);

	return 0;
}
