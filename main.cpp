
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
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/string_cast.hpp>

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

		std::vector<Accessor>::const_iterator indices;

		GLenum mode = GL_TRIANGLES;
	};
	std::vector<Primitive> primitives;
	std::vector<GLuint> vaos;
};

struct Node 
{
	//todo camera
	std::vector<std::vector<Node>::const_iterator> children;
	//todo skin
	glm::mat4 matrix;
	std::vector<Mesh>::const_iterator mesh;
	glm::quat rotation;
	glm::vec3 scale;
	glm::vec3 translation;
	std::vector<float> weights;

	static constexpr glm::quat DEFAULT_ROTATION = { 1.f, 0, 0, 0 };
	static constexpr glm::vec3 DEFAULT_SCALE = { 1.f, 1.f, 1.f };
	static constexpr glm::vec3 DEFAULT_TRANSLATION = { 0,0,0 };
};

struct Scene 
{
	std::vector<std::vector<Node>::const_iterator> nodes;
};
	
	
;


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

constexpr static uint32_t getAccessorTypeComponentCount(std::string type)
{
	if (type == "SCALAR")	return 1;
	if (type == "VEC2")		return 2;
	if (type == "VEC3")		return 3;
	if (type == "VEC4")		return 4;
	if (type == "MAT2")		return 4;
	if (type == "MAT3")		return 9;
	if (type == "MAT4")		return 16;
	return 0;
}
constexpr static uint32_t getComponentTypeByteSize(GLenum type)
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

path filepath = "2.0/Box/glTF/Box.gltf";

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

			if (!jop["indices"].is_null())
			{
				auto a = accessors.begin() + jop["indices"].as_int64();

				prim.indices = a;
				glVertexArrayElementBuffer(vao, a->bufferView->buffer);
			}

			prim.count = count;
			prim.mode = jop["mode"].is_null() ? GL_TRIANGLES : jop["mode"].as_int64();
		}



	}

	auto nodes = std::vector<Node>(gltf["nodes"].as_array().size());
	for (uint32_t i = 0; i < nodes.size(); i++)
	{
		auto jo = gltf["nodes"].as_array()[i].as_object();
		auto& n = nodes[i];
		
		if(!jo["children"].is_null())
			n.children.resize(jo["children"].as_array().size());
		for (uint32_t i = 0; i < n.children.size(); i++)
			n.children[i] = nodes.begin() + jo["children"].as_array()[i].as_int64();

		if (!jo["matrix"].is_null())
		{
			auto matrix = jo["matrix"].as_array();
			assert(matrix.size() == 16);

			for (uint32_t i = 0; i < matrix.size(); i++)
			{
				*(glm::value_ptr(n.matrix) + i) = (glm::f32)matrix[i].as_double();
			}
		}
		else n.matrix = glm::mat4(1.f);

		if (!jo["mesh"].is_null())
			n.mesh = meshes.begin() + jo["mesh"].as_int64();

		//n.roation = !jo["rotation"].is_null() ? Node::DEFAULT_ROTATION : 

		if (jo["rotation"].is_null())
			n.rotation = Node::DEFAULT_ROTATION;
		else
		{
			auto vec = jo["rotation"].as_array();

			n.rotation[0] = (float)vec[3].as_double();
			n.rotation[1] = (float)vec[0].as_double();
			n.rotation[2] = (float)vec[1].as_double();
			n.rotation[3] = (float)vec[3].as_double();		
		}


		if (jo["scale"].is_null())
			n.scale = Node::DEFAULT_SCALE;
		else
		{
			auto vec = jo["rotation"].as_array();

			n.scale[0] = (float)vec[0].as_double();
			n.scale[1] = (float)vec[1].as_double();
			n.scale[2] = (float)vec[2].as_double();
		}


		if (jo["translation"].is_null())
			n.translation = Node::DEFAULT_TRANSLATION;
		else
		{
			auto vec = jo["translation"].as_array();

			n.translation[0] = (float)vec[0].as_double();
			n.translation[1] = (float)vec[1].as_double();
			n.translation[2] = (float)vec[2].as_double();
		}

		if (!jo["weights"].is_null())
			n.weights = std::vector<float>(jo["weights"].as_array().size());
		for (uint32_t i = 0; i < n.weights.size(); i++)
			n.weights[i] = (float)jo["weights"].as_array()[i].as_double();


	}

	auto scenes = std::vector<Scene>(gltf["scenes"].as_array().size());
	for (uint32_t i = 0; i < scenes.size(); i++)
	{
		auto jo = gltf["scenes"].as_array()[i].as_object();

		auto& scene = scenes[i];
		
		if (!jo["nodes"].is_null())
		{
			auto ns = jo["nodes"].as_array();
			scene.nodes.resize(ns.size());

			for (uint32_t j = 0; j < ns.size(); j++)
			{
				scene.nodes[j] = nodes.begin() + ns[j].as_int64();
			}
		}
	}


	auto scene = scenes.begin() + gltf["scene"].as_int64();

	//
	// Rendering
	//

	glm::vec3
		pos = { 0, 0, -10 },
		dir = { 0, 0, 1 },
		up  = { 0, 1, 0 };
	bool firstClick = true;
	float speed = .1f;
	float sensitivity = 100.f;
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glfwShowWindow(window);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glfwGetFramebufferSize(window, (int*)& g_width, (int*)&g_height);

#pragma region Input handling
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			pos += speed * dir;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			pos += speed * -glm::normalize(glm::cross(dir, up));
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			pos += speed * -dir;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			pos += speed * glm::normalize(glm::cross(dir, up));
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			pos += speed * up;
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			pos += speed * -up;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			speed = 0.4f;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		{
			speed = 0.1f;
		}


		// Handles mouse inputs
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			// Hides mouse cursor
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

			// Prevents camera from jumping on the first click
			if (firstClick)
			{
				glfwSetCursorPos(window, (g_width / 2), (g_height / 2));
				firstClick = false;
			}

			// Stores the coordinates of the cursor
			double mouseX;
			double mouseY;
			// Fetches the coordinates of the cursor
			glfwGetCursorPos(window, &mouseX, &mouseY);

			// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
			// and then "transforms" them into degrees 
			float rotX = sensitivity * (float)(mouseY - (g_height / 2)) / g_height;
			float rotY = sensitivity * (float)(mouseX - (g_width / 2)) / g_width;

			// Calculates upcoming vertical change in the Orientation
			glm::vec3 newOrientation = glm::rotate(dir, glm::radians(-rotX), glm::normalize(glm::cross(dir, up)));

			// Decides whether or not the next vertical Orientation is legal or not
			if (abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
			{
				dir = newOrientation;
			}

			// Rotates the Orientation left and right
			dir = glm::rotate(dir, glm::radians(-rotY), up);

			// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
			glfwSetCursorPos(window, (g_width / 2), (g_height / 2));
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			// Unhides cursor since camera is not looking around anymore
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			// Makes sure the next time the camera looks around it doesn't jump
			firstClick = true;
		}
#pragma endregion


		glm::mat4
			projection = glm::perspective(glm::radians(45.f), (float)g_width / g_height, .1f, 100.f),
			view = glm::lookAt(pos, (pos + dir), up);

		glNamedBufferSubData(matrixUBO, 0 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
		glNamedBufferSubData(matrixUBO, 1 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, g_width, g_height);

		std::function<void(std::vector<Node>::const_iterator, glm::mat4)> drawNodes;

		drawNodes = [matrixUBO, &drawNodes](std::vector<Node>::const_iterator node, glm::mat4 mtx)
		{
			glm::mat4
				matrix = mtx * node->matrix, //todo
				normal = glm::inverse(glm::transpose(matrix));		

			if (node->mesh._Ptr != NULL)
			{
				glNamedBufferSubData(matrixUBO, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(matrix)); //let the node's matrix eb the mesh's matrix
				glNamedBufferSubData(matrixUBO, 3 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(normal));

				auto& mesh = *node->mesh;

				for (uint32_t i = 0; i < mesh.primitives.size(); i++)
				{
					auto prim = mesh.primitives[i];
					auto vao = mesh.vaos[i];

					glBindVertexArray(vao);

					if (prim.indices._Ptr == NULL)
						glDrawArrays(prim.mode, 0, prim.count);
					else
						glDrawElements(prim.mode, prim.indices->count, prim.indices->componentType, (void*)(prim.indices->bufferView->byteOffset + prim.indices->byteOffset));
				}
			}

			
			for (const auto& c : node->children)
				drawNodes(c, matrix);
		};

		for (const auto& n : scene->nodes)
			drawNodes(n, glm::mat4(1.f));

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
