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

#include "global.inc"

layout(binding = 1) uniform sampler2D texture1;
layout(binding = 2) uniform sampler2D texture7;

in vs_output
{
  vec2 texcoord;
  vec3 normal;
  vec3 binormal;
  vec3 tangent;
  vec4 color;
  float depth;
} frag;

float LinearizeDepth(float z) 
{ 
  float n = rpClipRange[0];    // camera z near 
  float f = rpClipRange[1]; // camera z far 
  return (2.0 * n) / (f + n - z * (f - n));
} 


float WorldDepth()
{
  vec2 depthTextureSize = textureSize(texture7, 0);
  vec2 texcoord = gl_FragCoord.xy / depthTextureSize;

  return texture2D(texture7, texcoord).x;
}

float ColorAlphaZero()
{
  float dist = abs( LinearizeDepth(WorldDepth()) - LinearizeDepth(gl_FragCoord.z) ) * rpClipRange[1];
  return clamp(dist / rpDepthBlendRange, 0.0, 1.0);
}

float ColorAlphaOne()
{  
  return 1.0 - ColorAlphaZero();
}

out vec4 result;

void main(void)
{  
  result = texture2D(texture1, frag.texcoord) *  frag.color;// * clamp(rpDiffuseColor, vec4(0,0,0,0), vec4(1,1,1,1));

  switch(rpDepthBlendMode)
  {
  case 0:
  case 1:
  case 2:
    break;
  case 3:
    result *= ColorAlphaZero();
    break;
  case 4:
    result *= ColorAlphaOne();    
    break;
  case 5:
    result.a *= ColorAlphaOne();        
    break;    
  case 6:
    result.a *= ColorAlphaZero();        
    break;        
  default:
    result = vec4(1,0,0,1);
    break;
  }
}
