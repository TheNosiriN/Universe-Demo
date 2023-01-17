#include "main.h"
#include "utils.h"
#include "structs.h"



#ifdef USE_BITMAP_FONT_RENDERER
void UIRenderer::InitFontRenderer(Application& app){
	int error = 0;

	error = FT_Init_FreeType(&fontRend.library);
	if (error){
		/// TODO: make error handling system or use HXRC
		std::cout << "freetype: Failed to load freetype" << '\n';
		return;
	}

	error = FT_New_Face(
		fontRend.library,
		"./assets/font/Bahnschrift/BAHNSCHRIFT.ttf",
		0, &fontRend.face
	);
	if (error == FT_Err_Unknown_File_Format){
		std::cout << "freetype: The font file isn't supported or can't be read" << '\n';
	}
	else if (error){
		std::cout << "freetype: The font file is broken or can't be read" << '\n';
	}


    FT_Set_Pixel_Sizes(fontRend.face, 0, 48);


    /// pre render all characters
    for (uint8_t i=0; i<FONTFACE_MAX_CHARACTER_COUNT; ++i){
        if (FT_Load_Char(fontRend.face, i, FT_LOAD_RENDER)){
            std::cout << "freetype: failed to load Glyph for char number: " << i << "\n";
            continue;
        }

        HXTextureConfig texconfig{};
    	texconfig.Type = HX_GRAPHICS_TEXTURE_2D;
    	texconfig.Depth = 1;
    	texconfig.Usage = HX_GRAPHICS_USAGE_STATIC;
    	texconfig.GenerateMips = false;
    	texconfig.Format = HX_R8_UINT;
    	texconfig.Width = fontRend.face->glyph->bitmap.width;
    	texconfig.Height = fontRend.face->glyph->bitmap.rows;
    	fontRend.fontmaps[i] = app.hxg.CreateTexture(texconfig, fontRend.face->glyph->bitmap.buffer);

        BitmapFontRenderer::Character ch{};
        ch.size = ivec2(fontRend.face->glyph->bitmap.width, fontRend.face->glyph->bitmap.rows);
        ch.bearing = ivec2(fontRend.face->glyph->bitmap_left, fontRend.face->glyph->bitmap_top);
        ch.advance = fontRend.face->glyph->advance.x;
        fontRend.characters[i] = ch;
    }

    /// free freetype
    FT_Done_Face(fontRend.face);
    FT_Done_FreeType(fontRend.library);
}



void UIRenderer::RenderText(Application& app, const std::string& text, vec2 pos, float scale){
    for (int i=0; i<text.size(); ++i){
        Character ch = fontRend.characters[text[i]];


    }
}

#else

int __Font_Atlas_ParseInt(const std::string& t){
    uint pos = 0;
    for (int i=0; i<t.size(); ++i){
        if (t[i] == '='){
            pos = i;
            break;
        }
    }

    return std::stoi(t.substr(pos+1, t.size()-pos));
}

std::string __Font_Atlas_ParseString(const std::string& t){
    uint pos = 0;
    for (int i=0; i<t.size(); ++i){
        if (t[i] == '='){
            pos = i;
            break;
        }
    }

    return t.substr(pos+2, (t.size()-pos)-3);
}



#include "stb_image.h"

uint8_t* __Font_Atlas_Load_PNG(const std::string& name, int width, int height, int channels){
	return stbi_load(name.c_str(), &width, &height, &channels, 0);
}
void __Font_Atlas_Free_PNG(uint8_t* b){
    stbi_image_free(b);
}




void UIRenderer::InitFontRenderer(Application& app){
    // load fnt file
    std::ifstream fnt("assets/font/bahnschrift.fnt");
    if (fnt.fail()){
        std::cout << "SDF font: The font file is broken or can't be read" << '\n';
    }

    // parse fnt file
    std::string temp;
    // skip to second line
    fnt.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    fnt >> temp; // common
    fnt >> temp; // lineHeight
    int lineHeight = __Font_Atlas_ParseInt(temp);
    fnt >> temp; // base
    fnt >> temp; // scaleW (width)
    int texwidth = __Font_Atlas_ParseInt(temp);
    fnt >> temp; // scaleH (Height)
    int texheight = __Font_Atlas_ParseInt(temp);
    fnt >> temp; // pages
    int pagescount = __Font_Atlas_ParseInt(temp);
    fnt >> temp; // packed

    // loading pages
    fnt >> temp; // pages
    for (int i=0; i<pagescount; ++i){
        fnt >> temp; // id
        fnt >> temp; // file
        std::string mapname = __Font_Atlas_ParseString(temp);

        // load texture file
        uint8_t* tbuff = __Font_Atlas_Load_PNG("assets/font/"+mapname, texwidth, texheight, 4);
        if (!tbuff){
            std::cout << "SDF font: The font atlas " << mapname << " is broken or can't be read" << '\n';
        }

        // TODO: transform byte array to rg not rgba

        HXTextureConfig texconfig{};
        texconfig.Type = HX_GRAPHICS_TEXTURE_2D;
        texconfig.Depth = 1;
        texconfig.Usage = HX_GRAPHICS_USAGE_STATIC;
        texconfig.GenerateMips = false;
        texconfig.Format = HX_R8_G8_B8_A8_UINT;
        texconfig.Width = texwidth;
        texconfig.Height = texheight;
        fontRend.fontmaps[i] = app.hxg.CreateTexture(texconfig, tbuff);
        __Font_Atlas_Free_PNG(tbuff);
    }
    fontRend.fontmapsCount = pagescount;


    // load characters
    fnt >> temp; // chars
    fnt >> temp; // count
    int charcount = __Font_Atlas_ParseInt(temp);
    std::cout << "charcount: "<<charcount << '\n';

    Utils::TypedVector<SDFFontRenderer::Character> tcharacters;
    tcharacters.reserve(FONTFACE_MAX_CHARACTER_COUNT);

    for (int i=0; i<charcount; ++i){
        fnt >> temp; // char
        fnt >> temp; // id
        int id = __Font_Atlas_ParseInt(temp);
        fnt >> temp; // x
        int x = __Font_Atlas_ParseInt(temp);
        fnt >> temp; // y
        int y = __Font_Atlas_ParseInt(temp);
        fnt >> temp; // width
        int width = __Font_Atlas_ParseInt(temp);
        fnt >> temp; // height
        int height = __Font_Atlas_ParseInt(temp);
        fnt >> temp; // xoffset
        int xoff = __Font_Atlas_ParseInt(temp);
        fnt >> temp; // yoffset
        int yoff = __Font_Atlas_ParseInt(temp);
        fnt >> temp; // xadvance
        int xadvance = __Font_Atlas_ParseInt(temp);
        fnt >> temp; // page
        int pagenum = __Font_Atlas_ParseInt(temp);
        fnt >> temp; // chnl

        SDFFontRenderer::Character ch{};
        ch.pos = ivec3(x, y, pagenum);
        ch.size = ivec2(width, height);
        ch.bearing = ivec2(xoff, yoff);
        ch.advance = xadvance;

        size_t realmap = tcharacters.push_back(ch);
        SDFFontRenderer::CharacterPair cp{};
        // cp.alias = id;
        cp.realmap = (uint8_t)realmap;
        fontRend.charactermaps[id] = cp;
        // std::cout << "got char: id:"<<id << ", pos:"<<ch.pos.x<<","<<ch.pos.y<<","<<ch.pos.z << ", advance:"<<ch.advance << '\n';
    }

    // init char gpu buffer
    fontRend.charactersBuff = app.hxg.CreateStorageBuffer(
        HXSBufferConfig{ charcount*sizeof(SDFFontRenderer::Character), HX_GRAPHICS_USAGE_STATIC }, tcharacters.data(),
		HX_GRAPHICS_SHADERBUFFER
    );


    // init frame text buffer
    fontRend.frameTextBuffer = app.hxg.CreateStorageBuffer(
        HXSBufferConfig{ FONT_CHAR_BUFFER_START_SIZE*sizeof(FONT_CHAR_DEFAULT_TYPE), HX_GRAPHICS_USAGE_DYNAMIC }, NULL,
		HX_GRAPHICS_SHADERBUFFER
    );

    // init compute pipeline
    // HXComputePipelineConfig c_pipconfig{};
	// char* comp_blob = LoadShaderBlob("sdf_font_render_pass_compute");
	// ShaderC::LoadShaderBinary(comp_blob, HX_SHADERC_TYPE_COMPUTE, c_pipconfig.ComputeShader, c_pipconfig.Metadata);
	// CreateDefaultComputeBundle(app, fontRend.fontRenderPipeline, 1, HX_R16_G16_B16_A16_FLOAT, c_pipconfig);
}


void UIRenderer::RenderText(Application& app, const std::string& text, vec2 pos, float scale){
    
}


#endif



void UIRenderer::Init(Application& app){
	InitFontRenderer(app);

    // init command buffer

}
