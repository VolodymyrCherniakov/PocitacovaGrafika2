App::init() {

    //init GL context
    //...

    bool vsync_on = true;
    
    // for current GL context switch vsync
    if (vsync_on)
        glfwSwapInterval(1);          // Set V-Sync ON.
    else
        glfwSwapInterval(0);          // Set V-Sync OFF.

    
    // dtto...
    glfwSwapInterval((vsync_on?1:0);
}


App::key_callback(...) {

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			std::cout << "ESC has been pressed!\n";
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
        case GLFW_KEY_ ???   :
            // if vsync is on, set vsync off
            // if vsync is off, set vsync on
        default:
            break;
        }
    }

}
