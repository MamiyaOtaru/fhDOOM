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
#include "shading.inc"

layout(binding = 1) uniform sampler2D normalMap;
layout(binding = 2) uniform sampler2D lightFalloff;
layout(binding = 3) uniform sampler2D lightTexture;
layout(binding = 4) uniform sampler2D diffuseMap;
layout(binding = 5) uniform sampler2D specularMap;

in vs_output
{
  vec4 color;
  vec2 texNormal;
  vec2 texDiffuse;
  vec2 texSpecular;
  vec4 texLight;
  vec3 L;
  vec3 V;
  vec3 H;
  vec4 shadow[6];
  vec3 toLightOrigin; 
  float depth; 
  vec4 worldspacePosition;
} frag;

#include "shadows.inc"

out vec4 result;

vec4 diffuse(vec2 texcoord, vec3 N, vec3 L) 
{
  return texture(diffuseMap, texcoord) * rpDiffuseColor * lambert(N, L);
}

vec4 specular(vec2 texcoord, vec3 N, vec3 L, vec3 V)
{
  vec4 spec = texture(specularMap, texcoord) * rpSpecularColor;
  if(rpShading == 1) {
    spec *= phong(N, L, V, rpSpecularExp);
  } else {
    vec3 H = normalize(frag.H);
    spec *= blinn(N, H, rpSpecularExp);
  }

  return spec;
}

vec3 normal(vec2 texcoord)
{
#if 0
  vec3 N = normalize(2.0 * texture(normalMap, texcoord).rgb - 1.0);
#else
  const int NORMAL_RGB = 0;
  const int NORMAL_RxGB = 1;
  const int NORMAL_AG = 2;
  const int NORMAL_RG = 3;

  vec3 N;

  if(rpNormalMapEncoding == NORMAL_RxGB)
  {
    N = normalize(2.0 * texture(normalMap, texcoord).rgb - 1.0);
  }
  else if(rpNormalMapEncoding == NORMAL_AG)
  {
    N = 2.0 * texture(normalMap, texcoord).agb - 1.0;  
    N.z = sqrt(1.0 - N.x * N.x - N.y * N.y);
  }
  else if(rpNormalMapEncoding == NORMAL_RG)
  {
    N = 2.0 * texture(normalMap, texcoord).rgb - 1.0;  
    N.z = sqrt(1.0 - N.x * N.x - N.y * N.y);  
  }  
  else /*if(rpNormalMapEncoding == NORMAL_RGB)*/
  {
    N = normalize(2.0 * texture(normalMap, texcoord).rgb - 1.0);
  }

#endif
  return N;
}

vec4 shadow()
{
  vec4 shadowness = vec4(1,1,1,1);

  if(rpShadowMappingMode == 1)  
  {
#if 0
    float softness = (0.003 * rpShadowParams.x);
#else
    float lightDistance = length(frag.toLightOrigin); 
    float softness = (0.003 * rpShadowParams.x) * (1 - ( lightDistance * 4.0 ) / rpShadowParams.w ); 
#endif      
    shadowness = pointlightShadow(frag.shadow, frag.toLightOrigin, vec2(softness, softness));  
  }
  else if(rpShadowMappingMode == 2)
  {
    float softness = (0.007 * rpShadowParams.x);
    shadowness = projectedShadow(frag.shadow[0], vec2(softness, softness)); 
  }
  else if(rpShadowMappingMode == 3)
  {
    shadowness = parallelShadow(frag.shadow, -frag.depth, vec2(rpShadowParams.x * 0.0095, rpShadowParams.x * 0.0095));     
  }
 
  return shadowness;
}

void main(void)
{  
  vec3 V = normalize(frag.V);
  vec3 L = normalize(frag.L);  
  vec2 offset = parallaxOffset(specularMap, frag.texSpecular.st, V);      
  vec3 N = normal(frag.texNormal + offset);
 

  result = vec4(0,0,0,0);

  result += diffuse(frag.texDiffuse + offset, N, L);    
  result += specular(frag.texSpecular + offset, N, L, V);

  result *= frag.color;
  result *= texture2DProj(lightTexture, frag.texLight.xyw);
  result *= texture2D(lightFalloff, vec2(frag.texLight.z, 0.5));
  result *= shadow();
}
