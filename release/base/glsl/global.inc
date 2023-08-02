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

#version 330
#extension GL_ARB_shading_language_420pack: enable

uniform mat4 rpModelMatrix;
uniform mat4 rpViewMatrix;
uniform mat4 rpModelViewMatrix;
uniform mat4 rpProjectionMatrix;

uniform vec4 rpLocalLightOrigin;
uniform vec4 rpLocalViewOrigin;
uniform vec4 rpLightProjectionS;
uniform vec4 rpLightProjectionT;
uniform vec4 rpLightProjectionQ;
uniform vec4 rpLightFallOff;

uniform vec4 rpBumpMatrixS;
uniform vec4 rpBumpMatrixT;
uniform vec4 rpDiffuseMatrixS;
uniform vec4 rpDiffuseMatrixT;
uniform vec4 rpSpecularMatrixS;
uniform vec4 rpSpecularMatrixT;

uniform vec4 rpColorModulate;
uniform vec4 rpColorAdd;
uniform vec4 rpDiffuseColor;
uniform vec4 rpSpecularColor;

uniform vec4 shaderParm0;
uniform vec4 shaderParm1;
uniform vec4 shaderParm2;
uniform vec4 shaderParm3;

uniform mat4 textureMatrix0;

uniform int rpAlphaTestEnabled;
uniform float rpAlphaTestThreshold;
uniform vec4 rpCurrentRenderSize;

uniform vec2 rpClipRange;
uniform int rpDepthBlendMode;
uniform float rpDepthBlendRange;
uniform float rpPomMaxHeight;
uniform int rpShading;
uniform float rpSpecularExp;

uniform int rpShadowMappingMode;
uniform int rpNormalMapEncoding;
uniform mat4 rpSpotlightProjection;
uniform mat4 rpPointlightProjection[6];
uniform int rpAmbientLight;

uniform vec4 rpGlobalLightOrigin;
uniform vec4 rpShadowParams; //x=minBias, y=maxbias, z=fuzzyness, w=samples
uniform vec4 rpShadowCoords[6];
uniform vec4 rpShadowMapSize[6];  //xy = width/height of far plane, zw = near/far clip distance
                                  //TODO(johl): is width/height correctly set for non-parallel lights? 
uniform float rpCascadeDistances[5];
uniform mat4 rpInverseLightRotation;


vec3 toOpenGlCorrdinates(vec3 v) {
	return vec3( -v.y, v.z, -v.x );
}