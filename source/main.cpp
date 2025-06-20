#include <iostream>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// STL
#include <filesystem>
#include <iostream>
#include <fstream>
#include <memory>

// Custom
#include <OpenGLCamera.hpp>
#include <FrenchieShader.hpp>
#include <FrenchieLogger.hpp>

// STB
#include <stb_image.h>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboard(COUNTERCLOCKWISE, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboard(CLOCKWISE, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
static bool lbutton_down = false;
static bool rbutton_down = false;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        std::cout << "mouse_button_callback() called GLFW_MOUSE_BUTTON_LEFT GLFW_PRESS!!! \n";

        camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if(GLFW_PRESS == action)
            rbutton_down = true;
        else if(GLFW_RELEASE == action)
            rbutton_down = false;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if(GLFW_PRESS == action)
            lbutton_down = true;
        else if(GLFW_RELEASE == action)
            lbutton_down = false;
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

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
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void displayCone(void)
{
    // clear the drawing buffer.
    glClear(GL_COLOR_BUFFER_BIT);  // <---- add

    // set matrix mode
    glMatrixMode(GL_MODELVIEW);
    // clear model view matrix
    glLoadIdentity();
    // multiply view matrix to current matrix
    //gluLookAt(3.0, 3.0, 3.0-4.5, 0.0, 0.0,-4.5,0,1,0);

    // ******
    glPushMatrix();

    // glLoadIdentity(); <---- delete

    glTranslatef(0.0, 0.0, -4.5);

    glBegin(GL_LINES);

    glColor3f (1.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(2.0, 0.0, 0.0);

    glColor3f (1.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 2.0, 0.0);

    glColor3f (1.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 2.0);
    glEnd();

    glPopMatrix();

    // clear the drawing buffer.
    // glClear(GL_COLOR_BUFFER_BIT);  // <---- delete

    // traslate the draw by z = -4.0
    // Note this when you decrease z like -8.0 the drawing will looks far , or smaller.
    glTranslatef(0.0,0.0,-4.5);
    // Red color used to draw.
    glColor3f(0.8, 0.2, 0.1);
    // changing in transformation matrix.
    // rotation about X axis
    //glRotatef(xRotated,1.0,0.0,0.0);
    // rotation about Y axis
    //glRotatef(yRotated,0.0,1.0,0.0);
    // rotation about Z axis
    //glRotatef(zRotated,0.0,0.0,1.0);

    // scaling transfomation
    glScalef(1.0,1.0,1.0);
    // built-in (glut library) function , draw you a Cone.

    // move the peak of the cone to the origin
    //glTranslatef(0.0, 0.0, -height);

    //glutSolidCone(base,height,slices,stacks);
    // Flush buffers to screen
    // gluLookAt(3,3,3,0,0,-4.5,0,1,0); <----------------------- delete

    glFlush();
    // sawp buffers called because we are using double buffering
    // glutSwapBuffers();
}

int main(int, char**)
{
    // configure GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_Bulldog::OpenGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    //glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    //glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);

    // create Bulldog::OpenGL window using GLFW
    GLFWwindow* window = 
        glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bulldog::OpenGL", nullptr, nullptr);

    if(window == nullptr)
    {
        std::cout << "Could not create Bulldog::OpenGL window \n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // initialize GLAD that gives us an interface to Bulldog::OpenGL function pointers
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD \n";
        glfwTerminate();
        return -1;
    }

    // setup viewport top left and size according to the previously created Window
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    //---------------------------------------------------------
    // geometry to render
    //---------------------------------------------------------

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f
    };

    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    //-------------------------------------------------------------------------------------
    // Load and compile shaders and then create shader program and attach shaders to it
    //-------------------------------------------------------------------------------------
    Frenchie::OpenGL::ShaderProgram defaultShader(
        {
            Frenchie::Create<Frenchie::OpenGL::VertexShader>(L"C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.vert"),
            Frenchie::Create<Frenchie::OpenGL::FragmetShader>(L"C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.frag")
        }
    );

    Frenchie::OpenGL::ShaderProgram lightShader(
        {
            Frenchie::Create<Frenchie::OpenGL::VertexShader>(L"C:/SDK/Qt_Projects/OpenGL/shared/shaders/Light/Light.vert"),
            Frenchie::Create<Frenchie::OpenGL::FragmetShader>(L"C:/SDK/Qt_Projects/OpenGL/shared/shaders/Light/Light.frag")
        }
    );

    std::cout << defaultShader.get_status_message();
    std::cout << lightShader.get_status_message();

    //-------------------------------------------------------------------------------------
    // save geometry to graphics card memory 
    //-------------------------------------------------------------------------------------
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // vertex buffer object (VBO)
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //---------------------------------------------------------------------------------------------------
    // load image
    //---------------------------------------------------------------------------------------------------
    int width, height, nrChannels;
    unsigned char *data = stbi_load("C:/SDK/Qt_Projects/OpenGL/shared/images/container.jpg", &width, &height, &nrChannels, 0);

    unsigned int texture;
    glGenTextures(1, &texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    if (data != nullptr)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Succeeded to load texture" << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    //---------------------------------------------------------------------------------------------------

    glEnable(GL_DEPTH_TEST); 

    // create logger
    std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>("Main");
    
    //logger->sinks().push_back(spdlog::stdout_color_mt("console"));

    //spdlog::init_thread_pool(8192, 1);
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    //auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("mylog.txt", 1024*1024*10, 3);
    //std::vector<spdlog::sink_ptr> sinks {stdout_sink, rotating_sink};
    logger->sinks().push_back(stdout_sink);
    // auto logger = std::make_shared<spdlog::async_logger>("err_logger", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    spdlog::register_logger(logger); //<-- this line registers logger for spdlog::get

    
    Frenchie::Logger::instance().register_sink<spdlog::sinks::stdout_color_sink_mt>();
    Frenchie::Logger::instance().info("Welcome to spdlog!");
    Frenchie::Logger::instance().warn("This is a warning message.");
    Frenchie::Logger::instance().error("An error occurred.");

    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // process input
        processInput(window);

        //------------------------------------------------------------------------------------------
        // drawing is done here...
        //-----------------------------------------------------------------------------------------
        if(lbutton_down) {
            camera.ProcessKeyboard(FORWARD, deltaTime);
        }

        if(rbutton_down){
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        }

        // draw cubes
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)2048 / (float)1024, 0.1f, 100.0f);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        float angle = 2.f * glm::pi<float>() * 0.1 * (float)glfwGetTime();
        float radius = 5.f;
        auto light = glm::vec3(cos(angle), 0.f, sin(angle)) * radius;

        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

            glUseProgram(defaultShader.get_id());
            defaultShader.set_vec3("lightColor", glm::vec3(1.f, 1.f, 1.f));
            defaultShader.set_vec3("lightPos", light);
            defaultShader.set_vec3("viewPos", camera.Position);

            defaultShader.set_mat4("projection", projection);
            defaultShader.set_mat4("view", camera.GetViewMatrix());
            defaultShader.set_mat4("model", model);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // draw light source

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, light);
        model = glm::scale(model, glm::vec3(0.2f));

        glUseProgram(lightShader.get_id());
        lightShader.set_mat4("projection", projection);
        lightShader.set_mat4("view", camera.GetViewMatrix());
        lightShader.set_mat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //------------------------------------------------------------------------------------------

        // swap buffers
        glfwSwapBuffers(window);

        // check and call events
        glfwPollEvents();    
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // finish GLFW
    glfwTerminate();

    return 0;
}