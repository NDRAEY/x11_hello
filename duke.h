#ifndef IMAGING_H
#define IMAGING_H

#define pixidx(width, x, y) (width*y + x)

struct DukeImageMeta {
    short width;
    short height;
    int data_length;
    char alpha;
};

void duke_get_image_metadata(struct DukeImageMeta*, char *filename);
void duke_get_data(struct DukeImageMeta* meta, char* filename, char* out);
unsigned int duke_calculate_bufsize(unsigned int width, unsigned int height, unsigned int alpha);
void duke_scale(char* pixels, unsigned int w1, unsigned int h1, int w2, int h2, char alpha, char* out);

#endif
