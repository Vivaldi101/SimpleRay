#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include <float.h>
#include <assert.h>

#define internal static
#define persistent static
#define global static
#define BYTES_PER_PIXEL 4
#define R32_MIN -FLT_MAX
#define R32_MAX FLT_MAX
#ifdef ERROR
#undef ERROR
#endif // ERROR
#define ERROR assert(0);

#define ArrayCount(arr) (sizeof(arr)/sizeof(arr[0]))
#define NUM_RAYS_PER_PIXEL (1<<4)
#define NUM_RAY_BOUNCES (1<<6)

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef unsigned char byte;

typedef u32 b32;

typedef float r32;
typedef double r64;

#include "ray_math.h"
#include "bmp.h"

// ray_origin is in Worldspace
   internal vec3_t
RayCast(world_t *world, vec3_t ray_origin, vec3_t ray_dir)
{
   vec3_t result = {};
   vec3_t attenuation = {1.0f, 1.0f, 1.0f};

   r32 tolerance = 0.00001f;
   r32 min_distance = 0.01f;
   vec3_t next_normal = {};
   vec3_t next_origin = {};
   for(u32 ray = 0; ray < NUM_RAY_BOUNCES; ray++)
   {
      u32 hit_material_index = 0;
      r32 hit_distance = R32_MAX;
      for(u32 i = 0; i < world->num_planes; i++)
      {
	 plane_t plane = world->planes[i];

	 r32 denom = Dot3(plane.n, ray_dir);
	 if((denom < -tolerance) || (denom > tolerance))
	 {
	    r32 t = (-plane.d - Dot3(plane.n, ray_origin)) / denom;
	    if((t < hit_distance) && (t > min_distance))
	    {
	       hit_distance = t;
	       next_origin = ray_origin + ray_dir*hit_distance;
	       next_normal = Vec3Norm(plane.n);
	       hit_material_index = plane.material_index;
	    }
	 }
      }

      for(u32 i = 0; i < world->num_spheres; i++)
      {
	 sphere_t sphere = world->spheres[i];

	 // Translate the ray origin
	 vec3_t sphere_to_origin = ray_origin - sphere.p;
	 r32 a = Dot3(ray_dir, ray_dir);
	 r32 b = 2.0f*Dot3(ray_dir, sphere_to_origin);
	 r32 c = Dot3(sphere_to_origin, sphere_to_origin) - sphere.r*sphere.r;

	 r32 denom = 2.0f*a;
	 r32 root_term = (r32)sqrt(b*b - 4.0f*a*c);
	 if(root_term > tolerance)
	 {
	    r32 tp = (-b + root_term) / denom;
	    r32 tn = (-b - root_term) / denom;

	    r32 t = tp;
	    if((tn > 0) && (tn < tp))
	    {
	       t = tn;
	    }
	    if((t < hit_distance) && (t > min_distance))
	    {
	       hit_distance = t;
	       next_origin = ray_origin + ray_dir*hit_distance;
	       next_normal = Vec3Norm(next_origin - sphere.p);
	       hit_material_index = sphere.material_index;
	    }
	 }
      }
      if(hit_material_index)
      {
	 material_t mat = world->materials[hit_material_index];
	 r32 cos_term = Dot3(-ray_dir, next_normal);

	 if(cos_term < 0.0f)
	 {
	    cos_term = 0.0f;
	 }
	 result = result + Hadamard(mat.emit_color, attenuation);
	 attenuation = Hadamard(cos_term*mat.refl_color, attenuation);

	 //vec3_t pure_bounce = ray_origin + (2.0f*Dot3(Vec3Norm(ray_dir), next_normal)*next_normal) - Vec3Norm(ray_dir);

	 vec3_t unit_ray_dir = Vec3Norm(ray_origin - next_origin);

	 ray_origin = next_origin;
	 vec3_t pure_bounce =
	    2.0f*Dot3(next_normal, unit_ray_dir)*next_normal - unit_ray_dir;


	 vec3_t random_bounce = Vec3Norm(next_normal + vec3_t{RandomBilateral(), RandomBilateral(), RandomBilateral()});

	 ray_dir = Vec3Norm(Lerp(random_bounce, pure_bounce, mat.scatter));
	 //ray_dir = Vec3Norm(random_bounce);
      }
      else
      {
	 material_t mat = world->materials[hit_material_index];
	 result = result + Hadamard(mat.emit_color, attenuation);

	 break;
      }
   }

   return result;
}

   internal void
RayTrace(world_t *world, image_t *image, viewpoint_t *eye)
{
   u32 *pixel = image->pixels;
   r32 pixel_filter = 1.0f / (r32)NUM_RAYS_PER_PIXEL;
   u32 image_width = image->width;
   u32 image_height = image->height;
   r32 image_plane_x = 1.0f;
   r32 image_plane_y = 1.0f;

   if(image_width < image_height)
   {
      r32 image_ratio = ((r32)image_width / (r32)image_height);
      image_plane_x = image_plane_y * image_ratio;
   }
   else if(image_width > image_height)
   {
      r32 image_ratio = ((r32)image_height / (r32)image_width);
      image_plane_y = image_plane_x * image_ratio;
   }
   r32 image_plane_width = (image_plane_x - (-image_plane_x));
   r32 image_plane_height = (image_plane_y - (-image_plane_y));
   r32 eye_dist = 1.0f;
   vec3_t eye_look_at = eye->pos - eye_dist*eye->local_z;
   vec3_t look_at_min = eye_look_at - (eye->local_x + eye->local_y);

   for(u32 y = 0; y < image->height; y++)
   {
      r32 image_y = ((image_plane_height / (r32)(image_height-1))*(y - 0)) - image_plane_y;
      for(u32 x = 0; x < image->width; x++)
      {
	 r32 image_x = ((image_plane_width / (r32)(image_width-1))*(x - 0)) - image_plane_x;

	 vec3_t point_in_plane = look_at_min + (image_x+1.0f)*eye->local_x + (image_y+1.0f)*eye->local_y;

	 vec3_t ray_origin = eye->pos;   // in Worldspace
	 vec3_t ray_dir = Vec3Norm(point_in_plane - ray_origin);

	 vec3_t temp_color = {};
	 for(u32 ray = 0; ray < NUM_RAYS_PER_PIXEL; ray++)
	 {
	    temp_color = temp_color + pixel_filter*RayCast(world, ray_origin, ray_dir);
	 }
	 vec4_t final_color = 
	 {
	    ExactLinearTosRGB(temp_color.r), 
	    ExactLinearTosRGB(temp_color.g), 
	    ExactLinearTosRGB(temp_color.b), 
	    255.0f
	 };
	 *pixel++ = PackRGBA(final_color);
      }
      printf("\rRaycasting: %d%%", (100*(y+1))/image_height);
      fflush(stdout);
   }
}

   internal void
SaveBMP(image_t *image, char *filename)
{
   bmp_t header = {0};
   header.file_type = 0x4d42;
   header.file_size = sizeof(header) + image->num_bytes;
   header.bmp_offset = sizeof(header);
   header.size = sizeof(header) - 14;
   header.width = image->width;
   header.height = image->height;
   header.planes = 1;
   header.bpp = 32;
   header.compression = 0;
   header.size_of_bmp = image->num_bytes;
   header.horz_res = 0;
   header.vert_hes = 0;
   header.colors_used = 0;
   header.colors_important = 0;

   FILE *bmp_output_file = fopen(filename, "wb");

   if(bmp_output_file)
   {
      fwrite(&header, sizeof(header), 1, bmp_output_file);
      fwrite(image->pixels, image->num_bytes, 1, bmp_output_file);
      fclose(bmp_output_file);
   }
   else
   {
      fprintf(stderr, "Unable to write output file %s", filename);
      ERROR;
   }
}

   internal image_t
CreateImage(u32 width, u32 height, void* (*allocator)(u32 num_bytes))
{
   image_t result = {0};
   result.width = width;
   result.height = height;
   result.num_bytes = result.width*result.height*BYTES_PER_PIXEL;
   if(!allocator)
   {
      result.pixels = (u32 *)malloc(result.num_bytes);
   }
   else
   {
      result.pixels = (u32 *)allocator(result.num_bytes);
   }
   if(!result.pixels)
   {
      fprintf(stderr, "Error allocating memory for image!\n");
      ERROR;
   }

   return result;
}

   int 
main(int argc, char **argv)
{
   image_t ray_traced_image = CreateImage(1280, 720, NULL);

   material_t materials[7] = {};
   materials[0].refl_color = {0.4f, 0.4f, 0.4f};
   materials[1].refl_color = {0.3f, 0.3f, 0.3f};
   materials[1].scatter = 0.75f;
   materials[2].refl_color = {0.7f, 0.5f, 0.3f};
   materials[3].emit_color = {5.0f, 0.0f, 0.0f};
   materials[4].refl_color = {0.2f, 0.8f, 0.2f};
   materials[4].scatter = 0.7f;
   materials[5].refl_color = {0.4f, 0.8f, 0.9f};
   materials[5].scatter = 1.0f;
   materials[6].emit_color = {1.0f, 1.0f, 2.0f};

   // Worldspace: x goes to right, y goes forward, z goes up the plane
   plane_t planes[5] = {};
   planes[0].n = {0.0f, 0.0f, 1.0f};
   planes[0].d = 0;
   planes[0].material_index = 1;

   planes[1].n = {-0.707f, -0.707f, 0.0f};
   planes[1].d = 10;
   planes[1].material_index = 1;

   planes[2].n = {0.707f, -0.707f, 0.0f};
   planes[2].d = 10;
   planes[2].material_index = 1;

   planes[3].n = {0.0f, -1.0f, 0.0f};
   planes[3].d = 10;
   planes[3].material_index = 1;

   planes[4].n = {0.0f, 0.0f, -1.0f};
   planes[4].d = 80;
   planes[4].material_index = 6;


   sphere_t spheres[4] = {};
   spheres[0].p = {0.0f, 0.0f, 0.0f};
   spheres[0].r = 3.5f;
   spheres[0].material_index = 3;

   spheres[1].p = {3.0f, -2.0f, 0.0f};
   spheres[1].r = 1.0f;
   spheres[1].material_index = 2;

   spheres[2].p = {-2.0f, -1.0f, 2.0f};
   spheres[2].r = 1.0f;
   spheres[2].material_index = 4;

   spheres[3].p = {0.0f, 9.0f, 10.0f};
   spheres[3].r = 1.0f;
   spheres[3].material_index = 5;


   world_t world = {};
   world.num_materials = ArrayCount(materials);
   world.materials = materials;
   world.num_planes = ArrayCount(planes);
   world.planes = planes;
   world.num_spheres = ArrayCount(spheres);
   world.spheres = spheres;

   vec3_t world_eye_pos = {-34.125f, -20.0f, 10.0f};

   // Camera space: x goes right, -z goes into the scene, y goes up
   vec3_t eye_z = Vec3Norm(world_eye_pos - vec3_t{0.0f, 0.0f, 0.0f});
   vec3_t eye_x = Vec3Norm(Cross3(vec3_t{0.0f, 0.0f, 1.0f}, eye_z));
   vec3_t eye_y = Vec3Norm(Cross3(eye_z, eye_x));

   viewpoint_t eye = {};
   eye.pos = world_eye_pos;
   eye.local_x = eye_x;
   eye.local_y = eye_y;
   eye.local_z = eye_z;

   RayTrace(&world, &ray_traced_image, &eye);
   SaveBMP(&ray_traced_image, "ray.bmp");

   printf("\nDone!\n");
   system("start ray.bmp");

   return 0;
}
