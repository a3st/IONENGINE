@echo off
python -m setup.py bdist_wheel
pip install dist/ionengine-1.0.0-cp311-cp311-win_amd64.whl --force-reinstall