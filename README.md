# ImageToLogic
A C program for converting images into logic code in Mindustry.

# External Libraries
Uses raylib for the graphical interface and image processing.
Uses tinyfiledialogs for invoking file selection/save dialogs.

raylib: https://github.com/raysan5/raylib/
tinyfiledialogs: https://sourceforge.net/projects/tinyfiledialogs/

# Usage Guide
1. After installation, double-click ImageToLogic.exe, then click the "Import image" button and select the image you want to convert.

2. Use the following four buttons to adjust parameters. You can preview the adjusted image on the left (Note: Due to the limit of 999 lines of logic code per processor in Mindustry, the image will be compressed to avoid exceeding this limit, which may result in lower actual rendering quality compared to the preview).

3. Click the "Generate logic" button and choose where you want to save the generated logic code.

4. Open the saved Logic.txt (or the filename you specified), copy the logic code for the corresponding logic display, then paste it into the logic processor in Mindustry by clicking "Edit" → "Import from Clipboard".
