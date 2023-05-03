
//#define assert(expr) if(!(expr)) __debugbreak();

#define BOOST_JSON_NO_LIB
#define BOOST_CONTAINER_NO_LIB
#include <boost/json.hpp>

#include <iostream>
#include  <fstream>
#include  <sstream>
#include <filesystem>

#include <vector>
#include <string>

#include <stb_include.h>
#include <stb_image.h>


#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace json{using namespace boost::json;}

using namespace std::filesystem;


uint32_t
	g_width = 1460,
	g_height = 780;

const uint32_t
	ATTRIB_POS		= 0,
	ATTRIB_NORMAL	= 1,
	ATTRIB_TEX0		= 2;

struct BufferView
{
	GLuint buffer;
	uint32_t
		byteOffset = 0,
		byteLength,
		byteStride = 0;
	GLenum target = 0;
};

struct Accessor
{
	std::vector<BufferView>::const_iterator bufferView;

	uint32_t byteOffset = 0;
	GLenum	 componentType;
	bool	 normalized;
	uint32_t count;
	std::string type;
	//todo min max and sparse
};

struct Mesh
{
	struct Primitive
	{
		uint32_t count = 0; // vertex count

		GLenum mode = GL_TRIANGLES;
	};
	std::vector<Primitive> primitives;
	std::vector<GLuint> vaos;
};


static GLuint makeShader(GLenum type, const char* path)
{
	char error[256];
	char* source = stb_include_file((char*)path, NULL, NULL, error);
	if (source == NULL)
	{
		glDebugMessageInsert(GL_DEBUG_SOURCE_THIRD_PARTY, GL_DEBUG_TYPE_ERROR, 1, GL_DEBUG_SEVERITY_HIGH, -(signed)strlen(error), error);
		return 0;
	}
	int32_t length = strlen(source);

	GLuint shader = glCreateShader(type);
	
	glShaderSource(shader, 1, &source, &length);
	glCompileShader(shader);

	GLint isCompiled = false;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

#ifndef glDebugMessageCallback
	if (!isCompiled)
	{
		GLuint len = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		assert(len != 0);
		const char* msg = malloc(len * sizeof * msg);
		glGetShaderInfoLog(shader, len, NULL, msg);
		OutputDebugStringA(msg);
		free(msg);

		glDeleteShader(shader);
		shader = 0;
	}
#endif // glDebugMessageCallback

	if (!isCompiled)
	{
		glDeleteShader(shader);
		shader = 0;
	}

	free(source);
	return shader;
}

static uint32_t getAccessorTypeComponentCount(std::string type)
{
	if (type == "SCALAR")	return 1;
	if (type == "VEC2")		return 2;
	if (type == "VEC3")		return 3;
	if (type == "VEC4")		return 4;
	if (type == "MAT2")		return 4;
	if (type == "MAT3")		return 9;
	if (type == "MAT4")		return 16;
}
static uint32_t getComponentTypeByteSize(GLenum type)
{
	switch (type)
	{
	case GL_BYTE:  case GL_UNSIGNED_BYTE:	return sizeof(char);
	case GL_SHORT: case GL_UNSIGNED_SHORT:	return sizeof(short);
	case GL_INT:   case GL_UNSIGNED_INT:	return sizeof(int);
	case GL_FLOAT:							return sizeof(float);
	default:								return 0;
	}
}

path filepath = "2.0/TriangleWithoutIndices/glTF/TriangleWithoutIndices.gltf";

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_VISIBLE, false);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(g_width, g_height, "glTF Manifest", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoaderLoadGL();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) 
		{
			std::cout << message << "\n";
		
		}, NULL);

	GLuint program = glCreateProgram();
	{
		GLuint
			vShader = makeShader(GL_VERTEX_SHADER, "shader.vert"),
			fShader = makeShader(GL_FRAGMENT_SHADER, "shader.frag");

		assert(vShader != 0);
		assert(fShader != 0);

		glAttachShader(program, vShader);
		glAttachShader(program, fShader);

		glLinkProgram(program);

		glDetachShader(program, vShader);
		glDetachShader(program, fShader);

		glDeleteShader(vShader);
		glDeleteShader(fShader);
	}
	glUseProgram(program);

	GLuint matrixUBO;
	glCreateBuffers(1, &matrixUBO);
	glNamedBufferData(matrixUBO, 4 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, matrixUBO);

	//
	// Reading
	//

	json::error_code err;
	auto gltf = json::parse([]()->std::string
		{
			std::ifstream in(filepath);
			if (in.fail()) throw std::exception("Failed to read gltf file");

			return (std::stringstream() << in.rdbuf()).str();
		}
	(), err).as_object();
	if (err)
	{
		std::cout << err.message() << std::endl;
		return -1;
	}


	//
	// Parsing
	//

	auto buffers = std::vector<GLuint>(gltf["buffers"].as_array().size());
	glCreateBuffers((GLsizei)buffers.size(), buffers.data());
	for (uint32_t i = 0; i < buffers.size(); i++)
	{
		auto jo = gltf["buffers"].as_array()[i].as_object();

		auto uri = (std::string)jo["uri"].as_string();
		auto byteLenth = jo["byteLength"].as_int64();
		
		glObjectLabel(GL_BUFFER, buffers[i], -1, jo["name"].is_null() ? uri.c_str() : jo["name"].as_string().c_str());
		glNamedBufferData(buffers[i], byteLenth, NULL, GL_STATIC_DRAW);

		auto input = std::ifstream((filepath.parent_path() / uri).string(), std::ios::binary);
		char* block = (char*)glMapNamedBuffer(buffers[i], GL_WRITE_ONLY);
		{
			input.read(block, byteLenth);
		}
		glUnmapNamedBuffer(buffers[i]);
		input.close();
	}

	auto bufferViews = std::vector<BufferView>(gltf["bufferViews"].as_array().size());
	for (uint32_t i = 0; i < bufferViews.size(); i++)
	{
		auto jo = gltf["bufferViews"].as_array()[i].as_object();

		bufferViews[i].buffer = buffers[jo["buffer"].as_int64()];

		if (!jo["byteOffset"].is_null())
			bufferViews[i].byteOffset = jo["byteOffset"].as_int64();

		bufferViews[i].byteLength = jo["byteLength"].as_int64();

		if (!jo["byteStride"].is_null())
			bufferViews[i].byteStride = jo["byteStride"].as_int64();

		if (!jo["target"].is_null())
		{
			auto val = jo["target"].as_int64();
			assert(val == GL_ARRAY_BUFFER || val == GL_ELEMENT_ARRAY_BUFFER);
			bufferViews[i].target = val;
		}

		
	}

	auto accessors = std::vector<Accessor>(gltf["accessors"].as_array().size());
	for (uint32_t i = 0; i < accessors.size(); i++)
	{
		auto jo = gltf["accessors"].as_array()[i].as_object();
		
		auto& a = accessors[i];

		//todo sparse
		a.bufferView = bufferViews.begin() + jo["bufferView"].as_int64();
		
		if (!jo["byteOffset"].is_null())
			a.byteOffset = jo["byteOffset"].as_int64();

		a.componentType = jo["componentType"].as_int64();

		if (!jo["normalized"].is_null())
			a.normalized = jo["normalized"].as_bool();

		a.count = jo["count"].as_int64();

		a.type = (std::string)jo["type"].as_string();

		//todo min max
	}

	auto meshes = std::vector<Mesh>(gltf["meshes"].as_array().size());
	for (uint32_t i = 0; i < meshes.size(); i++)
	{
		auto jom = gltf["meshes"].as_array()[i].as_object();

		meshes[i].primitives.resize(jom["primitives"].as_array().size());
		meshes[i].vaos.resize(meshes[i].primitives.size());
		glCreateVertexArrays(meshes[i].vaos.size(), meshes[i].vaos.data());
		for (uint32_t j = 0; j < meshes[i].vaos.size(); j++)
		{
			auto& prim = meshes[i].primitives[j];
			auto& vao = meshes[i].vaos[j];

			auto jop = jom["primitives"].as_array()[j].as_object();
			
			///
			///	Name:		| Accessor Type	| Component Type
			///-------------+---------------+---------------
			/// POSITION	| VEC3			| float
			/// NORMAL		| VEC3			| float
			/// TANGENT		| VEC3			| float
			/// TEXCOORD_n	| VEC2			| float / (un)signed (byte / short)
			/// COLOR_n		| VEC3 / VEC4	| float / (un)signed (byte / short)
			/// indices		| SCALAR		| unsigned (byte / short / int)
			/// 
			auto attribs = jop["attributes"].as_object();

			auto linkAttribute = [attribs, vao, accessors](Accessor const& a, uint32_t location) 
			{
				auto stride = a.bufferView->byteStride != 0 ? a.bufferView->byteStride : getAccessorTypeComponentCount(a.type) * getComponentTypeByteSize(a.componentType);

				glVertexArrayVertexBuffer (vao, location, a.bufferView->buffer, a.bufferView->byteOffset,  stride);
				glEnableVertexArrayAttrib (vao, location);
				glVertexArrayAttribBinding(vao, location, location); //I feel like this is wrong but I don't know why
				glVertexArrayAttribFormat (vao, location, getAccessorTypeComponentCount(a.type), a.componentType, a.normalized, a.byteOffset);
			};

			uint32_t count = 0;

			if (!attribs["POSITION"].is_null())
			{
				auto a = accessors.begin() + attribs["POSITION"].as_int64();
				
				assert(a->type == "VEC3");
				assert(a->componentType == GL_FLOAT);
				linkAttribute(*a, ATTRIB_POS);


				count = std::max(count, a->count);
			}


			prim.count = count;
			prim.mode = jop["mode"].is_null() ? GL_TRIANGLES : jop["mode"].as_int64();
		}



	}


	//
	// Rendering
	//

	glm::vec3
		pos = { 0, 0, -1 },
		dir = { 0, 0, 1 },
		up  = { 0, 1, 0 };
	

	glfwShowWindow(window);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glm::mat4
			projection = glm::perspective(glm::radians(45.f), (float)g_width / g_height, .1f, 100.f),
			view = glm::lookAt(pos, (pos + dir), up),
			model = glm::mat4(1.f), //todo
			normal = glm::inverse(glm::transpose(model));

		glNamedBufferSubData(matrixUBO, 0 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
		glNamedBufferSubData(matrixUBO, 1 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glNamedBufferSubData(matrixUBO, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(model));
		glNamedBufferSubData(matrixUBO, 3 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(normal));

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, g_width, g_height);

		
		for (const auto& mesh : meshes)
		{

			for (uint32_t i = 0; i < mesh.primitives.size(); i++)
			{
				auto prim = mesh.primitives[i];
				auto vao = mesh.vaos[i];

				glBindVertexArray(vao);

				glDrawArrays(prim.mode, 0, prim.count);
			}
		}
		


		glfwSwapBuffers(window);
	}

	//
	// Cleanup
	//

	glDeleteBuffers(1, &matrixUBO);

	glDeleteBuffers(buffers.size(), buffers.data());


	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
