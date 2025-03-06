// Author: JJ

#include "app.hpp"
#include "getinfo.hpp"
#include "gl_err_callback.h"
#include "glerror.h"

int main(int argc, char** argv) {
    App app;

    if (!app.init()) { 
        return EXIT_FAILURE; 
    }

    return app.run();
}
