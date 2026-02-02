#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define WIDTH 3200
#define HEIGHT 3200
#define M_PI 3.14159265358979323846

void _calc_pixel(float x, float y, int* px, int* py) {
    *px = (int)((x + 1) * 0.5 * (WIDTH - 1));
    *py = (int)((1 - (y + 1) * 0.5) * (HEIGHT - 1));
}

void _set_pixel(unsigned char* buffer, int px, int py, float intensity) {
    if (!(px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT)) {
        printf("Invalid pixel coordinates: (%d, %d)\n", px, py);
        return;
    }

    int pixel_idx = (py * WIDTH + px) * 3;
    buffer[pixel_idx] = (unsigned char)(intensity * 255);
    buffer[pixel_idx + 1] = (unsigned char)(intensity * 192);
    buffer[pixel_idx + 2] = (unsigned char)(intensity * 203);
}

void draw_line(unsigned char* buffer, float start_x, float start_y, float end_x, float end_y) {
    float dx = end_x - start_x;
    float dy = end_y - start_y;

    int px_start, px_end, py_start, py_end;
    _calc_pixel(start_x, start_y, &px_start, &py_start);
    _calc_pixel(end_x, end_y, &px_end, &py_end);

    int pixel_dx = px_end - px_start;
    int pixel_dy = py_end - py_start;
    int pixel_dist = (int)sqrt(pixel_dx * pixel_dx + pixel_dy * pixel_dy);

    int steps = pixel_dist > 0 ? pixel_dist : 1;

    for (int i = 0; i <= steps; i++) {
        float t = (float)i / (float)steps;

        float x = start_x + t * dx;
        float y = start_y + t * dy;

        int px, py;
        _calc_pixel(x, y, &px, &py);
        _set_pixel(buffer, px, py, 1.0);
    }
}

void draw_thick_line(unsigned char* buffer, float start_x, float start_y, float end_x, float end_y, int thickness) {
    float dx = end_x - start_x;
    float dy = end_y - start_y;

    int px_start, px_end, py_start, py_end;
    _calc_pixel(start_x, start_y, &px_start, &py_start);
    _calc_pixel(end_x, end_y, &px_end, &py_end);

    int pixel_dx = px_end - px_start;
    int pixel_dy = py_end - py_start;
    int pixel_dist = (int)sqrt(pixel_dx * pixel_dx + pixel_dy * pixel_dy);

    int steps = pixel_dist > 0 ? pixel_dist : 1;

    for (int i = 0; i <= steps; i++) {
        float t = (float)i / (float)steps;

        float x = start_x + t * dx;
        float y = start_y + t * dy;

        int px, py;
        _calc_pixel(x, y, &px, &py);
        for (int dy_off = -thickness; dy_off <= thickness; dy_off++) {
            for (int dx_off = -thickness; dx_off <= thickness; dx_off++) {
                int dist_sq = dx_off * dx_off + dy_off * dy_off;

                if (dist_sq <= thickness * thickness) {
                    int px_ = px + dx_off;
                    int py_ = py + dy_off;
                    _set_pixel(buffer, px_, py_, 1.0);
                }
            }
        }
    }
}

void draw_tree(unsigned char* buffer, float start_x, float start_y, float end_x, float end_y, float theta, int depth, int init_thick) {
    draw_thick_line(buffer, start_x, start_y, end_x, end_y, init_thick);

    if (depth <= 0) return;

    float dx = end_x - start_x, dy = end_y - start_y;

    float parent_length = sqrt(dx * dx + dy * dy);
    float child_length = parent_length / 2.0;

    float parent_angle = atan2(dy, dx);
    float left_angle = parent_angle + theta, right_angle = parent_angle - theta;

    float l_end_x = end_x + child_length * cos(left_angle);
    float l_end_y = end_y + child_length * sin(left_angle);
    
    float r_end_x = end_x + child_length * cos(right_angle);
    float r_end_y = end_y + child_length * sin(right_angle);

    int new_thick = (int)(init_thick * 0.7);
    if (new_thick < 1) new_thick = 1;

    draw_tree(buffer, end_x, end_y, l_end_x, l_end_y, theta, depth - 1, new_thick);
    draw_tree(buffer, end_x, end_y, r_end_x, r_end_y, theta, depth - 1, new_thick);
}

int main(){
    unsigned char *buffer = (unsigned char*)malloc(WIDTH * HEIGHT * 3);

    memset(buffer, 0, WIDTH * HEIGHT * 3);

    float theta = M_PI / 4.0;
    int depth = 8;
    int init_thickness = 20;

    draw_tree(buffer, 0.0, 0.0, 0.0, 0.5, theta, depth, init_thickness);
    draw_tree(buffer, 0.0, 0.0, 0.0, -0.5, theta, depth, init_thickness);
    draw_tree(buffer, 0.0, 0.0, -0.5, 0.0, theta, depth, init_thickness);
    draw_tree(buffer, 0.0, 0.0, 0.5, 0.0, theta, depth, init_thickness);

    draw_tree(buffer, 0.0, 0.5, 0.0, 0.0,theta, depth, init_thickness);
    draw_tree(buffer, 0.0, -0.5, 0.0, 0.0, theta, depth, init_thickness);
    draw_tree(buffer, -0.5, 0.0, 0.0, 0.0,theta, depth, init_thickness);
    draw_tree(buffer, 0.5, 0.0, 0.0, 0.0,theta, depth, init_thickness);

    printf("Buffer created.\n");

    FILE *fp = fopen("output.ppm", "wb");
    fprintf(fp, "P6\n%d %d\n255\n", WIDTH, HEIGHT);

    fwrite(buffer, 1, WIDTH * HEIGHT * 3, fp);
    fclose(fp);
    printf("Image saved to output.ppm\n");

    system("convert output.ppm output.png");
    printf("Image saved as output.png\n");

    free(buffer);

    printf("Buffer deallocated.\n");

    return 0;
}