from libwebview import App
import os

root_path = os.path.dirname(__file__)
os.chdir(root_path)

app = App("ionengine-editor", "Shader Editor", size=(1280, 720), resizeable=True, is_debug=True)

app.run(os.path.join("resources", "index.html"))