--push info :
    this is first push for this project


--motivation :
    this is my first time that i can render text from any string by sending every character outline data in a string to gpu , and render it every frame !


--description :
    this project made using opengl for graphics api , and sdl3 for window management and input handling , and freetype library for extracting font data , and c language
    the main goal of this project is to render text using fragment shader and real time font rendering 


--how it works :
    --general idea:
        i need to divide the screen into a grid n * m, each grid cell contain at most one character
        each character described by its outline data
        and in the fragement shader i have one sdf function that work for all characters
        i check for each cell what character data has and create the appropriate sdf for it
    --font :
        in first step i need every character outline data , i use freetype library for extract characters outline data
        doing this by give a font to the library (my editor work only on true type font, so the font must be true type) and extract the outlines data from every character 
    --type of data that i send to gpu :
        there are to shader storage buffer object
        1-a buffer taht has all characters data that will be rendering into the screen
        2-a buffer that has for every grid cell the specific position the first for its character data



--next step :
    refactore the code and clean it
    make the editor open a file and edit the file 
    change the size of the font and update the shader buffer that contain infos abot text

