

int main(int* argc, char** agrv) {
    
    auto window = platform::Window();
    window.label("Window");
    window.clientSize(800, 600);

    window.run([&]() {

    });

    return EXIT_SUCCESS;
}
