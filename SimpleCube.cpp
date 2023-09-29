// This has been adapted from the Vulkan tutorial

#include "Starter.hpp"

struct MeshUniformBlock {
	alignas(4) float amb;
	alignas(4) float gamma;
	alignas(16) glm::vec3 sColor;
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct OverlayUniformBlock {
	alignas(4) float visible;
};

struct GlobalUniformBlock {
	alignas(16) glm::vec3 DlightDir;
	alignas(16) glm::vec3 DlightColor;
	alignas(16) glm::vec3 AmbLightColor;
	alignas(16) glm::vec3 eyePos;
	alignas(16) glm::vec3 lightPos;
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec4 lightColor;
};



struct PieUniformBlock {
	alignas(16) glm::vec3 lightPos1;
	alignas(16) glm::vec3 lightDir1;
	alignas(16) glm::vec4 lightColor1;
	alignas(16) glm::vec3 lightPos2;
	alignas(16) glm::vec3 lightDir2;
	alignas(16) glm::vec4 lightColor2;
	alignas(16) glm::vec3 lightPos3;
	alignas(16) glm::vec3 lightDir3;
	alignas(16) glm::vec4 lightColor3;
	alignas(16) glm::vec3 eyePos;
};


// The vertices data structures
struct VertexMesh {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};

struct VertexOverlay {
	glm::vec2 pos;
	glm::vec2 UV;
};


// MAIN ! 
class Game : public BaseProject {
protected:

	// Current aspect ratio (used by the callback that resized the window
	float Ar;

	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSLGubo1, DSLGubo5;
	DescriptorSetLayout DSLMesh1;
	DescriptorSetLayout DSLOverlay;

	// Vertex formats
	VertexDescriptor VMesh1, VOverlay;

	// Pipelines [Shader couples]
	Pipeline PMesh1, PMesh2, PMesh3, PMesh4, PMesh5, POverlay;

	// Models, textures and Descriptors (values assigned to the uniforms)
// Please note that Model objects depends on the corresponding vertex structure
	Model<VertexMesh> MBody, Mcity, Mpie, Mfarmer, Mkitchen, Mcar, Mapple, Mdungeon, Mcourt;
	Model<VertexOverlay> MKey, MSplash, MSplash2, MSplash3, Mwin, Mwin2;
	DescriptorSet DSGubo1, DSGubo2, DSGubo3, DSGubo4, DSGubo5;
	DescriptorSet DSBody,DSKey, DSSplash, DSSplash2, DSSplash3, DScity, DSCity2, DScar, DSapple1, DSapple2, DSwin, DSwin2; // , DSchar;
	DescriptorSet DSfarmer, DSkitchen, DSpie2;
	DescriptorSet DSdungeon, DSpie;
	DescriptorSet DScourt, DSapple3;

	Texture TBody, TWheel, TKey, TSplash, TCity, Tcar, Tapple;
	Texture Tkitchen, Tfarmer;
	Texture Tdungeon, Tpie, Twin, Twin2;

	// C++ storage for uniform variables
	MeshUniformBlock uboBody, uboCity, ubocar, uboapple1, uboapple2; 
	MeshUniformBlock uboKitchen, ubofarmer, ubopie2;
	MeshUniformBlock ubodungeon, ubopie;
	MeshUniformBlock ubocourt, uboapple3;
	GlobalUniformBlock gubo1, gubo2, gubo3, gubo4;
	OverlayUniformBlock uboKey, uboSplash, uboSplash2, uboSplash3, uboWin, uboWin2;
	PieUniformBlock gubo5;

	// Other application parameters
	int currScene = 0;
	float CamH, CamRadius, CamPitch, CamYaw;
	int gameState;
	glm::vec3 Pos = glm::vec3(0, 0.5f, -1);
	glm::vec3 cameraPos;
	float Yaw = -39.2076f;;
	float Pitch = glm::radians(0.0f);
	float Roll = glm::radians(0.0f);
	int apple1 = 0;
	int apple2 = 0;
	int apple3 = 0;
	int win = 0;
	glm::vec3 apple1Pos = glm::vec3(-8, 0, 12);
	glm::vec3 apple2Pos = glm::vec3(6, 0, 7);
	glm::vec3 apple3Pos = glm::vec3(0.7f, 0, 6.77f);
	glm::vec3 pie2Pos = glm::vec3(0, -0.5f, -1);
	int third_person = 0;

	float alpha = 0.0;
	float beta = 0.0;
	float rho = 0.0;

	float cx = 0;
	float cy = -(-0.25);
	float cz = -1.5 * glm::cos(10);

	glm::mat4 Worldcar_position = glm::mat4(1);
	glm::mat4 Worldcar_rotation = glm::mat4(1);

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "The 3 apples";
		windowResizable = GLFW_TRUE;
		initialBackgroundColor = { 0.0f, 0.005f, 0.01f, 1.0f };

		// Descriptor pool sizes
		uniformBlocksInPool = 24;
		texturesInPool = 24;
		setsInPool = 24;

		Ar = (float)windowWidth / (float)windowHeight;
	}

	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		Ar = (float)w / (float)h;
	}

	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]


		DSLMesh1.init(this, {
			// this array contains the bindings:
			// first  element : the binding number
			// second element : the type of element (buffer or texture)
			//                  using the corresponding Vulkan constant
			// third  element : the pipeline stage where it will be used
			//                  using the corresponding Vulkan constant
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});


		DSLOverlay.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});

		DSLGubo1.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
			});

		DSLGubo5.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
			});




		// Vertex descriptors
		VMesh1.init(this, {
			// this array contains the bindings
			// first  element : the binding number
			// second element : the stride of this binging
			// third  element : whether this parameter change per vertex or per instance
			//                  using the corresponding Vulkan constant
			{0, sizeof(VertexMesh), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {

				{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, pos),
					   sizeof(glm::vec3), POSITION},
				{0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, norm),
					   sizeof(glm::vec3), NORMAL},
				{0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexMesh, UV),
					   sizeof(glm::vec2), UV}
			});


		VOverlay.init(this, {
				  {0, sizeof(VertexOverlay), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
			  {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, pos),
					 sizeof(glm::vec2), OTHER},
			  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, UV),
					 sizeof(glm::vec2), UV}
			});


		// Pipelines [Shader couples]
// The second parameter is the pointer to the vertex definition
// Third and fourth parameters are respectively the vertex and fragment shaders
// The last array, is a vector of pointer to the layouts of the sets that will
// be used in this pipeline. The first element will be set 0, and so on..

		PMesh1.init(this, &VMesh1, "shaders/MeshVert.spv", "shaders/MeshFrag.spv", { &DSLGubo1, &DSLMesh1 });

		PMesh2.init(this, &VMesh1, "shaders/MeshVert.spv", "shaders/Cityfrag.spv", { &DSLGubo1, &DSLMesh1 });

		PMesh3.init(this, &VMesh1, "shaders/MeshVert.spv", "shaders/MeshFragCity2.spv", { &DSLGubo1, &DSLMesh1 });

		PMesh4.init(this, &VMesh1, "shaders/MeshVert.spv", "shaders/BlinnFrag3.spv", { &DSLGubo1, &DSLMesh1 });

		PMesh5.init(this, &VMesh1, "shaders/MeshVert.spv", "shaders/BlinnFrag5.spv", { &DSLGubo1, &DSLMesh1 });


		POverlay.init(this, &VOverlay, "shaders/OverlayVert.spv", "shaders/OverlayFrag.spv", { &DSLOverlay });
		POverlay.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE, false);

		// Models, textures and Descriptors (values assigned to the uniforms)

// Create models
// The second parameter is the pointer to the vertex definition for this model
// The third parameter is the file name
// The last is a constant specifying the file type: currently only OBJ or GLTF
		MBody.init(this, &VMesh1, "Models/SlotBody.obj", OBJ);
		Mcity.init(this, &VMesh1, "Models/City.obj", OBJ);
		Mcar.init(this, &VMesh1, "Models/Car.obj", OBJ);
		Mpie.init(this, &VMesh1, "Models/dessert_pie.obj", OBJ);
		Mkitchen.init(this, &VMesh1, "Models/Interior.obj", OBJ);
		Mfarmer.init(this, &VMesh1, "Models/Character.obj", OBJ);
		Mapple.init(this, &VMesh1, "Models/apple.obj", OBJ);
		Mdungeon.init(this, &VMesh1, "Models/Dungeon.obj", OBJ);
		Mcourt.init(this, &VMesh1, "Models/courts.mgcg", MGCG);




		//// Creates a mesh with direct enumeration of vertices and indices
		MKey.vertices = { {{-0.8f, 0.6f}, {0.0f,0.0f}}, {{-0.8f, 0.95f}, {0.0f,1.0f}},
						 {{ 0.8f, 0.6f}, {1.0f,0.0f}}, {{ 0.8f, 0.95f}, {1.0f,1.0f}} };
		MKey.indices = { 0, 1, 2,    1, 2, 3 };
		MKey.initMesh(this, &VOverlay);

		//// Creates a mesh with direct enumeration of vertices and indices
		MSplash.vertices = { {{0.6f, 0.75f}, {0.0f,0.0f}}, {{0.6f, 0.95f}, {0.0f,1.0f}},
						 {{ 0.8f, 0.75f}, {1.0f,0.0f}}, {{ 0.8f, 0.95f}, {1.0f,1.0f}} };
		MSplash.indices = { 0, 1, 2,    1, 2, 3 };
		MSplash.initMesh(this, &VOverlay);

		MSplash2.vertices = { {{0.4f, 0.75f}, {0.0f,0.0f}}, {{0.4f, 0.95f}, {0.0f,1.0f}},
						 {{ 0.6f, 0.75f}, {1.0f,0.0f}}, {{ 0.6f, 0.95f}, {1.0f,1.0f}} };
		MSplash2.indices = { 0, 1, 2,    1, 2, 3 };
		MSplash2.initMesh(this, &VOverlay);

		MSplash3.vertices = { {{0.8f, 0.75f}, {0.0f,0.0f}}, {{0.8f, 0.95f}, {0.0f,1.0f}},
						 {{ 1.0f, 0.75f}, {1.0f,0.0f}}, {{ 1.0f, 0.95f}, {1.0f,1.0f}} };
		MSplash3.indices = { 0, 1, 2,    1, 2, 3 };
		MSplash3.initMesh(this, &VOverlay);

		Mwin.vertices = { {{-0.8f, 0.6f}, {0.0f,0.0f}}, {{-0.8f, 0.95f}, {0.0f,1.0f}},
						 {{ 0.8f, 0.6f}, {1.0f,0.0f}}, {{ 0.8f, 0.95f}, {1.0f,1.0f}} };
		Mwin.indices = { 0, 1, 2,    1, 2, 3 };
		Mwin.initMesh(this, &VOverlay);

		Mwin2.vertices = { {{-0.8f, 0.6f}, {0.0f,0.0f}}, {{-0.8f, 0.95f}, {0.0f,1.0f}},
						 {{ 0.8f, 0.6f}, {1.0f,0.0f}}, {{ 0.8f, 0.95f}, {1.0f,1.0f}} };
		Mwin2.indices = { 0, 1, 2,    1, 2, 3 };
		Mwin2.initMesh(this, &VOverlay);

		// Create the textures
		// The second parameter is the file name
		TBody.init(this, "textures/SlotBody.png");
		TWheel.init(this, "textures/SlotWheel.png");
		TKey.init(this, "textures/Layout_1.png");
		TSplash.init(this, "textures/apple_logo.jpg");
		TCity.init(this, "textures/TexturesCity.png");
		Tkitchen.init(this, "textures/TexturesInterior.png");
		Tcar.init(this, "textures/Colors2.png");
		Tpie.init(this, "textures/dessert_pie.jpg");
		Tfarmer.init(this, "textures/Colors2.png");
		Tapple.init(this, "textures/apple_color.png");
		Tdungeon.init(this, "textures/TexturesDungeon.png");
		Twin.init(this, "textures/wintokitchen.png");
		Twin2.init(this, "textures/winzoom.png");

		// Init local variables
		CamH = 1.0f;
		CamRadius = 3.0f;
		CamPitch = glm::radians(15.f);
		CamYaw = glm::radians(30.f);
		gameState = 0;
	}

	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		PMesh1.create();
		PMesh2.create();
		PMesh3.create();
		PMesh4.create();
		PMesh5.create();
		POverlay.create();




		// Here you define the data set
		DSBody.init(this, &DSLMesh1, {
			// the second parameter, is a pointer to the Uniform Set Layout of this set
			// the last parameter is an array, with one element per binding of the set.
			// first  elmenet : the binding number
			// second element : UNIFORM or TEXTURE (an enum) depending on the type
			// third  element : only for UNIFORMs, the size of the corresponding C++ object. For texture, just put 0
			// fourth element : only for TEXTUREs, the pointer to the corresponding texture object. For uniforms, use nullptr
						{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
						{1, TEXTURE, 0, &TBody}
			});
		DSKey.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TKey}
			});
		DSSplash.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TSplash}
			});
		DSSplash2.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TSplash}
			});
		DSSplash3.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TSplash}
			});

		DSwin.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &Twin}
			});

		DSwin2.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &Twin2}
			});

		DSGubo1.init(this, &DSLGubo1, {
			{0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
			});

		DSGubo2.init(this, &DSLGubo1, {
					{0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
			});

		DSGubo3.init(this, &DSLGubo1, {
			{0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
			});

		DSGubo4.init(this, &DSLGubo1, {
			{0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
			});

		DSGubo5.init(this, &DSLGubo5, {
			{0, UNIFORM, sizeof(PieUniformBlock), nullptr}
			});

		DScity.init(this, &DSLMesh1, {
						{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
						{1, TEXTURE, 0, &TCity}
			});

		DSkitchen.init(this, &DSLMesh1, {
						{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
						{1, TEXTURE, 0, &Tkitchen}
			});

		DSapple1.init(this, &DSLMesh1, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &Tapple}
			});

		DSapple2.init(this, &DSLMesh1, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &Tapple}
			});

		DSapple3.init(this, &DSLMesh1, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &Tapple}
			});

		DScar.init(this, &DSLMesh1, {
						{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
						{1, TEXTURE, 0, &Tcar}
			});
		DSpie.init(this, &DSLMesh1, {
						{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
						{1, TEXTURE, 0, &Tpie}
			});

		DSpie2.init(this, &DSLMesh1, {
						{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
						{1, TEXTURE, 0, &Tpie}
			});

		DSfarmer.init(this, &DSLMesh1, {
				{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
				{1, TEXTURE, 0, &Tfarmer}
			});

		DSdungeon.init(this, &DSLMesh1, {
				{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
				{1, TEXTURE, 0, &Tdungeon}
			});

		DScourt.init(this, &DSLMesh1, {
				{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
				{1, TEXTURE, 0, &TCity}
			});
	}


	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		PMesh1.cleanup();
		PMesh2.cleanup();
		PMesh3.cleanup();
		PMesh4.cleanup();
		PMesh5.cleanup(); 
		POverlay.cleanup();

		// Cleanup datasets
		DSBody.cleanup();
		DSapple1.cleanup();
		DSapple2.cleanup();
		DSapple3.cleanup();
		DScity.cleanup();
		DSkitchen.cleanup();
		DScar.cleanup();
		DSpie.cleanup();
		DSfarmer.cleanup();
		DSdungeon.cleanup();
		DSpie.cleanup();
		DScourt.cleanup();

		DSwin.cleanup();
		DSwin2.cleanup();
		DSKey.cleanup();
		DSSplash.cleanup();
		DSSplash2.cleanup();
		DSSplash3.cleanup();
		DSGubo1.cleanup();
		DSGubo2.cleanup();
		DSGubo3.cleanup();
		DSGubo4.cleanup();
		DSGubo5.cleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {
		// Cleanup textures
		TBody.cleanup();
		TWheel.cleanup();
		TKey.cleanup();
		Tkitchen.cleanup();
		TSplash.cleanup();
		Tcar.cleanup();
		Tpie.cleanup();
		Tfarmer.cleanup();
		Tapple.cleanup();
		Tdungeon.cleanup();
		Twin.cleanup();
		Twin2.cleanup();

		// Cleanup models
		MBody.cleanup();
		MKey.cleanup();
		Mkitchen.cleanup();
		MSplash.cleanup();
		MSplash2.cleanup();
		MSplash3.cleanup();
		Mwin.cleanup();
		Mwin2.cleanup();
		Mcity.cleanup();
		Mcar.cleanup();
		Mpie.cleanup();
		Mfarmer.cleanup();
		Mapple.cleanup();
		Mdungeon.cleanup();
		Mcourt.cleanup();

		// Cleanup descriptor set layouts

		DSLMesh1.cleanup();
		DSLOverlay.cleanup();

		DSLGubo1.cleanup();
		DSLGubo5.cleanup();

		// Destroies the pipelines
		PMesh1.destroy();
		PMesh2.destroy();
		PMesh3.destroy();
		PMesh4.destroy();
		PMesh5.destroy();
		POverlay.destroy();
	}

	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures

	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {


		switch (currScene) {
		case 0:


			// binds the pipeline
			PMesh1.bind(commandBuffer);

			DSGubo1.bind(commandBuffer, PMesh1, 0, currentImage);


			Mfarmer.bind(commandBuffer);
			DSfarmer.bind(commandBuffer, PMesh1, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mfarmer.indices.size()), 1, 0, 0, 0);

			Mkitchen.bind(commandBuffer);
			DSkitchen.bind(commandBuffer, PMesh1, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mkitchen.indices.size()), 1, 0, 0, 0);

			Mpie.bind(commandBuffer);
			DSpie2.bind(commandBuffer, PMesh1, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mpie.indices.size()), 1, 0, 0, 0);

			break;

		case 1:

			// binds the pipeline
			PMesh2.bind(commandBuffer);
			// For a pipeline object, this command binds the corresponing pipeline to the command buffer passed in its parameter

			// sets global uniforms (see below fro parameters explanation)
			DSGubo2.bind(commandBuffer, PMesh2, 0, currentImage);
			// For a pipeline object, this command binds the corresponing pipeline to the command buffer passed in its parameter

			// binds the model
			MBody.bind(commandBuffer);
			// For a Model object, this command binds the corresponing index and vertex buffer
			// to the command buffer passed in its parameter
						// binds the data set
			DSBody.bind(commandBuffer, PMesh2, 1, currentImage);
			// For a Dataset object, this command binds the corresponing dataset
			// to the command buffer and pipeline passed in its first and second parameters.
			// The third parameter is the number of the set being bound
			// As described in the Vulkan tutorial, a different dataset is required for each image in the swap chain.
			// This is done automatically in file Starter.hpp, however the command here needs also the index
			// of the current image in the swap chain, passed in its last parameter

			// record the drawing command in the command buffer
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MBody.indices.size()), 1, 0, 0, 0);
			// the second parameter is the number of indexes to be drawn. For a Model object,
			// this can be retrieved with the .indices.size() method.

			Mcar.bind(commandBuffer);
			DScar.bind(commandBuffer, PMesh2, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mcar.indices.size()), 1, 0, 0, 0);


			Mcity.bind(commandBuffer);
			DScity.bind(commandBuffer, PMesh2, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mcity.indices.size()), 1, 0, 0, 0);

			Mapple.bind(commandBuffer);
			DSapple1.bind(commandBuffer, PMesh2, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mapple.indices.size()), 1, 0, 0, 0);
			break;

		case 2:

			PMesh3.bind(commandBuffer);
			Mdungeon.bind(commandBuffer);
			DSGubo3.bind(commandBuffer, PMesh3, 0, currentImage);
			DSdungeon.bind(commandBuffer, PMesh3, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mdungeon.indices.size()), 1, 0, 0, 0);

			Mapple.bind(commandBuffer);
			DSapple2.bind(commandBuffer, PMesh3, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mapple.indices.size()), 1, 0, 0, 0);

			break;


		case 3:

			PMesh4.bind(commandBuffer);
			DSGubo4.bind(commandBuffer, PMesh4, 0, currentImage);
			Mcourt.bind(commandBuffer);
			DScourt.bind(commandBuffer, PMesh4, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mcourt.indices.size()), 1, 0, 0, 0);

			Mapple.bind(commandBuffer);
			DSapple3.bind(commandBuffer, PMesh4, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mapple.indices.size()), 1, 0, 0, 0);

			break;


		case 4:

			PMesh5.bind(commandBuffer);
			DSGubo5.bind(commandBuffer, PMesh5, 0, currentImage);

			Mpie.bind(commandBuffer);
			DSpie.bind(commandBuffer, PMesh5, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mpie.indices.size()), 1, 0, 0, 0);

			break;

		}


		POverlay.bind(commandBuffer);
		MKey.bind(commandBuffer);
		DSKey.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MKey.indices.size()), 1, 0, 0, 0);

		POverlay.bind(commandBuffer);
		Mwin2.bind(commandBuffer);
		DSwin2.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mwin2.indices.size()), 1, 0, 0, 0);

		MSplash.bind(commandBuffer);
		DSSplash.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MSplash.indices.size()), 1, 0, 0, 0);

		MSplash2.bind(commandBuffer);
		DSSplash2.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MSplash2.indices.size()), 1, 0, 0, 0);

		MSplash3.bind(commandBuffer);
		DSSplash3.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MSplash3.indices.size()), 1, 0, 0, 0);

		Mwin.bind(commandBuffer);
		DSwin.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mwin.indices.size()), 1, 0, 0, 0);

	}

	void updateUniformBuffer(uint32_t currentImage) {

		static bool debounce = false;
		static int curDebounce = 0;

		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			if (!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_SPACE;
				currScene = (currScene + 1) % 4;
				if (currScene == 1)
					Yaw = 0.0f;
				else
					Yaw = -39.2076f;
				std::cout << "Scene : " << currScene << "\n";
				Pos = glm::vec3(0, currScene == 1 ? 0 : 0.5f, currScene == 1 ? 15 : -1);
				RebuildPipeline();
			}
		}
		else {
			if ((curDebounce == GLFW_KEY_SPACE) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}


		// Standard procedure to quit when the ESC key is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		// Integration with the timers and the controllers
		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);
		// getSixAxis() is defined in Starter.hpp in the base class.
		// It fills the float point variable passed in its first parameter with the time
		// since the last call to the procedure.
		// It fills vec3 in the second parameters, with three values in the -1,1 range corresponding
		// to motion (with left stick of the gamepad, or ASWD + RF keys on the keyboard)
		// It fills vec3 in the third parameters, with three values in the -1,1 range corresponding
		// to motion (with right stick of the gamepad, or Arrow keys + QE keys on the keyboard, or mouse)
		// If fills the last boolean variable with true if fire has been pressed:
		//          SPACE on the keyboard, A or B button on the Gamepad, Right mouse button

		// To debounce the pressing of the fire button, and start the event when the key is released

		fire = false;   //disattivo un attimo la slot machine COMMENT 4
		static bool wasFire = false;
		bool handleFire = (wasFire && (!fire));
		wasFire = fire;

		const float FOVy = glm::radians(45.0f);
		const float nearPlane = 0.1f;
		const float farPlane = 100.0f;
		const float rotSpeed = glm::radians(90.0f);
		const float movSpeed = 6.0f;
		const float camDist = 1.5;
		const float minPitch = glm::radians(-60.0f);
		const float maxPitch = glm::radians(60.0f);

		glm::vec3 ux = glm::rotate(glm::mat4(1.0f), Yaw, glm::vec3(0, 1, 0)) * glm::vec4(1, 0, 0, 1);
		glm::vec3 uz = glm::rotate(glm::mat4(1.0f), Yaw, glm::vec3(0, 1, 0)) * glm::vec4(0, 0, -1, 1);


		if (currScene > 0) {
			Pos = Pos + movSpeed * m.x * ux * deltaT;
			Pos = Pos + movSpeed * m.y * glm::vec3(0, 1, 0) * deltaT;
			Pos.y = Pos.y < 0.0f ? 0.0f : Pos.y;
			Pos = Pos + movSpeed * m.z * uz * deltaT;
			// Rotation
			Yaw = Yaw - rotSpeed * deltaT * r.y;
			Pitch = Pitch + rotSpeed * deltaT * r.x;
			Pitch = Pitch < minPitch ? minPitch :
				(Pitch > maxPitch ? maxPitch : Pitch);
			Roll = Roll - rotSpeed * deltaT * r.z;
			Roll = Roll < glm::radians(-175.0f) ? glm::radians(-175.0f) :
				(Roll > glm::radians(175.0f) ? glm::radians(175.0f) : Roll);
		}



		// Final world matrix computaiton
		glm::mat4 World = glm::translate(glm::mat4(1), Pos) * glm::rotate(glm::mat4(1.0f), Yaw, glm::vec3(0, 1, 0));

		// Projection
		glm::mat4 Prj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
		Prj[1][1] *= -1;

		// View
		// Target
		glm::vec3 target = Pos + glm::vec3(0.0f, CamH, 0.0f);

		// Camera position, depending on Yaw parameter, but not character direction
		cameraPos = World * glm::vec4(0.0f, CamH + camDist * sin(Pitch), camDist * cos(Pitch), 1.0);
		// Damping of camera
		glm::mat4 View = glm::rotate(glm::mat4(1.0f), -Roll, glm::vec3(0, 0, 1)) *
			glm::lookAt(cameraPos, target, glm::vec3(0, 1, 0));

		if (third_person % 2 == 1 && currScene == 1) {
			alpha += rotSpeed * 0.1f * r[0];
			beta += rotSpeed * 0.1f * r[1];
			rho += rotSpeed * 0.1f * r[2];
			Pos += ux * (movSpeed * 0.1f) * m[0];
			Pos += glm::vec3(0, 1, 0) * (movSpeed * 0.1f) * m[1];
			Pos += uz * (movSpeed * 0.1f) * m[2];
			cx = Pos[0] - 0.0;
			cy = Pos[1] - (-1);
			cz = Pos[2] - 4 * glm::cos(10);


			// To be done in the assignment
			View = glm::lookAt(glm::vec3(cx, cy, cz), Pos, glm::vec3(0, 1, 0));

			Worldcar_rotation = glm::rotate(glm::mat4(1.0), alpha, glm::vec3(0, 1, 0)) *
				glm::rotate(glm::mat4(1.0), beta, glm::vec3(1, 0, 0)) *
				glm::rotate(glm::mat4(1.0), rho, glm::vec3(0, 0, 1));

			Worldcar_position = glm::translate(glm::mat4(1.0), Pos);
		}

		switch (currScene) {
		case 0: {
			Pos = glm::vec3(0, 0.5f, -1.5);
			Yaw = glm::degrees(90.0f);
			Pitch = 0;
			Roll = 0;
			gubo1.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
			gubo1.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			gubo1.AmbLightColor = glm::vec3(0.1f);
			gubo1.eyePos = cameraPos;

			gubo1.lightPos = glm::vec3(0, 0, 0);
			gubo1.lightDir = glm::normalize(glm::vec3(1, 1, 1));
			gubo1.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

			// Writes value to the GPU
			DSGubo1.map(currentImage, &gubo1, sizeof(gubo1), 0);
			// the .map() method of a DataSet object, requires the current image of the swap chain as first parameter
			// the second parameter is the pointer to the C++ data structure to transfer to the GPU
			// the third parameter is its size
			// the fourth parameter is the location inside the descriptor set of this uniform block


			World = glm::mat4(1);
			uboKitchen.amb = 1.0f; uboKitchen.gamma = 180.0f; uboKitchen.sColor = glm::vec3(1.0f);
			uboKitchen.mvpMat = Prj * View * World;
			uboKitchen.mMat = World;
			uboKitchen.nMat = glm::inverse(glm::transpose(World));
			DSkitchen.map(currentImage, &uboKitchen, sizeof(uboKitchen), 0);

			World = glm::translate(glm::mat4(1), pie2Pos) * glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1), glm::radians(180.0f), glm::vec3(0, 1, 0)) *
				glm::scale(glm::mat4(1), glm::vec3(0.02f, 0.02f, 0.02f));
			ubopie2.amb = 1.0f; ubopie2.gamma = 180.0f; ubopie2.sColor = glm::vec3(1.0f);
			ubopie2.mvpMat = Prj * View * World;
			ubopie2.mMat = World;
			ubopie2.nMat = glm::inverse(glm::transpose(World));
			DSpie2.map(currentImage, &ubopie2, sizeof(ubopie2), 0);

			World = glm::translate(glm::mat4(1), glm::vec3(2, 1, -1)) * glm::rotate(glm::mat4(1), glm::radians(180.0f), glm::vec3(0, 1, 0)) *
				glm::scale(glm::mat4(1), glm::vec3(1.5f, 1.5f, 1.5f));
			ubofarmer.amb = 1.0f; ubofarmer.gamma = 180.0f; ubofarmer.sColor = glm::vec3(1.0f);
			ubofarmer.mvpMat = Prj * View * World;
			ubofarmer.mMat = World;
			ubofarmer.nMat = glm::inverse(glm::transpose(World));
			DSfarmer.map(currentImage, &ubofarmer, sizeof(ubofarmer), 0);

			uboKey.visible = 1;
			DSKey.map(currentImage, &uboKey, sizeof(uboKey), 0);

			if (win == 1) {
				uboWin2.visible = 1;
				DSwin2.map(currentImage, &uboWin2, sizeof(uboWin2), 0);
				if (glfwGetKey(window, GLFW_KEY_P)) {
					win = 0;
					apple1 = 0;
					apple2 = 0;
					apple3 = 0;
					if (!debounce) {
						debounce = true;
						curDebounce = GLFW_KEY_P;
						currScene = 4;
						std::cout << "Scene : " << currScene << "\n";
						Pos = glm::vec3(-1, 0.5, 9);
						Yaw = 5152;
						RebuildPipeline();
					}
				}
				else {
					if ((curDebounce == GLFW_KEY_P) && debounce) {
						debounce = false;
						curDebounce = 0;
					}
				}
			}
		}break;
		case 1:
		{
			gubo2.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
			gubo2.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			gubo2.eyePos = cameraPos;

			// Writes value to the GPU
			DSGubo2.map(currentImage, &gubo2, sizeof(gubo2), 0);
			// the .map() method of a DataSet object, requires the current image of the swap chain as first parameter
			// the second parameter is the pointer to the C++ data structure to transfer to the GPU
			// the third parameter is its size
			// the fourth parameter is the location inside the descriptor set of this uniform block


			World = glm::translate(glm::mat4(1), glm::vec3(-5, 0, -2));
			uboBody.amb = 1.0f; uboBody.gamma = 180.0f; uboBody.sColor = glm::vec3(1.0f);
			uboBody.mvpMat = Prj * View * World;
			uboBody.mMat = World;
			uboBody.nMat = glm::inverse(glm::transpose(World));
			DSBody.map(currentImage, &uboBody, sizeof(uboBody), 0);


			World = glm::mat4(1);
			uboCity.amb = 1.0f; uboCity.gamma = 180.0f; uboCity.sColor = glm::vec3(1.0f);
			uboCity.mvpMat = Prj * View * World;
			uboCity.mMat = World;
			uboCity.nMat = glm::inverse(glm::transpose(World));
			DScity.map(currentImage, &uboCity, sizeof(uboCity), 0);

			World = Worldcar_position * glm::scale(glm::mat4(1.0f), glm::vec3(0.08f, 0.08f, 0.08f)) *
				glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0)) * Worldcar_rotation;
			ubocar.amb = 1.0f; ubocar.gamma = 180.0f; ubocar.sColor = glm::vec3(1.0f);
			ubocar.mvpMat = Prj * View * World;
			ubocar.mMat = World;
			ubocar.nMat = glm::inverse(glm::transpose(World));
			DScar.map(currentImage, &ubocar, sizeof(ubocar), 0);

			World = glm::translate(glm::mat4(1.0f), apple1Pos) * glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1)) *
				glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
			uboapple1.amb = 1.0f; uboapple1.gamma = 180.0f; uboapple1.sColor = glm::vec3(1.0f);
			uboapple1.mvpMat = Prj * View * World;
			uboapple1.mMat = World;
			uboapple1.nMat = glm::inverse(glm::transpose(World));
			DSapple1.map(currentImage, &uboapple1, sizeof(uboapple1), 0);

			uboSplash.visible = apple1;
			DSSplash.map(currentImage, &uboSplash, sizeof(uboSplash), 0);

			uboSplash2.visible = apple2;
			DSSplash2.map(currentImage, &uboSplash2, sizeof(uboSplash2), 0);

			uboSplash3.visible = apple3;
			DSSplash3.map(currentImage, &uboSplash3, sizeof(uboSplash3), 0);

			uboWin.visible = win;
			DSwin.map(currentImage, &uboWin, sizeof(uboWin), 0);

			if (glfwGetKey(window, GLFW_KEY_K)) {
				if (!debounce) {
					debounce = true;
					curDebounce = GLFW_KEY_K;
					third_person = (third_person + 1);
					Roll = 0; 
					Yaw = 0; 
					Pitch = 0; 
				}
			}
			else {
				if ((curDebounce == GLFW_KEY_K) && debounce) {
					debounce = false;
					curDebounce = 0;
				}
			}

		}break;
		case 2: {

			gubo3.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
			gubo3.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			gubo3.AmbLightColor = glm::vec3(0.1f);
			gubo3.eyePos = cameraPos;

			gubo3.lightPos = glm::vec3(0, 0, 2);
			gubo3.lightDir = glm::normalize(glm::vec3(1, 1, 1));
			gubo3.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

			// Writes value to the GPU
			DSGubo3.map(currentImage, &gubo3, sizeof(gubo3), 0);
			World = glm::mat4(1);
			ubodungeon.amb = 1.0f; ubodungeon.gamma = 180.0f; ubodungeon.sColor = glm::vec3(1.0f);
			ubodungeon.mvpMat = Prj * View * World;
			ubodungeon.mMat = World;
			ubodungeon.nMat = glm::inverse(glm::transpose(World));
			DSdungeon.map(currentImage, &ubodungeon, sizeof(ubodungeon), 0);

			World = glm::translate(glm::mat4(1), apple2Pos);
			uboapple2.amb = 1.0f; uboapple2.gamma = 180.0f; uboapple2.sColor = glm::vec3(1.0f);
			uboapple2.mvpMat = Prj * View * World;
			uboapple2.mMat = World;
			uboapple2.nMat = glm::inverse(glm::transpose(World));
			DSapple2.map(currentImage, &uboapple2, sizeof(uboapple2), 0);

			uboSplash.visible = apple1;
			DSSplash.map(currentImage, &uboSplash, sizeof(uboSplash), 0);

			uboSplash2.visible = apple2;
			DSSplash2.map(currentImage, &uboSplash2, sizeof(uboSplash2), 0);

			uboSplash3.visible = apple3;
			DSSplash3.map(currentImage, &uboSplash3, sizeof(uboSplash3), 0);

			uboWin.visible = win;
			DSwin.map(currentImage, &uboWin, sizeof(uboWin), 0);
		} break;
		case 3: {

			gubo4.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
			gubo4.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			gubo4.AmbLightColor = glm::vec3(0.1f);

			float dang = Pitch + glm::radians(15.0f);
			gubo4.lightPos = Pos + glm::vec3(0, 1, 0);
			gubo4.lightDir = glm::vec3(cos(dang) * sin(Yaw), sin(dang), cos(dang) * cos(Yaw));
			gubo4.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			gubo4.eyePos = cameraPos;

			// Writes value to the GPU
			DSGubo4.map(currentImage, &gubo4, sizeof(gubo4), 0);
			World = glm::mat4(1);
			ubocourt.amb = 1.0f; ubocourt.gamma = 180.0f; ubocourt.sColor = glm::vec3(1.0f);
			ubocourt.mvpMat = Prj * View * World;
			ubocourt.mMat = World;
			ubocourt.nMat = glm::inverse(glm::transpose(World));
			DScourt.map(currentImage, &ubocourt, sizeof(ubocourt), 0);

			World = glm::translate(glm::mat4(1), apple3Pos);
			uboapple3.amb = 1.0f; uboapple3.gamma = 180.0f; uboapple3.sColor = glm::vec3(1.0f);
			uboapple3.mvpMat = Prj * View * World;
			uboapple3.mMat = World;
			uboapple3.nMat = glm::inverse(glm::transpose(World));
			DSapple3.map(currentImage, &uboapple3, sizeof(uboapple3), 0);

			uboSplash.visible = apple1;
			DSSplash.map(currentImage, &uboSplash, sizeof(uboSplash), 0);

			uboSplash2.visible = apple2;
			DSSplash2.map(currentImage, &uboSplash2, sizeof(uboSplash2), 0);

			uboSplash3.visible = apple3;
			DSSplash3.map(currentImage, &uboSplash3, sizeof(uboSplash3), 0);

			uboWin.visible = win;
			DSwin.map(currentImage, &uboWin, sizeof(uboWin), 0);
		} break;
		case 4: {

			gubo5.eyePos = cameraPos;

			gubo5.lightPos1 = glm::vec3(1, 5, -2);
			gubo5.lightDir1 = glm::normalize(glm::vec3(1, 1, 1));
			gubo5.lightColor1 = glm::vec4(5.0f, 8.0f, 6.0f, 1.0f);

			gubo5.lightPos2 = glm::vec3(4, 6, -1);
			gubo5.lightDir2 = glm::normalize(glm::vec3(1, 1, 1));
			gubo5.lightColor2 = glm::vec4(3.0f, 4.0f, 2.0f, 1.0f);

			gubo5.lightPos3 = glm::vec3(5, 4, 1);
			gubo5.lightDir3 = glm::normalize(glm::vec3(1, 1, 1));
			gubo5.lightColor3 = glm::vec4(9.0f, 2.0f, 4.0f, 1.0f);

			DSGubo5.map(currentImage, &gubo5, sizeof(gubo5), 0);
			// the .map() method of a DataSet object, requires the current image of the swap chain as first parameter
			// the second parameter is the pointer to the C++ data structure to transfer to the GPU
			// the third parameter is its size
			// the fourth parameter is the location inside the descriptor set of this uniform block

			World = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5)) * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(1, 0, 0)) *
				glm::scale(glm::mat4(1), glm::vec3(0.5f, 0.5f, 0.5f));
			ubopie.amb = 1.0f; ubopie.gamma = 180.0f; ubopie.sColor = glm::vec3(1.0f);
			ubopie.mvpMat = Prj * View * World;
			ubopie.mMat = World;
			ubopie.nMat = glm::inverse(glm::transpose(World));
			DSpie.map(currentImage, &ubopie, sizeof(ubopie), 0);
		}

		}

		if (Pos[0] <= -8 + 1 && Pos[0] >= -8 - 1 && Pos[2] >= 12 - 1 && Pos[2] <= 12 + 1 && currScene == 1) {
			apple2 = 1;
			apple1Pos = glm::vec3(0, -2, 0);
		}

		if (Pos[0] <= 6 + 1 && Pos[0] >= 6 - 1 && Pos[2] >= 7 - 1 && Pos[2] <= 7 + 1 && currScene == 2) {
			apple1 = 1;
			apple2Pos = glm::vec3(0, -2, 0);
		}
		if (Pos[0] <= 0.7 + 1 && Pos[0] >= 0.7 - 1 && Pos[2] >= 6.7 - 1 && Pos[2] <= 6.7 + 1 && currScene == 3) {
			apple3 = 1;
			apple3Pos = glm::vec3(0, -2, 0);
		}

		if (apple1 == 1 && apple2 == 1 && apple3 == 1) {
			win = 1;
			apple1 = 0;
			apple2 = 0;
			apple3 = 0;
		}
		if (win == 1) {
			pie2Pos = glm::vec3(1.6, 1, -1.2);
		}


	}
};

// This is the main: probably you do not need to touch this!
int main() {
	Game app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


















