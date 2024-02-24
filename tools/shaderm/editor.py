from libwebview import App
from ionengine.engine import Engine, Window
import os

root_path = os.path.dirname(__file__)
os.chdir(root_path)

app = App(
    "ionengine-editor",
    "Shader Editor",
    size=(1280, 720),
    resizeable=True,
    is_debug=True,
)

engine = Engine(os.path.join(root_path, "resources", "shaders"), None)


@app.updated
def loop():
    engine.tick()


app.run(os.path.join(root_path, "resources", "index.html"))
