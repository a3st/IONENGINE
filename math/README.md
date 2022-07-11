
<p><h1 align="center">IONENGINE Math Module</h1></p>

**IONENGINE** (Image Open Engine) is an Graphic 3D Engine written in C++. It can be used to learn and exploring renderer techniques or write cross-platform C++ applications like games. A basic version of engine only includes **Renderer** module.

<p><h1 align="left">Features</h1></p>

* Templated classes (float_t, double_t specialization)
* Matrix4x4
* Vector2, Vector3, Vector4
* Quaternion
* Color

<p><h1 align="left">Hello matrix</h1></p>

```c++
#include <math/matrix.hpp>

math::Matrixf identity_matrix = math::Matrixf::identity();
math::Matrixf translate = math::Matrixf::translate(math::Vector3f(1.0f, 1.0f, 1.0f));
math::Matrixf scale = math::Matrixf::scale(math::Vector3f(2.0f, 2.0f, 2.0f));

```