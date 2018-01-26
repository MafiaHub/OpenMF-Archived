# Lightmaps in Mafia

## Author
Originally, credits go to GOLOD55 for his raw lightmap description. More details and this docs
were added by Romop5.
## Briefing

In Mafia, there are two types of shadows:
- dynamic - computed every frame real-time
- static - prebaked using editors

*Dynamic shadows* are casted by player/NPCs and cars with relation to lights.

*Static shadows* (lightmaps) are precreated offline and can be stored in two formats:
* bitmaps - for each texel of mesh, the amount of occlusion (darkness / lightness) is stored
* vertex colour - for each face (triangle) of visual object, there are 3 floats describing the amout of darkness.

Lightmaps are stored in scene2.bin as a part of object definition. The section with lightmaps starts with magic byte *0x40A0*.

## Structure

The following paragraph describes how typical lightmap sections looks like:

1. `Header`

    Marks and comfirms the start of lightmap block.

~~~~c
struct {
    uint16_t magic_byte;    // must equal to 0x40A0
    uint8_t unk[3];         // unk
    uint32_t magic_string;  // ASCII "LMAP"
    uint32_t unk2;          // checksum (according to GOLOD)
    uint16_t unk3;
} Header;
~~~~


2. `Section_Header`

    Consist of only one number: the bitmap where each bit states the presence of lightmap for specified Level of Details level.
For example, bit at possition 1 specifies if lightmap for LOD no. 1 is presented.

    In original Mafia, the number has to match the count of LOD of visual object (frame).
~~~~c
struct {
	uint8_t bitmapOfLevelOfDetails; 
} Section_Header;
~~~~


3. For each level in `Section_Header`: 
    * `General_Data`
    * if BITMAP:
        * `Header_Bitmap`

~~~~c
enum typeOfLightmap = { BITMAP = 0x0,   // lightmap stored as RGBA texels
                        VERTEX = 0x1    // a color value for each vertex in face
                        };
struct {
    uint8_t lightmapVersion;    // only 0x21 is supported by Mafia 
    uint8_t typeOfLightmap;     // usually 0x6 (BMP) / 0x5 (vertex) 
    uint32_t currentLOD;        // the level of currently parsed block ?
    float unkA;                 // somehow related to LOD of visual object
                                // maybe these 2 float relates to distance / blending 
    float unkB;             
    typeOfLightmap type;        // type of stored lightmap, which follows
} General_Data;

struct {
    uint16_t    numberOfVertices;
    uint16_t    numberOfFaceGroups;
    uint16_t    type; ?
} Header_Bitmap;
~~~~
