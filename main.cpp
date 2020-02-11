#include "Plane.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"
#include "Surface.hpp"
#include "Vect3.hpp"

#include <SDL2/SDL.h>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#define MAX_RAY_DEPTH 5


double mix(const double & a, const double & b, const double & mix) { return b * mix + a * (1 - mix); }

unsigned long createRGB(int r, int g, int b) { return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff); }

int createRGBA(int r, int g, int b, int a) { return ((r & 0xff) << 24) + ((g & 0xff) << 16) + ((b & 0xff) << 8) + (a & 0xff); }

Vect3f trace(const Ray & ray_shot, const std::vector<Surface *> & surfaces, const int & depth)
{
    // if (raydir.length() != 1) std::cerr << "Error " << raydir << std::endl;
    float tnear       = INFINITY;
    Surface * surface = NULL;
    // find intersection of this ray with the sphere in the scene
    for (unsigned i = 0; i < surfaces.size(); ++i)
    {
        double t0 = INFINITY, t1 = INFINITY;
        if (surfaces[i]->intersect(ray_shot, t0, t1))
        {
            if (t0 < 0) t0 = t1;
            if (t0 < tnear)
            {
                tnear   = t0;
                surface = surfaces[i];
                // return surfaces[i]->surface_color;
            }
        }
    }
    // if there's no intersection return black or background color
    if (!surface) return Vect3f(2);

    Vect3f surfaceColor = 0;                                           // color of the ray/surfaceof the object intersected by the ray
    Vect3f phit         = ray_shot.point + ray_shot.direction * tnear; // point of intersection
    Vect3f nhit         = surface->normal_hit(phit);                   // normal at the intersection point

    nhit.normalize(); // normalize normal direction
    // If the normal and the view direction are not opposite to each other
    // reverse the normal direction. That also means we are inside the sphere so
    // set the inside bool to true. Finally reverse the sign of IdotN which we
    // want positive.
    float bias  = 1e-4; // add some bias to the point from which we will be tracing
    bool inside = false;
    if (ray_shot.direction.dot(nhit) > 0) nhit = -nhit, inside = true;
    if ((surface->transparency > 0 || surface->reflection > 0) && depth < MAX_RAY_DEPTH)
    {
        float facingratio = -ray_shot.direction.dot(nhit);
        // change the mix value to tweak the effect
        float fresneleffect = mix(pow(1 - facingratio, 3), 1, 0.1);
        // compute reflection direction (not need to normalize because all
        // vectors are already normalized)
        Vect3f refldir = ray_shot.direction - nhit * 2 * ray_shot.direction.dot(nhit);
        refldir.normalize();
        Vect3f reflection = trace(Ray(phit + nhit * bias, refldir), surfaces, depth + 1);
        Vect3f refraction = 0;
        // if the sphere is also transparent compute refraction ray
        // (transmission)
        if (surface->transparency)
        {
            float ior      = 1.1,
                  eta      = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
            float cosi     = -nhit.dot(ray_shot.direction);
            float k        = 1 - eta * eta * (1 - cosi * cosi);
            Vect3f refrdir = ray_shot.direction * eta + nhit * (eta * cosi - sqrt(k));
            refrdir.normalize();
            refraction = trace(Ray(phit - nhit * bias, refrdir), surfaces, depth + 1);
        }
        // the result is a mix of reflection and refraction (if the sphere is
        // transparent)
        surfaceColor = (reflection * fresneleffect + refraction * (1 - fresneleffect) * surface->transparency) * surface->surface_color;
    }
    else
    {
        // it's a diffuse object, no need to raytrace any further
        for (unsigned i = 0; i < surfaces.size(); ++i)
        {
            // std::cout << surface[i].emission_color.x <<
            // surface[i].emission_color.y << surface[i].emission_color.z <<
            // std::endl;
            if (surfaces[i]->emission_color.x > 0)
            {
                // this is a light
                Vect3f transmission = 1;

                Vect3f lightDirection = surfaces[i]->center - phit;
                lightDirection.normalize();
                for (unsigned j = 0; j < surfaces.size(); ++j)
                {
                    if (i != j)
                    {
                        double t0, t1;
                        if (surfaces[j]->intersect(Ray(phit + nhit * bias, lightDirection), t0, t1))
                        {
                            transmission = 0;
                            break;
                        }
                    }
                }
                surfaceColor += surface->surface_color * transmission * std::max(double(0), nhit.dot(lightDirection)) * surfaces[i]->emission_color;
            }
        }
    }

    return surfaceColor + surface->emission_color;
}


int main(int argc, char ** argv)
{

    bool leftMouseButtonDown = false;
    bool quit                = false;
    bool rendered            = false;

    constexpr unsigned int WIDTH  = 1080; // 1600;
    constexpr unsigned int HEIGHT = 840;  // 1200;

    Uint32 * pixels = new Uint32[WIDTH * HEIGHT];

    double invWidth     = 1 / double(WIDTH);
    double invHeight    = 1 / double(HEIGHT);
    double fov          = 30;
    double angle        = std::tan(M_PI * 0.5 * fov / 180);
    double aspect_ratio = (WIDTH) / double(HEIGHT);

    Vect3f camera_origin(0, 0, 0);
    Vect3f camera_up(0, 1, 0);

    SDL_Event event;


    std::vector<Surface *> surfaces;

    surfaces.push_back(new Plane(Vect3f(0, 1, 0), Vect3f(0, -10, 0), Vect3f(0.0, 0.80, 0.20), 0, 0, Vect3f(0)));

    surfaces.push_back(new Sphere(Vect3f(0.0, 0, -20), 4, Vect3f(1, 1, 1), 0, 0, Vect3f(0)));
    // light
    surfaces.push_back(new Sphere(Vect3f(20, 20, -20), 2, Vect3f(0.00, 0.00, 0.00), 0, 0.0, Vect3f(1)));


    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window     = SDL_CreateWindow("SDL2 Pixel Drawing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture * texture   = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
    memset(pixels, 255, WIDTH * HEIGHT * sizeof(Uint32));


    std::vector<std::vector<int>> canvas = {};
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            std::vector<int> point = { x, y, 0 };
            canvas.push_back(point);
        }
    }

    while (!quit)
    {
        SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(Uint32));

        SDL_WaitEvent(&event);

        switch (event.type)
        {
            case SDL_QUIT: quit = true; break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_LEFT:
                        camera_origin.x -= 2;
                        rendered = false;
                        break;
                    case SDLK_RIGHT:
                        camera_origin.x += 2;
                        rendered = false;
                        break;
                    case SDLK_UP:
                        camera_origin.y += 2;
                        rendered = false;
                        break;
                    case SDLK_DOWN:
                        camera_origin.y -= 2;
                        rendered = false;
                        break;
                    case SDLK_w:
                        camera_origin.z -= 2;
                        rendered = false;
                        break;
                    case SDLK_s:
                        camera_origin.z += 2;
                        rendered = false;
                        break;
                }
        }

        if (!rendered)
        {
            for (unsigned y = 0; y < HEIGHT; ++y)
            {
                for (unsigned x = 0; x < WIDTH; ++x)
                {

                    double xx = (2 * ((canvas.at(WIDTH * y + x).at(0) + 0.5) * invWidth) - 1) * angle * aspect_ratio;
                    double yy = (1 - 2 * ((canvas.at(WIDTH * y + x).at(1) + 0.5) * invHeight)) * angle;

                    Ray shoot_ray(camera_origin, Vect3f(xx, yy, -1).normalize());
                    Vect3f color = trace(shoot_ray, surfaces, 0);
                    Ray r(camera_origin, Vect3f(xx, yy, -1).normalize());


                    color.x = std::min(double(1), color.x) * 255;
                    color.y = std::min(double(1), color.y) * 255;
                    color.z = std::min(double(1), color.z) * 255;

                    pixels[WIDTH * y + x] = createRGB(color.x, color.y, color.z);
                }
            }

            rendered = true;
        }


        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }


    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
