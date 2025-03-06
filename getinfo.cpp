#include "getinfo.hpp"


void printOpenGLInfo() {
    // Ověření, zda existuje platný OpenGL kontext
    if (!glGetString(GL_VENDOR)) {
        throw std::runtime_error("OpenGL context was not created before calling printOpenGLInfo()");
    }

    // Získání základních informací o OpenGL
    const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    const char* shadingLanguageVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

    std::cout << "OpenGL Vendor: " << (vendor ? vendor : "<Unknown>") << '\n';
    std::cout << "OpenGL Renderer: " << (renderer ? renderer : "<Unknown>") << '\n';
    std::cout << "OpenGL Version: " << (version ? version : "<Unknown>") << '\n';
    std::cout << "GLSL Version: " << (shadingLanguageVersion ? shadingLanguageVersion : "<Unknown>") << '\n';

    GLint numExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

    std::cout << "Supported OpenGL Extensions:\n";
    for (GLint i = 0; i < numExtensions; i++) {
        std::cout << glGetStringi(GL_EXTENSIONS, i) << '\n';
    }

    // Ověření podpory GL_ARB_debug_output
    if (glewIsSupported("GL_ARB_debug_output")) {
        std::cout << "GL_ARB_debug_output is SUPPORTED!\n";
    }
    else {
        std::cout << "GL_ARB_debug_output is NOT supported on this system!\n";
    }


    // Kontrola OpenGL chyb
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error after retrieving basic info: " << err << '\n';
    }

    // Ověření minimální požadované verze OpenGL
    GLint major = 0, minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    std::cout << "OpenGL Major Version: " << major << '\n';
    std::cout << "OpenGL Minor Version: " << minor << '\n';

    if (major < 4 || (major == 4 && minor < 6)) {
        std::cerr << "Warning: OpenGL version is lower than 4.6!\n";
    }

    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error after retrieving version info: " << err << '\n';
    }

    // Ověření profilu OpenGL kontextu
    GLint profile = 0;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
    std::cout << "Raw GL_CONTEXT_PROFILE_MASK value: " << profile << '\n';  // Debug výpis

    std::cout << "OpenGL Profile: ";
    if (profile & GL_CONTEXT_CORE_PROFILE_BIT) {
        std::cout << "CORE\n";
    }
    else if (profile & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) {
        std::cout << "COMPATIBILITY\n";
    }
    else {
        //throw std::runtime_error("Unknown OpenGL profile! Raw value: " + std::to_string(profile));            //tady je chyba
    }

    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error after retrieving profile info: " << err << '\n';
    }

    // Ověření dalších kontextových vlastností
    GLint contextFlags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
    std::cout << "Context Flags: ";
    if (contextFlags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) std::cout << "FORWARD_COMPATIBLE ";
    if (contextFlags & GL_CONTEXT_FLAG_DEBUG_BIT) std::cout << "DEBUG ";
    if (contextFlags & GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT) std::cout << "ROBUST_ACCESS ";
    if (contextFlags & GL_CONTEXT_FLAG_NO_ERROR_BIT) std::cout << "NO_ERROR ";
    std::cout << '\n';

    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error after retrieving context flags: " << err << '\n';
    }
}
