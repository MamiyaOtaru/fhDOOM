/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).  

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
/*
** QGL_WIN.C
**
** This file implements the operating system binding of GL to QGL function
** pointers.  When doing a port of Doom you must implement the following
** two functions:
**
** QGL_Init() - loads libraries, assigns function pointers, etc.
** QGL_Shutdown() - unloads libraries, NULLs function pointers
*/
#include "../../idlib/precompiled.h"
#pragma hdrstop

#include <float.h>
#include "win_local.h"
#include "../../renderer/tr_local.h"

typedef struct {
	GLenum	e;
	const char *name;
} glEnumName_t;

#define	DEF(x) { x, #x },

glEnumName_t	glEnumNames[] = {
DEF(GL_FALSE)
DEF(GL_TRUE)

	{ GL_BYTE, "GL_BYTE" },
	{ GL_UNSIGNED_BYTE, "GL_UNSIGNED_BYTE" },
	{ GL_SHORT, "GL_SHORT" },
	{ GL_UNSIGNED_SHORT, "GL_UNSIGNED_SHORT" },
	{ GL_INT, "GL_INT" },
	{ GL_UNSIGNED_INT, "GL_UNSIGNED_INT" },
	{ GL_FLOAT, "GL_FLOAT" },
	{ GL_DOUBLE, "GL_DOUBLE" },

	{ GL_TEXTURE_CUBE_MAP_EXT, "GL_TEXTURE_CUBE_MAP_EXT" },
	{ GL_TEXTURE_3D, "GL_TEXTURE_3D" },
	{ GL_TEXTURE_2D, "GL_TEXTURE_2D" },
	{ GL_BLEND, "GL_BLEND" },
	{ GL_DEPTH_TEST, "GL_DEPTH_TEST" },
	{ GL_CULL_FACE, "GL_CULL_FACE" },
	{ GL_CLIP_PLANE0, "GL_CLIP_PLANE0" },
	{ GL_COLOR_ARRAY, "GL_COLOR_ARRAY" },
	{ GL_TEXTURE_COORD_ARRAY, "GL_TEXTURE_COORD_ARRAY" },
	{ GL_VERTEX_ARRAY, "GL_VERTEX_ARRAY" },
	{ GL_ALPHA_TEST, "GL_ALPHA_TEST" },
	{ GL_TEXTURE_GEN_S, "GL_TEXTURE_GEN_S" },
	{ GL_TEXTURE_GEN_T, "GL_TEXTURE_GEN_T" },
	{ GL_TEXTURE_GEN_R, "GL_TEXTURE_GEN_R" },
	{ GL_TEXTURE_GEN_Q, "GL_TEXTURE_GEN_Q" },
	{ GL_STENCIL_TEST, "GL_STENCIL_TEST" },
	{ GL_POLYGON_OFFSET_FILL, "GL_POLYGON_OFFSET_FILL" },

	{ GL_TRIANGLES,	"GL_TRIANGLES" },
	{ GL_TRIANGLE_STRIP, "GL_TRIANGLE_STRIP" },
	{ GL_TRIANGLE_FAN, "GL_TRIANGLE_FAN" },
	{ GL_QUADS, "GL_QUADS" },
	{ GL_QUAD_STRIP, "GL_QUAD_STRIP" },
	{ GL_POLYGON, "GL_POLYGON" },
	{ GL_POINTS, "GL_POINTS" },
	{ GL_LINES, "GL_LINES" },
	{ GL_LINE_STRIP, "GL_LINE_STRIP" },
	{ GL_LINE_LOOP, "GL_LINE_LOOP" },

	{ GL_ALWAYS, "GL_ALWAYS" },
	{ GL_NEVER, "GL_NEVER" },
	{ GL_LEQUAL, "GL_LEQUAL" },
	{ GL_LESS, "GL_LESS" },
	{ GL_EQUAL, "GL_EQUAL" },
	{ GL_GREATER, "GL_GREATER" },
	{ GL_GEQUAL, "GL_GEQUAL" },
	{ GL_NOTEQUAL, "GL_NOTEQUAL" },

	{ GL_ONE, "GL_ONE" },
	{ GL_ZERO, "GL_ZERO" },
	{ GL_SRC_ALPHA, "GL_SRC_ALPHA" },
	{ GL_ONE_MINUS_SRC_ALPHA, "GL_ONE_MINUS_SRC_ALPHA" },
	{ GL_DST_COLOR, "GL_DST_COLOR" },
	{ GL_ONE_MINUS_DST_COLOR, "GL_ONE_MINUS_DST_COLOR" },
	{ GL_DST_ALPHA, "GL_DST_ALPHA" },

	{ GL_MODELVIEW, "GL_MODELVIEW" },
	{ GL_PROJECTION, "GL_PROJECTION" },
	{ GL_TEXTURE, "GL_TEXTURE" },

/* DrawBufferMode */
DEF(GL_NONE)
DEF(GL_FRONT_LEFT)
DEF(GL_FRONT_RIGHT)
DEF(GL_BACK_LEFT)
DEF(GL_BACK_RIGHT)
DEF(GL_FRONT)
DEF(GL_BACK)
DEF(GL_LEFT)
DEF(GL_RIGHT)
DEF(GL_FRONT_AND_BACK)
DEF(GL_AUX0)
DEF(GL_AUX1)
DEF(GL_AUX2)
DEF(GL_AUX3)

/* GetTarget */
DEF(GL_CURRENT_COLOR)
DEF(GL_CURRENT_INDEX)
DEF(GL_CURRENT_NORMAL)
DEF(GL_CURRENT_TEXTURE_COORDS)
DEF(GL_CURRENT_RASTER_COLOR)
DEF(GL_CURRENT_RASTER_INDEX)
DEF(GL_CURRENT_RASTER_TEXTURE_COORDS)
DEF(GL_CURRENT_RASTER_POSITION)
DEF(GL_CURRENT_RASTER_POSITION_VALID)
DEF(GL_CURRENT_RASTER_DISTANCE)
DEF(GL_POINT_SMOOTH)
DEF(GL_POINT_SIZE)
DEF(GL_POINT_SIZE_RANGE)
DEF(GL_POINT_SIZE_GRANULARITY)
DEF(GL_LINE_SMOOTH)
DEF(GL_LINE_WIDTH)
DEF(GL_LINE_WIDTH_RANGE)
DEF(GL_LINE_WIDTH_GRANULARITY)
DEF(GL_LINE_STIPPLE)
DEF(GL_LINE_STIPPLE_PATTERN)
DEF(GL_LINE_STIPPLE_REPEAT)
DEF(GL_LIST_MODE)
DEF(GL_MAX_LIST_NESTING)
DEF(GL_LIST_BASE)
DEF(GL_LIST_INDEX)
DEF(GL_POLYGON_MODE)
DEF(GL_POLYGON_SMOOTH)
DEF(GL_POLYGON_STIPPLE)
DEF(GL_EDGE_FLAG)
DEF(GL_CULL_FACE)
DEF(GL_CULL_FACE_MODE)
DEF(GL_FRONT_FACE)
DEF(GL_LIGHTING)
DEF(GL_LIGHT_MODEL_LOCAL_VIEWER)
DEF(GL_LIGHT_MODEL_TWO_SIDE)
DEF(GL_LIGHT_MODEL_AMBIENT)
DEF(GL_SHADE_MODEL)
DEF(GL_COLOR_MATERIAL_FACE)
DEF(GL_COLOR_MATERIAL_PARAMETER)
DEF(GL_COLOR_MATERIAL)
DEF(GL_FOG)
DEF(GL_FOG_INDEX)
DEF(GL_FOG_DENSITY)
DEF(GL_FOG_START)
DEF(GL_FOG_END)
DEF(GL_FOG_MODE)
DEF(GL_FOG_COLOR)
DEF(GL_DEPTH_RANGE)
DEF(GL_DEPTH_TEST)
DEF(GL_DEPTH_WRITEMASK)
DEF(GL_DEPTH_CLEAR_VALUE)
DEF(GL_DEPTH_FUNC)
DEF(GL_ACCUM_CLEAR_VALUE)
DEF(GL_STENCIL_TEST)
DEF(GL_STENCIL_CLEAR_VALUE)
DEF(GL_STENCIL_FUNC)
DEF(GL_STENCIL_VALUE_MASK)
DEF(GL_STENCIL_FAIL)
DEF(GL_STENCIL_PASS_DEPTH_FAIL)
DEF(GL_STENCIL_PASS_DEPTH_PASS)
DEF(GL_STENCIL_REF)
DEF(GL_STENCIL_WRITEMASK)
DEF(GL_MATRIX_MODE)
DEF(GL_NORMALIZE)
DEF(GL_VIEWPORT)
DEF(GL_MODELVIEW_STACK_DEPTH)
DEF(GL_PROJECTION_STACK_DEPTH)
DEF(GL_TEXTURE_STACK_DEPTH)
DEF(GL_MODELVIEW_MATRIX)
DEF(GL_PROJECTION_MATRIX)
DEF(GL_TEXTURE_MATRIX)
DEF(GL_ATTRIB_STACK_DEPTH)
DEF(GL_CLIENT_ATTRIB_STACK_DEPTH)
DEF(GL_ALPHA_TEST)
DEF(GL_ALPHA_TEST_FUNC)
DEF(GL_ALPHA_TEST_REF)
DEF(GL_DITHER)
DEF(GL_BLEND_DST)
DEF(GL_BLEND_SRC)
DEF(GL_BLEND)
DEF(GL_LOGIC_OP_MODE)
DEF(GL_INDEX_LOGIC_OP)
DEF(GL_COLOR_LOGIC_OP)
DEF(GL_AUX_BUFFERS)
DEF(GL_DRAW_BUFFER)
DEF(GL_READ_BUFFER)
DEF(GL_SCISSOR_BOX)
DEF(GL_SCISSOR_TEST)
DEF(GL_INDEX_CLEAR_VALUE)
DEF(GL_INDEX_WRITEMASK)
DEF(GL_COLOR_CLEAR_VALUE)
DEF(GL_COLOR_WRITEMASK)
DEF(GL_INDEX_MODE)
DEF(GL_RGBA_MODE)
DEF(GL_DOUBLEBUFFER)
DEF(GL_STEREO)
DEF(GL_RENDER_MODE)
DEF(GL_PERSPECTIVE_CORRECTION_HINT)
DEF(GL_POINT_SMOOTH_HINT)
DEF(GL_LINE_SMOOTH_HINT)
DEF(GL_POLYGON_SMOOTH_HINT)
DEF(GL_FOG_HINT)
DEF(GL_TEXTURE_GEN_S)
DEF(GL_TEXTURE_GEN_T)
DEF(GL_TEXTURE_GEN_R)
DEF(GL_TEXTURE_GEN_Q)
DEF(GL_PIXEL_MAP_I_TO_I)
DEF(GL_PIXEL_MAP_S_TO_S)
DEF(GL_PIXEL_MAP_I_TO_R)
DEF(GL_PIXEL_MAP_I_TO_G)
DEF(GL_PIXEL_MAP_I_TO_B)
DEF(GL_PIXEL_MAP_I_TO_A)
DEF(GL_PIXEL_MAP_R_TO_R)
DEF(GL_PIXEL_MAP_G_TO_G)
DEF(GL_PIXEL_MAP_B_TO_B)
DEF(GL_PIXEL_MAP_A_TO_A)
DEF(GL_PIXEL_MAP_I_TO_I_SIZE)
DEF(GL_PIXEL_MAP_S_TO_S_SIZE)
DEF(GL_PIXEL_MAP_I_TO_R_SIZE)
DEF(GL_PIXEL_MAP_I_TO_G_SIZE)
DEF(GL_PIXEL_MAP_I_TO_B_SIZE)
DEF(GL_PIXEL_MAP_I_TO_A_SIZE)
DEF(GL_PIXEL_MAP_R_TO_R_SIZE)
DEF(GL_PIXEL_MAP_G_TO_G_SIZE)
DEF(GL_PIXEL_MAP_B_TO_B_SIZE)
DEF(GL_PIXEL_MAP_A_TO_A_SIZE)
DEF(GL_UNPACK_SWAP_BYTES)
DEF(GL_UNPACK_LSB_FIRST)
DEF(GL_UNPACK_ROW_LENGTH)
DEF(GL_UNPACK_SKIP_ROWS)
DEF(GL_UNPACK_SKIP_PIXELS)
DEF(GL_UNPACK_ALIGNMENT)
DEF(GL_PACK_SWAP_BYTES)
DEF(GL_PACK_LSB_FIRST)
DEF(GL_PACK_ROW_LENGTH)
DEF(GL_PACK_SKIP_ROWS)
DEF(GL_PACK_SKIP_PIXELS)
DEF(GL_PACK_ALIGNMENT)
DEF(GL_MAP_COLOR)
DEF(GL_MAP_STENCIL)
DEF(GL_INDEX_SHIFT)
DEF(GL_INDEX_OFFSET)
DEF(GL_RED_SCALE)
DEF(GL_RED_BIAS)
DEF(GL_ZOOM_X)
DEF(GL_ZOOM_Y)
DEF(GL_GREEN_SCALE)
DEF(GL_GREEN_BIAS)
DEF(GL_BLUE_SCALE)
DEF(GL_BLUE_BIAS)
DEF(GL_ALPHA_SCALE)
DEF(GL_ALPHA_BIAS)
DEF(GL_DEPTH_SCALE)
DEF(GL_DEPTH_BIAS)
DEF(GL_MAX_EVAL_ORDER)
DEF(GL_MAX_LIGHTS)
DEF(GL_MAX_CLIP_PLANES)
DEF(GL_MAX_TEXTURE_SIZE)
DEF(GL_MAX_PIXEL_MAP_TABLE)
DEF(GL_MAX_ATTRIB_STACK_DEPTH)
DEF(GL_MAX_MODELVIEW_STACK_DEPTH)
DEF(GL_MAX_NAME_STACK_DEPTH)
DEF(GL_MAX_PROJECTION_STACK_DEPTH)
DEF(GL_MAX_TEXTURE_STACK_DEPTH)
DEF(GL_MAX_VIEWPORT_DIMS)
DEF(GL_MAX_CLIENT_ATTRIB_STACK_DEPTH)
DEF(GL_SUBPIXEL_BITS)
DEF(GL_INDEX_BITS)
DEF(GL_RED_BITS)
DEF(GL_GREEN_BITS)
DEF(GL_BLUE_BITS)
DEF(GL_ALPHA_BITS)
DEF(GL_DEPTH_BITS)
DEF(GL_STENCIL_BITS)
DEF(GL_ACCUM_RED_BITS)
DEF(GL_ACCUM_GREEN_BITS)
DEF(GL_ACCUM_BLUE_BITS)
DEF(GL_ACCUM_ALPHA_BITS)
DEF(GL_NAME_STACK_DEPTH)
DEF(GL_AUTO_NORMAL)
DEF(GL_MAP1_COLOR_4)
DEF(GL_MAP1_INDEX)
DEF(GL_MAP1_NORMAL)
DEF(GL_MAP1_TEXTURE_COORD_1)
DEF(GL_MAP1_TEXTURE_COORD_2)
DEF(GL_MAP1_TEXTURE_COORD_3)
DEF(GL_MAP1_TEXTURE_COORD_4)
DEF(GL_MAP1_VERTEX_3)
DEF(GL_MAP1_VERTEX_4)
DEF(GL_MAP2_COLOR_4)
DEF(GL_MAP2_INDEX)
DEF(GL_MAP2_NORMAL)
DEF(GL_MAP2_TEXTURE_COORD_1)
DEF(GL_MAP2_TEXTURE_COORD_2)
DEF(GL_MAP2_TEXTURE_COORD_3)
DEF(GL_MAP2_TEXTURE_COORD_4)
DEF(GL_MAP2_VERTEX_3)
DEF(GL_MAP2_VERTEX_4)
DEF(GL_MAP1_GRID_DOMAIN)
DEF(GL_MAP1_GRID_SEGMENTS)
DEF(GL_MAP2_GRID_DOMAIN)
DEF(GL_MAP2_GRID_SEGMENTS)
DEF(GL_TEXTURE_1D)
DEF(GL_TEXTURE_2D)
DEF(GL_FEEDBACK_BUFFER_POINTER)
DEF(GL_FEEDBACK_BUFFER_SIZE)
DEF(GL_FEEDBACK_BUFFER_TYPE)
DEF(GL_SELECTION_BUFFER_POINTER)
DEF(GL_SELECTION_BUFFER_SIZE)

/* PixelCopyType */
DEF(GL_COLOR)
DEF(GL_DEPTH)
DEF(GL_STENCIL)

/* PixelFormat */
DEF(GL_COLOR_INDEX)
DEF(GL_STENCIL_INDEX)
DEF(GL_DEPTH_COMPONENT)
DEF(GL_RED)
DEF(GL_GREEN)
DEF(GL_BLUE)
DEF(GL_ALPHA)
DEF(GL_RGB)
DEF(GL_RGBA)
DEF(GL_LUMINANCE)
DEF(GL_LUMINANCE_ALPHA)

/* PixelMap */
/*      GL_PIXEL_MAP_I_TO_I */
/*      GL_PIXEL_MAP_S_TO_S */
/*      GL_PIXEL_MAP_I_TO_R */
/*      GL_PIXEL_MAP_I_TO_G */
/*      GL_PIXEL_MAP_I_TO_B */
/*      GL_PIXEL_MAP_I_TO_A */
/*      GL_PIXEL_MAP_R_TO_R */
/*      GL_PIXEL_MAP_G_TO_G */
/*      GL_PIXEL_MAP_B_TO_B */
/*      GL_PIXEL_MAP_A_TO_A */

/* PixelStore */
/*      GL_UNPACK_SWAP_BYTES */
/*      GL_UNPACK_LSB_FIRST */
/*      GL_UNPACK_ROW_LENGTH */
/*      GL_UNPACK_SKIP_ROWS */
/*      GL_UNPACK_SKIP_PIXELS */
/*      GL_UNPACK_ALIGNMENT */
/*      GL_PACK_SWAP_BYTES */
/*      GL_PACK_LSB_FIRST */
/*      GL_PACK_ROW_LENGTH */
/*      GL_PACK_SKIP_ROWS */
/*      GL_PACK_SKIP_PIXELS */
/*      GL_PACK_ALIGNMENT */

/* PixelTransfer */
/*      GL_MAP_COLOR */
/*      GL_MAP_STENCIL */
/*      GL_INDEX_SHIFT */
/*      GL_INDEX_OFFSET */
/*      GL_RED_SCALE */
/*      GL_RED_BIAS */
/*      GL_GREEN_SCALE */
/*      GL_GREEN_BIAS */
/*      GL_BLUE_SCALE */
/*      GL_BLUE_BIAS */
/*      GL_ALPHA_SCALE */
/*      GL_ALPHA_BIAS */
/*      GL_DEPTH_SCALE */
/*      GL_DEPTH_BIAS */

/* PixelType */
DEF(GL_BITMAP)
/*      GL_BYTE */
/*      GL_UNSIGNED_BYTE */
/*      GL_SHORT */
/*      GL_UNSIGNED_SHORT */
/*      GL_INT */
/*      GL_UNSIGNED_INT */
/*      GL_FLOAT */

/* PolygonMode */
DEF(GL_POINT)
DEF(GL_LINE)
DEF(GL_FILL)

/* RenderingMode */
DEF(GL_RENDER)
DEF(GL_FEEDBACK)
DEF(GL_SELECT)

/* ShadingModel */
DEF(GL_FLAT)
DEF(GL_SMOOTH)

/* StencilOp */
/*      GL_ZERO */
DEF(GL_KEEP)
DEF(GL_REPLACE)
DEF(GL_INCR)
DEF(GL_DECR)
/*      GL_INVERT */

/* StringName */
DEF(GL_VENDOR)
DEF(GL_RENDERER)
DEF(GL_VERSION)
DEF(GL_EXTENSIONS)

/* TextureCoordName */
DEF(GL_S)
DEF(GL_T)
DEF(GL_R)
DEF(GL_Q)

/* TexCoordPointerType */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* TextureEnvMode */
DEF(GL_MODULATE)
DEF(GL_DECAL)
/*      GL_BLEND */
/*      GL_REPLACE */

/* TextureEnvParameter */
DEF(GL_TEXTURE_ENV_MODE)
DEF(GL_TEXTURE_ENV_COLOR)

/* TextureEnvTarget */
DEF(GL_TEXTURE_ENV)

/* TextureGenMode */
DEF(GL_EYE_LINEAR)
DEF(GL_OBJECT_LINEAR)
DEF(GL_SPHERE_MAP)

/* TextureGenParameter */
DEF(GL_TEXTURE_GEN_MODE)
DEF(GL_OBJECT_PLANE)
DEF(GL_EYE_PLANE)

/* TextureMagFilter */
DEF(GL_NEAREST)
DEF(GL_LINEAR)

/* TextureMinFilter */
/*      GL_NEAREST */
/*      GL_LINEAR */
DEF(GL_NEAREST_MIPMAP_NEAREST)
DEF(GL_LINEAR_MIPMAP_NEAREST)
DEF(GL_NEAREST_MIPMAP_LINEAR)
DEF(GL_LINEAR_MIPMAP_LINEAR)

/* TextureParameterName */
DEF(GL_TEXTURE_MAG_FILTER)
DEF(GL_TEXTURE_MIN_FILTER)
DEF(GL_TEXTURE_WRAP_S)
DEF(GL_TEXTURE_WRAP_T)
/*      GL_TEXTURE_BORDER_COLOR */
/*      GL_TEXTURE_PRIORITY */

/* TextureTarget */
/*      GL_TEXTURE_1D */
/*      GL_TEXTURE_2D */
/*      GL_PROXY_TEXTURE_1D */
/*      GL_PROXY_TEXTURE_2D */

/* TextureWrapMode */
DEF(GL_CLAMP)
DEF(GL_REPEAT)

/* VertexPointerType */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* ClientAttribMask */
DEF(GL_CLIENT_PIXEL_STORE_BIT)
DEF(GL_CLIENT_VERTEX_ARRAY_BIT)
DEF(GL_CLIENT_ALL_ATTRIB_BITS)

/* polygon_offset */
DEF(GL_POLYGON_OFFSET_FACTOR)
DEF(GL_POLYGON_OFFSET_UNITS)
DEF(GL_POLYGON_OFFSET_POINT)
DEF(GL_POLYGON_OFFSET_LINE)
DEF(GL_POLYGON_OFFSET_FILL)

	{ 0, NULL }
};

static const char *EnumString( GLenum t )
{
	static char buffer[8][1024];
	static int index = 0;

	for ( glEnumName_t *n = glEnumNames ; n->name ; n++ ) {
		if ( t == n->e ) {
			return n->name;
		}
	}

	int oldIndex = index;
	index = ( index + 1 ) & 7;
	sprintf( buffer[oldIndex], "0x%x", t );

	return buffer[oldIndex];
}

static const char *FloatData( const GLfloat *v, int count ) {
	static char buffer[8][1024];
	static int index = 0;
	char *name;

	name = buffer[index&7];
	sprintf( name, "f%i", index );
	index++;

	fprintf( tr.logFile, "static float %s[%i] = {", name, count );
	for( int i = 0 ; i < count ; i++ ) {
		if ( i < count - 1 ) {
			fprintf( tr.logFile, "%f,", v[i] );
		} else {
			fprintf( tr.logFile, "%f};\n", v[i] );
		}
	}

	return name;
}

/*
** QGL_Shutdown
**
** Unloads the specified DLL then nulls out all the proc pointers.  This
** is only called during a hard shutdown of the OGL subsystem (e.g. vid_restart).
*/
void QGL_Shutdown( void )
{
	common->Printf( "...shutting down QGL\n" );

	if ( win32.hinstOpenGL )
	{
		common->Printf( "...unloading OpenGL DLL\n" );
		FreeLibrary( win32.hinstOpenGL );
	}

	win32.hinstOpenGL = NULL;
}

#pragma warning (disable : 4113 4133 4047 )

/*
** QGL_Init
**
** This is responsible for binding our gl function pointers to 
** the appropriate GL stuff.  In Windows this means doing a 
** LoadLibrary and a bunch of calls to GetProcAddress.  On other
** operating systems we need to do the right thing, whatever that
** might be.
*/
bool QGL_Init( const char *dllname )
{
	assert( win32.hinstOpenGL == 0 );

	common->Printf( "...initializing QGL\n" );

	common->Printf( "...calling LoadLibrary( '%s' ): ", dllname );

	if ( ( win32.hinstOpenGL = LoadLibraryA( dllname ) ) == 0 )
	{
		common->Printf( "failed\n" );
		return false;
	}
	common->Printf( "succeeded\n" );

	glActiveTextureARB = 0;
	glClientActiveTextureARB = 0;
	glMultiTexCoord2fARB = 0;

	return true;
}
