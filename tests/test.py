from ionengine.engine import Engine, Window

window = Window("Engine", 800, 600)
engine = Engine("shaders", window)

engine.add_callback()
engine.add_callback()

engine.run()