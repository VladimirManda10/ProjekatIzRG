#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <Shader.h>
#include <Mesh.h>
#include <assimp/Importer.hpp>
//#include <LearnOpenGL-master/includes/assimp/Importer.hpp>
#include <assimp/scene.h>
//#include <LearnOpenGL-master/includes/assimp/scene.h>
#include <assimp/postprocess.h>
//#include <LearnOpenGL-master/includes/assimp/postprocess.h>
//#include <unordered_map>

unsigned int TextureFromFile(const char* filename, std::string directory);

class Model {
	//da bi nacrtali neki model
	//crtamo svaki od mesheva od kog se sastoji
public:
	//svaki model ce imati niz mesheva
	//mi cemo model crtati tako sto nacrtamo
	//svaki od tih mesheva
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> loaded_textures;
//	std::unordered_map<std::string, Texture> loaded_textures_map;
	//konstruktor
	//uzece putanju od nekog fajla i onda ce
	//na toj putanji ucitati ceo backpack
	Model(std::string path) {
		//ucitavanje modela ce se svesti na popunjavanje
		//mesheva,da bi popunio ceo ovaj vektor Mesheva
		//mora da popuni informacije o vertexima,
		 //indeksima i teksturama
		loadModel(path);
	}
	//PRVO SMO URADILI UCITAVANJE MODELA I
	//IMPLEMENTACIJU F-JE DRAW

	void Draw(Shader& shader) {
		//model crtamo tako sto nacrtamo svaki od meshova
		for (Mesh& mesh : meshes) {
			mesh.Draw(shader);
		}
	}
private:
	void loadModel(std::string& path) {
		//da bismo ucitali model sa assimpom
		//moramo da napravimo assimp objekat koji se 
		//zove importer
		Assimp::Importer importer;
		//ova f-ja ce u pozadini da procita
		//fajl koji budemo prosledili,da alocira
		//sve potrebne strukture podataka i da nam
		//vrati pokazivac na Scene(pogledaj crtez u paint-u)
		//2. argument je sta assimp treba da uradi sa modelom
		//kada ga bude ucitavao 
		const aiScene* scene = importer.ReadFile(path,aiProcess_Triangulate |
		aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "Failed to load a model";
			return;
		}

		this->directory = path.substr(0, path.find_last_of('/'));


		//procesiraj Node,kreni od korenog u sceni
		processNode(scene->mRootNode, scene);
	}
	//potrebno je da prodjemo kroz sve nodove,
	//da obradimo sve mesheve i onda da isto to uradimo
	//za svu decu
	void processNode(aiNode* node, const aiScene* scene) {
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			//assimp kada ucita sve podatke
			//smesti ih u niz u scene objektu
			//imamo jedan niz mesheva (u scenu)
			//a svaki od mesheva ima vertexe,normale,koord tekstura itd
			//a onda u node-ovima koji predstavljaju podobjekte
			//niz mesheva sadrzi indeks u scene meshu
			//niz scene mesheva gledamo kao neki niz
			//koji ima mesh1,mesh2,mesh3 itd,a root node  i
			//child nodovi njihov niz je samo niz indeksa
			//koji kaze da se taj podobjekat sastoji 
			//npr od mesha2 mesha3 i mesha 5 i onda kada
			//i onda kada hocemo da pristupimo npr meshu2
			//mi uzmemo indeks 2 i pristupimo nizu mesheva
			//u sceni i on ce vratiti odg mesh
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			//sada treba da ga dodamo u vektor mesheva
			//medjutim da bismo imali poklapanje tipova
			//izmedju naseg mesha i assimpovog
			//moramo da konvertujemo
			meshes.push_back(processMesh(mesh, scene));
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}
	}
	Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
		//treba da pokupimo sve ove atribute koji su
		//u assimpu sacuvani i da ih konvertujemo u nas
		//mesh
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			//prolazimo kroz niz pozicija,
			//vadimo x,y,z za svaki 
			//i prepisujemo ga u vertex na odg polje
			Vertex vertex;
			vertex.Position.x =  mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;
			if (mesh->HasNormals()) {
				vertex.Normals.x = mesh->mNormals[i].x;
				vertex.Normals.y = mesh->mNormals[i].y;
				vertex.Normals.z = mesh->mNormals[i].z;
			}
			//svaki vertex moze da ima do 8 koord tekstura
			//mTextureCoords je niz nizova tekstura koordinata
			//posto svaki vertex moze da ima vise tekstura
			//koordinata
			if (mesh->mTextureCoords[0]) {
				vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
				vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
				
				vertex.Tangent.x = mesh->mTangents[i].x;
				vertex.Tangent.y = mesh->mTangents[i].y;
				vertex.Tangent.z = mesh->mTangents[i].z;
				
				vertex.Bitangent.x = mesh->mBitangents[i].x;
				vertex.Bitangent.y = mesh->mBitangents[i].y;
				vertex.Bitangent.z = mesh->mBitangents[i].z;
			}
			else {
				vertex.TexCoords = glm::vec2(0.0);
			}
			vertices.push_back(vertex);
		}
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];

			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
		//sada treba da izvucemo teksture
		//scena kao sto ima niz svih meseva
		//tako ima i niz svih materijala
		//jedan materijal moze da se sastoji iz vise
		//tekstura,takodje moze da sadrzi vise tekstura istog
		//tipa
		
		//treba sve da ih potrpamo u 1 niz tekstura jer mesh
		//uzima samo jedan niz tekstura

		
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		
		//ovaj deo je mogao efikasnije
		//mi ovde uvek push backujemo na kraj
		//
		//std::vector<Texture> diffuseMaps = loadTextureMaterial(material,aiTextureType_DIFFUSE,"texture_diffuse");
		loadTextureMaterial(material, aiTextureType_DIFFUSE, "texture_diffuse",textures);
		//textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		
		//std::vector<Texture> specularMaps = loadTextureMaterial(material,aiTextureType_SPECULAR,"texture_specular",textures);
		loadTextureMaterial(material, aiTextureType_SPECULAR, "texture_specular", textures);
		//textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		
		//std::vector<Texture> normalMaps = loadTextureMaterial(material,aiTextureType_NORMALS,"texture_normal",textures);
		loadTextureMaterial(material, aiTextureType_NORMALS, "texture_normal", textures);
		//textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		
		//std::vector<Texture> heightMaps = loadTextureMaterial(material,aiTextureType_HEIGHT,"texture_height",textures);
		loadTextureMaterial(material, aiTextureType_HEIGHT, "texture_height", textures);
		//textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		
		return Mesh(vertices,indices,textures);
	}

	//pozovemo loadTextureMaterial
	//iz materiala vraca nam sve teksture odredjenog tipa
	//zatim u for petlji idemo od 0 pa do
	//koliko imamo tekstura tog tipa
	//svaka od tekstura u materijalu
	//se cuva u zasebnom nizu,difuzne idu u jedan niz
	//normale u drugi,spekularne u treci itd
	//kada pozovemo metodu getTexture
	//mi njoj prosledimo tip
	//to njemu kaze kom nizu treba da pristupi i koju
	//teksturu po redu iz niza mi zapravo ucitavamo
	
	//sada kada pushujemo,pushujemo direktno u originalni
	//vektor(ovo se odnosi na vremensko poboljsanje)


	//std::vector<Texture> loadTextureMaterial(aiMaterial* mat, aiTextureType type, std::string typeName) {
	void loadTextureMaterial(aiMaterial* mat, aiTextureType type, std::string typeName,std::vector<Texture>& textures) {
		//std::vector<Texture> textures;
		
		//iz materijala strukturu odredjenog tipa
		//mozemo da izvucemo sa 
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
			//kada ucitavamo teksturu,kazemo kog je tipa
			//i koja je po redu
			aiString str;

			//ne vraca apsolutnu putanju vec npr samo diffuse.jpg
			//tako da mi moramo nekako da zapamtimo i direktorijum u
			//kom se nalazi nas model
			mat->GetTexture(type, i, &str);

			bool skip = false;
			/*auto it = loaded_textures_map.find(str);
			if (it != loaded_textures_map.end()) {
				skip = true;
				textures.push_back(*it);
			}*/

			//ovde takodje mozemo da dobijemo na vremenu
			//posto je ovo dupla petlja,slozenost je
			//m*n
			for (unsigned int j = 0; j < loaded_textures.size(); j++) {
				if (std::strcmp(str.C_Str(), loaded_textures[j].path.c_str()) == 0) {
					//ako je tekstura vec ucitana
					//onda cemo mi u ovaj rezultujuci 
					//vektor koji vracamo iz f-je
					//samo da ubacimo tu vec ucitanu
					//teksturu
					textures.push_back(loaded_textures[j]);
					skip = true;
					break;
				}
			}

			//proveravamo ako smo vec ucitali teksturu
			//onda nemoj ponovo da je ucitavas
			//a ako je nismo do sada vec ucitali,onda treba
			//da je ucitas
			if (!skip) {
				Texture texture;
				texture.ID = TextureFromFile(str.C_Str(), this->directory);//koja ce da generise id
				//tako sto ce iz teksture na prosledjenoj putanji
				//da napravi opengl texture obj i da nam vrati
				//njegov id
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				loaded_textures.push_back(texture);
			}
		}
	//return textures;
	
	}
};

unsigned int TextureFromFile(const char* filename, std::string directory) {
	std::string fullPath(directory + '/' +filename);
	unsigned int textureID;
	//generisemo 1 tekstura objekat
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(fullPath.c_str(),&width,&height,&nrComponents,0);
	//ukoliko smo uspeli da ucitamo sliku
	//treba da saznamo kog je ona formata
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		
		//aktiviramo teksutru ssa datim ID-em kao
		//2d teksturu
		glBindTexture(GL_TEXTURE_2D, textureID);
		//na trenutno aktivnu 2D teksturu nakaci
		//sliku sa sl. parametrima
		//0 nivo mipmape,unutrasnji format u kome ces 
		//cuvati je isti kao i format slike koja je ucitana
		//sirina je width,visina height
		//legacy par,zapravo format slike koji je sacuvan u nizu bajtova
		//je isti kao onaj u kom cemo ga cuvati,
		//on je sacuvan kao niz unsigned bajtova i pocinje na memorijskoj
		//lokaciji data
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		//podesavamo parametre za wrapovanje i parametre
		//za filtriranje
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//postavljamo parametre za filtriranje teksture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}
	
	return textureID;
}
#endif
