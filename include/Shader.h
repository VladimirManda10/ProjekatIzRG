#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <sstream> // zbog stringstream
#include <fstream> // zbog ifstream (input file stream)
#include <string>

//f-ja koja cita sadrzaj fajla
std::string readFileContent(std::string path) {
	//ifstream je objekat koji ima konstruktor koji uzima
	//putanju do fajla i ono sto ce uraditi sa tim fajlom je 
	//da ce ga otvoriti za citanje
	std::ifstream in(path);
	//ako imamo bafer u koji hocemo da upisujemo iznova i iznova,
	//i onda kada zavrsimo to upisivanje da pokupimo sadrzaj iz njega
	//u tu svrhu koristimo stringstream objekat
	std::stringstream buffer;
	//ucitavamo sadrzaj fajla u bafer
	buffer << in.rdbuf();
	//vracamo sadrzaj bafera u vidu stringa
	return buffer.str();
}



class Shader {
public:
	unsigned int id;
	Shader(const char* fragmentShaderPath, const char* vertexShaderPath) {
		std::string vsString = readFileContent(vertexShaderPath);
		std::string fsString = readFileContent(fragmentShaderPath);

		//f-ja c_str() vraca pokazivac na bafer u kom
		//vsString cuva karaktere iz odg fajla
		const char* vsSource = vsString.c_str();
		int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vsSource, NULL);
		glCompileShader(vertexShader);
		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog;
		}
		const char* fsSource = fsString.c_str();
		int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fsSource, NULL);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog;
		}

		//linkovanje sejdera
		int shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog;
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		id = shaderProgram;
	}
	void use() {
		//arg je id programa koji 
		//treba da aktiviramo
		glUseProgram(id);
	}
	void setUniform4f(std::string name, float x, float y, float z, float w) {
		int uniformId = glGetUniformLocation(id, name.c_str());
		glUniform4f(uniformId, x, y, z, w);
	}
	void setUniform1i(std::string name, int x) {
		int uniformId = glGetUniformLocation(id, name.c_str());
		glUniform1i(uniformId, x);
	}
	void setUniform1f(std::string name, double value) {
		int uniformId = glGetUniformLocation(id,name.c_str());
		glUniform1f(uniformId, value);
	}
	void setVec3(std::string name, glm::vec3 value) {
		int uniformId = glGetUniformLocation(id, name.c_str());
		glUniform3fv(uniformId, 1, &value[0]);
	}
	void setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
	}
	void setMat4(std::string name, glm::mat4 mat) {
		int uniformId = glGetUniformLocation(id, name.c_str());
		glUniformMatrix4fv(uniformId, 1, GL_FALSE, &mat[0][0]);
	}
	void deleteProgram() {
		glDeleteProgram(id);
		id = 0;
	}

};

#endif