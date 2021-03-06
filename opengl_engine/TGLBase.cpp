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


TGLBase::TGLBase(): 
	default_shader_program(0),
	shadow_maps_enabled(true)
{
	
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


void TGLBase::load_model(float * vertices)
{

	unsigned int VBO;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


}

void TGLBase::load_shader(char * vertex_shader, char * fragment_shader)
{
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShader, 1, &vertex_shader, NULL);
	glCompileShader(vertexShader);

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragment_shader, NULL);
	glCompileShader(fragmentShader);

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glUseProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
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

	ray_bounce.init();
#endif
}

void TGLBase::update()
{
	// render loop
	//while (!glfwWindowShouldClose(window))
	auto duration = std::chrono::duration_cast< std::chrono::microseconds> (end - begin);
	time_count++;
	double time_delta = duration.count() / 1000000.0;
	//time_delta = 1.0 / 1000;
	time_sum += time_delta;
	begin = std::chrono::steady_clock::now();


	update_sun(time_delta);

	int shadow_map_interval = 20;
	
	if (time_count % 60 == 0)
	{
		printf("%f\n",(time_count/time_sum));
		time_count = 0;
		time_sum = 0;
	}
	{
#ifdef _TGL_CLIENT
		unsigned int shadow_map_size = ray_bounce.shadow_map_size;
		// input
		processInput(window);

		// rendering commands here
		//glClearColor(0.7f, 0.8f, 1.0f, 1.0f);
		glClearColor(sun_intensity.r, sun_intensity.g, sun_intensity.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 cam_view = active_camera->get_view();
#endif

#ifdef _TGL_CLIENT
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

		//for (auto actor_it = actors.begin(); actor_it != actors.end(); ++actor_it)
		//{
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
		physics_engine.tick(time_delta, actors, (TGLChunkSpawn*)chunks_spawner);
			// check and call events and swap the buffers
#ifdef _TGL_CLIENT
		glfwPollEvents();
		glfwSwapBuffers(window);

		if (shadow_maps_enabled && time_count % shadow_map_interval == 0)
		{
			ray_bounce.swap_buffers();
			shadow_pos2 = shadow_pos1;
			depthMVP2 = depthMVP1;
			sun_pos_buf2 = sun_pos_buf1;
		}

#endif
	}

	end = std::chrono::steady_clock::now();
	/*
	if (std::chrono::duration_cast<std::chrono::microseconds> (end - begin).count() / 1000000.0 == 0.0)
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
	mult += 0.002*time_delta;
	//sun_pos = glm::vec3(0, 50, 0) + active_camera->get_pos();
	return;
}
