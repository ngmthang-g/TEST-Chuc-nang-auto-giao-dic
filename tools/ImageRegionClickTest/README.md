# ThanLong Image Region Click Test v0.1.0

Standalone Windows x64 test tool for one feature only: find a template image inside a user-selected region and click the center of the best match.

## Test flow
1. Click **1. Chọn cửa sổ sau 1.5s**, then focus the game window.
2. Use **2A** to load a PNG/JPG/BMP template, or **2B** to drag-capture the button itself.
3. Click **3. Khoanh vùng tìm kiếm** and drag the area where the button can appear.
4. Use **Tìm thử** to verify score/position without clicking.
5. Use **TÌM + CLICK 1 LẦN** or global **F6** to locate and click the match center once.

Default threshold is 88%. Lower it slightly if the same button renders with small color/anti-alias differences. The template must be at the same render scale as the on-screen button.

This tool does not inject into the game, read game memory, call game APIs, or automate any other feature.
