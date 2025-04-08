conda activate tulip_python311
/snap/bin/cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/home/luod/ProteinCraft/tulip/cmake-build-release/install -DPYTHON_EXECUTABLE=/home/luod/miniconda3/envs/tulip_python311/bin/python -DTULIP_BUILD_CORE_ONLY=OFF -DTULIP_BUILD_TESTS=ON -DTULIP_BUILD_GL_TEX_LOADER=ON -DTULIP_BUILD_DOC=OFF -DTULIP_PYTHON_SITE_INSTALL=ON -DTULIP_USE_CCACHE=ON
ninja -j$(nproc)
ninja install
