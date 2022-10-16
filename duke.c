#include <stdio.h>
#include "duke.h"

void duke_get_image_metadata(struct DukeImageMeta* data, char *filename) {
	FILE* fp = fopen(filename, "r");
	if(!fp) return;
    fread(data, 1, 9, fp);

    fclose(fp);
}

void duke_get_data(struct DukeImageMeta* data, char* filename, char* out) {
	FILE* fp = fopen(filename, "r");
	if(!fp) return;

	fseek(fp, 9, SEEK_SET);
	fread(out, 1, data->data_length, fp);

	fclose(fp);
}

void duke_scale(char* pixels, unsigned int w1, unsigned int h1, int w2, int h2, char alpha, char* out) {
    int scr_w = (w1<<16)/w2;
    int scr_h = (h1<<16)/h2;

    int x = 0;
    int y = 0;

    int x2 = 0;
    int y2 = 0;

    char mod = alpha?4:3;
    while(y<h2) {
        x = 0;
        while(x<w2) {
            x2 = (x*scr_w)>>16;
            y2 = (y*scr_h)>>16;

            out[pixidx(w2*mod, x*mod, y)] = pixels[pixidx(w1*mod, x2*mod, y2)];
            out[pixidx(w2*mod, x*mod, y)+1] = pixels[pixidx(w1*mod, x2*mod, y2)+1];
            out[pixidx(w2*mod, x*mod, y)+2] = pixels[pixidx(w1*mod, x2*mod, y2)+2];
            if(alpha) {
                out[pixidx(w2*mod, x*mod, y)+3] = pixels[pixidx(w1*mod, x2*mod, y2)+3];
            }
            x++;
        }
        y++;
    }
}

unsigned int duke_calculate_bufsize(unsigned int width, unsigned int height, unsigned int alpha) {
    return width*height*(alpha?4:3);
}
