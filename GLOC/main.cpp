#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

#include <cmath>

// GLFW
#include <GLFW/glfw3.h>


/**
 1、艺术家和程序员更喜欢使用纹理(Texture)。纹理是一个2D图片（甚至也有1D和3D的纹理），它可以用来添加物体的细节；你可以想象纹理是一张绘有砖块的纸，无缝折叠贴合到你的3D的房子上，这样你的房子看起来就像有砖墙外表了。因为我们可以在一张图片上插入非常多的细节，这样就可以让物体非常精细而不用指定额外的顶点 
 
 
 **/



//function peototype
//按键回调。在用户有键盘交互时回调

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);


//着色器  Shaders

// Shaders
// Shaders
const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position, 1.0);\n"
"ourColor = color;\n"
"}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
"in vec3 ourColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(ourColor, 1.0f);\n"
"}\n\0";




int main() {
    
    
    glfwInit();//初始化一个窗口
    
    //设置窗口的一些值
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //使用核心模式
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); //使用旧函数会报错
    
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //mac需要加这个
    
    
    
    GLFWwindow *window  = glfwCreateWindow(800, 600, "first opengl", nullptr, nullptr);//创建窗口
    
    if (window == nullptr ) {
        
        std::cout << "Failed to create Window" <<std::endl;
        glfwTerminate();
        
        return -1;
        
    }
    
    glfwMakeContextCurrent(window); //设置当前窗口上下文设置为当前线程的主上下文
    
    
    //使用OpenGL的函数之前需要初始化GLEW。GLEW用于管理OpenGL的函数指针
    
    glewExperimental = GL_TRUE; //true的话使用可以更多的使用新的技术。false的话会在使用核心模式时出问题
    
    //在glew使用之前需要进行初始化，否则会出现BAD_ACCESS错误。
    //https://stackoverflow.com/questions/12329082/glcreateshader-is-crashing
    if(glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed");
    
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    
    glViewport(0, 0, width, height);
    
    
    //编译着色器
    ///Vertex shader 顶点着色器
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    
    ///把着色器源码附加到着色器对象上，然后编译
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    
    
    ///检查编译有没有成功，并查看错误的原因
    
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    
    /// Fragment shader 片段着色器
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    
    ///note 着色器程序对象(Shader Program Object)是多个着色器合并之后并最终链接完成的版本。如果要使用刚才编译的着色器我们必须把它们链接为一个着色器程序对象，然后在渲染对象的时候激活这个着色器程序。已激活着色器程序的着色器将在我们发送渲染调用的时候被使用。
    
    //link shader
    GLuint shaderProgram = glCreateProgram();
    
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    
    glLinkProgram(shaderProgram);
    
    //检查连接错误
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    
    if (!success) {
        
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        
    }
    
    //着色器对象链接到程序对象以后，记得删除着色器对象，不再需要
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    
    //顶点输入
    GLfloat vertices[] = {
        // 位置              // 颜色
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
    };
    
    GLuint VBO;
    glGenBuffers(1, &VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //把之前定义的顶点数据复制到缓冲的内存中
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    
    //顶点数组对象
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    //链接顶点属性
    /*
     
     第一个参数指定我们要配置的顶点属性。还记得我们在顶点着色器中使用layout(location = 0)定义了position顶点属性的位置值(Location)吗？它可以把顶点属性的位置值设置为0。因为我们希望把数据传递到这一个顶点属性中，所以这里我们传入0。
     第二个参数指定顶点属性的大小。顶点属性是一个vec3，它由3个值组成，所以大小是3。
     第三个参数指定数据的类型，这里是GL_FLOAT(GLSL中vec*都是由浮点数值组成的)。
     下个参数定义我们是否希望数据被标准化(Normalize)。如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。我们把它设置为GL_FALSE。
     第五个参数叫做步长(Stride)，它告诉我们在连续的顶点属性组之间的间隔。由于下个组位置数据在3个GLfloat之后，我们把步长设置为3 * sizeof(GLfloat)。要注意的是由于我们知道这个数组是紧密排列的（在两个顶点属性之间没有空隙）我们也可以设置为0来让OpenGL决定具体步长是多少（只有当数值是紧密排列时才可用）。一旦我们有更多的顶点属性，我们就必须更小心地定义每个顶点属性之间的间隔，我们在后面会看到更多的例子(译注: 这个参数的意思简单说就是从这个属性第二次出现的地方到整个数组0位置之间有多少字节)。
     最后一个参数的类型是GLvoid*，所以需要我们进行这个奇怪的强制类型转换。它表示位置数据在缓冲中起始位置的偏移量(Offset)。由于位置数据在数组的开头，所以这里是0。
     
     */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    
    //设置顶点属性指针
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    
    //解绑VAO
    glBindVertexArray(0);
    
    
    
    
    
    
    
    
    
    
    
    
    
    //函数注册需要放在循环开始之前
    glfwSetKeyCallback(window, key_callback);
    
    
    //Game LOOP 一直循环，直到用户明确退出
    
    while (!glfwWindowShouldClose(window)) {//每次检查是否要退出
        
        glfwPollEvents(); //检查有没有触发什么事件
        
        
        //渲染(Rendering)操作放到游戏循环中，因为我们想让这些渲染指令在每次游戏循环迭代的时候都能被执行。
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        
        
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        
        
        
        
        
        
        
        glfwSwapBuffers(window);
    }
    
    
    glfwTerminate(); //释放glfw分配的内存
    
    
    return 0;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    
    //当用户按下ESC键，设置windowshouldclose为true
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        
        glfwSetWindowShouldClose(window, GL_TRUE);
        
    }
    
    
}
