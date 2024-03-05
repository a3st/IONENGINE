from libwebview import App
import os

root_path = os.path.dirname(__file__)
os.chdir(root_path)

app = App("ionengine-editor", "Editor", size=(800, 600), resizeable=True, is_debug=True)
app.run(os.path.join(root_path, "resources", "index.html"))