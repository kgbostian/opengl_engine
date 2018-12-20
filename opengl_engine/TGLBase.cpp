#define _CRT_SECURE_NO_WARNINGS
#include "TGLBase.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <chrono>
#include <stdio.h>
#include <thread>
#include <ctime>
#include <algorithm>
#include <stdlib.h>
#ifndef _TGL_CLIENT
#include <unistd.h>
#endif


TGLBase::TGLBase(): 
#ifndef _TGL_CLIENT
	heartbeat_period(1.0),
	tick_rate(20),
#endif
	default_shader_program(0),
	game_state_buf(1024,0),
	shadow_maps_enabled(true)
{
	srand(0);
}

TGLBase::~TGLBase()
{
}

#ifdef _TGL_CLIENT
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

	glViewport(0, 0, width, height);
}

bool TGLBase::gl_init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	return true;
}

GLFWwindow * TGLBase::gl_create_window(int in_width, int in_height)
{
	window_height = in_height;
	window_width = in_width;
	window = glfwCreateWindow(in_width, in_height, "LearnOpenGL", NULL, NULL);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	return window;
}

bool TGLBase::glad_init()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	return true;
}

void TGLBase::processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

GLFWwindow * TGLBase::get_window()
{
	return window;
}

void TGLBase::add_camera(TGLCamera * in_camera)
{
	active_camera = in_camera;
}

void TGLBase::add_hud_element(TGLHudElement * in_element)
{
	HUD_elements.push_back(in_element);
}

void TGLBase::apply_game_state(std::vector <char> * in_state)
{
	int offset = 1;
	short actor_id = *(short*)&(*in_state)[offset];
	offset += sizeof(short);
	if( active_camera->id != actor_id )
	{
		for (auto actor :actors)
		{
			if (actor->id = actor_id)
			{
				active_camera = (TGLCamera*)actor;
			}
		}
	}
	short num_actors = *(short*)&(*in_state)[offset];
	offset += sizeof(short);
	for (int i = 0; i < num_actors; ++i)
	{
		TGLActor * cur_actor = nullptr;
		short actor_id = *(short*)&(*in_state)[offset];
		offset += sizeof(short);
		for (auto actor : actors)
		{
			if (actor->id = actor_id)
			{
				cur_actor = actor;
			}
		}
		short actor_type = *(short*)&(*in_state)[offset];
		offset += sizeof(short);
		glm::mat4 actor_trans;
		std::copy(&(*in_state)[offset], &(*in_state)[offset] + 16, glm::value_ptr(actor_trans));
		cur_actor->transform = actor_trans;
		offset += sizeof(GLfloat)*16;
		
		short num_int_props = *(short*)&(*in_state)[offset];
		offset += sizeof(short);
		for(int i = 0; i < num_int_props; ++i)
		{
			short int_prop_id = *(short*)&(*in_state)[offset];
			offset += sizeof(short);
			int int_prop_val = *(int*)&(*in_state)[offset];
			offset += sizeof(int);
		}
		
		short num_float_props = *(short*)&(*in_state)[offset];
		offset += sizeof(short);
		for(int i = 0; i < num_int_props; ++i)
		{
			short int_prop_id = *(short*)&(*in_state)[offset];
			offset += sizeof(short);
			float int_prop_val = *(float*)&(*in_state)[offset];
			offset += sizeof(float);
		}
		
		short num_vec3_props = *(short*)&(*in_state)[offset];
		offset += sizeof(short);
		for(int i = 0; i < num_int_props; ++i)
		{
			short int_prop_id = *(short*)&(*in_state)[offset];
			offset += sizeof(short);
			glm::vec3 vec3_prop_val;
			std::copy(&(*in_state)[offset], &(*in_state)[offset] + 3, glm::value_ptr(vec3_prop_val));
			offset += sizeof(GLfloat)*3;
		}
		for (auto actor : actors)
		{
			if (actor_id == actor->id)
			{
				actor->transform = actor_trans;
			}
		}
	}
	
	// for (auto as : last_received_game_state.actors())
	// {
	// 	for (auto actor : actors)
	// 	{
	// 		if (as.id() == actor->id)
	// 		{
	// 			memcpy(glm::value_ptr(actor->transform), as.mutable_transform(), sizeof(GLfloat) * 16);
	// 		}
	// 	}
	// }
}

void TGLBase::process_msg(std::pair<sockaddr_in, std::vector<char>>* in_pair)
{
//	std::string message_string(in_pair->second.begin(), in_pair->second.end());
//	last_received_game_state.ParseFromString(message_string);
	apply_game_state(&in_pair->second);
}

#else

void TGLBase::send_game_state_to_all()
{
	std::vector <char> test;
    udp_interface.send_to_all(test);
}

void TGLBase::generate_game_state(bool full)
{
	game_state_buf.resize(1024);
	int offset = 0;
	game_state_buf[offset] = TGLNetMsgType::GameState;
	offset += sizeof(char);
	*(short*)&game_state_buf[offset] = 0;
    offset += sizeof(short);
	short * num_actors = (short*)&game_state_buf[offset];
	*num_actors = 0;
	offset += sizeof(short);
    for (auto actor : actors)
    {
        if (full)
        {
        	*num_actors += 1;
        	*(short*)&game_state_buf[offset] = (short)actor->id;
        	offset += sizeof(short);
        	*(short*)&game_state_buf[offset] = (short)actor->type;
        	offset += sizeof(short);
        	auto trans_p = glm::value_ptr(actor->get_transform());
        	std::copy(trans_p, trans_p + 16, &game_state_buf[offset]);
        	offset += sizeof(GLfloat)*16;
        	// # int props
        	*(short*)&game_state_buf[offset] = 0;
        	offset += sizeof(short);
        	// # float props
        	*(short*)&game_state_buf[offset] = 0;
        	offset += sizeof(short);
        	// # vec3 props
        	*(short*)&game_state_buf[offset] = 0;
        	offset += sizeof(short);
        	
        	
            // const float * at = glm::value_ptr(actor->get_transform());
            // game_state::GameState::ActorState * as = last_generated_game_state.add_actors();
            // as->set_id(actor->id);
            // as->set_type(actor->type);
            // *(as->mutable_transform()->mutable_val()) = {at, at + 15};
            // //as->transform().add_val();
        }
    }
    game_state_buf.resize(offset);
}

void TGLBase::update_clients()
{
	auto now = std::chrono::steady_clock::now();
	int time_since = (std::chrono::duration_cast< std::chrono::microseconds> (now - time_of_last_send)).count();
	if (time_since*1.0/1000000 > 1.0/tick_rate)
	{
		generate_game_state(true);
		//std::vector<char> buffer(size);
		//last_generated_game_state.SerializeToArray(&buffer[0], size);
		for (auto client : clients)
		{
			//printf("GLIENCTS %u\n", ntohs(client.first.addr.sin_port));	
			*(short*)&game_state_buf[1] = 0;

			udp_interface.s_send(game_state_buf, client.first.addr);
			
			
		}
		time_of_last_send = std::chrono::steady_clock::now();
	}
	
	// for (auto client : clients)
	// {
	// 	int time_since = (std::chrono::duration_cast< std::chrono::microseconds> (now - client.second)).count();
	// 	if (time_since*1.0/1000000 > heartbeat_period)
	// 	{
	// 		std::vector <char> heartbeat(1,0);
	// 		udp_interface.s_send(buffer, client.first.addr);
	// 	}
	// }
}

void TGLBase::process_msg(std::pair<sockaddr_in, std::vector<char>>* in_pair)
{
	//printf("PROCESS\n");
	if (in_pair->second[0] == 0)
	{
		
		in_pair->first.sin_port = ntohs(12347);
		
		if (clients.find(in_pair->first) != clients.end())
		{
			clients[in_pair->first].time_of_last_heartbeat = std::chrono::steady_clock::now();
		}
		else
		{
#ifdef USER_PLAYER_CLASS
			USER_PLAYER_CLASS * new_player = new USER_PLAYER_CLASS;
#else
			TGLPlayer * new_player = new TGLPlayer;
#endif
			add_actor((TGLActor*)new_player);
			clients[in_pair->first].actor_id = new_player->id;	
		}
		//clients[in_pair->first].time_of_last_heartbeat = std::chrono::steady_clock::now();	
		//udp_interface.s_send(in_pair->second,in_pair->first);
	}
}
#endif

int TGLBase::init()
{
#ifdef _TGL_CLIENT
	gl_init();
	
	window_width = 3000;
	window_height = 2000;
	GLFWwindow * window = gl_create_window(window_width, window_height);
	if (window == nullptr)
	{
		return -1;
	}
	
	glad_init();
	glViewport(0, 0, window_width, window_height);
	
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glDepthRange(0.0f, 1.0f);
	TGLMaterial * default_material = new TGLMaterial;
	TGLShader v_shader("vertex_shader_default.glsl", GL_VERTEX_SHADER);
	TGLShader f_shader("fragment_shader_default.glsl", GL_FRAGMENT_SHADER);

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR: %d\n", err);
	}

	default_material->add_shader(&v_shader);
	default_material->add_shader(&f_shader);
	default_material->link_shader();
	default_shader_program = default_material->get_shader_program();
	udp_interface.s_bind("192.168.1.68",12345);
	udp_interface.start_receive_thread();

	std::pair<sockaddr_in, std::vector <char>> * net_msg;
	std::vector<char>handshake(1, 0);
	udp_interface.pop_msg(net_msg);
	while (net_msg == nullptr)
	{
		udp_interface.s_send(handshake, "192.168.1.66", 12347);
		udp_interface.pop_msg(net_msg);
		Sleep(1000);
	}
	printf("Connected");
	ray_bounce.init();

#else
	udp_interface.s_bind("127.0.0.1",12345);
	udp_interface.start_receive_thread();
	time_of_last_send = std::chrono::steady_clock::now();
#endif
}

void TGLBase::update()
{
	while(1)
	{
		auto light_begin = std::chrono::steady_clock::now();
		recalculate_light(0,0);
		auto light_end = std::chrono::steady_clock::now();
		int u_sec = std::chrono::duration_cast<std::chrono::microseconds> (light_end - light_begin).count();
		std::cout << "light time " << u_sec << "\n";
	}
	// render loop
	//while (!glfwWindowShouldClose(window))
	auto duration = std::chrono::duration_cast< std::chrono::microseconds> (end - begin);
	time_count++;
	double time_delta = duration.count() / 1000000.0;
	//time_delta = 1.0 / 1000;
	time_sum += time_delta;
	begin = std::chrono::steady_clock::now();

	

	int shadow_map_interval = 20;
	
	if (time_sum > 1)
	{
		printf("%f\n",(time_count/time_sum));
		time_count = 0;
		time_sum = 0;
	}
	{
#ifdef _TGL_CLIENT
		update_sun(time_delta);
		unsigned int shadow_map_size = ray_bounce.shadow_map_size;
		// input
		processInput(window);

		// rendering commands here
		//glClearColor(0.7f, 0.8f, 1.0f, 1.0f);
		glClearColor(sun_intensity.r, sun_intensity.g, sun_intensity.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 cam_view = active_camera->get_view();

		// process network messages
		std::pair<sockaddr_in, std::vector <char>> * net_msg;
		udp_interface.pop_msg(net_msg);
		while (net_msg != nullptr)
		{
			process_msg(net_msg);
			udp_interface.return_msg(net_msg);
			udp_interface.pop_msg(net_msg);
		}
#else
		// process network messages
		update_clients();
		std::pair<sockaddr_in,std::vector <char>> * net_msg;
		udp_interface.pop_msg(net_msg);
		while (net_msg != nullptr)
		{
			process_msg(net_msg);
			udp_interface.return_msg(net_msg);
			udp_interface.pop_msg(net_msg);
		}
		
#endif


#ifdef _TGL_CLIENT
///////////////////////////////////////////
// SHADOW MAP DRAWING
		if (shadow_maps_enabled && time_count % shadow_map_interval == 0)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, ray_bounce.get_framebuffer());
			glClearColor(1.0, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glViewport(0, 0, shadow_map_size, shadow_map_size); // Render on the whole framebuffer, complete from the lower left corner to the upper right
			glUseProgram(ray_bounce.mat->get_shader_program());

			for (int i = 0; i < actors.size(); ++i)
			{
				if (actors[i]->is_chunk)
				{
					TGLChunk * act_chunk = (TGLChunk*)actors[i];
					int chunk_x, chunk_y;
					((TGLChunkSpawn*)chunks_spawner)->get_chunk_of_point(act_chunk->get_pos() + glm::vec3(1, 0, 1), chunk_x, chunk_y);
					if (!((TGLChunkSpawn*)chunks_spawner)->chunk_in_fov(chunk_x, chunk_y, active_camera->get_pos(), ((TGLPlayer*)active_camera)->forward_vec))
					{
						continue;
					}
				}
				std::vector <TGLComponent*> components = actors[i]->get_components();
				//std::vector <TGLComponent*> components = (*actor_it)->get_components();
				for (auto mesh_it = components.end() - 1; mesh_it != components.begin() - 1; --mesh_it)
				{

					if ((*mesh_it)->get_draw_flag())
					{
						TGLMesh * mesh_comp = (TGLMesh*)(*mesh_it);
						glBindVertexArray(mesh_comp->get_VAO());

						// Compute the MVP matrix from the light's point of view

						if (shadow_maps_enabled)
						{
							shadow_pos1 = active_camera->get_pos();
							sun_pos_buf1 = sun_pos;
							glm::vec3 light_pos(shadow_pos1.x + 10, 200, shadow_pos1.z + 10);
							glm::vec3 side_vec = glm::cross(shadow_pos1 - sun_pos_buf1, glm::vec3(1, 0, 0) - sun_pos_buf1 - sun_pos_buf1);
							double light_dist = glm::length(shadow_pos1 - sun_pos_buf1);

							glm::mat4 depthProjectionMatrix = glm::ortho<float>(-30, 30, -30, 30, light_dist - 20, light_dist + 20);

							glm::mat4 depthViewMatrix = glm::lookAt(sun_pos_buf1, shadow_pos1, glm::cross(side_vec, shadow_pos2 - sun_pos_buf1));
							//glm::mat4 depthModelMatrix = glm::mat4(1.0);
							depthMVP1 = depthProjectionMatrix * depthViewMatrix * actors[i]->get_transform() * mesh_comp->get_transform();

							GLuint depthMatrixID = glGetUniformLocation(ray_bounce.mat->get_shader_program(), "depthMVP");
							// Send our transformation to the currently bound shader,
							// in the "MVP" uniform

							glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP1[0][0]);


							if (mesh_comp->get_instanced_flag())
							{
								glDrawArraysInstanced(GL_TRIANGLES, 0, mesh_comp->get_length(), mesh_comp->get_instance_count());
							}
							else
							{
								glDrawArrays(GL_TRIANGLES, 0, mesh_comp->get_length());
							}
						}

					}
				}
			}

		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window_width, window_height);
#endif

///////////////////////////////////////////
// ACTOR DRAWING AND TICKING
		//for (auto actor_it = actors.begin(); actor_it != actors.end(); ++actor_it)
		//{
		for (int i = 0; i < actors.size(); ++i)
		{
#ifdef _TGL_CLIENT
			if (actors[i]->is_chunk)
			{
				TGLChunk * act_chunk = (TGLChunk*)actors[i];
				int chunk_x, chunk_y;
				((TGLChunkSpawn*)chunks_spawner)->get_chunk_of_point(act_chunk->get_pos() + glm::vec3(1, 0, 1), chunk_x, chunk_y);
				if (!((TGLChunkSpawn*)chunks_spawner)->chunk_in_fov(chunk_x, chunk_y, active_camera->get_pos(), ((TGLPlayer*)active_camera)->forward_vec))
				{
					continue;
				}
			}
#endif
			std::vector <TGLComponent*> components = actors[i]->get_components();
			//std::vector <TGLComponent*> components = (*actor_it)->get_components();
			for (auto mesh_it = components.end() - 1; mesh_it != components.begin() - 1; --mesh_it)
			{
#ifdef _TGL_CLIENT
				if ((*mesh_it)->get_draw_flag())
				{
					TGLMesh * mesh_comp = (TGLMesh*)(*mesh_it);
					glBindVertexArray(mesh_comp->get_VAO());
					//glBindBuffer(GL_ARRAY_BUFFER,(*mesh_it)->get_VBO());
					//std::vector <GLuint> attribs = mesh_comp->get_attribs();
					//for (auto attrib_it = attribs.begin(); attrib_it != attribs.end(); ++attrib_it)
					//{
						//glEnableVertexAttribArray((*attrib_it));
					//}
					GLuint shader_id = 0;
					if (!mesh_comp->get_material()->default_program)
					{

						shader_id = mesh_comp->get_shader_program();
						glUseProgram(mesh_comp->get_shader_program());
					}
					else
					{

						shader_id = default_shader_program;
						glUseProgram(default_shader_program);
					}

					GLuint mesh_loc = glGetUniformLocation(shader_id, "mesh");
					glUniformMatrix4fv(mesh_loc, 1, GL_FALSE, glm::value_ptr(mesh_comp->get_transform()));

					GLuint model_loc = glGetUniformLocation(shader_id, "model");
					glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(actors[i]->get_transform()));

					active_camera->set_projection(glm::perspective(glm::radians(90.0f), (1.0f*window_width / window_height), 0.1f, 1000.0f));
					GLuint projection = glGetUniformLocation(shader_id, "projection");
					glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(active_camera->get_projection()));

					GLuint view = glGetUniformLocation(shader_id, "view");
					glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(cam_view));

					GLuint light_dir_loc = glGetUniformLocation(shader_id, "in_light_pos");
					glUniform3fv(light_dir_loc, 1, glm::value_ptr(sun_pos - active_camera->get_pos()));

					GLuint light_color_loc = glGetUniformLocation(shader_id, "in_light_color");
					glUniform3fv(light_color_loc, 1, glm::value_ptr(sun_intensity));

					std::vector <TGLTexture*> textures = mesh_comp->get_textures();
					int count = 0;
					for (auto tex_it = textures.begin(); tex_it != textures.end(); ++tex_it)
					{
						glActiveTexture(GL_TEXTURE0 + count);
						glBindTexture(GL_TEXTURE_2D, (*tex_it)->get_name());
						count += 1;
					}

					// SHADOW MAPS

					if (shadow_maps_enabled)
					{
						glm::vec3 light_pos(shadow_pos2.x + 10, 200, shadow_pos2.z + 10);
						glm::vec3 side_vec = glm::cross(shadow_pos2 - sun_pos_buf2, glm::vec3(1, 0, 0) - sun_pos_buf2);
						double light_dist = glm::length(shadow_pos2 - sun_pos_buf2);

						glm::mat4 depthProjectionMatrix = glm::ortho<float>(-30, 30, -30, 30, light_dist - 20, light_dist + 20);

						glm::mat4 depthViewMatrix = glm::lookAt(sun_pos_buf2, shadow_pos2, glm::cross(side_vec, shadow_pos2 - sun_pos_buf2));
						//glm::mat4 depthModelMatrix = glm::mat4(1.0);
						depthMVP2 = depthProjectionMatrix * depthViewMatrix * actors[i]->get_transform() * mesh_comp->get_transform();


						
						glm::mat4 depthBiasMVP;
						//glUseProgram(shader_id);

						glm::mat4 biasMatrix(
							0.5, 0.0, 0.0, 0.0,
							0.0, 0.5, 0.0, 0.0,
							0.0, 0.0, 0.5, 0.0,
							0.5, 0.5, 0.5, 1.0
						);
						depthBiasMVP = biasMatrix * depthMVP2;

						GLuint dbmvp = glGetUniformLocation(shader_id, "DepthBiasMVP");
						glUniformMatrix4fv(dbmvp, 1, GL_FALSE, glm::value_ptr(depthBiasMVP));
						glActiveTexture(GL_TEXTURE0 + 20);
						glBindTexture(GL_TEXTURE_2D, ray_bounce.get_texture());
					}
					if (mesh_comp->get_instanced_flag())
					{
						glDrawArraysInstanced(GL_TRIANGLES, 0, mesh_comp->get_length(), mesh_comp->get_instance_count());
					}
					else
					{
						glDrawArrays(GL_TRIANGLES, 0, mesh_comp->get_length());
					}
					
				}
#endif
			}

			actors[i]->tick(time_delta);
			
			//(*actor_it)->tick(time_delta);
		}

///////////////////////////////////////////
// HUD DRAWING
#ifdef _TGL_CLIENT
		for (int i = 0; i < HUD_elements.size(); ++i)
		{
			GLuint shader_id = HUD_elements[i]->mat.get_shader_program();
			glUseProgram(shader_id);

			GLuint params_loc = glGetUniformLocation(shader_id, "params");
			GLfloat * params = HUD_elements[i]->get_params();
			params[0] /= window_width;
			params[1] /= window_height;
			params[2] /= window_width;
			params[3] /= window_height;
			glUniform4fv(params_loc, 1, params);

			GLuint color_loc = glGetUniformLocation(shader_id, "color");
			glUniform3fv(color_loc, 1, glm::value_ptr(HUD_elements[i]->color));

			if (HUD_elements[i]->texture_active)
			{
				GLuint offset_loc_1 = glGetUniformLocation(shader_id, "tex_offset_1");
				GLuint offset_loc_2 = glGetUniformLocation(shader_id, "tex_offset_2");
				glUniform2fv(offset_loc_1, 1, glm::value_ptr(HUD_elements[i]->top_left_tex_offset));
				glUniform2fv(offset_loc_2, 1, glm::value_ptr(HUD_elements[i]->bottom_right_tex_offset));
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, HUD_elements[i]->tex->get_name());
			}

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			for (int j = 0; j < HUD_elements[i]->sub_elements.size(); ++j)
			{
				TGLHudElement * sub_el = HUD_elements[i]->sub_elements[j];
				shader_id = sub_el->mat.get_shader_program();
				glUseProgram(shader_id);

				params_loc = glGetUniformLocation(shader_id, "params");
				GLfloat * params_sub = sub_el->get_params();
				params_sub[0] /= window_width;
				params_sub[1] /= window_height;
				params_sub[2] /= window_width;
				params_sub[3] /= window_height;
				params_sub[2] += params[2];
				params_sub[3] += params[3];
				glUniform4fv(params_loc, 1, params_sub);

				color_loc = glGetUniformLocation(shader_id, "color");
				glUniform3fv(color_loc, 1, glm::value_ptr(sub_el->color));

				if (sub_el->texture_active)
				{
					GLuint offset_loc_1 = glGetUniformLocation(shader_id, "tex_offset_1");
					GLuint offset_loc_2 = glGetUniformLocation(shader_id, "tex_offset_2");
					glUniform2fv(offset_loc_1, 1, glm::value_ptr(sub_el->top_left_tex_offset));
					glUniform2fv(offset_loc_2, 1, glm::value_ptr(sub_el->bottom_right_tex_offset));
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, sub_el->tex->get_name());
				}

				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
		}
#endif

///////////////////////////////////////////
// PHYSICS UPDATE
#ifndef _TGL_CLIENT
		physics_engine.tick(time_delta, actors, (TGLChunkSpawn*)chunks_spawner);
#endif
			// check and call events and swap the buffers
#ifdef _TGL_CLIENT
		glfwPollEvents();
		glfwSwapBuffers(window);

///////////////////////////////////////////
// SHADOW MAP BUFFER SWAPPING
		if (shadow_maps_enabled && time_count % shadow_map_interval == 0)
		{
			ray_bounce.swap_buffers();
			shadow_pos2 = shadow_pos1;
			depthMVP2 = depthMVP1;
			sun_pos_buf2 = sun_pos_buf1;
		}

#endif
	}
	#ifndef _TGL_CLIENT
	

	
	end = std::chrono::steady_clock::now();
	int time_taken = std::chrono::duration_cast<std::chrono::microseconds> (end - begin).count();
	if (1000000/tick_rate > time_taken)
	{
		usleep(1000000/tick_rate - time_taken);
	}
	#endif

	end = std::chrono::steady_clock::now();
	

	/*
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(5us);
		end = std::chrono::steady_clock::now();
	}
	*/

}

void TGLBase::add_mesh(TGLMesh * in_mesh)
{
	meshes.push_back(in_mesh);
}

void TGLBase::add_actor(TGLActor * in_actor)
{
	actors.push_back(in_actor);
}

void TGLBase::remove_actor(TGLActor * in_actor)
{
	for (int i = 0; i < actors.size(); ++i)
	{
		if (actors[i] == in_actor)
		{
			actors[i] = actors[actors.size() - 1];
			actors.resize(actors.size() - 1);
		}
	}
}

TGLPlayer * TGLBase::get_player()
{
	return (TGLPlayer*)active_camera;
}

glm::vec3 TGLBase::get_player_pos()
{
	return active_camera->get_pos();
}

void TGLBase::set_world_actor(TGLActor * in_actor)
{
	chunks_spawner = in_actor;
}

void TGLBase::get_game_state()
{
	for (auto actor : actors)
	{
		//actor->get_game_state();
	}
}

void TGLBase::update_sun(double time_delta)
{
	static double mult = 0;
	std::time_t now_time = time(NULL);
	struct tm * aTime = std::localtime(&now_time);
	double ssy = aTime->tm_yday * 24 * 60 * 60 + aTime->tm_hour * 60 * 60 + aTime->tm_sec;
	int adj_yday = ssy / 60 / 60 / 24;
	int adj_hour = ssy / 60 / 60 - adj_yday * 24;
	int adj_min = ssy / 60 - adj_hour * 60 - adj_yday * 24 * 60;

	aTime->tm_hour = 17;
	aTime->tm_min = 0;
	aTime->tm_sec = 0;
	double ssm = aTime->tm_hour * 60 * 60 + aTime->tm_min * 60 + aTime->tm_sec;
	double full_day = 24 * 60 * 60;

	double sun_degrees = 3.1415926 * 2 * ssm / full_day - 3.1415926 / 2;

	sun_degrees = mult * 3.141592654 * 2;

	glm::vec3 out_vec;
	//dir = glm::vec3(1, 0, 0);
	if (sun_degrees > 2 * 3.1415926)
	{
		sun_degrees -= 2 * 3.1415926;
		mult = 0;
	}
	sun_intensity = glm::vec3(0.8, 0.8 - abs(cos(sun_degrees)*0.3), 0.8 - abs(cos(sun_degrees)*0.3))*std::max(float(0), float(sin(sun_degrees))) + glm::vec3(0.2, 0.2, 0.5);
	if (sun_degrees < 3.14159)
	{
		sun_intensity = glm::vec3(1.0, 1.0 - pow(abs(cos(sun_degrees))*0.3, 2), 1.0 - pow(abs(cos(sun_degrees))*0.3, 2));
	}
	else
	{
		sun_intensity = glm::vec3(0.3, 0.3, 0.3);
	}
	//out_vec = glm::vec3(cos(sun_degrees), sin(sun_degrees), 0)*std::max(float(0), float(1 - sin(sun_degrees))) + glm::vec3(-1, -1, 0)*std::max(float(0), float(-sin(sun_degrees)));
	out_vec = glm::vec3(cos(sun_degrees), sin(sun_degrees), 0);
	sun_pos = out_vec * 50.0f + active_camera->get_pos();
	sun_dir = out_vec;
	mult += 0.002*time_delta;
	//sun_pos = glm::vec3(0, 50, 0) + active_camera->get_pos();
	return;
}

void TGLBase::recalculate_light(int in_chunk_x, int in_chunk_y)
{
	int ray_grid_width = 256;
	int secondary_bounces = 16;
	
	std::vector<glm::vec3> secondary_lights;
	
	for (int i = 0; i < ray_grid_width; ++i)
	{
		for (int j = 0; j < ray_grid_width; ++j)
		{
			glm::vec3 light_origin = glm::vec3(16*in_chunk_x + i, 16*in_chunk_y + j, 150) + sun_dir*30.0f;
			glm::vec3 light_dir = sun_dir*-1.0f;
			e_block_type hit_type;
			glm::vec3 prev_block;
			glm::vec3 pointed_at = ((TGLChunkSpawn*)chunks_spawner)->get_block_pointed_at(light_origin, light_dir, 50, hit_type, prev_block);
			if (glm::length(pointed_at) < 1000)
			{
				secondary_lights.push_back(pointed_at);
			}
		}
	}

	for (auto light : secondary_lights)
	{
		glm::vec3 light_origin = light;
		for (int i = 0; i < secondary_bounces; ++i)
		{
			float theta = 2*3.1415926*rand()/RAND_MAX;
			float phi = 3.1415926*rand()/RAND_MAX;
			float x = cos(theta);
			float z = sin(theta);
			float y = sin(phi);
			glm::vec3 ray = glm::normalize(glm::vec3(x,z,y));
			
			glm::vec3 light_dir = ray;
			e_block_type hit_type;
			glm::vec3 prev_block;
			glm::vec3 pointed_at = ((TGLChunkSpawn*)chunks_spawner)->get_block_pointed_at(light_origin, light_dir, 50, hit_type, prev_block);
		}
	}
}