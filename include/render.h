#pragma once
#include "vmath.h"
#include "glad.h"
#include "image.h"

typedef struct Perspective Perspective;
struct Perspective
{
	float fov_z_rad;
	float aspect;
	float near;
	float far;
};

typedef struct Camera Camera;
struct Camera
{
	Vec3 pos;
	Vec3 dir;
	Vec3 up;
};

typedef struct Uv Uv;
PACK(struct Uv
{
	float u;
	float v;
});

typedef struct MeshVertex MeshVertex;
PACK(struct MeshVertex
{
	Vec3 pos;
	Uv uv;
});

typedef struct Mesh Mesh;
struct Mesh
{
	GLsizei count;
	GLuint vao;
};

typedef struct MeshBuilder MeshBuilder;
struct MeshBuilder
{
	GLsizei count;
	GLsizei capacity;
	MeshVertex* items;
};

GLuint load_pixel_texture(const Image *image);

int render_init(void);
void render_draw_quad(GLuint texture, Mat transform);
GLuint render_tmp_texture(void);
GLuint render_chunk_shader_program(void);

void mb_init(MeshBuilder* mb);
void mb_deinit(MeshBuilder* mb);
Mesh mb_create(const MeshBuilder* mb);
void mb_append(MeshBuilder* mb, MeshVertex vertex);
void mesh_draw(const Mesh* mesh, GLuint texture, Camera cam, Perspective p);
void mesh_draw_matrix(const Mesh* mesh, GLuint texture, Mat transform);
void mesh_deinit(Mesh* mesh);
