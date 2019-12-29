#include "std_include.hpp"

#include "camera.hpp"
#include "model.hpp"
#include "context_saver.hpp"

#define MAX_LIGHTS 5

model::model(const std::vector<model::vec<3>>& vertices,
	const std::vector<model::vec<3>>& normals,
	const std::vector<model::vec<2>>& uvs,
	const std::vector<surface>& surfaces,
	const std::vector<texture>& textures)
{
	context_saver _;

	//lights.push_back(std::make_unique<light>(glm::vec3(679.0f, 967.0f, 114.0f), glm::vec3(0.956127f, 0.854841f, 0.777573f)));

	this->create_vertex_buffer(vertices, normals, uvs);

	this->faces_buffers.reserve(surfaces.size() / 3);
	for (auto& surf : surfaces)
	{
		this->create_index_buffer(surf);
	}

	this->texture_buffers.reserve(textures.size());
	for (auto& tex : textures)
	{
		this->create_texture(tex);
	}

	this->create_shader();
}

void model::create_texture(const model::texture& tex)
{
	if(tex.data.size() == 6)
	{
		this->create_texture_cube(tex);
	}
	else
	{
		this->create_texture_2d(tex);
	}
}

void model::create_texture_cube(const model::texture& tex)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLuint texture = 0;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glEnable(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (auto i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.data[i].data());
	}
	
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	this->texture_buffers.push_back(texture);
}

void model::create_texture_2d(const model::texture& tex)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLuint texture = 0;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.data[0].data());

	glGenerateMipmap(GL_TEXTURE_2D);

	this->texture_buffers.push_back(texture);
}

void model::create_vertex_buffer(const std::vector<model::vec<3>>& vertices,
	const std::vector<model::vec<3>>& normals,
	const std::vector<model::vec<2>>& uvs)
{
	glGenBuffers(1, &this->vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(double) * 3 * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &this->uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(double) * 2 * uvs.size(), uvs.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &this->normal_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(double) * 3 * normals.size(), normals.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &this->vertex_array);
	glBindVertexArray(this->vertex_array);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, this->uv_buffer);
	glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, this->normal_buffer);
	glVertexAttribPointer(2, 3, GL_DOUBLE, GL_FALSE, 0, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void model::create_index_buffer(const surface& surf)
{
	model::faces faces_buffer;
	faces_buffer.count = static_cast<int>(surf.indices.size());

	glGenBuffers(1, &faces_buffer.index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faces_buffer.index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * surf.indices.size(), surf.indices.data(), GL_STATIC_DRAW);

	this->faces_buffers.push_back(std::move(faces_buffer));
}

model::~model()
{
	glDeleteVertexArrays(1, &this->vertex_array);
	glDeleteBuffers(1, &this->uv_buffer);
	glDeleteBuffers(1, &this->normal_buffer);
	glDeleteBuffers(1, &this->vertex_buffer);

	for (auto& faces : this->faces_buffers)
	{
		glDeleteBuffers(1, &faces.index_buffer);
	}
}

void model::set_camera(camera* _cam)
{
	this->cam = _cam;
}

void model::paint()
{
	static auto last_pressed = false;
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (!last_pressed)
		{
			last_pressed = true;

			while (lights.size() >= MAX_LIGHTS) {
				lights.erase(lights.begin());
			}

			lights.push_back(std::make_unique<light>(this->cam->get_position(), glm::vec3((rand() & 1) * 1.0f , (rand() & 1) * 1.0f, (rand() & 1) * 1.0f)));
		}
	} else {
		last_pressed = false;
	}

	for(auto& light : lights)
	{
		if(!light->depthMap) {
			this->generate_lightmap(*light);
			return;
		}
	}
	
	glColor3f(1, 1, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCullFace(GL_FRONT_AND_BACK);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (GetKeyState(VK_CAPITAL) & 0x0001)
		this->shader_program_no->use();
	else
		this->shader_program->use();

	GLint program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);

	GLint loc = glGetUniformLocation(program, "camera_position");
	if (loc != -1)
	{
		glm::vec3 pos = this->cam->get_position();
		glUniform3f(loc, pos[0], pos[1], pos[2]);
	}
	
	loc = glGetUniformLocation(program, "light_color_ambient");
	if (loc != -1)
	{
		glm::vec3 pos = this->cam->get_position();
		glUniform3f(loc, 0.5f, 0.5f, 0.5f);
	}
	
	loc = glGetUniformLocation(program, "ambient_intensity");
	if (loc != -1)
	{
		glUniform1f(loc, 0.4f);
	}

	loc = glGetUniformLocation(program, "diffuse_intensity");
	if (loc != -1)
	{
		glUniform1f(loc, 1.0f);
	}

	loc = glGetUniformLocation(program, "specular_intensity");
	if (loc != -1)
	{
		glUniform1f(loc, 1.0f);
	}

	loc = glGetUniformLocation(program, "shininess");
	if (loc != -1)
	{
		glUniform1f(loc, 64.0f);
	}

	loc = glGetUniformLocation(program, "texture_sampler");
	if (loc != -1)
	{
		glUniform1i(loc, 0);
	}

	const int light_count = std::min(int(lights.size()), MAX_LIGHTS);
	loc = glGetUniformLocation(program, "light_count");
	if (loc != -1)
	{
		glUniform1i(loc, light_count);
	}

	for (int i = 0; i < light_count; ++i) {
		loc = glGetUniformLocation(program, ("depth_map[" + std::to_string(i) + "]").data());
		if (loc != -1)
		{
			glUniform1i(loc, 1 + i);
		}

		loc = glGetUniformLocation(program, ("light_pos[" + std::to_string(i) + "]").data());
		if (loc != -1)
		{
			glUniform3f(loc, lights[i]->origin[0], lights[i]->origin[1], lights[i]->origin[2]);
		}

		loc = glGetUniformLocation(program, ("light_color[" + std::to_string(i) + "]").data());
		if (loc != -1)
		{
			glUniform3f(loc, lights[i]->color[0], lights[i]->color[1], lights[i]->color[2]);
		}
	}

	
	glBindVertexArray(this->vertex_array);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	for (size_t i = 0; i < this->faces_buffers.size() && i < this->texture_buffers.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, this->texture_buffers[i]);

		for(int j = 0; j < light_count; ++j)
		{
			glActiveTexture(GL_TEXTURE0 + 1 + j);
			glBindTexture(GL_TEXTURE_CUBE_MAP, lights[j]->depthMap);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->faces_buffers[i].index_buffer);
		glDrawElements(GL_TRIANGLES, this->faces_buffers[i].count, GL_UNSIGNED_INT, nullptr);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void model::create_shader()
{
	static auto vertex_shader_source
		= GLSL(130,
		       in vec3 vertex_position;
		       in vec2 vertex_uv;
		       in vec3 vertex_normal;

		       out vec2 uv;
		       out vec3 normal;
		       out vec3 vertex;
		       out vec3 cam_pos;
		       out float shininess_val;

		       uniform vec3 camera_position;
		       uniform float shininess;

		       void main(void)
		       {
		           uv = vertex_uv;
				   normal = normalize(vertex_normal);
				   vertex = vertex_position;
				   cam_pos = camera_position;
				   shininess_val = shininess;
			       gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex_position, 1.0);
		       }
		);

	static auto fragment_shader_source
		= GLSL(130,
		       in vec2 uv;
		       in vec3 vertex;
		       in vec3 normal;
		       in vec3 cam_pos;
		       uniform vec3 light_pos[5];
		       in float shininess_val;

		       uniform sampler2D texture_sampler;
		       uniform samplerCube depth_map[5];

			   uniform vec3 light_color_ambient;
			   uniform vec3 light_color[5];
			   uniform float ambient_intensity;
			   uniform float diffuse_intensity;
			   uniform float specular_intensity;

			   uniform int light_count;

			   float far_plane = 5000.0;

			   vec3 sampleOffsetDirections[20] = vec3[]
			   (
				   vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
				   vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
				   vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
				   vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
				   vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
			   );

			   float ShadowCalculationComplex(int ln)
			   {
				   vec3 fragToLight = vertex - light_pos[ln];
				   // use the light to fragment vector to sample from the depth map    
				   float closestDepth = texture(depth_map[ln], fragToLight).r;
				   // it is currently in linear range between [0,1]. Re-transform back to original value
				   closestDepth *= far_plane;
				   // now get current linear depth as the length between the fragment and light position
				   float currentDepth = length(fragToLight);
				   // now test for shadows

				   float shadow = 0.0;
				   float bias = 10.0;
				   int samples = 20;
				   float viewDistance = length(cam_pos - vertex);
				   float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
				   for (int i = 0; i < samples; ++i)
				   {
					   float closestDepth = texture(depth_map[ln], fragToLight + sampleOffsetDirections[i] * diskRadius).r;
					   closestDepth *= far_plane;   // Undo mapping [0;1]
					   if (currentDepth - bias > closestDepth)
						   shadow += 1.0;
				   }
				   shadow /= float(samples);
			   	
				   return shadow;
			   }

			   float ShadowCalculationSimple(int ln)
			   {
				   // get vector between fragment position and light position
				   vec3 fragToLight = vertex - light_pos[ln];
				   // use the light to fragment vector to sample from the depth map    
				   float closestDepth = texture(depth_map[ln], fragToLight).r;
				   // it is currently in linear range between [0,1]. Re-transform back to original value
				   closestDepth *= far_plane;
				   // now get current linear depth as the length between the fragment and light position
				   float currentDepth = length(fragToLight);
				   // now test for shadows
				   float bias = 10.0;
				   float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

				   return shadow;
			   }

			   vec3 get_reflect_dir(int ln)
			   {
				   vec3 light_direction = normalize(light_pos[ln] - vertex);
				   return reflect(-light_direction, normal);
			   }

			   float get_specular_strength(int ln)
			   {
				   vec3 reflect_dir = get_reflect_dir(ln);
				   vec3 cam_dir = normalize(cam_pos - vertex);

				   float val = max(dot(reflect_dir, cam_dir), 0.0);
				   return pow(val, shininess_val);
			   }

		       vec3 do_specular(int ln)
		       {
				   float val = get_specular_strength(ln);
		           return val * light_color[ln] * diffuse_intensity;
		       }

		       vec3 do_ambient()
		       {
				   return light_color_ambient * ambient_intensity;
		       }

		       vec3 do_diffuse(int ln)
		       {
				   vec3 light_direction = normalize(light_pos[ln] - vertex);
				   float scale = max(dot(light_direction, normal), 0.0);
				   return scale * light_color[ln] * specular_intensity;
		       }

			   vec3 do_light(int ln)
			   {
				   //float shadow = ShadowCalculationComplex(ln);
				   float shadow = ShadowCalculationSimple(ln);
				   float scale = (1.0 - shadow);
				   if (scale == 0.0) return vec3(0.0);
				   return scale * (do_diffuse(ln) + do_specular(ln));
			   }

		       vec3 do_shading(vec3 color)
		       {
				   vec3 _light_color = vec3(0.0);

			   		for(int i = 0; i < light_count; ++i) {
						_light_color += do_light(i);
			   		}
			   	
				   return color * (do_ambient() + _light_color);
		       }

		       void main(void)
		       {
				   gl_FragColor = texture2D(texture_sampler, vec2(uv.x, -uv.y));

				   //vec2 mapCoord = 2.0 * uv - 1.0;
				   //gl_FragColor = texture(depth_map, vec3(mapCoord.xy, 1.0));
			   	
				   gl_FragColor.rgb = do_shading(gl_FragColor.rgb);
		       };
		);

	this->shader_program = std::make_unique<shader>(vertex_shader_source, fragment_shader_source, std::vector<std::string>{ "vertex_position", "vertex_uv", "vertex_normal" });


	static auto vertex_shader_source_no
		= GLSL(130,
		       in vec3 vertex_position;
		       in vec2 vertex_uv;
		       in vec3 vertex_normal;

		       out vec2 uv;
		       out vec3 normal;
		       out vec3 vertex;

		       void main(void)
		       {
		           uv = vertex_uv;
		           normal = vertex_normal;
		           vertex = vertex_position;
		           gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex_position, 1.0);
		       }
		);

	static auto fragment_shader_source_no
		= GLSL(130,
		       in vec2 uv;
		       in vec3 vertex;
		       in vec3 normal;

		       uniform sampler2D texture_sampler;

		       void main(void)
		       {
		           gl_FragColor = texture2D(texture_sampler, vec2(uv.x, -uv.y));
		       }
		);

	this->shader_program_no = std::make_unique<shader>(vertex_shader_source_no, fragment_shader_source_no, std::vector<std::string>{ "vertex_position", "vertex_uv", "vertex_normal" });

	static auto vertex_shader_shadow
		= GLSL(330,
			in vec3 vertex_position;
			in vec2 vertex_uv;
			in vec3 vertex_normal;

			void main(void)
			{
				gl_Position = vec4(vertex_position, 1.0);
			}
	);

	static auto fragment_shader_shadow
		= GLSL(330,
			in vec4 FragPos;

			uniform vec3 light_position;
			uniform float far_plane;

			void main()
			{
				// get distance between fragment and light source
				float lightDistance = length(FragPos.xyz - light_position);

				// map to [0;1] range by dividing by far_plane
				lightDistance = lightDistance / far_plane;

				// write this as modified depth
				gl_FragDepth = lightDistance;
			}
	);

	static auto geometry_shader_shadow
		= GLSL(330,
			layout(triangles) in;
			layout(triangle_strip, max_vertices = 18) out;

			uniform mat4 lightSpaceMatrix[6];

			out vec4 FragPos; // FragPos from GS (output per emitvertex)

			void main()
			{
				for (int face = 0; face < 6; ++face)
				{
					gl_Layer = face; // built-in variable that specifies to which face we render.
					for (int i = 0; i < 3; ++i) // for each triangle's vertices
					{
						FragPos = gl_in[i].gl_Position;
						gl_Position = lightSpaceMatrix[face] * FragPos;
						EmitVertex();
					}
					EndPrimitive();
				}
			}
	);
	
	this->shadow_map = std::make_unique<shader>(vertex_shader_shadow, fragment_shader_shadow, geometry_shader_shadow, std::vector<std::string>{ "vertex_position", "vertex_uv", "vertex_normal" });
}

void model::generate_lightmap(model::light& light)
{
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	const unsigned int SHADOW_WIDTH = 1024 * 4;
	const unsigned int SHADOW_HEIGHT = SHADOW_WIDTH;

	if(!light.depthMap)
	{
		glGenTextures(1, &light.depthMap);
	}
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, light.depthMap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
	float _near = 1.0f;
	float _far = 5000.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, _near, _far);

	std::vector<glm::mat4> lightSpaceMatrix;
	lightSpaceMatrix.push_back(shadowProj *
		glm::lookAt(light.origin, light.origin + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	lightSpaceMatrix.push_back(shadowProj *
		glm::lookAt(light.origin, light.origin + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	lightSpaceMatrix.push_back(shadowProj *
		glm::lookAt(light.origin, light.origin + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	lightSpaceMatrix.push_back(shadowProj *
		glm::lookAt(light.origin, light.origin + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	lightSpaceMatrix.push_back(shadowProj *
		glm::lookAt(light.origin, light.origin + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	lightSpaceMatrix.push_back(shadowProj *
		glm::lookAt(light.origin, light.origin + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, light.depthMap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLint m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);
	glCullFace(GL_FRONT);
	const auto _ = gsl::finally([&m_viewport]()
		{
			glCullFace(GL_BACK);
			glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
		});

	this->shadow_map->use();

	GLint program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);

	if (glGetUniformLocation(program, "lightSpaceMatrix[0]") != -1)
	{
		glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix[0]"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix[0]));
		glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix[1]"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix[1]));
		glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix[2]"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix[2]));
		glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix[3]"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix[3]));
		glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix[4]"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix[4]));
		glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix[5]"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix[5]));
	}

	GLint loc = glGetUniformLocation(program, "light_position");
	if (loc != -1)
	{
		glUniform3f(loc, light.origin[0], light.origin[1], light.origin[2]);
	}

	loc = glGetUniformLocation(program, "far_plane");
	if (loc != -1)
	{
		glUniform1f(loc, _far);
	}

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBindVertexArray(this->vertex_array);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	for (size_t i = 0; i < this->faces_buffers.size() && i < this->texture_buffers.size(); ++i)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->faces_buffers[i].index_buffer);
		glDrawElements(GL_TRIANGLES, this->faces_buffers[i].count, GL_UNSIGNED_INT, nullptr);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glfwSwapBuffers(*this->cam->get_frame());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteFramebuffers(1, &depthMapFBO);
}
