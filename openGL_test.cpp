// openGL_test.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "openGL_test.h"

#define MAX_LOADSTRING 100

// Global Variables:
GLFWwindow* window{};

//using namespace std;
namespace stdc = std::chrono;
namespace stdfs = std::filesystem;

GLuint LoadShaders(stdfs::path const& pathVertexFile, stdfs::path const& pathFragmentFile);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     [[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance,
                     [[maybe_unused]] _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    if (!glfwInit()) {
        OutputDebugStringA("FAILED to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1024, 768, "openGL", nullptr, nullptr);
    if (!window) {
        OutputDebugStringA("Failed to open GLFW window. If you have only an Intel GPU, they are not 3.3 compatible.\n");
        glfwTerminate();
        return -1;
    }
    if (auto hWnd = glfwGetWin32Window(window)) {
        auto style = GetWindowLong(hWnd, GWL_STYLE);
        style |= (CS_HREDRAW|CS_VREDRAW);
        SetWindowLong(hWnd, GWL_STYLE, style);
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        OutputDebugStringA("Failed to Initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    std::vector<float> data{
        -1.0f, -1.f, 0.,
        1.0f, -1.f, 0.,
        0.0f,  1.f, 0.,
    };
    //cv::Mat mat = cv::Mat::zeros(3, 1, CV_32FC3);
    //mat.at<cv::Vec3f>(0, 0) = { -1.0f, -1.f, 0., };
    //mat.at<cv::Vec3f>(1, 0) = {  1.0f, -1.f, 0., };
    //mat.at<cv::Vec3f>(2, 0) = {  0.0f,  1.f, 0., };

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    //glBufferData(GL_ARRAY_BUFFER, mat.rows()*mat.cols()*mat.channel()*mat.elemSize1(), mat.ptr(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(data[0]), data.data(), GL_STATIC_DRAW);

    GLuint idProgram = LoadShaders(L"simpleVertexShader.vertexshader", L"simpleFragmentShader.fragmentshader");

    auto t0 = stdc::steady_clock::now();
    do {
        auto t1 = stdc::steady_clock::now();
        auto ts = t1-t0;
        t0 = t1;
        OutputDebugStringA(std::format("{}\n", stdc::duration_cast<stdc::milliseconds>(ts)).c_str());

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glUseProgram(idProgram);

        // Draw...
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
        //glfwPollEvents();
        glfwWaitEventsTimeout(0.060);

        std::this_thread::sleep_until(t1+60ms);
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS and glfwWindowShouldClose(window) == 0);

    glfwTerminate();

    return 0;
}

GLuint LoadShaders(stdfs::path const& pathVertexFile, stdfs::path const& pathFragmentFile) {
    GLuint idVertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint idFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string strVertexShaderCode;
    if (std::ifstream streamVertexShader(pathVertexFile, std::ios::binary); (bool)streamVertexShader) {
        std::stringstream sstr;
        sstr << streamVertexShader.rdbuf();
        strVertexShaderCode = sstr.str();
    }
    else {
        OutputDebugString(std::format(L"파일 {}를 읽을 수 없습니다.\n", pathVertexFile.wstring()).c_str());
        return 0;
    }

    std::string strFragmentShaderCode;
    if (std::ifstream streamFragmentShader(pathFragmentFile, std::ios::binary); (bool)streamFragmentShader) {
        std::stringstream sstr;
        sstr << streamFragmentShader.rdbuf();
        strFragmentShaderCode = sstr.str();
    }

    auto InitShader = [](stdfs::path const& path, int idShader, std::string const& strShaderCode) {
        GLint result{}, nInfoLog{};

        // vertex shader
        OutputDebugString(std::format(L"Compiling shader : {}\n", path.wstring()).c_str());
        const char* pszSource = strShaderCode.c_str();
        glShaderSource(idShader, 1, &pszSource, nullptr);
        glCompileShader(idShader);

        glGetShaderiv(idShader, GL_COMPILE_STATUS, &result);
        glGetShaderiv(idShader, GL_INFO_LOG_LENGTH, &nInfoLog);
        if (nInfoLog > 0) {
            std::string strErrorMessage(nInfoLog+1, '\n');
            glGetShaderInfoLog(idShader, nInfoLog, nullptr, strErrorMessage.data());
            OutputDebugStringA(strErrorMessage.c_str());
        }
    };
    InitShader(pathVertexFile, idVertexShader, strVertexShaderCode);
    InitShader(pathFragmentFile, idFragmentShader, strFragmentShaderCode);

    // 프로그램에 링크
    OutputDebugStringA("Linking program\n");
    GLuint idProgram = glCreateProgram();
    glAttachShader(idProgram, idVertexShader);
    glAttachShader(idProgram, idFragmentShader);
    glLinkProgram(idProgram);

    GLint result{}, nInfoLog{};
    glGetProgramiv(idProgram, GL_LINK_STATUS, &result);
    glGetProgramiv(idProgram, GL_INFO_LOG_LENGTH, &nInfoLog);
    if (nInfoLog > 0) {
        std::string strLogMessage(nInfoLog+1, '\n');
        glGetProgramInfoLog(idProgram, nInfoLog, nullptr, strLogMessage.data());
        OutputDebugStringA(strLogMessage.c_str());
    }

    glDetachShader(idProgram, idVertexShader);
    glDetachShader(idProgram, idFragmentShader);

    glDeleteShader(idVertexShader);
    glDeleteShader(idFragmentShader);

    return idProgram;
}
