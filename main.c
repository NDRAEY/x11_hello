#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <xcb/xcb.h>
#include "duke.h"

#define DESKTOP_COLOR 0xECA605
#define HELLO "Hello, Carol Tea!"
#define PIX(r, g, b) ((r&0xff)<<16)|((g&0xff)<<8)|(b&0xff)

xcb_connection_t    *c;
xcb_screen_t        *screen;
xcb_drawable_t       fakeroot, root;
xcb_gcontext_t       foreground, background;
xcb_gcontext_t       desktop;

/*
xcb_poly_point (connection, XCB_COORD_MODE_ORIGIN, window, fore/background, count, points);
*/

/*
void draw_duke_image(char* filename, int x, int y) {
	struct DukeImageMeta* imagemeta = calloc(9, 1);
	duke_get_image_metadata(imagemeta, filename);
	
	char* imagedata = calloc(imagemeta->data_length, 1);
	duke_get_data(imagemeta, filename, imagedata);
	
	unsigned int ax = 0;
	unsigned int ay = 0;
	
	// GC
	xcb_gcontext_t thisgc = xcb_generate_id(c);
	xcb_create_gc(c, thisgc, root,
								XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES,
								(uint32_t[]){screen->white_pixel, 0});
	
	// GC: END

	char mod = imagemeta->alpha?4:3;
	int calculated = (imagemeta->height*imagemeta->width);
	while(ay < imagemeta->height) {
		ax = 0;
		while(ax < imagemeta->width) {
			int pix = pixidx(imagemeta->width*mod, ax*mod, ay);
			unsigned char r = imagedata[pix];
			unsigned char g = imagedata[pix+1];
			unsigned char b = imagedata[pix+2];
			unsigned char a = imagedata[pix+3];
			
			xcb_change_gc(c, thisgc, XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES,
										(uint32_t[]){PIX(r, g, b), 0});
			
			xcb_poly_point(c, XCB_COORD_MODE_ORIGIN, root, thisgc, 1, (xcb_point_t[]){
				{
					ax + x, ay + y
				}
			});
			ax += 1;
		}
		ay += 1;
	}
	
	free(imagedata);
	free(imagemeta);

	printf("Calculated: %d\n", calculated);
}
*/

int main() {
	uint32_t             mask = 0;
	uint32_t             values[2];

	c = xcb_connect (NULL, NULL);

	/* get the first screen */
	screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;

	/* root window */
	root = screen->root;

	/* create black (foreground) graphic context */
	foreground = xcb_generate_id(c);
	mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
	xcb_create_gc(c, foreground, root, mask, (uint32_t[]){screen->black_pixel, 0});
	
	background = xcb_generate_id(c);
	mask = XCB_GC_BACKGROUND | XCB_GC_GRAPHICS_EXPOSURES;
	xcb_create_gc(c, background, root, mask, (uint32_t[]){screen->white_pixel, 0});

	fakeroot = xcb_generate_id(c);
	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = DESKTOP_COLOR;
	values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_POINTER_MOTION;
	xcb_create_window(c,
						XCB_COPY_FROM_PARENT,
						fakeroot,
						screen->root,
						0, 0,
						screen->width_in_pixels ,screen->height_in_pixels,
						0,
						XCB_WINDOW_CLASS_INPUT_OUTPUT,
						screen->root_visual,
						mask, values);

	root = xcb_generate_id(c);
	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = screen->white_pixel;
	values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
	xcb_create_window (c,                             /* connection    */
	                   XCB_COPY_FROM_PARENT,          /* depth         */
	                   root,                           /* window Id     */
	                   screen->root,                  /* parent window */
	                   0, 0,                          /* x, y          */
	                   screen->width_in_pixels,
	                   screen->height_in_pixels,      /* width, height */
	                   2,                            /* border_width  */
	                   XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class         */
	                   screen->root_visual,           /* visual        */
	                   mask, values);                 /* masks         */

	/* map the window on the screen */
	xcb_map_window(c, fakeroot);
	xcb_map_window(c, root);
	xcb_flush(c);

        xcb_generic_event_t *event;
        while ((event = xcb_wait_for_event(c))) {
            switch (event->response_type & ~0x80) {
            case XCB_EXPOSE: {
            	printf("Expose!\n");
				xcb_image_text_8(c, strlen(HELLO), root, background,
								 screen->width_in_pixels/2, screen->height_in_pixels/2, HELLO);
				//draw_duke_image("SASH_LILAC.png.duke", 50, 50);
                xcb_flush(c);
                break;
            }
            
            default: break;
            }

            free(event);
        }

	return 0;
}
