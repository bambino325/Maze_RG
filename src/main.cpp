#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <iostream>
#include <fstream>
#include <model.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const * path);
unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));


float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int matrix[20][20];
int hint = 0;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    std::fstream file;
    file.open("src/map.txt");
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 19; j++) {
            file >> matrix[i][j];
        }
    }
    file.close();

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Maze", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);

    Shader Shader1("resources/shaders/wall.vs", "resources/shaders/wall.fs");
    Shader Shader2("resources/shaders/floor.vs", "resources/shaders/floor.fs");
    Shader skyboxShader("resources/shaders/Skybox.vs", "resources/shaders/Skybox.fs");
    Shader ShaderTransp("resources/shaders/transparent.vs", "resources/shaders/transparent.fs");


    // build and compile our shader program
    // ------------------------------------

    float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };

    float transparentVertices[] = {
            // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
            1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };

    glm::vec3 pointLightPositions[5];
    for(int i = 0; i < 5; i++)
        pointLightPositions[i] = glm::vec3(0.58f+4*i, 1.0f,1.0f+4*i);


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);


    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);


    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    vector<std::string> faces
            {
                    FileSystem::getPath("resources/textures/skybox1/right1.jpg"),
                    FileSystem::getPath("resources/textures/skybox1/left1.jpg"),
                    FileSystem::getPath("resources/textures/skybox1/top1.jpg"),
                    FileSystem::getPath("resources/textures/skybox1/bottom1.jpg"),
                    FileSystem::getPath("resources/textures/skybox1/front1.jpg"),
                    FileSystem::getPath("resources/textures/skybox1/back1.jpg")
            };
    unsigned int cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // wall
    Shader1.use();
    unsigned int diffuseMapWall = loadTexture(FileSystem::getPath("resources/textures/brickwall.jpg").c_str());
    unsigned int specularMapWall = loadTexture(FileSystem::getPath("resources/textures/brickwall_normal.jpg").c_str());

    Shader1.setInt("material.diffuse", 1);
    Shader1.setInt("material.specular", 2);

    ShaderTransp.use();
    unsigned int transparentTexture = loadTexture(FileSystem::getPath("resources/textures/powerup_speed.png").c_str());
    ShaderTransp.setInt("texture1", 4);


    // floor
    Shader2.use();
    unsigned int diffuseMapFloor = loadTexture(FileSystem::getPath("resources/textures/stone.jpg").c_str());
    Shader2.setInt("material.diffuse", 3);


    Shader ShaderModel("resources/shaders/model.vs", "resources/shaders/model.fs");
    ShaderModel.use();

    Model  lantern(FileSystem::getPath("resources/objects/lantern/Gamelantern_updated.obj"));

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glEnable(GL_DEPTH_TEST);

        glBindVertexArray(VAO);

        Shader1.use();

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view;
        glm::mat4 projection;
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        Shader1.setMat4("view", view);
        Shader1.setMat4("projection", projection);

        Shader1.setVec3("viewPos", camera.Position);

        for(int i = 0; i < 5; i++) {
            Shader1.setVec3("pointLights[" + to_string(i) + "].position", pointLightPositions[i]);
            Shader1.setVec3("pointLights[" + to_string(i) + "].ambient", 0.005f, 0.005f, 0.005f);
            Shader1.setVec3("pointLights[" + to_string(i) + "].diffuse", 0.45f, 0.45f, 0.0f);
            Shader1.setVec3("pointLights[" + to_string(i) + "].specular", 0.3f, 0.3f, 0.0f);
            Shader1.setFloat("pointLights[" + to_string(i) + "].constant", 1.0f);
            Shader1.setFloat("pointLights[" + to_string(i) + "].linear", 0.22);
            Shader1.setFloat("pointLights[" + to_string(i) + "].quadratic", 0.20);
        }

        Shader1.setVec3("spotLight.position", camera.Position);
        Shader1.setVec3("spotLight.direction", camera.Front);
        Shader1.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        Shader1.setVec3("spotLight.diffuse", 0.4f, 0.4f, 0.4f);
        Shader1.setVec3("spotLight.specular", 0.04f, 0.04f, 0.04f);
        Shader1.setFloat("spotLight.constant", 1.0f);
        Shader1.setFloat("spotLight.linear", 0.045);
        Shader1.setFloat("spotLight.quadratic", 0.0075);
        Shader1.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        Shader1.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        Shader1.setFloat("material.shininess", 4.0f);

        // walls
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, diffuseMapWall);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, specularMapWall);

        for(int i = 0; i < 19; i++) {
            for(int j = 0; j < 19; j++) {
                if(matrix[i][j] == 1) {
                    model = glm::translate(glm::mat4(1.0f), glm::vec3((float)j, 0.0f, (float)i));
                    Shader1.setMat4("model", model);
                    Shader1.use();

                    glDrawArrays(GL_TRIANGLES, 0, 36);
                    model = glm::translate(glm::mat4(1.0f), glm::vec3((float)j, 1.0f, (float)i));
                    Shader1.setMat4("model", model);
                    //Shader1.use();
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }

        // floor
        Shader2.use();
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, diffuseMapFloor);

        Shader2.setMat4("view", view);
        Shader2.setMat4("projection", projection);
        Shader2.setVec3("viewPos", camera.Position);

        for(int i = 0; i < 5; i++) {
            Shader2.setVec3("pointLights[" + to_string(i) + "].position", pointLightPositions[i]);
            Shader2.setVec3("pointLights[" + to_string(i) + "].ambient", 0.005f, 0.005f, 0.0f);
            Shader2.setVec3("pointLights[" + to_string(i) + "].diffuse", 0.4f, 0.4f, 0.0f);
            Shader2.setVec3("pointLights[" + to_string(i) + "].specular", 0.05f, 0.05f, 0.0f);
            Shader2.setFloat("pointLights[" + to_string(i) + "].constant", 1.0f);
            Shader2.setFloat("pointLights[" + to_string(i) + "].linear", 0.22f);
            Shader2.setFloat("pointLights[" + to_string(i) + "].quadratic", 0.20f);
        }

        Shader2.setVec3("spotLight.position", camera.Position);
        Shader2.setVec3("spotLight.direction", camera.Front);
        Shader2.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        Shader2.setVec3("spotLight.diffuse", 0.6f, 0.6f, 0.6f);
        Shader2.setVec3("spotLight.specular", 0.04f, 0.04f, 0.04f);
        Shader2.setFloat("spotLight.constant", 1.0f);
        Shader2.setFloat("spotLight.linear", 0.14);
        Shader2.setFloat("spotLight.quadratic", 0.07);
        Shader2.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        Shader2.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        Shader2.setFloat("material.shininess", 10.0f);

        for(int i = 0; i < 19; i++) {
            for(int j = 0; j < 19; j++) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3((float)j, -1.0f, (float)i));
                Shader2.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);

            }
        }


        glActiveTexture(GL_TEXTURE0);

        // model
        ShaderModel.use();
        ShaderModel.setMat4("view", view);
        ShaderModel.setMat4("projection", projection);

        for(int i=0; i < 5; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model,glm::vec3(0.7f, 0.7f, 0.7f));
            model = glm::rotate(model, 1.57f ,glm::vec3(0.0f, 0.5f, 0.0f));
            ShaderModel.setMat4("model", model);
            lantern.Draw(ShaderModel);
        }


        if(hint == 1) {
            glActiveTexture(GL_TEXTURE4);
            ShaderTransp.use();
            ShaderTransp.setMat4("view", view);
            ShaderTransp.setMat4("projection", projection);
            glBindVertexArray(transparentVAO);
            glBindTexture(GL_TEXTURE_2D, transparentTexture);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(10.5f, 0.0f, 9.5f));
            ShaderTransp.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(5.5f, 0.0f, 2.5f));
            model = glm::rotate(model, 1.57f, glm::vec3(0.0f, 1.0f, 0.0f));
            ShaderTransp.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(7.5f, 0.0f, 4.5f));
            ShaderTransp.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(12.5f, 0.0f, 15.5f));
            ShaderTransp.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }


        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteBuffers(1, &skyboxVAO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        camera.lock = !camera.lock;
    if(glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        hint = !hint;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
