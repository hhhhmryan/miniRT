/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test1.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mryan <mryan@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/13 13:24:34 by mryan             #+#    #+#             */
/*   Updated: 2021/02/13 14:57:01 by mryan            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./minilibx/mlx.h"
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include <math.h>
​
#define INF			65000
#define WINDOW_W	600
#define WINDOW_H	600
​
typedef struct  s_vars {
        void    *mlx;
        void    *win;
}               t_vars;
​
typedef struct  s_data {
    void        *img;
    char        *addr;
    int         bits_per_pixel;
    int         line_length;
    int         endian;
}               t_data;
​
typedef struct	s_intersec_param {
	double		t1;
	double		t2;
}				t_intersec_param;
​
typedef struct	s_color {
	int			t;
	int			r;
	int			g;
	int			b;
}				t_color;
​
typedef struct	s_sphere {
	t_vector	center;
	double		radius;
	t_color		color;
}				t_sphere;
​
typedef struct	s_ambient_light {
	t_color		color;
	double		intensity;
}				t_ambient_light;
​
typedef struct s_light {
	t_vector	coord;
	t_color		color;
	double		intensity;
}				t_light;
​
t_color			set_color(int t , int r, int g, int b)
{
	t_color		color;
​
	color.t = t;
	color.r = r;
	color.g = g;
	color.b = b;
	return (color);
}
​
t_vector		set_coordinates(double x, double y, double z)
{
	t_vector vector;
	vector.x = x;
	vector.y = y;
	vector.z = z;
	return (vector);
}
​
t_vector		vectors_diff(t_vector *v1, t_vector *v2)
{
	t_vector res;
​
	res.x = v1->x - v2->x;
	res.y = v1->y - v2->y;
	res.z = v1->z - v2->z;
	return (res);
}	
​
double			scalar_product(t_vector *v1, t_vector *v2)
{
	return ((v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z));
}
​
int				create_trgb(int t, int r, int g, int b)
{
	return(t << 24 | r << 16 | g << 8 | b);
}
​
int             close_on_esc(int keycode, t_vars *vars)
{
   	if (keycode == 53)
		exit(0);
	return 2;
}
​
int             close_on_close_button(int keycode, t_vars *vars)
{
	exit(0);
	return 2;
}
​
void            my_mlx_pixel_put(t_data *data, int x, int y, t_color color)
{
    char    *dst;
​
	x = WINDOW_W / 2 + x;
	y = WINDOW_H / 2 - y - 1;
	if (x < 0 || x >= WINDOW_W || y < 0 || y >= WINDOW_H)
    	return;
    dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
    *(unsigned int*)dst = create_trgb(color.t, color.r, color.g, color.b);
}
​
double Vw = 1;
double Vh = 1;
double d = 1;
​
t_vector		window_to_viewport(int x, int y)
{
	t_vector direction;
	direction = set_coordinates(x * Vw / WINDOW_W, y * Vh / WINDOW_H, d);
	return (direction);
}
​
t_intersec_param quadratic_eq(double k1, double k2, double k3)
{
	t_intersec_param	solution;
	double				discr;
​
	solution.t1 = INF;
	solution.t2 = INF;
	discr = k2 * k2 - 4 * k1  * k3;
	if (discr < 0)
		return (solution);
	solution.t1 = (-k2 + sqrt(discr)) / (2 * k1);
	solution.t2 = (-k2 - sqrt(discr)) / (2 * k1);
	return (solution);
}
​
t_intersec_param intercect_ray_sphere(t_vector *camera_pos, t_vector *direction, t_sphere *sphere)
{
	t_vector			oc;
	double				k1;
	double				k2;
	double				k3;
	
​
	oc = set_coordinates(camera_pos->x - sphere->center.x, camera_pos->y - sphere->center.y, camera_pos->z - sphere->center.z);
	k1 = scalar_product(direction, direction);
	k2 = 2 * scalar_product(&oc, direction);
	k3 = scalar_product(&oc, &oc) - sphere->radius * sphere->radius;
	return (quadratic_eq(k1, k2, k3));
}	
​
double			compute_lighting(t_vector *sphere_point, t_vector *normal, t_ambient_light *a_linght, t_light*light)
{
	double intensity = 0.0;
	t_vector light_vector;
	double scalar_n_to_l;
​
	intensity = a_linght->intensity;
	light_vector = set_coordinates(light->coord.x - sphere_point->x, light->coord.y - sphere_point->y, light->coord.z - sphere_point->z);
	scalar_n_to_l = scalar_product(normal, &light_vector);
	if (scalar_n_to_l > 0)
		intensity += light->intensity * scalar_n_to_l / (sqrt(scalar_product(normal, normal)) * sqrt(scalar_product(&light_vector, &light_vector)));
	light_vector = set_coordinates(1, 4, 4);
	scalar_n_to_l = scalar_product(normal, &light_vector);
	if (scalar_n_to_l > 0)
		intensity += 0.2 * scalar_n_to_l / (sqrt(scalar_product(normal, normal)) * sqrt(scalar_product(&light_vector, &light_vector)));	
	return (intensity);
}
​
// t_color			trace_ray(t_sphere *sphere, t_vector *camera_pos, t_vector *direction, double t_min, double t_max, t_ambient_light *a_light, t_light *light)
// {
// 	double closest_t = INF;
// 	t_intersec_param inter;
// 	t_sphere *closest_sphere = NULL;
// 	t_color  background_color;
// 	t_vector P;
// 	t_vector N;
// 	double tmp;
	
// 	background_color = set_color(0, 255, 255, 255);
// 	inter = intercect_ray_sphere(camera_pos, direction, sphere);
// 	if (t_min <= inter.t1 && inter.t1 <= t_max && inter.t1 < closest_t)
// 	{
// 		closest_t = inter.t1;
// 		closest_sphere = sphere;
// 	}
// 	if (t_min <= inter.t2 && inter.t2 <= t_max && inter.t2 < closest_t)
// 	{
// 		closest_t = inter.t2;
// 		closest_sphere = sphere;
// 	}
// 	if (!closest_sphere)
// 		return (background_color);
// 	P = set_coordinates(camera_pos->x + closest_t * direction->x, camera_pos->y + closest_t * direction->y, camera_pos->z + closest_t * direction->z);
// 	N = vectors_diff(&P, &(closest_sphere->center));
// 	tmp = sqrt(scalar_product(&N, &N));
// 	N = set_coordinates(N.x / tmp, N.y / tmp, N.z / tmp);
// 	tmp = compute_lighting(&P, &N, a_light, light);
// 	closest_sphere->color = set_color(0, closest_sphere->color.r * tmp, closest_sphere->color.g * tmp, closest_sphere->color.b *tmp);
// 	return (closest_sphere->color);
// }
​
t_sphere		sphere1;
t_sphere		sphere2;
t_sphere		sphere3;
t_sphere		big_sphere;
t_sphere		*spheres[] = {&sphere1, &sphere2, &sphere3, &big_sphere, NULL};
​
​
t_color			trace_ray(t_vector *camera_pos, t_vector *direction, double t_min, double t_max, t_ambient_light *a_light, t_light *light)
{
	double closest_t = INF;
	t_intersec_param inter;
	t_color  background_color;
	t_vector sphere_point;
	t_vector normal;
	double tmp;
	t_sphere *closest_sphere = NULL;
	background_color = set_color(0, 255, 255, 255);
	int i = 0;
​
	while (spheres[i])
	{
		inter = intercect_ray_sphere(camera_pos, direction, spheres[i]);
		if (t_min <= inter.t1 && inter.t1 <= t_max && inter.t1 < closest_t)
		{
			closest_t = inter.t1;
			closest_sphere = spheres[i];
		}
		if (t_min <= inter.t2 && inter.t2 <= t_max && inter.t2 < closest_t)
		{
			closest_t = inter.t2;
			closest_sphere = spheres[i];
		}
		i++;
	}
	if (closest_t == INF)
		return (background_color);
	sphere_point = set_coordinates(camera_pos->x + closest_t * direction->x, camera_pos->y + closest_t * direction->y, camera_pos->z + closest_t * direction->z);
	normal = vectors_diff(&sphere_point, &(closest_sphere->center));
	tmp = sqrt(scalar_product(&normal, &normal));
	normal = set_coordinates(normal.x / tmp, normal.y / tmp, normal.z / tmp);
	tmp = compute_lighting(&sphere_point, &normal, a_light, light);
	return (set_color(0, closest_sphere->color.r * tmp, closest_sphere->color.g * tmp, closest_sphere->color.b * tmp));
}
​
void			render_scene(t_data *img)
{
	t_vector	camera_pos;
	int				x = -WINDOW_W / 2;
	int				y;
	t_color			color;
	
	t_vector		direction;
	t_ambient_light	a_light;
	t_light			light;
​
	sphere1.center = set_coordinates(0, -1, 3);
	sphere1.radius = 1;
	sphere1.color = set_color(0, 255, 0, 0);
	sphere2.center = set_coordinates(2, 0, 4);
	sphere2.radius = 1;
	sphere2.color = set_color(0, 0, 0, 255);
	sphere3.center = set_coordinates(-2, 0, 4);
	sphere3.radius = 1;
	sphere3.color = set_color(0, 0, 255, 0);
	big_sphere.center = set_coordinates(0, -5001, 0);
	big_sphere.radius = 5000;
	big_sphere.color = set_color(0, 255, 255, 0);
	a_light.intensity = 0.2;
	a_light.color = set_color(0, 255, 255, 255);
	light.intensity = 0.6;
	light.color = set_color(0, 10, 0, 255);
	light.coord = set_coordinates(2.0, 1.0, 0.0);
 	camera_pos = set_coordinates(0.0, 0.0, 0.0);
	while (x < WINDOW_W/ 2)
	{
		y = -WINDOW_H/ 2;
		while (y < WINDOW_H/ 2)
		{
			direction = window_to_viewport(x, y);
			color = trace_ray(&camera_pos, &direction, 1, INF, &a_light, &light);
			my_mlx_pixel_put(img, x, y, color);
			y++;
		}
		x++;
	}
}
​
int             main(void)
{
    t_vars  vars;
    t_data  img;
​
​
    vars.mlx = mlx_init();
    vars.win = mlx_new_window(vars.mlx, WINDOW_W, WINDOW_H, "Hello world!");
    img.img = mlx_new_image(vars.mlx, WINDOW_W, WINDOW_H);
    img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length,
                                &img.endian);
	render_scene(&img);
    mlx_put_image_to_window(vars.mlx, vars.win, img.img, 0, 0);
    mlx_hook(vars.win, 2, 1L<<0, close_on_esc, &vars);
    mlx_hook(vars.win, 17, 1L<<0, close_on_close_button, &vars);
    mlx_loop(vars.mlx);
}