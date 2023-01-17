
// #define USE_BITMAP_FONT_RENDERER

#ifdef USE_BITMAP_FONT_RENDERER
    #include "ft2build.h"
    #include "freetype/freetype.h"
#endif


using namespace StaticUtils;



#define FONT_CHAR_DEFAULT_TYPE uint8_t
#define FONTFACE_MAX_CHARACTER_COUNT (std::numeric_limits<FONT_CHAR_DEFAULT_TYPE>::max()+1)
#define FONTFACE_MAX_PAGES_COUNT 3
#define FONT_CHAR_BUFFER_START_SIZE 1024


#ifdef USE_BITMAP_FONT_RENDERER
    struct BitmapFontRenderer {
        struct Character {
            ivec2 size;       // Size of glyph
            ivec2 bearing;    // Offset from baseline to left/top of glyph
            uint32_t advance;
        };
        Character characters[FONTFACE_MAX_CHARACTER_COUNT];
    	HXTexture fontmaps[FONTFACE_MAX_CHARACTER_COUNT];
    	FT_Library library;
    	FT_Face face;
    };

#else
    struct SDFFontRenderer {
        struct alignas(16) Character {
            ivec3 pos;        // position in atlas: x, y, pagenum
            uint32_t advance;
            ivec2 size;       // Size of glyph
            ivec2 bearing;    // Offset from baseline to left/top of glyph
        };

        struct CharacterPair {
            uint8_t realmap;
        };

        uint8_t fontmapsCount;
        HXTexture fontmaps[FONTFACE_MAX_PAGES_COUNT];

        CharacterPair charactermaps[FONTFACE_MAX_CHARACTER_COUNT];
        HXStorageBuffer charactersBuff;

        size_t frameTextBufferPointer;
        HXStorageBuffer frameTextBuffer;

        HXComputePipeline fontRenderPipeline;
    };
#endif




struct UIRenderer {
#ifdef USE_BITMAP_FONT_RENDERER
	BitmapFontRenderer fontRend;
#else
    SDFFontRenderer fontRend;
#endif

    HXCommandBuffer cmdbuff;
    HXGPUFence fence;

	void InitFontRenderer(Application&);
    void RenderText(Application&, const std::string&, vec2, float scale);

	void Init(Application&);
	void Update(Application&);
	void Resize(Application&);
	void Cleanup(Application&);
};
