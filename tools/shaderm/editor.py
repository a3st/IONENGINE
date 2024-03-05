import base64
import os
from libwebview import App
from ionengine.platform import Window
from ionengine.engine import Engine, TextureFlags, Color

root_path = os.path.dirname(__file__)
os.chdir(root_path)

app = App(
    "ionengine-editor",
    "Shader Editor",
    size=(1280, 720),
    resizeable=True,
    is_debug=True,
)


class MyEngine(Engine):
    def __init__(self, window: Window):
        Engine.__init__(self, window)

    def init(self):
        self.texture = self.create_texture(800, 600, TextureFlags.RenderTarget)

    def update(self, dt):

        output = bytes(self.texture.dump())

        with open("output.png", mode="wb") as fp:
            fp.write(output)

        image = base64.b64encode(output).decode(encoding="utf-8")
        url = f"data:image/png;base64,{image}"
        app.emit("onPreviewUpdate", url)

    def render(self):
        self.renderer.begin_draw(
            [self.texture], None, Color(0.2, 0.3, 0.1, 1.0), 0.0, 0
        )
        self.renderer.end_draw()


engine = MyEngine(None)
engine.run()

@app.route
def test():
    pass


@app.updated
def loop():
    engine.tick()


app.run(os.path.join(root_path, "resources", "index.html"))
