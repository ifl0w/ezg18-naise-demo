#include <systems/render-engine/materials/shaders/ShaderUtils.hpp>

#include <fstream>
#include <iostream>

using namespace NAISE::Engine;

std::string readFile(const std::string path) {
	std::string content;
	std::ifstream fileStream(path);

	if (!fileStream.is_open()) {
		std::string s("Could not open file ");
		s = s.append(path);
		throw std::runtime_error(s);
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

GLuint NAISE::Engine::reloadShaderProgram(std::string vertexShaderFile, std::string fragmentShaderFile) {
	GLuint program = 0;
	//glCreateProgram();
	return program;
}

GLuint NAISE::Engine::createShaderProgram(std::string vertexShaderFile, std::string fragmentShaderFile) {
	//Read our shaders into the appropriate buffers
	std::string vertexSource = readFile(vertexShaderFile); //Get source code for vertex shader.
	std::string fragmentSource = readFile(fragmentShaderFile); //Get source code for fragment shader.

	//Create an empty vertex shader handle
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//Send the vertex shader source code to GL
	//Note that std::string's .c_str is NULL character terminated.
	const auto* source = (const GLchar*) vertexSource.c_str();
	glShaderSource(vertexShader, 1, &source, 0);

	//Compile the vertex shader
	glCompileShader(vertexShader);

	GLint isCompiled = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == 0) {
		GLint maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

		//We don't need the shader anymore.
		glDeleteShader(vertexShader);


		throw std::runtime_error("[" + vertexShaderFile + "] Could not compile vertex shaders. " + std::string(infoLog.data()));
	}

	//Create an empty fragment shader handle
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Send the fragment shader source code to GL
	//Note that std::string's .c_str is NULL character terminated.
	source = (const GLchar*) fragmentSource.c_str();
	glShaderSource(fragmentShader, 1, &source, 0);

	//Compile the fragment shader
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == 0) {
		GLint maxLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

		//We don't need the shader anymore.
		glDeleteShader(fragmentShader);
		//Either of them. Don't leak shaders.
		glDeleteShader(vertexShader);

		// loginfo to stdout.
		std::cout << std::string(infoLog.data()) << std::endl;

		throw std::runtime_error("[" + fragmentShaderFile + "] Could not compile fragment shaders." + std::string(infoLog.data()));
	}

	//Vertex and fragment shaders are successfully compiled.
	//Now time to link them together into a program.
	//Get a program object.
	GLuint program = glCreateProgram();

	//Attach our shaders to our program
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	//Link our program
	glLinkProgram(program);

	//Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*) &isLinked);
	if (isLinked == 0) {
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		//We don't need the program anymore.
		glDeleteProgram(program);
		//Don't leak shaders either.
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		throw std::runtime_error("Could not link shaders: " + std::string(infoLog.data()));
	}

	//Always detach shaders after a successful link.
	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

GLuint NAISE::Engine::createComputeShader(std::string singleShaderFile)
{
	//Read our shader into the appropriate buffers
	std::string sourceCode = readFile(singleShaderFile);

	//Create an empty vertex shader handle
	GLuint shader = glCreateShader(GL_COMPUTE_SHADER);

	//Send the vertex shader source code to GL
	//Note that std::string's .c_str is NULL character terminated.
	const auto* source = sourceCode.c_str();
	glShaderSource(shader, 1, &source, 0);

	//Compile the vertex shader
	glCompileShader(shader);

	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == 0) {
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

		//We don't need the shader anymore.
		glDeleteShader(shader);

		throw std::runtime_error("Could not compile shader. ("+ singleShaderFile + ")\n" + std::string(infoLog.data()));
	}

	// link the program and get a program object
	GLuint program = glCreateProgram();

	//Attach our shaders to our program
	glAttachShader(program, shader);

	//Link our program
	glLinkProgram(program);

	//Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*) &isLinked);
	if (isLinked == 0) {
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		//We don't need the program anymore.
		glDeleteProgram(program);
		//Don't leak shaders either.
		glDeleteShader(shader);

		throw std::runtime_error("Could not link shaders: " + std::string(infoLog.data()));
	}

	//Always detach shaders after a successful link.
	glDetachShader(program, shader);

	glDeleteShader(shader);

	return program;
}

GLint NAISE::Engine::uniformLocation(GLint shader, std::string name) {
	GLint res = glGetUniformLocation(shader, name.c_str());

	if (res == -1) {
#ifdef _DEBUG
		// TODO: investigate how to handle
		//throw std::runtime_error(std::string("Uniform location not found: ") + name);
		//std::cout << std::string("Uniform location not found: ") + name << std::endl;
#endif // !_DEBUG
	}

	return res;
}