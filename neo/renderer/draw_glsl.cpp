/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 2016 Johannes Ohlemacher (http://github.com/eXistence/fhDOOM)

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

#include "../idlib/precompiled.h"
#pragma hdrstop

#include "tr_local.h"
#include "RenderProgram.h"
#include "ImmediateMode.h"
#include "RenderList.h"
#include "Framebuffer.h"

idCVar r_pomEnabled("r_pomEnabled", "0", CVAR_ARCHIVE | CVAR_RENDERER | CVAR_BOOL, "POM enabled or disabled");
idCVar r_pomMaxHeight("r_pomMaxHeight", "0.045", CVAR_ARCHIVE | CVAR_RENDERER | CVAR_FLOAT, "maximum height for POM");
idCVar r_shading("r_shading", "0", CVAR_ARCHIVE | CVAR_RENDERER | CVAR_INTEGER, "0 = Doom3 (Blinn-Phong), 1 = Phong");
idCVar r_specularExp("r_specularExp", "10", CVAR_ARCHIVE | CVAR_RENDERER | CVAR_FLOAT, "exponent used for specularity");
idCVar r_specularScale("r_specularScale", "1", CVAR_ARCHIVE | CVAR_RENDERER | CVAR_FLOAT, "scale specularity globally for all surfaces");

/*
=====================
RB_GLSL_BlendLight

=====================
*/
static void RB_GLSL_BlendLight(const viewLight_t& vLight, const drawSurf_t& surf) {
	const srfTriangles_t*  tri = surf.geo;

  if(backEnd.currentSpace != surf.space)
  {
	idPlane	lightProject[4];

	for (int i = 0; i < 4; i++) {
		R_GlobalPlaneToLocal(surf.space->modelMatrix, vLight.lightProject[i], lightProject[i]);
	}

	fhRenderProgram::SetBumpMatrix(lightProject[0].ToVec4(), lightProject[1].ToVec4());
	fhRenderProgram::SetSpecularMatrix(lightProject[2].ToVec4(), idVec4());
	fhRenderProgram::SetDiffuseMatrix(lightProject[3].ToVec4(), idVec4());
  }

  // this gets used for both blend lights and shadow draws
  if (tri->ambientCache) {
    int offset = vertexCache.Bind(tri->ambientCache);
	GL_SetupVertexAttributes(fhVertexLayout::DrawPosOnly, offset);
  }
  else if (tri->shadowCache) {
    int offset = vertexCache.Bind(tri->shadowCache);
	GL_SetupVertexAttributes(fhVertexLayout::Shadow, offset);
  }

  RB_DrawElementsWithCounters(tri);
}


/*
=====================
RB_GLSL_BlendLight

Dual texture together the falloff and projection texture with a blend
mode to the framebuffer, instead of interacting with the surface texture
=====================
*/
void RB_GLSL_BlendLight(const viewLight_t& vLight) {
  const idMaterial	*lightShader;
  const shaderStage_t	*stage;
  int					i;
  const float	*regs;

  if (!vLight.globalInteractions) {
    return;
  }
  if (r_skipBlendLights.GetBool()) {
    return;
  }
  RB_LogComment("---------- RB_GLSL_BlendLight ----------\n");

  GL_UseProgram(blendLightProgram);

  lightShader = vLight.lightShader;
  regs = vLight.shaderRegisters;

  // texture 1 will get the falloff texture
  vLight.falloffImage->Bind(1);

  for (i = 0; i < lightShader->GetNumStages(); i++) {
    stage = lightShader->GetStage(i);

    if (!regs[stage->conditionRegister]) {
      continue;
    }

    GL_State(GLS_DEPTHMASK | stage->drawStateBits | GLS_DEPTHFUNC_EQUAL);

    // texture 0 will get the projected texture
    stage->texture.image->Bind(0);

    // get the modulate values from the light, including alpha, unlike normal lights
	idVec4 lightColor;
    lightColor[0] = regs[stage->color.registers[0]];
    lightColor[1] = regs[stage->color.registers[1]];
    lightColor[2] = regs[stage->color.registers[2]];
    lightColor[3] = regs[stage->color.registers[3]];
	fhRenderProgram::SetDiffuseColor(lightColor);

    RB_RenderDrawSurfChainWithFunction(vLight, vLight.globalInteractions, RB_GLSL_BlendLight);
    RB_RenderDrawSurfChainWithFunction(vLight, vLight.localInteractions, RB_GLSL_BlendLight);
  }
}


/*
==================
R_WobbleskyTexGen
==================
*/
static void R_CreateWobbleskyTexMatrix(const drawSurf_t *surf, float time, float matrix[16]) {

  const int *parms = surf->material->GetTexGenRegisters();

  float	wobbleDegrees = surf->shaderRegisters[parms[0]];
  float	wobbleSpeed = surf->shaderRegisters[parms[1]];
  float	rotateSpeed = surf->shaderRegisters[parms[2]];

  wobbleDegrees = wobbleDegrees * idMath::PI / 180;
  wobbleSpeed = wobbleSpeed * 2 * idMath::PI / 60;
  rotateSpeed = rotateSpeed * 2 * idMath::PI / 60;

  // very ad-hoc "wobble" transform
  float	a = time * wobbleSpeed;
  float	s = sin(a) * sin(wobbleDegrees);
  float	c = cos(a) * sin(wobbleDegrees);
  float	z = cos(wobbleDegrees);

  idVec3	axis[3];

  axis[2][0] = c;
  axis[2][1] = s;
  axis[2][2] = z;

  axis[1][0] = -sin(a * 2) * sin(wobbleDegrees);
  axis[1][2] = -s * sin(wobbleDegrees);
  axis[1][1] = sqrt(1.0f - (axis[1][0] * axis[1][0] + axis[1][2] * axis[1][2]));

  // make the second vector exactly perpendicular to the first
  axis[1] -= (axis[2] * axis[1]) * axis[2];
  axis[1].Normalize();

  // construct the third with a cross
  axis[0].Cross(axis[1], axis[2]);

  // add the rotate
  s = sin(rotateSpeed * time);
  c = cos(rotateSpeed * time);

  matrix[0] = axis[0][0] * c + axis[1][0] * s;
  matrix[4] = axis[0][1] * c + axis[1][1] * s;
  matrix[8] = axis[0][2] * c + axis[1][2] * s;

  matrix[1] = axis[1][0] * c - axis[0][0] * s;
  matrix[5] = axis[1][1] * c - axis[0][1] * s;
  matrix[9] = axis[1][2] * c - axis[0][2] * s;

  matrix[2] = axis[2][0];
  matrix[6] = axis[2][1];
  matrix[10] = axis[2][2];

  matrix[3] = matrix[7] = matrix[11] = 0.0f;
  matrix[12] = matrix[13] = matrix[14] = 0.0f;
}

/*
===============
RB_RenderTriangleSurface

Sets texcoord and vertex pointers
===============
*/
static void RB_GLSL_RenderTriangleSurface(const srfTriangles_t *tri) {
  if (!tri->ambientCache) {
    RB_DrawElementsImmediate(tri);
    return;
  }

  auto offset = vertexCache.Bind(tri->ambientCache);

  GL_SetupVertexAttributes(fhVertexLayout::DrawPosOnly, offset);
  RB_DrawElementsWithCounters(tri);
}

static idPlane	fogPlanes[4];

/*
=====================
RB_T_BasicFog

=====================
*/
static void RB_GLSL_BasicFog(const viewLight_t&, const drawSurf_t& surf) {

	if(backEnd.currentSpace != surf.space)
	{
		idPlane	local;

		R_GlobalPlaneToLocal(surf.space->modelMatrix, fogPlanes[0], local);
		local[3] += 0.5;
		const idVec4 bumpMatrixS = local.ToVec4();

		local[0] = local[1] = local[2] = 0; local[3] = 0.5;

		const idVec4 bumpMatrixT = local.ToVec4();
		fhRenderProgram::SetBumpMatrix(bumpMatrixS, bumpMatrixT);

		// GL_S is constant per viewer
		R_GlobalPlaneToLocal(surf.space->modelMatrix, fogPlanes[2], local);
		local[3] += FOG_ENTER;
		const idVec4 diffuseMatrixT = local.ToVec4();

		R_GlobalPlaneToLocal(surf.space->modelMatrix, fogPlanes[3], local);
		const idVec4 diffuseMatrixS = local.ToVec4();

		fhRenderProgram::SetDiffuseMatrix(diffuseMatrixS, diffuseMatrixT);
	}

	RB_GLSL_RenderTriangleSurface(surf.geo);
}

/*
==================
RB_GLSL_FogPass
==================
*/
void RB_GLSL_FogPass(const viewLight_t& vLight) {
  assert(fogLightProgram);

  RB_LogComment("---------- RB_GLSL_FogPass ----------\n");

  GL_UseProgram(fogLightProgram);

  // create a surface for the light frustom triangles, which are oriented drawn side out
  const srfTriangles_t* frustumTris = vLight.frustumTris;

  // if we ran out of vertex cache memory, skip it
  if (!frustumTris->ambientCache) {
    return;
  }

  drawSurf_t ds;
  memset(&ds, 0, sizeof(ds));
  ds.space = &backEnd.viewDef->worldSpace;
  ds.geo = frustumTris;
  ds.scissorRect = backEnd.viewDef->scissor;

  // find the current color and density of the fog
  const idMaterial *lightShader = vLight.lightShader;
  const float	     *regs        = vLight.shaderRegisters;
  // assume fog shaders have only a single stage
  const shaderStage_t	*stage = lightShader->GetStage(0);

  idVec4 lightColor;
  lightColor[0] = regs[stage->color.registers[0]];
  lightColor[1] = regs[stage->color.registers[1]];
  lightColor[2] = regs[stage->color.registers[2]];
  lightColor[3] = regs[stage->color.registers[3]];

  fhRenderProgram::SetDiffuseColor(lightColor);

  // calculate the falloff planes
  float	a;

  // if they left the default value on, set a fog distance of 500
  if (lightColor[3] <= 1.0) {
    a = -0.5f / DEFAULT_FOG_DISTANCE;
  }
  else {
    // otherwise, distance = alpha color
    a = -0.5f / lightColor[3];
  }

  // texture 0 is the falloff image
  globalImages->fogImage->Bind(0);

  fogPlanes[0][0] = a * backEnd.viewDef->worldSpace.modelViewMatrix[2];
  fogPlanes[0][1] = a * backEnd.viewDef->worldSpace.modelViewMatrix[6];
  fogPlanes[0][2] = a * backEnd.viewDef->worldSpace.modelViewMatrix[10];
  fogPlanes[0][3] = a * backEnd.viewDef->worldSpace.modelViewMatrix[14];

  fogPlanes[1][0] = a * backEnd.viewDef->worldSpace.modelViewMatrix[0];
  fogPlanes[1][1] = a * backEnd.viewDef->worldSpace.modelViewMatrix[4];
  fogPlanes[1][2] = a * backEnd.viewDef->worldSpace.modelViewMatrix[8];
  fogPlanes[1][3] = a * backEnd.viewDef->worldSpace.modelViewMatrix[12];

  // texture 1 is the entering plane fade correction
  globalImages->fogEnterImage->Bind(1);

  // T will get a texgen for the fade plane, which is always the "top" plane on unrotated lights
  fogPlanes[2][0] = 0.001f * vLight.fogPlane[0];
  fogPlanes[2][1] = 0.001f * vLight.fogPlane[1];
  fogPlanes[2][2] = 0.001f * vLight.fogPlane[2];
  fogPlanes[2][3] = 0.001f * vLight.fogPlane[3];

  // S is based on the view origin
  float s = backEnd.viewDef->renderView.vieworg * fogPlanes[2].Normal() + fogPlanes[2][3];

  fogPlanes[3][0] = 0;
  fogPlanes[3][1] = 0;
  fogPlanes[3][2] = 0;
  fogPlanes[3][3] = FOG_ENTER + s;

  // draw it
  backEnd.glState.forceGlState = true;
  GL_State(GLS_DEPTHMASK | GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA | GLS_DEPTHFUNC_EQUAL);
  RB_RenderDrawSurfChainWithFunction(vLight, vLight.globalInteractions, RB_GLSL_BasicFog);
  RB_RenderDrawSurfChainWithFunction(vLight, vLight.localInteractions, RB_GLSL_BasicFog);

  // the light frustum bounding planes aren't in the depth buffer, so use depthfunc_less instead
  // of depthfunc_equal
  GL_State(GLS_DEPTHMASK | GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA | GLS_DEPTHFUNC_LESS);
  GL_Cull(CT_BACK_SIDED);
  RB_RenderDrawSurfChainWithFunction(vLight, &ds, RB_GLSL_BasicFog);
  GL_Cull(CT_FRONT_SIDED);
}




/*
==============================================================================

BACK END RENDERING OF STENCIL SHADOWS

==============================================================================
*/


/*
=====================
RB_GLSL_Shadow

the shadow volumes face INSIDE
=====================
*/

static void RB_GLSL_Shadow(const viewLight_t& vLight, const drawSurf_t& surf) {
  const srfTriangles_t	*tri;

  // set the light position if we are using a vertex program to project the rear surfaces
  if (surf.space != backEnd.currentSpace) {
    idVec4 localLight;

    R_GlobalPointToLocal(surf.space->modelMatrix, vLight.globalLightOrigin, localLight.ToVec3());
    localLight.w = 0.0f;

    assert(shadowProgram);
    fhRenderProgram::SetLocalLightOrigin(localLight);
    fhRenderProgram::SetProjectionMatrix(backEnd.viewDef->projectionMatrix);
    fhRenderProgram::SetModelViewMatrix(surf.space->modelViewMatrix);
  }

  tri = surf.geo;

  if (!tri->shadowCache) {
    return;
  }

  const auto offset = vertexCache.Bind(tri->shadowCache);

  GL_SetupVertexAttributes(fhVertexLayout::Shadow, offset);
  //GL_SetVertexLayout(fhVertexLayout::Shadow);
  //glVertexAttribPointer(fhRenderProgram::vertex_attrib_position_shadow, 4, GL_FLOAT, false, sizeof(shadowCache_t), attributeOffset(offset, 0));

  // we always draw the sil planes, but we may not need to draw the front or rear caps
  int	numIndexes;
  bool external = false;

  if (!r_useExternalShadows.GetInteger()) {
    numIndexes = tri->numIndexes;
  }
  else if (r_useExternalShadows.GetInteger() == 2) { // force to no caps for testing
    numIndexes = tri->numShadowIndexesNoCaps;
  }
  else if (!(surf.dsFlags & DSF_VIEW_INSIDE_SHADOW)) {
    // if we aren't inside the shadow projection, no caps are ever needed needed
    numIndexes = tri->numShadowIndexesNoCaps;
    external = true;
  }
  else if (!vLight.viewInsideLight && !(surf.geo->shadowCapPlaneBits & SHADOW_CAP_INFINITE)) {
    // if we are inside the shadow projection, but outside the light, and drawing
    // a non-infinite shadow, we can skip some caps
    if (vLight.viewSeesShadowPlaneBits & surf.geo->shadowCapPlaneBits) {
      // we can see through a rear cap, so we need to draw it, but we can skip the
      // caps on the actual surface
      numIndexes = tri->numShadowIndexesNoFrontCaps;
    }
    else {
      // we don't need to draw any caps
      numIndexes = tri->numShadowIndexesNoCaps;
    }
    external = true;
  }
  else {
    // must draw everything
    numIndexes = tri->numIndexes;
  }

  // set depth bounds
  if (glConfig.depthBoundsTestAvailable && r_useDepthBoundsTest.GetBool()) {
    glDepthBoundsEXT(surf.scissorRect.zmin, surf.scissorRect.zmax);
  }
/*
  // debug visualization
  if (r_showShadows.GetInteger()) {

    if (r_showShadows.GetInteger() == 3) {
      if (external) {
        glColor3f(0.1 / backEnd.overBright, 1 / backEnd.overBright, 0.1 / backEnd.overBright);
      }
      else {
        // these are the surfaces that require the reverse
        glColor3f(1 / backEnd.overBright, 0.1 / backEnd.overBright, 0.1 / backEnd.overBright);
      }
    }
    else {
      // draw different color for turboshadows
      if (surf->geo->shadowCapPlaneBits & SHADOW_CAP_INFINITE) {
        if (numIndexes == tri->numIndexes) {
          glColor3f(1 / backEnd.overBright, 0.1 / backEnd.overBright, 0.1 / backEnd.overBright);
        }
        else {
          glColor3f(1 / backEnd.overBright, 0.4 / backEnd.overBright, 0.1 / backEnd.overBright);
        }
      }
      else {
        if (numIndexes == tri->numIndexes) {
          glColor3f(0.1 / backEnd.overBright, 1 / backEnd.overBright, 0.1 / backEnd.overBright);
        }
        else if (numIndexes == tri->numShadowIndexesNoFrontCaps) {
          glColor3f(0.1 / backEnd.overBright, 1 / backEnd.overBright, 0.6 / backEnd.overBright);
        }
        else {
          glColor3f(0.6 / backEnd.overBright, 1 / backEnd.overBright, 0.1 / backEnd.overBright);
        }
      }
    }

    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glDisable(GL_STENCIL_TEST);
    GL_Cull(CT_TWO_SIDED);
    RB_DrawShadowElementsWithCounters(tri, numIndexes);
    GL_Cull(CT_FRONT_SIDED);
    glEnable(GL_STENCIL_TEST);
    return;
  }
*/

  // depth-fail/Z-Fail stencil shadows
  if ( !external )
  {
	  glStencilOp( GL_KEEP, GL_DECR_WRAP, GL_KEEP );
	  GL_Cull( CT_FRONT_SIDED );
	  RB_DrawShadowElementsWithCounters( tri, numIndexes );

	  glStencilOp( GL_KEEP, GL_INCR_WRAP, GL_KEEP );
	  GL_Cull( CT_BACK_SIDED );
	  RB_DrawShadowElementsWithCounters( tri, numIndexes );
  }
  // traditional depth-pass stencil shadows
  else
  {
	  glStencilOp( GL_KEEP, GL_KEEP, GL_INCR_WRAP );
	  GL_Cull( CT_FRONT_SIDED );
	  RB_DrawShadowElementsWithCounters( tri, numIndexes );

	  glStencilOp( GL_KEEP, GL_KEEP, GL_DECR_WRAP );
	  GL_Cull( CT_BACK_SIDED );
	  RB_DrawShadowElementsWithCounters( tri, numIndexes );
  }
}


/*
=====================
RB_GLSL_StencilShadowPass

Stencil test should already be enabled, and the stencil buffer should have
been set to 128 on any surfaces that might receive shadows
=====================
*/



static void RB_GLSL_StencilShadowPass(const viewLight_t& vLight, const drawSurf_t *drawSurfs) {
  assert(shadowProgram);

  if (vLight.lightDef->ShadowMode() != shadowMode_t::StencilShadow) {
    return;
  }

  if (!drawSurfs) {
    return;
  }

  GL_UseProgram(shadowProgram);

  RB_LogComment("---------- RB_StencilShadowPass ----------\n");

  // for visualizing the shadows
  if (r_showShadows.GetInteger()) {
    if (r_showShadows.GetInteger() == 2) {
      // draw filled in
      GL_State(GLS_DEPTHMASK | GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHFUNC_LESS);
    }
    else {
      // draw as lines, filling the depth buffer
      GL_State(GLS_SRCBLEND_ONE | GLS_DSTBLEND_ZERO | GLS_POLYMODE_LINE | GLS_DEPTHFUNC_ALWAYS);
    }
  }
  else {
    // don't write to the color buffer, just the stencil buffer
    GL_State(GLS_DEPTHMASK | GLS_COLORMASK | GLS_ALPHAMASK | GLS_DEPTHFUNC_LESS);
  }

  if (r_shadowPolygonFactor.GetFloat() || r_shadowPolygonOffset.GetFloat()) {
    glPolygonOffset(r_shadowPolygonFactor.GetFloat(), -r_shadowPolygonOffset.GetFloat());
    glEnable(GL_POLYGON_OFFSET_FILL);
  }

  glStencilFunc(GL_ALWAYS, 1, 255);

  if (glConfig.depthBoundsTestAvailable && r_useDepthBoundsTest.GetBool()) {
    glEnable(GL_DEPTH_BOUNDS_TEST_EXT);
  }

  RB_RenderDrawSurfChainWithFunction(vLight, drawSurfs, RB_GLSL_Shadow);

  GL_Cull(CT_FRONT_SIDED);

  if (r_shadowPolygonFactor.GetFloat() || r_shadowPolygonOffset.GetFloat()) {
    glDisable(GL_POLYGON_OFFSET_FILL);
  }

  if (glConfig.depthBoundsTestAvailable && r_useDepthBoundsTest.GetBool()) {
    glDisable(GL_DEPTH_BOUNDS_TEST_EXT);
  }

  glStencilFunc(GL_GEQUAL, 128, 255);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

/*
=====================
RB_GLSL_FillDepthBuffer

If we are rendering a subview with a near clip plane, use a second texture
to force the alpha test to fail when behind that clip plane
=====================
*/
void RB_GLSL_FillDepthBuffer(drawSurf_t **drawSurfs, int numDrawSurfs) {
	fhTimeElapsed timeElapsed( &backEnd.stats.groups[backEndGroup::DepthPrepass].time );
	backEnd.stats.groups[backEndGroup::DepthPrepass].passes += 1;

	DepthRenderList depthRenderList;
	depthRenderList.AddDrawSurfaces( drawSurfs, numDrawSurfs );
	depthRenderList.Submit();
}

/*
=================
R_ReloadGlslPrograms_f
=================
*/
void R_ReloadGlslPrograms_f( const idCmdArgs &args ) {
	fhRenderProgram::ReloadAll();
}

static idList<viewLight_t*> shadowCastingViewLights[3];
static idList<viewLight_t*> batch;

/*
==================
RB_GLSL_DrawInteractions
==================
*/
void RB_GLSL_DrawInteractions( void ) {

	InteractionList interactionList;

	batch.SetNum( 0 );
	shadowCastingViewLights[0].SetNum( 0 );
	shadowCastingViewLights[1].SetNum( 0 );
	shadowCastingViewLights[2].SetNum( 0 );

	for (viewLight_t* vLight = backEnd.viewDef->viewLights; vLight; vLight = vLight->next) {
		// do fogging later
		if (vLight->lightShader->IsFogLight()) {
			continue;
		}

		if (vLight->lightShader->IsBlendLight()) {
			continue;
		}

		if (!vLight->localInteractions && !vLight->globalInteractions && !vLight->translucentInteractions) {
			continue;
		}

		if (vLight->lightDef->ShadowMode() == shadowMode_t::ShadowMap) {
			int lod = Min( 2, Max( vLight->shadowMapLod, 0 ) );
			shadowCastingViewLights[lod].Append( vLight );
		}
		else {
			//light does not require shadow maps to be rendered. Render this light with the first batch.
			batch.Append( vLight );
		}
	}

	while(true) {
		fhFramebuffer* renderBuffer = fhFramebuffer::GetCurrentDrawBuffer();

		if (shadowCastingViewLights[0].Num() != 0 || shadowCastingViewLights[1].Num() != 0 || shadowCastingViewLights[2].Num() != 0) {
			GL_UseProgram( shadowmapProgram );
			glStencilFunc( GL_ALWAYS, 0, 255 );
			GL_State( GLS_DEPTHFUNC_LESS | GLS_SRCBLEND_ONE | GLS_DSTBLEND_ZERO );	// make sure depth mask is off before clear
			glDepthMask( GL_TRUE );
			glEnable( GL_DEPTH_TEST );

			globalImages->BindNull( 6 );
			fhFramebuffer::shadowmapFramebuffer->Bind();
			glViewport( 0, 0, fhFramebuffer::shadowmapFramebuffer->GetWidth(), fhFramebuffer::shadowmapFramebuffer->GetHeight() );
			glScissor( 0, 0, fhFramebuffer::shadowmapFramebuffer->GetWidth(), fhFramebuffer::shadowmapFramebuffer->GetHeight() );
			const float clearDepth = 1.0f;
			glClearBufferfv( GL_DEPTH, 0, &clearDepth );

			for(int lod = 0; lod < 3; ++lod) {
				idList<viewLight_t*>& lights = shadowCastingViewLights[lod];

				while (lights.Num() > 0) {
					auto light = lights.Last();

					if (RB_RenderShadowMaps(light)) {
						//shadow map was rendered successfully, so add the light to the next batch
						batch.Append(light);
						lights.RemoveLast();
					}
					else {
						break;
					}
				}
			}
		}

		renderBuffer->Bind();

		globalImages->shadowmapImage->Bind( 6 );
		globalImages->jitterImage->Bind( 7 );

		glDisable( GL_POLYGON_OFFSET_FILL );
		glEnable( GL_CULL_FACE );
		glFrontFace( GL_CCW );

		//reset viewport
		glViewport( backEnd.viewDef->viewport.x1, backEnd.viewDef->viewport.y1,
			backEnd.viewDef->viewport.x2 + 1 - backEnd.viewDef->viewport.x1,
			backEnd.viewDef->viewport.y2 + 1 - backEnd.viewDef->viewport.y1 );

		// the scissor may be smaller than the viewport for subviews
		glScissor( backEnd.viewDef->viewport.x1 + backEnd.viewDef->scissor.x1,
			backEnd.viewDef->viewport.y1 + backEnd.viewDef->scissor.y1,
			backEnd.viewDef->scissor.x2 + 1 - backEnd.viewDef->scissor.x1,
			backEnd.viewDef->scissor.y2 + 1 - backEnd.viewDef->scissor.y1 );

		backEnd.currentScissor = backEnd.viewDef->scissor;

		for(int i=0; i<batch.Num(); ++i) {
			const viewLight_t* vLight = batch[i];

			backEnd.stats.groups[backEndGroup::Interaction].passes += 1;
			fhTimeElapsed timeElapsed( &backEnd.stats.groups[backEndGroup::Interaction].time );

			const idMaterial* lightShader = vLight->lightShader;

			// clear the stencil buffer if needed
			if (vLight->globalShadows || vLight->localShadows) {
				backEnd.currentScissor = vLight->scissorRect;
				if (r_useScissor.GetBool()) {
					glScissor( backEnd.viewDef->viewport.x1 + backEnd.currentScissor.x1,
						backEnd.viewDef->viewport.y1 + backEnd.currentScissor.y1,
						backEnd.currentScissor.x2 + 1 - backEnd.currentScissor.x1,
						backEnd.currentScissor.y2 + 1 - backEnd.currentScissor.y1 );
				}
				int clearStencil = 1 << (glConfig.stencilBits - 1);
				glClearBufferiv( GL_STENCIL, 0, &clearStencil );
			}
			else {
				// no shadows, so no need to read or write the stencil buffer
				// we might in theory want to use GL_ALWAYS instead of disabling
				// completely, to satisfy the invarience rules
				glStencilFunc( GL_ALWAYS, 128, 255 );
			}

			RB_GLSL_StencilShadowPass( *vLight, vLight->globalShadows );
			interactionList.Clear();
			interactionList.AddDrawSurfacesOnLight( *vLight, vLight->localInteractions );
			interactionList.Submit(*vLight);

			RB_GLSL_StencilShadowPass(*vLight, vLight->localShadows );
			interactionList.Clear();
			interactionList.AddDrawSurfacesOnLight( *vLight, vLight->globalInteractions );
			interactionList.Submit(*vLight);

			// translucent surfaces never get stencil shadowed
			if (r_skipTranslucent.GetBool()) {
				continue;
			}

			glStencilFunc( GL_ALWAYS, 128, 255 );

			backEnd.depthFunc = GLS_DEPTHFUNC_LESS;

			interactionList.Clear();
			interactionList.AddDrawSurfacesOnLight( *vLight, vLight->translucentInteractions );
			interactionList.Submit(*vLight);

			backEnd.depthFunc = GLS_DEPTHFUNC_EQUAL;
		}

		RB_FreeAllShadowMaps();
		batch.SetNum(0);

		if (shadowCastingViewLights[0].Num() == 0 && shadowCastingViewLights[1].Num() == 0 && shadowCastingViewLights[2].Num() == 0) {
			break;
		}
	}

	glStencilFunc( GL_ALWAYS, 128, 255 );
}