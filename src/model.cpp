#include "std_include.hpp"

#include "camera.hpp"
#include "model.hpp"
#include "context_saver.hpp"

glm::mat4 lightSpaceMatrix;

model::model(const std::vector<model::vec<3>>& vertices,
	const std::vector<model::vec<3>>& normals,
	const std::vector<model::vec<2>>& uvs,
	const std::vector<surface>& surfaces,
	const std::vector<texture>& textures)
{
	context_saver _;

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
	static bool l = false;
	if(!l)
	{
		l = true;
		this->generate_lightmap();
		return;
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

	GLint loc = glGetUniformLocation(program, "lightSpaceMatrix");
	if (loc != -1)
	{
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	}

	loc = glGetUniformLocation(program, "camera_position");
	if (loc != -1)
	{
		glm::vec3 pos = this->cam->get_position();
		glUniform3f(loc, pos[0], pos[1], pos[2]);
	}

	loc = glGetUniformLocation(program, "light_position");
	if (loc != -1)
	{
		const float scale = 10.0f;
		glUniform3f(loc, 679.0f * scale, 967.0f * scale, 114.0f * scale);
	}

	loc = glGetUniformLocation(program, "light_color_ambient");
	if (loc != -1)
	{
		const float scale = 0.3f;
		glUniform3f(loc, 0.956127f * scale, 0.854841f * scale, 0.777573f * scale);
	}

	loc = glGetUniformLocation(program, "light_color_diffuse");
	if (loc != -1)
	{
		const float scale = 1.0f;
		glUniform3f(loc, 0.956127f * scale, 0.854841f * scale, 0.777573f * scale);
	}

	loc = glGetUniformLocation(program, "light_color_specular");
	if (loc != -1)
	{
		glUniform3f(loc, 0.5f, 0.5f, 0.5f);
	}

	loc = glGetUniformLocation(program, "shininess");
	if (loc != -1)
	{
		glUniform1f(loc, 64.0f);
	}

	loc = glGetUniformLocation(program, "depth_map");
	if (loc != -1)
	{
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, this->depthMap);
		glUniform1i(loc, 0);
	}

	loc = glGetUniformLocation(program, "texture_sampler");
	if (loc != -1)
	{
		glUniform1i(loc, 0);
	}
	
	loc = glGetUniformLocation(program, "depth_map");
	if (loc != -1)
	{
		glUniform1i(loc, 1);
	}
	
	glBindVertexArray(this->vertex_array);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	for (size_t i = 0; i < this->faces_buffers.size() && i < this->texture_buffers.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, this->texture_buffers[i]);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, this->depthMap);

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
		       out vec4 vertex_light;
		       out vec3 cam_pos;
		       out vec3 light_pos;
		       out vec3 light_color_specular_val;
		       out vec3 light_color_ambient_val;
		       out vec3 light_color_diffuse_val;
		       out float shininess_val;

		       uniform vec3 camera_position;
		       uniform vec3 light_position;
		       uniform vec3 light_color_specular;
		       uniform vec3 light_color_ambient;
		       uniform vec3 light_color_diffuse;
		       uniform float shininess;

			   uniform mat4 lightSpaceMatrix;

		       void main(void)
		       {
		           uv = vertex_uv;
				   normal = normalize(vertex_normal);
				   vertex = vertex_position;
				   vertex_light = lightSpaceMatrix * vec4(vertex_position, 1.0);
				   cam_pos = camera_position;
				   light_pos = light_position;
				   light_color_specular_val = light_color_specular;
				   light_color_ambient_val = light_color_ambient;
				   light_color_diffuse_val = light_color_diffuse;
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
		       in vec3 light_pos;
		       in vec3 light_color_specular_val;
		       in vec3 light_color_ambient_val;
		       in vec3 light_color_diffuse_val;
		       in float shininess_val;
			   in vec4 vertex_light;

		       uniform sampler2D texture_sampler;
		       uniform sampler2D depth_map;

			   float ShadowCalculation(vec4 fragPosLightSpace)
			   {
				   vec3 light_direction = normalize(light_pos - vertex);
				   vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
				   projCoords = projCoords * 0.5 + 0.5;

				   if (projCoords.z > 1.0) return 0.0;
			   	
				   float closestDepth = texture(depth_map, projCoords.xy).r;
				   float currentDepth = projCoords.z;
			   	
				   float bias = max(0.05 * (1.0 - dot(normal, light_direction)), 0.005);

				   float shadow = 0.0;
				   vec2 texelSize = 1.0 / textureSize(depth_map, 0);
			   	
				   for (int x = -1; x <= 1; ++x)
				   {
					   for (int y = -1; y <= 1; ++y)
					   {
						   float pcfDepth = texture(depth_map, projCoords.xy + vec2(x, y) * texelSize).r;
						   shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
					   }
				   }
				   shadow /= 9.0;

				   return shadow;
			   }

			   vec3 get_reflect_dir()
			   {
				   vec3 light_direction = normalize(light_pos - vertex);
				   return reflect(-light_direction, normal);
			   }

			   float get_specular_strength()
			   {
				   vec3 reflect_dir = get_reflect_dir();
				   vec3 cam_dir = normalize(cam_pos - vertex);

				   float val = max(dot(reflect_dir, cam_dir), 0.0);
				   return pow(val, shininess_val);
			   }

		       vec3 do_specular()
		       {
				   float val = get_specular_strength();
		           return val * light_color_specular_val;
		       }

		       vec3 do_ambient()
		       {
				   return light_color_ambient_val;
		       }

		       vec3 do_diffuse()
		       {
				   vec3 light_direction = normalize(light_pos - vertex);
				   float scale = max(dot(light_direction, normal), 0.0);
				   return scale * light_color_diffuse_val;
		       }

		       vec3 do_shading(vec3 color)
		       {
				   float shadow = ShadowCalculation(vertex_light);
				   return color * (do_ambient() + (1.0 - shadow) * (do_diffuse() + do_specular()));
		       }

		       void main(void)
		       {
				   gl_FragColor = texture2D(texture_sampler, vec2(uv.x, -uv.y));
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
		= GLSL(130,
			in vec3 vertex_position;
	in vec2 vertex_uv;
	in vec3 vertex_normal;

	uniform mat4 lightSpaceMatrix;

	void main(void)
	{
		gl_Position = lightSpaceMatrix * vec4(vertex_position, 1.0);
	}
	);

	static auto fragment_shader_shadow
		= GLSL(130,

	uniform sampler2D texture_sampler;

	void main(void)
	{
		//gl_FragColor = texture2D(texture_sampler, vec2(uv.x, -uv.y));
	}
	);

	this->shadow_map = std::make_unique<shader>(vertex_shader_shadow, fragment_shader_shadow, std::vector<std::string>{ "vertex_position", "vertex_uv", "vertex_normal" });
}

void model::generate_lightmap()
{
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	const unsigned int SHADOW_WIDTH = 1024 * 2 * 2 * 2;
	const unsigned int SHADOW_HEIGHT = SHADOW_WIDTH;

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float near_plane = 1.0f, far_plane = 5000.0f;
	float radius = 4000.0f;
	glm::mat4 lightProjection = glm::ortho(-radius, radius, -radius, radius, near_plane, far_plane);

	glm::mat4 lightView = glm::lookAt(glm::vec3(679.0f, 967.0f, 114.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	lightSpaceMatrix = lightProjection * lightView;

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

	GLint loc = glGetUniformLocation(program, "lightSpaceMatrix");
	if (loc != -1)
	{
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
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
		glBindTexture(GL_TEXTURE_2D, this->texture_buffers[i]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->faces_buffers[i].index_buffer);
		glDrawElements(GL_TRIANGLES, this->faces_buffers[i].count, GL_UNSIGNED_INT, nullptr);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glfwSwapBuffers(*this->cam->get_frame());
	
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
