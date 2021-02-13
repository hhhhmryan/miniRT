/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   miniRT.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mryan <mryan@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/05 11:56:39 by mryan             #+#    #+#             */
/*   Updated: 2021/02/13 15:28:27 by mryan            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minilibx_mms/mlx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define INF			65000
#define WINDOW_W	600
#define WINDOW_H	600


// mlx 
typedef struct  s_vars {
        void    *mlx;
        void    *win;
}               t_vars;

typedef struct  s_data {
    void        *img;
    char        *addr;
    int         bits_per_pixel;
    int         line_length;
    int         endian;
}               t_data;

int             close_on_close_button(int keycode, t_vars *vars)
{
	exit(0);
	return (2);
}

int             close_on_esc(int keycode, t_vars *vars)
{
   	if (keycode == 53)
		exit(0);
	return (2);
}

typedef struct  s_vector {
	double		x;
	double		y;
	double		z;
}               t_vector;

typedef struct  s_color {
	int			t;
	int			r;
	int			g;
	int			b;
}               t_color;

typedef struct  s_sphere {
	t_vector	center;
	double		radius;
	t_color		color;
}               t_sphere;

typedef struct  s_result {
	double	t1;
	double	t2;
}               t_result;

typedef struct s_light {
	t_vector	coord;
	t_color		color;
	double		intensity;
}				t_light;

typedef struct	s_ambient_light {
	t_color		color;
	double		intensity;
}				t_ambient_light;

// typedef struct  s_cam {
// 	t_vector	*origin;
// 	t_vector	*direction;
// 	double		fov;
// }				t_cam;

// typedef struct  s_scene {
// 	t_cam 		*cams;
// 	t_sphere	*spheres;
// 	double		width;
// 	double		hight;
// }				t_scene;

t_color	ColorInput(int t, int r, int g, int b)
{
	t_color color;

	color.t = t;
	color.r = r;
	color.g = g;
	color.b = b;
	return (color);
}

int				create_trgb(int t, int r, int g, int b)
{
	return(t << 24 | r << 16 | g << 8 | b);
}

void            my_mlx_pixel_put(t_data *data, int x, int y, t_color color)
{
    char    *dst;

	x = WINDOW_W / 2 + x;
	y = WINDOW_H / 2 - y - 1;

    dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
    *(unsigned int*)dst = create_trgb(color.t, color.r, color.g, color.b);
}

// t_cam	*CamInput(t_vector *origin, t_vector *direction, double fov)
// {
// 	t_cam *cam;

// 	cam->origin = origin;
// 	cam->direction = direction;
// 	cam->fov = fov;
// 	return (cam); 
// }

// t_scene	*SceneInput(t_cam *cams, t_sphere *sphrs, double w, double h)
// {
// 	t_scene *sc;

// 	sc->cams = cams;
// 	sc->spheres = sphrs;
// 	sc->hight = h;
// 	sc->width = w;
// 	return (sc);
// }

t_sphere SphereInput(t_vector center, double radius, t_color color)
{
	t_sphere sphere;

	sphere.center = center;
	sphere.radius = radius;
	sphere.color = color;
	return (sphere);
}

t_vector VectorInput(double x, double y, double z)
{
	t_vector vec;

	vec.x = x;
	vec.y = y;
	vec.z = z;
	return (vec);
}

double	 DotProduct(t_vector *v1, t_vector *v2)
{
	return v1->x*v2->x + v1->y*v2->y + v1->z*v2->z;
}

t_vector Subtract(t_vector *v1, t_vector *v2)
{
	t_vector res;

	res.x = v1->x - v2->x;
	res.y = v1->y - v2->y;
	res.z = v1->z - v2->z;
	return (res);
}

// 
double VectorLength(t_vector *v)
{
	double len;

	len = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
	return (len);
}

double Vw = 1;
double Vh = 1;
double d = 1;

t_vector	WindowToViewPort(int x, int y)
{
	t_vector direction;
	
	direction = VectorInput(x * Vw / WINDOW_W, y * Vh / WINDOW_H, d);
	return (direction);
}

// // переделать уравнение в фкц
// t_result	*IntersectRaySphere(t_vector *origin, t_vector *direction, t_sphere *sphere)
// {
// 	t_vector *oc = Subtract(origin, sphere->center);

// 	double k1 = DotProduct(direction, direction);
// 	double k2 = 2 * DotProduct(oc, direction);
// 	double k3 = DotProduct(oc, oc) - sphere->radius * sphere->radius;
// 	t_result *res;

// 	double discr = k2 * k2 - 4 * k1 * k3;
// 	if (discr < 0)
// 	{
// 		return ;
// 	}
// 	res->t1 = (-k2 + sqrt(discr)) / (2*k1);
// 	res->t2 = (-k2 - sqrt(discr)) / (2*k1);
// 	return res;
// }


t_sphere		sphere1;
t_sphere		sphere2;
t_sphere		sphere3;
t_sphere		big_sphere;
t_sphere		*spheres[] = {&sphere1, &sphere2, &sphere3, &big_sphere, NULL};

t_result QuadraticEq(double k1, double k2, double k3)
{
	t_result	solution;
	double		discr;

	solution.t1 = INF;
	solution.t2 = INF;
	discr = k2 * k2 - 4 * k1 * k3;
	if (discr < 0)
		return(solution);
	solution.t1 = (-k2 + sqrt(discr)) / (2 * k1);
	solution.t2 = (-k2 - sqrt(discr)) / (2 * k1);
	return (solution);
}

t_result	IntercectRaySphere(t_vector *camera_position, t_vector *direction, t_sphere *sphere)
{
	t_vector	oc;
	double		k1;
	double		k2;
	double		k3;

	oc = VectorInput(camera_position->x - sphere->center.x, camera_position->y - sphere->center.y, camera_position->z - sphere->center.z);
	k1 = DotProduct(direction, direction);
	k2 = 2 * DotProduct(&oc, direction);
	k3 = DotProduct(&oc, &oc) - sphere->radius * sphere->radius;
	return (QuadraticEq(k1, k2, k3));
}

double		ComputeLightning(t_vector *sphere_point, t_vector *normal, t_ambient_light *a_light, t_light *light)
{
	double intensity = 0.0;
	t_vector light_vector;
	double scalar_n_to_l;

	intensity = a_light->intensity;
	light_vector = VectorInput(light->coord.x - sphere_point->x, light->coord.y - sphere_point->y, light->coord.z - sphere_point->z);
	scalar_n_to_l = DotProduct(normal, &light_vector);
	if (scalar_n_to_l > 0)
		intensity += light->intensity * scalar_n_to_l / (sqrt(DotProduct(normal, normal)) * sqrt(DotProduct(&light_vector, &light_vector)));
	light_vector = VectorInput(1, 4, 4);
	scalar_n_to_l = DotProduct(normal, &light_vector);
	if (scalar_n_to_l > 0)
		intensity += 0.2 * scalar_n_to_l / (sqrt(DotProduct(normal, normal)) * sqrt(DotProduct(&light_vector, &light_vector)));
	return (intensity);
}

t_color		RayTrace(t_vector *camera_position, t_vector *direction, double t_min, double t_max, t_ambient_light *a_light, t_light *light)
{
	double closest_t = INF;
	t_result inter;
	t_color background_color;
	t_vector sphere_point;
	t_vector normal;
	double tmp;
	t_sphere *closest_sphere = NULL;
	
	background_color = ColorInput(0, 255, 255, 255);
	int i = 0;
	
	while (spheres[i])
	{
		inter = IntercectRaySphere(camera_position, direction, spheres[i]);
		if (t_min <= inter.t1 && inter.t1 <= t_max && inter.t1 < closest_t)
		{
			closest_t = inter.t1;
			closest_sphere = spheres[i];
		}
		if (t_min <+ inter.t2 && inter.t2 <= t_max && inter.t2 < closest_t)
		{
			closest_t = inter.t2;
			closest_sphere = spheres[i];
		}
		i++;
	}
	if (closest_t == INF)
		return (background_color);
	sphere_point = VectorInput(camera_position->x + closest_t * direction->x,
							camera_position->y + closest_t * direction->y, camera_position->z + closest_t * direction->z);
	normal = Subtract(&sphere_point, &(closest_sphere->center));
	tmp = sqrt(DotProduct(&normal, &normal));
	normal = VectorInput(normal.x / tmp, normal.y / tmp, normal.z / tmp);
	tmp = ComputeLightning(&sphere_point, &normal, a_light, light);
	return (ColorInput(0, closest_sphere->color.r, closest_sphere->color.g, closest_sphere->color.b));
}

void		RenderScene(t_data *img)
{
	t_vector		camera_position;
	int				x;
	int				y;
	t_color			color;
	t_vector		direction;
	t_ambient_light a_light;
	t_light			light;
	
	// задача параметров
	x = -WINDOW_W / 2;
	sphere1.center = VectorInput(0, -1, 3);
	sphere1.radius = 1;
	sphere1.color = ColorInput(0, 255, 0, 0);
	sphere2.center = VectorInput(2, 0, 4);
	sphere2.radius = 1;
	sphere2.color = ColorInput(0, 0, 0, 255);
	sphere3.center = VectorInput(-2, 0, 4);
	sphere3.radius = 1;
	sphere3.color = ColorInput(0, 0, 255, 0);
	big_sphere.center = VectorInput(0, - 5001, 0);
	big_sphere.radius = 5000;
	big_sphere.color = ColorInput(0, 255, 255, 0);
	a_light.intensity = 0.2;
	a_light.color = ColorInput(0, 255, 255, 255);
	light.intensity = 0.6;
	light.color = ColorInput(0, 10, 0, 255);
	light.coord = VectorInput(2.0, 1.0, 0.0);
	camera_position = VectorInput(0.0, 0.0, 0.0);

	while (x < WINDOW_W / 2)
	{
		y = -WINDOW_H / 2;
		while (y < WINDOW_H / 2) 
		{
			direction = WindowToViewPort(x, y);
			color = RayTrace(&camera_position, &direction, 1, INF, &a_light, &light);
			my_mlx_pixel_put(img, x, y, color);
			y++;
		}
		x++;
	}

}

int main(void)
{
	t_vars	vars;
	t_data	img;

	vars.mlx = mlx_init();
	vars.win = mlx_new_window(vars.mlx, WINDOW_W, WINDOW_H, "Window");
	img.img = mlx_new_image(vars.mlx, WINDOW_W, WINDOW_H);
	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
	RenderScene(&img);
	mlx_put_image_to_window(vars.mlx, vars.win, img.img, 0, 0);
	mlx_hook(vars.win, 2, 1L<<0, close_on_esc, &vars);
	mlx_hook(vars.win, 17, 1L<<0, close_on_close_button, &vars);
	mlx_loop(vars.mlx);
}
