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

layout(binding = 0) uniform sampler2D texture0;
layout(binding = 1) uniform sampler2D texture1;
layout(binding = 2) uniform sampler2D texture2;

in vs_output
{
  vec4 color;
  vec2 texcoord;
  vec4 texcoord1;
  vec4 texcoord2;  
} frag;

out vec4 result;

vec2 fixScreenTexCoord(vec2 st)
{
  float x = rpCurrentRenderSize.z / rpCurrentRenderSize.x;
  float y = rpCurrentRenderSize.w / rpCurrentRenderSize.y;
  return st * vec2(x, y);  
}

float linearDepth()
{
  float ndcDepth = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);
  return ndcDepth / gl_FragCoord.w;
}

vec4 mad_sat(vec4 a, vec4 b, vec4 c)
{
  return clamp(a * b + c, 0, 1);
}

void main(void)
{
  // load the distortion map
  vec4 mask = texture2D(texture2, frag.texcoord);
  mask *=  frag.color;
  // kill the pixel if the distortion wound up being very small
  
  mask.xy -= 0.01f;
  if(mask.x < 0 || mask.y < 0)
    discard;

  // load the filtered normal map and convert to -1 to 1 range
  vec4 localNormal = texture2D( texture1, frag.texcoord1.xy );
  //localNormal.x = localNormal.a;
  localNormal = localNormal * 2 - 1;
  localNormal = localNormal * mask;

  // calculate the screen texcoord in the 0.0 to 1.0 range
  vec4 screenTexCoord = vec4(gl_FragCoord.x/rpCurrentRenderSize.z, gl_FragCoord.y/rpCurrentRenderSize.w, 0, 0); //vposToScreenPosTexCoord( fragment.position.xy );

  screenTexCoord = mad_sat(localNormal, frag.texcoord2, screenTexCoord);
/*
  screenTexCoord += ( localNormal * frag.texcoord2.xy );
  screenTexCoord = clamp(screenTexCoord, 0, 1);
*/
  result = texture2D( texture0, fixScreenTexCoord(screenTexCoord.xy));  
  
}