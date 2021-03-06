#include <algorithm>

#include "TGLChunkSpawn.h"
#include "TGLBase.h"
#include "useful_structures.h"
#include <cstdlib>

#ifndef _EXCLUDE_TMC_DROPPED_ITEM
#include "TMCDroppedItem.h"
#endif

#define PI 3.1415926

extern TGLBase gl_base;
/*
std::vector <GLfloat> vertex_data_block_small = {
-0.5f, -0.5f, 0.5f, //(0,-1,0)
0.5f, -0.5f, 0.5f,
0.5f, -0.5f, -0.5f,
-0.5f, -0.5f, 0.5f,
0.5f, -0.5f, -0.5f,
-0.5f, -0.5f, -0.5f,
-0.5f, 0.5f, -0.5f, //(-1,0,0)
-0.5f, 0.5f, 0.5f,
-0.5f, -0.5f, 0.5f,
-0.5f, 0.5f, -0.5f,
-0.5f, -0.5f, 0.5f,
-0.5f, -0.5f, -0.5f,
0.5f, 0.5f, -0.5f, //(0,0,-1)
-0.5f, 0.5f, -0.5f,
-0.5f, -0.5f, -0.5f,
0.5f, 0.5f, -0.5f,
-0.5f, -0.5f, -0.5f,
0.5f, -0.5f, -0.5f,
-0.5f, 0.5f, 0.5f, //(0,1,0)
0.5f, 0.5f, 0.5f,
0.5f, 0.5f, -0.5f,
-0.5f, 0.5f, 0.5f,
0.5f, 0.5f, -0.5f,
-0.5f, 0.5f, -0.5f,
0.5f, 0.5f, -0.5f, //(1,0,0)
0.5f, 0.5f, 0.5f,
0.5f, -0.5f, 0.5f,
0.5f, 0.5f, -0.5f,
0.5f, -0.5f, 0.5f,
0.5f, -0.5f, -0.5f,
0.5f, 0.5f, 0.5f, //(0,0,1)
-0.5f, 0.5f, 0.5f,
-0.5f, -0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
-0.5f, -0.5f, 0.5f,
0.5f, -0.5f, 0.5f,
};
*/

template <class keyClass, class elementClass>
void chunk_searcher<keyClass,elementClass>::add_item(elementClass in_item, keyClass key)
{
	if (listing.find(key) != listing.end())
	{
		listing[key].push_back(in_item);
	}
	else
	{
		listing[key] = std::vector<elementClass>(1, in_item);
	}
}

template <class keyClass, class elementClass>
void chunk_searcher<keyClass, elementClass>::remove_item(elementClass in_item, keyClass key)
{
	if (listing.find(key) != listing.end())
	{
		for (int i = 0; i < listing[key].size(); ++i)
		{
			if (listing[key][i] == in_item)
			{
				listing[key][i] = listing[key][listing[key].size() - 1];
				listing[key].resize(listing[key].size() - 1);
			}
		}
	}
}
template <class keyClass, class elementClass>
keyClass chunk_searcher<keyClass, elementClass>::find_item(elementClass in_item)
{}


TGLChunkSpawn::TGLChunkSpawn():
	block_type_count(7),
	test_chunk(false)
{
	block_generator = new BlockGenerator(test_chunk);

#ifdef _TGL_CLIENT
	std::vector <GLfloat> dirt_with_grass;
	std::vector <GLfloat> dirt;
	std::vector <GLfloat> stone;
	std::vector <GLfloat> tree;
	std::vector <GLfloat> leaves;
	std::vector <GLfloat> water;
	std::vector <GLfloat> sand;
	{
		face_map_pair bottom, left_x, forward_y, backward_y, right_x, top;
		bottom.normal = 0; bottom.x = 2; bottom.y = 15;
		left_x.normal = 1; left_x.x = 3; left_x.y = 15;
		forward_y.normal = 5; forward_y.x = 3; forward_y.y = 15;
		backward_y.normal = 2; backward_y.x = 3; backward_y.y = 15;
		right_x.normal = 4; right_x.x = 3; right_x.y = 15;
		top.normal = 3; top.x = 0; top.y = 15;

		std::vector <face_map_pair> new_pairs = { bottom, left_x, forward_y, backward_y, right_x, top };

		dirt_with_grass = create_uv_map(new_pairs);
	}
	{
		face_map_pair bottom, left_x, forward_y, backward_y, right_x, top;
		bottom.normal = 0; bottom.x = 2; bottom.y = 15;
		left_x.normal = 1; left_x.x = 2; left_x.y = 15;
		forward_y.normal = 5; forward_y.x = 2; forward_y.y = 15;
		backward_y.normal = 2; backward_y.x = 2; backward_y.y = 15;
		right_x.normal = 4; right_x.x = 2; right_x.y = 15;
		top.normal = 3; top.x = 2; top.y = 15;

		std::vector <face_map_pair> new_pairs = { bottom, left_x, forward_y, backward_y, right_x, top };

		dirt = create_uv_map(new_pairs);
	}
	{
		face_map_pair bottom, left_x, forward_y, backward_y, right_x, top;
		bottom.normal = 0; bottom.x = 1; bottom.y = 15;
		left_x.normal = 1; left_x.x = 1; left_x.y = 15;
		forward_y.normal = 5; forward_y.x = 1; forward_y.y = 15;
		backward_y.normal = 2; backward_y.x = 1; backward_y.y = 15;
		right_x.normal = 4; right_x.x = 1; right_x.y = 15;
		top.normal = 3; top.x = 1; top.y = 15;

		std::vector <face_map_pair> new_pairs = { bottom, left_x, forward_y, backward_y, right_x, top };

		stone = create_uv_map(new_pairs);
	}
	{
		face_map_pair bottom, left_x, forward_y, backward_y, right_x, top;
		bottom.normal = 0; bottom.x = 5; bottom.y = 14;
		left_x.normal = 1; left_x.x = 4; left_x.y = 14;
		forward_y.normal = 5; forward_y.x = 4; forward_y.y = 14;
		backward_y.normal = 2; backward_y.x = 4; backward_y.y = 14;
		right_x.normal = 4; right_x.x = 4; right_x.y = 14;
		top.normal = 3; top.x = 5; top.y = 14;

		std::vector <face_map_pair> new_pairs = { bottom, left_x, forward_y, backward_y, right_x, top };

		tree = create_uv_map(new_pairs);
	}
	{
		face_map_pair bottom, left_x, forward_y, backward_y, right_x, top;
		bottom.normal = 0; bottom.x = 4; bottom.y = 12;
		left_x.normal = 1; left_x.x = 4; left_x.y = 12;
		forward_y.normal = 5; forward_y.x = 4; forward_y.y = 12;
		backward_y.normal = 2; backward_y.x = 4; backward_y.y = 12;
		right_x.normal = 4; right_x.x = 4; right_x.y = 12;
		top.normal = 3; top.x = 4; top.y = 12;

		std::vector <face_map_pair> new_pairs = { bottom, left_x, forward_y, backward_y, right_x, top };

		leaves = create_uv_map(new_pairs);
	}
	{
		face_map_pair bottom, left_x, forward_y, backward_y, right_x, top;
		bottom.normal = 0; bottom.x = 13; bottom.y = 3;
		left_x.normal = 1; left_x.x = 13; left_x.y = 3;
		forward_y.normal = 5; forward_y.x = 13; forward_y.y = 3;
		backward_y.normal = 2; backward_y.x = 13; backward_y.y = 3;
		right_x.normal = 4; right_x.x = 13; right_x.y = 3;
		top.normal = 3; top.x = 13; top.y = 3;

		std::vector <face_map_pair> new_pairs = { bottom, left_x, forward_y, backward_y, right_x, top };

		water = create_uv_map(new_pairs);
	}
	{
		face_map_pair bottom, left_x, forward_y, backward_y, right_x, top;
		bottom.normal = 0; bottom.x = 2; bottom.y = 14;
		left_x.normal = 1; left_x.x = 2; left_x.y = 14;
		forward_y.normal = 5; forward_y.x = 2; forward_y.y = 14;
		backward_y.normal = 2; backward_y.x = 2; backward_y.y = 14;
		right_x.normal = 4; right_x.x = 2; right_x.y = 14;
		top.normal = 3; top.x = 2; top.y = 14;

		std::vector <face_map_pair> new_pairs = { bottom, left_x, forward_y, backward_y, right_x, top };

		sand = create_uv_map(new_pairs);
	}
	
	block_texture = new TGLTexture("content/textures/mc.png");
	block_material = new TGLMaterial;

	block_mesh_vertices = new TGLMeshVertices(useful_structures::vertex_data_block_small);
	
	TGLShader v_shader("vertex_shader.glsl", GL_VERTEX_SHADER);
	TGLShader f_shader("fragment_shader.glsl", GL_FRAGMENT_SHADER);

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR: %d\n", err);
	}

	block_material->add_shader(&v_shader);
	block_material->add_shader(&f_shader);
	block_material->link_shader();

	block_material->add_texture(block_texture, &dirt_with_grass[0], 72);
	block_material->add_texture(block_texture, &dirt[0], 72);
	block_material->add_texture(block_texture, &stone[0], 72);
	block_material->add_texture(block_texture, &tree[0], 72);
	block_material->add_texture(block_texture, &leaves[0], 72);
	block_material->add_texture(block_texture, &water[0], 72);
	block_material->add_texture(block_texture, &sand[0], 72);
	//TGLMesh tri_mesh2(vertex_data, 108);
	
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR: %d\n", err);
	}
#endif
	return;
	for (int i = -5; i < 5; ++i)
	{
		for (int j = -5; j < 5; ++j)
		{
			spawn_chunk(i, j);
		}
	}

}


extern TGLActor debug_actor;


template <typename T> int sign(T val) {
	return (T(0) < val) - (val < T(0));
}

glm::vec3 ray_cast_block_finder(glm::vec3 in_position, glm::vec3 in_ray, glm::vec3 ray_start, glm::vec3& next_ray)
{
	double next_x_intersect_x,
		next_x_intersect_y,
		next_x_intersect_z,
		next_y_intersect_x,
		next_y_intersect_y,
		next_y_intersect_z,
		next_z_intersect_x,
		next_z_intersect_y,
		next_z_intersect_z;

	double x_length,
		y_length,
		z_length;


	glm::vec3 next_x_ray,
		next_y_ray,
		next_z_ray;

	glm::vec3 x_intersected_block,
		y_intersected_block,
		z_intersected_block,
		intersected_block;


	if (in_ray.x != 0)
	{
		next_x_intersect_x = round(ray_start.x) + 0.5*sign(in_ray.x);
		next_x_intersect_y = in_position.y + (ray_start.y - in_position.y)*(next_x_intersect_x - in_position.x) / (ray_start.x - in_position.x);
		next_x_intersect_z = in_position.z + (ray_start.z - in_position.z)*(next_x_intersect_x - in_position.x) / (ray_start.x - in_position.x);
		next_x_ray = glm::vec3(next_x_intersect_x - in_position.x, next_x_intersect_y - in_position.y, next_x_intersect_z - in_position.z);
		x_length = glm::length(next_x_ray);
		x_intersected_block = glm::vec3(round(next_x_intersect_x + 0.1*sign(in_ray.x)), round(next_x_intersect_y), round(next_x_intersect_z));
	}
	else
	{
		x_length = 1000;
	}

	if (in_ray.y != 0)
	{
		next_y_intersect_y = round(ray_start.y) + 0.5*sign(in_ray.y);
		next_y_intersect_z = in_position.z + (ray_start.z - in_position.z)*(next_y_intersect_y - in_position.y) / (ray_start.y - in_position.y);
		next_y_intersect_x = in_position.x + (ray_start.x - in_position.x)*(next_y_intersect_y - in_position.y) / (ray_start.y - in_position.y);
		next_y_ray = glm::vec3(next_y_intersect_x - in_position.x, next_y_intersect_y - in_position.y, next_y_intersect_z - in_position.z);
		y_length = glm::length(next_y_ray);
		y_intersected_block = glm::vec3(round(next_y_intersect_x), round(next_y_intersect_y + 0.1*sign(in_ray.y)), round(next_y_intersect_z));
	}
	else
	{
		y_length = 1000;
	}

	if (in_ray.z != 0)
	{
		next_z_intersect_z = round(ray_start.z) + 0.5*sign(in_ray.z);
		next_z_intersect_y = in_position.y + (ray_start.y - in_position.y)*(next_z_intersect_z - in_position.z) / (ray_start.z - in_position.z);
		next_z_intersect_x = in_position.x + (ray_start.x - in_position.x)*(next_z_intersect_z - in_position.z) / (ray_start.z - in_position.z);
		next_z_ray = glm::vec3(next_z_intersect_x - in_position.x, next_z_intersect_y - in_position.y, next_z_intersect_z - in_position.z);
		z_length = glm::length(next_z_ray);
		z_intersected_block = glm::vec3(round(next_z_intersect_x), round(next_z_intersect_y), round(next_z_intersect_z + 0.1*sign(in_ray.z)));
	}
	else
	{
		z_length = 1000;
	}

	if (x_length < z_length && x_length < y_length)
	{
		intersected_block = x_intersected_block;
		next_ray = next_x_ray;
	}
	else if (y_length < z_length && y_length < x_length)
	{
		intersected_block = y_intersected_block;
		next_ray = next_y_ray;
	}
	else if (z_length < x_length && z_length < y_length)
	{
		intersected_block = z_intersected_block;
		next_ray = next_z_ray;
	}
	return intersected_block;
}

glm::vec3 TGLChunkSpawn::get_block_pointed_at(glm::vec3 origin, glm::vec3 pointing_vector, double max_distance, e_block_type& out_block_type, glm::vec3& out_prev_block)
{
		glm::vec3 next_ray_crosshair = pointing_vector*0.01f;
		e_block_type block_type_crosshair = bt_air;
		glm::vec3 next_block_crosshair = origin;
		glm::vec3 prev_block_crosshair = next_block_crosshair;
		while ((block_type_crosshair == bt_water || block_type_crosshair == bt_air) && glm::length(origin - next_block_crosshair) < max_distance)
		{
			prev_block_crosshair = next_block_crosshair;
			next_block_crosshair = ray_cast_block_finder(origin, pointing_vector, origin + next_ray_crosshair*1.01f, next_ray_crosshair);
			block_type_crosshair = block_generator->get_point(next_block_crosshair.x, next_block_crosshair.z, next_block_crosshair.y);
		}
		out_block_type = block_type_crosshair;
		out_prev_block = prev_block_crosshair;
		return next_block_crosshair;
}



void TGLChunkSpawn::tick(double time_delta)
{
	static double time_sum = 0;
	time_sum += time_delta;
	if (time_sum > 0.1)
	{
#ifdef _TGL_CLIENT
		TGLPlayer * player = gl_base.get_player();

		// 3D Crosshair
		glm::vec3 next_ray_crosshair = player->crosshair*0.01f;
		e_block_type block_type_crosshair = bt_air;
		glm::vec3 next_block_crosshair = player->get_pos();
		glm::vec3 prev_block_crosshair = next_block_crosshair;
		while (block_type_crosshair == bt_air && glm::length(player->get_pos() - next_block_crosshair) < 5)
		{
			prev_block_crosshair = next_block_crosshair;
			next_block_crosshair = ray_cast_block_finder(player->get_pos(), player->crosshair, player->get_pos() + next_ray_crosshair*1.01f, next_ray_crosshair);
			block_type_crosshair = block_generator->get_point(next_block_crosshair.x, next_block_crosshair.z, next_block_crosshair.y);
		}
		//debug_actor.set_pos(next_ray_crosshair + player->get_pos());
		// End 3D Crosshair
#endif
		
		// Process hits
		for (auto hit : posted_hits)
		{
			int chunk_x;
			int chunk_y;
			get_chunk_of_point(hit.loc, chunk_x, chunk_y);
			
			//std::cout << "REMOVING " << block_type << " FROM " << hit.loc.x - chunk_x * 16 << ", " << hit.loc.y << ", " << hit.loc.z - chunk_y * 16 << "\n";
			//std::cout << "from chunk " << chunk_x << ", " << chunk_y << "\n";
			glm::vec3 to_remove((unsigned int)(hit.loc.x - chunk_x * 16), (unsigned int)(hit.loc.y), (unsigned int)(hit.loc.z - chunk_y * 16));
			e_block_type type_to_remove = block_generator->get_point(hit.loc.x, hit.loc.z, hit.loc.y);
			if (type_to_remove != bt_air)
			{
				bool was_deleted = true;
#ifdef _TGL_CLIENT
				was_deleted = chunks[chunk_coord(chunk_x, chunk_y)]->remove_instance(type_to_remove, to_remove);
#endif
				if (was_deleted)
				{
					block_generator->set_point(bt_air, hit.loc.x, hit.loc.z, hit.loc.y);

#ifndef _EXCLUDE_TMC_DROPPED_ITEM
					TMCDroppedItem * added = new TMCDroppedItem(block_type_to_item_id(type_to_remove));
					((TGLMesh*)(added->get_components()[0]))->set_material(block_material, (e_block_type)(type_to_remove - 1));
					added->set_pos(hit.loc);
					//debug_actor.set_pos(hit_block);
					gl_base.add_actor((TGLActor*)added);
					dropped_items.add_item((TGLActor*)added, chunk_coord(chunk_x, chunk_y));
#endif
				}

#ifdef _TGL_CLIENT
				//glm::vec3 to_add(to_remove.x + 1, to_remove.y, to_remove.z);
				get_chunk_of_point(hit.loc + glm::vec3(1, 0, 0), chunk_x, chunk_y);
				glm::vec3 to_add((unsigned int)(hit.loc.x + 1 - chunk_x * 16), (unsigned int)(hit.loc.y), (unsigned int)(hit.loc.z - chunk_y * 16));
				int new_block_type = block_generator->get_point(hit.loc.x + 1, hit.loc.z, hit.loc.y);
				if (new_block_type != 0)
				{
					chunks[chunk_coord(chunk_x, chunk_y)]->add_instance(new_block_type, to_add);
				}
				get_chunk_of_point(hit.loc + glm::vec3(-1, 0, 0), chunk_x, chunk_y);
				//to_add = glm::vec3(to_remove.x - 1, to_remove.y, to_remove.z);
				to_add = glm::vec3((unsigned int)(hit.loc.x - 1 - chunk_x * 16), (unsigned int)(hit.loc.y), (unsigned int)(hit.loc.z - chunk_y * 16));
				new_block_type = block_generator->get_point(hit.loc.x - 1, hit.loc.z, hit.loc.y);
				if (new_block_type != 0)
				{
					chunks[chunk_coord(chunk_x, chunk_y)]->add_instance(new_block_type, to_add);
				}
				get_chunk_of_point(hit.loc + glm::vec3(0, 1, 0), chunk_x, chunk_y);
				//to_add = glm::vec3(to_remove.x, to_remove.y + 1, to_remove.z);
				to_add = glm::vec3((unsigned int)(hit.loc.x - chunk_x * 16), (unsigned int)(hit.loc.y + 1), (unsigned int)(hit.loc.z - chunk_y * 16));
				new_block_type = block_generator->get_point(hit.loc.x, hit.loc.z, hit.loc.y + 1);
				if (new_block_type != 0)
				{
					chunks[chunk_coord(chunk_x, chunk_y)]->add_instance(new_block_type, to_add);
				}
				get_chunk_of_point(hit.loc + glm::vec3(0, -1, 0), chunk_x, chunk_y);
				//to_add = glm::vec3(to_remove.x, to_remove.y - 1, to_remove.z);
				to_add = glm::vec3((unsigned int)(hit.loc.x - chunk_x * 16), (unsigned int)(hit.loc.y - 1), (unsigned int)(hit.loc.z - chunk_y * 16));
				new_block_type = block_generator->get_point(hit.loc.x, hit.loc.z, hit.loc.y - 1);
				if (new_block_type != 0)
				{
					chunks[chunk_coord(chunk_x, chunk_y)]->add_instance(new_block_type, to_add);
				}
				get_chunk_of_point(hit.loc + glm::vec3(0, 0, 1), chunk_x, chunk_y);
				//to_add = glm::vec3(to_remove.x, to_remove.y, to_remove.z + 1);
				to_add = glm::vec3((unsigned int)(hit.loc.x - chunk_x * 16), (unsigned int)(hit.loc.y), (unsigned int)(hit.loc.z + 1 - chunk_y * 16));
				new_block_type = block_generator->get_point(hit.loc.x, hit.loc.z + 1, hit.loc.y);
				if (new_block_type != 0)
				{
					chunks[chunk_coord(chunk_x, chunk_y)]->add_instance(new_block_type, to_add);
				}
				get_chunk_of_point(hit.loc + glm::vec3(0, 0, -1), chunk_x, chunk_y);
				//to_add = glm::vec3(to_remove.x, to_remove.y, to_remove.z - 1);
				to_add = glm::vec3((unsigned int)(hit.loc.x - chunk_x * 16), (unsigned int)(hit.loc.y), (unsigned int)(hit.loc.z - 1 - chunk_y * 16));
				new_block_type = block_generator->get_point(hit.loc.x, hit.loc.z - 1, hit.loc.y);
				if (new_block_type != 0)
				{
					chunks[chunk_coord(chunk_x, chunk_y)]->add_instance(new_block_type, to_add);
				}
#endif 
			}
		}
		posted_hits.clear();
		
		// Process placements
		for (auto placement : posted_placements)
		{
			int chunk_x, chunk_y;
			get_chunk_of_point(placement.loc.get_vec(), chunk_x, chunk_y);

			glm::vec3 to_create((unsigned int)(placement.loc.x - chunk_x * 16), (unsigned int)(placement.loc.y), (unsigned int)(placement.loc.z - chunk_y * 16));
#ifdef _TGL_CLIENT
			chunks[chunk_coord(chunk_x, chunk_y)]->add_instance(placement.type, to_create);
#endif
			block_generator->set_point(placement.type, placement.loc.x, placement.loc.z, placement.loc.y);
		}
		posted_placements.clear();
		
		
#ifdef _TGL_CLIENT
		//printf("Player pos %f, %f\n", player->get_pos().x, player->get_pos().z);
		float first_angle = player->x_angle - PI / 12 - PI;
		while (first_angle > PI) { first_angle -= 2 * PI; }
		while (first_angle < PI) { first_angle += 2 * PI; }

		float second_angle = player->x_angle + PI / 12 - PI;
		while (second_angle > PI) { second_angle -= 2 * PI; }
		while (second_angle < PI) { second_angle += 2 * PI; }

		if (second_angle < first_angle)
		{
			float temp = first_angle;
			first_angle = second_angle;
			second_angle = temp;
		}

		int chunk_x, chunk_y;
		get_chunk_of_point(player->get_pos(), chunk_x, chunk_y);
		glm::vec3 chunk_pos(chunk_x, 0, chunk_y);

		chunks_to_load.clear();
		int load_radius = 10;
		for (int i = 0; i < load_radius; ++i)
		{
			for (int j = -i; j < i; ++j)
			{
				chunks_to_load.push_back(chunk_coord(chunk_x + i, chunk_y + j));
				chunks_to_load.push_back(chunk_coord(chunk_x + j, chunk_y + i));
				chunks_to_load.push_back(chunk_coord(chunk_x + -i, chunk_y + j));
				chunks_to_load.push_back(chunk_coord(chunk_x + j, chunk_y + -i));
			}
		}

		glm::vec3 chunk_angle = glm::vec3(player->forward_vec.x,0, player->forward_vec.z)/glm::length(glm::vec3(player->forward_vec.x,0, player->forward_vec.z));
		glm::vec3 next_start_point = chunk_pos + chunk_angle*0.1f;
		glm::vec3 next_ray_chunk;
		glm::vec3 out_chunk;
		
		while (glm::length(next_start_point - chunk_pos) < 10)
		{
			out_chunk = ray_cast_block_finder(chunk_pos, chunk_angle, next_start_point, next_ray_chunk);
			next_start_point = chunk_pos + next_ray_chunk*1.01f;
			chunks_to_load.push_back(chunk_coord(out_chunk.x, out_chunk.z));
		}
		
		/*
		glm::vec3 next_ray_crosshair = player->crosshair*0.01f;
		e_block_type block_type_crosshair = bt_air;
		glm::vec3 next_block_crosshair = player->get_pos();
		glm::vec3 prev_block_crosshair = next_block_crosshair;
		while (block_type_crosshair == bt_air && glm::length(player->get_pos() - next_block_crosshair) < 5)
		{
			prev_block_crosshair = next_block_crosshair;
			next_block_crosshair = ray_cast_block_finder(player->get_pos(), player->crosshair, player->get_pos() + next_ray_crosshair*1.01f, next_ray_crosshair);
			block_type_crosshair = block_generator->get_point(next_block_crosshair.x, next_block_crosshair.z, next_block_crosshair.y);
		}
		chunks_to_load = get_chunks(chunk_x, chunk_y, 10, first_angle, second_angle);
		*/
#ifdef _TGL_CLIENT
		if (chunks.size() < 400)
		{
		for (auto it = chunks_to_load.begin(); it != chunks_to_load.end(); ++it)
		{
			if (chunks.find((*it)) == chunks.end())
			{
				//printf("Spawning %d, %d\n", (*it).x, (*it).y);

				//client_request_chunk((*it).x, (*it).y);
				
					spawn_chunk((*it).x, (*it).y);
				
				break;
			}
		}
		}
		for (auto it = chunks.begin(); it != chunks.end(); ++it)
		{
			double distance = glm::length(glm::vec3((*it).first.x,0, (*it).first.y) - chunk_pos);
			if (distance > 15)
			{
				despawn_chunk((*it).first.x, (*it).first.y);
			}
		}
#else
		for (auto it = chunks_to_load.begin(); it != chunks_to_load.end(); ++it)
		{
			if (chunks.find((*it)) == chunks.end())
			{
				//printf("Spawning %d, %d\n", (*it).x, (*it).y);
	
				client_request_chunk((*it).x, (*it).y);
				//spawn_chunk((*it).x, (*it).y);
				break;
			}
		}
		for (auto it = chunks_received.begin(); it != chunks_received.end(); ++it)
		{
			if (chunks.find((*it)) == chunks.end())
			{
				//printf("Spawning %d, %d\n", (*it).x, (*it).y);
				spawn_chunk((*it).x, (*it).y);
				break;
			}
		}
#endif
#endif
		time_sum = 0;
	}
	
}

void TGLChunkSpawn::spawn_chunk(int chunk_x, int chunk_y)
{
	std::vector <std::vector<GLfloat>> instances;

	for (int i = 0; i < block_type_count; ++i)
	{
		instances.push_back(std::vector<GLfloat>(0));
	}
	int count = 0;

	//block_generator->get_points((chunk_x * 16 - 1), (chunk_y * 16 - 1), 0, 18);
	block_generator->get_points((chunk_x * 16), (chunk_y * 16), 0, 18);
	
	for (int i = 1; i < 17; ++i)
	{
		for (int j = 1; j < 17; ++j)
		{
			if (test_chunk)
			{
				int tp = abs(chunk_x + chunk_y) % 6;
				instances[tp].push_back(i-1);
				instances[tp].push_back(180);
				instances[tp].push_back(j-1);
			}
			else if (chunk_x == 0 && chunk_y == 0 && 0)
			{
				for (int k = 150; k < 256; k++)
				{
					instances[bt_dirt_with_grass - 1].push_back(i-1);
					instances[bt_dirt_with_grass - 1].push_back(k);
					instances[bt_dirt_with_grass - 1].push_back(j - 1);
				}
			}
			else if (chunk_x == 5 && chunk_y == 0 && 0)
			{
				for (int k = 150; k < 256; k++)
				{
					instances[bt_dirt - 1].push_back(i - 1);
					instances[bt_dirt - 1].push_back(k);
					instances[bt_dirt - 1].push_back(j - 1);
				}
			}
			else if (chunk_x == 0 && chunk_y == 5 && 0)
			{
				for (int k = 150; k < 256; k++)
				{
					instances[bt_stone - 1].push_back(i - 1);
					instances[bt_stone - 1].push_back(k);
					instances[bt_stone - 1].push_back(j - 1);
				}
			}
			else
			{
				//unsigned char block_type = block_generator->get_point(chunk_x * 16 - 1 + i, chunk_y * 16 - 1 + j, 0);
				//instances[0].push_back(i);
				//instances[0].push_back(int(block_type));
				//instances[0].push_back(j);
				for (int k = 1; k < 256; ++k)
				{
					unsigned char block_type = block_generator->index(i, j, k);

					//int block_type_test = block_generator->get_point((chunk_x * 16 - 1)+i, (chunk_y * 16 - 1)+j, k);
					//if (block_type != block_type_test && block_type != 5 && block_type != 4)
					//{
					//	printf("FART\n");
					//}
					if (block_type > 0 && block_generator->is_visible(i, j, k) && k > 100)
					{
						instances[block_type - 1].push_back(i);
						instances[block_type - 1].push_back(k);
						instances[block_type - 1].push_back(j);
					}
				}
			}
		}
	}
#ifdef _TGL_CLIENT
	chunks[chunk_coord(chunk_x, chunk_y)] = new TGLChunk(block_mesh_vertices, block_material, block_type_count, instances);
	chunks[chunk_coord(chunk_x, chunk_y)]->translate(glm::vec3(16 * chunk_x, 0, 16 * chunk_y));
#endif
	gl_base.add_actor(chunks[chunk_coord(chunk_x, chunk_y)]);
}

void TGLChunkSpawn::despawn_chunk(int chunk_x, int chunk_y)
{
#ifdef _TGL_CLIENT
	gl_base.remove_actor(chunks[chunk_coord(chunk_x, chunk_y)]);
	delete chunks[chunk_coord(chunk_x, chunk_y)];
	chunks.erase(chunk_coord(chunk_x, chunk_y));
#endif
}

std::vector <GLfloat> TGLChunkSpawn::create_uv_map(std::vector <face_map_pair> pairs)
{
	std::vector <GLfloat> out_vector(72);
	int image_width = 256;
	int image_height = 256;
	int image_divisions_x = 16;
	int image_divisions_y = 16;
	for (auto it = pairs.begin(); it != pairs.end(); ++it)
	{
		int offset = (*it).normal * 12;

		if ((*it).normal > 2)
		{
			out_vector[0 + offset] = (((*it).x*1.0) / image_divisions_x);
			out_vector[0 + offset + 1] = 1 - (((*it).y*1.0 + 1) / image_divisions_y);
			out_vector[2 + offset] = (((*it).x*1.0 + 1) / image_divisions_x);
			out_vector[2 + offset + 1] = 1 - (((*it).y*1.0 + 1) / image_divisions_y);
			out_vector[4 + offset] = (((*it).x*1.0 + 1) / image_divisions_x);
			out_vector[4 + offset + 1] = 1 - (((*it).y*1.0) / image_divisions_y);
			out_vector[6 + offset] = (((*it).x*1.0) / image_divisions_x);
			out_vector[6 + offset + 1] = 1 - (((*it).y*1.0 + 1) / image_divisions_y);
			out_vector[8 + offset] = (((*it).x*1.0 + 1) / image_divisions_x);
			out_vector[8 + offset + 1] = 1 - (((*it).y*1.0) / image_divisions_y);
			out_vector[10 + offset] = (((*it).x*1.0) / image_divisions_x);
			out_vector[10 + offset + 1] = 1 - (((*it).y*1.0) / image_divisions_y);
		}
		else
		{
			out_vector[0 + offset] = (((*it).x*1.0) / image_divisions_x);
			out_vector[0 + offset + 1] = 1 - (((*it).y*1.0 + 1) / image_divisions_y);
			out_vector[4 + offset] = (((*it).x*1.0 + 1) / image_divisions_x);
			out_vector[4 + offset + 1] = 1 - (((*it).y*1.0 + 1) / image_divisions_y);
			out_vector[2 + offset] = (((*it).x*1.0 + 1) / image_divisions_x);
			out_vector[2 + offset + 1] = 1 - (((*it).y*1.0) / image_divisions_y);
			out_vector[6 + offset] = (((*it).x*1.0) / image_divisions_x);
			out_vector[6 + offset + 1] = 1 - (((*it).y*1.0 + 1) / image_divisions_y);
			out_vector[10 + offset] = (((*it).x*1.0 + 1) / image_divisions_x);
			out_vector[10 + offset + 1] = 1 - (((*it).y*1.0) / image_divisions_y);
			out_vector[8 + offset] = (((*it).x*1.0) / image_divisions_x);
			out_vector[8 + offset + 1] = 1 - (((*it).y*1.0) / image_divisions_y);
		}
		/*
		out_vector[0 + offset] = 0.0f;
		out_vector[0 + offset + 1] = 1.0f;
		out_vector[2 + offset] = 1.0f / 2;
		out_vector[2 + offset + 1] = 1.0f;
		out_vector[4 + offset] = 1.0f / 2;
		out_vector[4 + offset + 1] = 1.0f - 1.0/2;
		out_vector[6 + offset] = 0.0f;
		out_vector[6 + offset + 1] = 1.0f;
		out_vector[8 + offset] = 1.0f / 2;
		out_vector[8 + offset + 1] = 1.0f - 1.0 / 2;
		out_vector[10 + offset] = 0.0f;
		out_vector[10 + offset + 1] = 1.0f - 1.0 / 2;
		*/
	}
	return out_vector;
}

bool TGLChunkSpawn::between_angles(float x, float y, float in_angle_1, float in_angle_2)
{
	float angle = atan2(y, x);
	return (angle > in_angle_1 && angle < in_angle_2 || angle + 2 * PI > in_angle_1 && angle + 2 * PI < in_angle_2 || angle - 2 * PI > in_angle_1 && angle - 2 * PI < in_angle_2);
}

std::vector <chunk_coord> TGLChunkSpawn::get_chunks(int x0, int y0, int radius, float view_angle_1, float view_angle_2)
{
	std::vector <chunk_coord> out_vecs;
	int first_radius = 5;
	for (int x = 0; x < radius; ++x)
	{
		for (int y = 0; y < radius; ++y)
		{
			if (x*x + y*y <= radius*radius)
			{
				if (between_angles(x, y, view_angle_1, view_angle_2) || x*x + y*y < first_radius*first_radius)
				{
					out_vecs.push_back(chunk_coord(x0 + x, y0 + y));
				}
				if (between_angles(-x, y, view_angle_1, view_angle_2) || x*x + y*y < first_radius*first_radius)
				{
					out_vecs.push_back(chunk_coord(x0 - x, y0 + y));
				}
				if (between_angles(x, -y, view_angle_1, view_angle_2) || x*x + y*y < first_radius*first_radius)
				{
					out_vecs.push_back(chunk_coord(x0 + x, y0 - y));
				}
				if (between_angles(-x, -y, view_angle_1, view_angle_2) || x*x + y*y < first_radius*first_radius)
				{
					out_vecs.push_back(chunk_coord(x0 - x, y0 - y));
				}
			}
		}
	}
	return out_vecs;
	for (int i = 0; i < radius; i++)
	{
		int x = i;
		int y = 0;
		int err = 0;
		while (x >= y)
		{
			if (between_angles(x, y, view_angle_1, view_angle_2) && std::find(out_vecs.begin(), out_vecs.end(), chunk_coord(x0 + x, y0 + y)) == out_vecs.end())
			{
				out_vecs.push_back(chunk_coord(x0 + x, y0 + y));
			}
			if (between_angles(y, x, view_angle_1, view_angle_2) && std::find(out_vecs.begin(), out_vecs.end(), chunk_coord(x0 + y, y0 + x)) == out_vecs.end())
			{
				out_vecs.push_back(chunk_coord(x0 + y, y0 + x));
			}
			if (between_angles(-y, x, view_angle_1, view_angle_2) && std::find(out_vecs.begin(), out_vecs.end(), chunk_coord(x0 - y, y0 + x)) == out_vecs.end())
			{
				out_vecs.push_back(chunk_coord(x0 - y, y0 + x));
			}
			if (between_angles(-x, y, view_angle_1, view_angle_2) && std::find(out_vecs.begin(), out_vecs.end(), chunk_coord(x0 - x, y0 + y)) == out_vecs.end())
			{
				out_vecs.push_back(chunk_coord(x0 - x, y0 + y));
			}
			if (between_angles(-x, -y, view_angle_1, view_angle_2) && std::find(out_vecs.begin(), out_vecs.end(), chunk_coord(x0 - x, y0 - y)) == out_vecs.end())
			{
				out_vecs.push_back(chunk_coord(x0 - x, y0 - y));
			}
			if (between_angles(-y, -x, view_angle_1, view_angle_2) && std::find(out_vecs.begin(), out_vecs.end(), chunk_coord(x0 - y, y0 - x)) == out_vecs.end())
			{
				out_vecs.push_back(chunk_coord(x0 - y, y0 - x));
			}
			if (between_angles(y, -x, view_angle_1, view_angle_2) && std::find(out_vecs.begin(), out_vecs.end(), chunk_coord(x0 + y, y0 - x)) == out_vecs.end())
			{
				out_vecs.push_back(chunk_coord(x0 + y, y0 - x));
			}
			if (between_angles(x, -y, view_angle_1, view_angle_2) && std::find(out_vecs.begin(), out_vecs.end(), chunk_coord(x0 - y, y0 + x)) == out_vecs.end())
			{
				out_vecs.push_back(chunk_coord(x0 + x, y0 - y));
			}

			if (err <= 0)
			{
				y += 1;
				err += 2 * y + 1;
			}
			if (err > 0)
			{
				x -= 1;
				err -= 2 * x + 1;
			}
		}
	}
	return out_vecs;
}

e_block_type TGLChunkSpawn::get_point(int x, int y, int z)
{
	if (!test_chunk)
	{
		return block_generator->get_point(x, y, z);
	}
	else
	{
		if (z == 180)
		{
			return bt_dirt;
		}
		else
		{
			return bt_air;
		}
	}
}

e_block_type * TGLChunkSpawn::get_points(int x, int y, int division)
{
	return block_generator->get_points((x), (y), 0, division);
}

void TGLChunkSpawn::get_chunk_of_point(glm::vec3 in_point, int& out_chunk_x, int& out_chunk_y)
{
	out_chunk_x = floor(in_point.x / 16.0);
	out_chunk_y = floor(in_point.z / 16.0);
}

void TGLChunkSpawn::post_hit(block_hit in_hit)
{
	posted_hits.push_back(in_hit);
}

void TGLChunkSpawn::post_placement(block_def in_block)
{
	posted_placements.push_back(in_block);
}

std::vector <TGLActor*> TGLChunkSpawn::collect_nearby_dropped_items(glm::vec3 pos, double radius)
{
	std::vector <TGLActor*> out_items;
	chunk_coord chnk(0,0);
	get_chunk_of_point(pos, chnk.x, chnk.y);
	std::vector <chunk_coord> near_coords = { chnk, chunk_coord(chnk.x + 1,chnk.y), chunk_coord(chnk.x - 1,chnk.y), chunk_coord(chnk.x,chnk.y + 1), chunk_coord(chnk.x,chnk.y - 1) };

	for (auto coord : near_coords)
	{
		std::vector <TGLActor*>& chunk_items = dropped_items.listing[coord];
		for (auto vec_it : chunk_items)
		{
			glm::vec3 item_pos = vec_it->get_pos();
			if (glm::length(item_pos - pos) < radius)
			{
				dropped_items.remove_item(vec_it, coord);
				out_items.push_back(vec_it);
			}
		}
	}

	return out_items;
}


void TGLChunkSpawn::client_request_chunk(int chunk_x, int chunk_y)
{
	//void * chunk_request = create_client_request_message(chunk_x, chunk_y);
	//queue_network_message(chunk_request);
}

void TGLChunkSpawn::server_send_chunk_mods(int chunk_x, int chunk_y)
{
	//void * chunk_mods = create_chunk_mod_message(chunk_x, chunk_y);
	//queue_network_message(chunk_mods);
}

bool TGLChunkSpawn::chunk_in_fov(int chunk_x, int chunk_y, glm::vec3 player, glm::vec3 player_forward)
{
	glm::vec3 chunk_angle = glm::vec3(player_forward.x, 0, player_forward.z) / glm::length(glm::vec3(player_forward.x, 0, player_forward.z));

	glm::vec3 between_vec = glm::normalize(glm::vec3(chunk_x * 16, player.y, chunk_y * 16) - player);
	double angle_between = acos(glm::dot(chunk_angle, between_vec));
	int p_chunk_x, p_chunk_y;
	get_chunk_of_point(player, p_chunk_x, p_chunk_y);
	double chunk_distance = glm::length(glm::vec2(p_chunk_x, p_chunk_y) - glm::vec2(chunk_x, chunk_y));
	if (chunk_distance < 2)
	{
		return true;
	}
	if (angle_between > 3.14159 / 2.25)
	{
		return false;
	}
	else
	{
		return true;
	}
}

#ifdef _TGL_CLIENT

std::vector <GLshort> TGLChunkSpawn::get_block_light_value(int in_x, int in_y, int in_z)
{
	std::vector <GLbyte> light_vals;
	std::vector <GLshort> light_vals_out;
	int num_rays = 3;
	double light_val = 0;
	std::vector<glm::vec3> origins = {
		glm::vec3(in_x, in_y + 0.5, in_z),
		glm::vec3(0, 1, 0),
		 glm::vec3(in_x, in_y - 0.5, in_z),
		glm::vec3(0, -1, 0) ,
		 glm::vec3(in_x + 0.5, in_y, in_z),
		glm::vec3(1, 0, 0) ,
		 glm::vec3(in_x - 0.5, in_y + 0.5, in_z),
		glm::vec3(-1, 1, 0) ,
		 glm::vec3(in_x, in_y, in_z + 0.5),
		glm::vec3(0, 0, 1) ,
		 glm::vec3(in_x, in_y, in_z - 0.5),
		glm::vec3(0, 0, -1) ,
	};

	for (auto it = 0; it < origins.size(); it += 2)
	{
		glm::vec3 origin = origins[it];
		glm::vec3 normal = origins[it+1];
		for (int i = 0; i < num_rays; ++i)
		{
			double theta = rand() * 2 * PI / RAND_MAX;
			double phi = rand() * (PI / 2) / RAND_MAX;

			glm::vec3 ray(cos(theta)*cos(phi), sin(phi), sin(theta)*cos(phi));

			glm::vec3 next_ray = ray * 0.01f;
			e_block_type block_type_crosshair = bt_air;
			glm::vec3 next_block = origin;
			glm::vec3 prev_block = next_block;
			bool done = false;
			while (block_type_crosshair == bt_air && glm::length(origin - next_block) < 5)
			{
				prev_block = next_block;
				next_block = ray_cast_block_finder(origin, ray, origin + next_ray * 1.01f, next_ray);
				block_type_crosshair = block_generator->get_point(next_block.x, next_block.z, next_block.y);
				if (block_type_crosshair != bt_air)
				{
					done = true;
					break;
				}
			}
			if (done == false)
			{
				light_val += GLbyte(255*(1 / num_rays)*cos(glm::dot(ray, normal)));
			}
		}
		light_vals.push_back(light_val);
	}
	light_vals_out.assign((GLshort*)&light_vals[0], (GLshort*)&light_vals[0] + light_vals.size()/2);
	return light_vals_out;
}
#endif
