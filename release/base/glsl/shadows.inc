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

layout(binding = 6) uniform sampler2DShadow shadowMap;

float isOccluded(sampler2DShadow map, vec2 coord, vec2 offset, float ref)
{
  float f = texture(map, vec3(coord + offset, ref));
  return 1 - f;
}

float offsetLookup(sampler2DShadow map, vec4 pos, vec2 offset, vec2 texsize) {
    float f = texture(map, vec3(
        pos.x + offset.x * texsize.x * pos.w, 
        pos.z,
        pos.w)); 
    return f;
}

#define PCF_NUM_SAMPLES 16

const vec2 poissonDisk[ 16 ] = vec2[](
	vec2( -0.94201624, -0.39906216 ), 
	vec2( 0.94558609, -0.76890725 ), 
	vec2( -0.094184101, -0.92938870 ), 
	vec2( 0.34495938, 0.29387760 ), 
	vec2( -0.91588581, 0.45771432 ), 
	vec2( -0.81544232, -0.87912464 ), 
	vec2( -0.38277543, 0.27676845 ), 
	vec2( 0.97484398, 0.75648379 ), 
	vec2( 0.44323325, -0.97511554 ), 
	vec2( 0.53742981, -0.47373420 ), 
	vec2( -0.26496911, -0.41893023 ),
	vec2( 0.79197514, 0.19090188 ), 
	vec2( -0.24188840, 0.99706507 ), 
	vec2( -0.81409955, 0.91437590 ), 
	vec2( 0.19984126, 0.78641367 ), 
	vec2( 0.14383161, -0.14100790 )
);

float getShadow(vec4 pos, vec2 coordScale, vec2 coordOffset, vec2 softness) {   
    pos = pos / pos.w;

    pos.x = pos.x / 2.0 + 0.5;
    pos.y = pos.y / 2.0 + 0.5;
    pos.z = pos.z / 2.0 + 0.5;   

    pos.x *= coordScale.x; 
    pos.y *= coordScale.y; 

    pos.x += coordOffset.x;
    pos.y += coordOffset.y;

    vec2 texsize = coordScale * softness;
    float sum = 0.0;

#if 0
	for ( int i = 0; i < PCF_NUM_SAMPLES; ++i ) {
		vec2 offset = poissonDisk[ i ];
		sum += isOccluded(shadowMap, pos.st, offset * texsize, pos.z);
	}

  return sum / PCF_NUM_SAMPLES;
#else
  float numSamples = 0.0;
  for ( float y = -1.0; y < 1.0; y += 0.2 ) {
    for ( float x = -1.0; x < 1.0; x += 0.2 ) {
      vec2 offset = vec2( x, y );
      sum += isOccluded(shadowMap, pos.st, offset * texsize, pos.z);
      numSamples++;
    }
    numSamples++;
  }
#endif
	
    return sum / numSamples;
}

vec4 projectedShadow(vec4 pos, vec2 softness) {
    float f = getShadow(pos, rpShadowCoords[0].xy, rpShadowCoords[0].zw, vec2(0, 0));
    f = mix(1, rpShadowParams.y, f);
    return vec4(f,f,f,f);
}

vec4 getParallelShadow(vec4 pos[6], int index, vec2 softness)
{
  float f = getShadow(pos[index], rpShadowCoords[index].xy, rpShadowCoords[index].zw, softness);

  float ret = mix(1, rpShadowParams.y, f);

#if 0
  vec4 debugColors[6] = vec4[](
    vec4(1,1,1,1),
    vec4(1,0,0,1),
    vec4(1,1,0,1),
    vec4(0,1,0,1),
    vec4(0,1,1,1),
    vec4(0,0,1,1)
  );
  return debugColors[index] * ret;
#else
  return vec4(ret, ret, ret, 1);
#endif    
}

vec4 parallelShadow(vec4 pos[6], float distance, vec2 softness)
{ 
  const float referenceSize = 256;  

  int i=0;

  vec4 nextCascade = vec4(1,1,1,1);
  float nextfraction = 0; 

  for(; i<5; ++i)
  {
    float t = 10 * (i+1);//rpCascadeDistances[i+1] * 0.1;

    if(distance < (rpCascadeDistances[i] - t))
    {
      break;
    }

    if(distance < (rpCascadeDistances[i] + t))
    {
      vec2 shadowmapSize = rpShadowMapSize[i+1].zw;
      nextCascade = getParallelShadow(pos, i+1, softness * vec2(referenceSize / shadowmapSize.x, referenceSize / shadowmapSize.y));
      nextfraction = (distance - (rpCascadeDistances[i] - t)) / (2*t);
      break;
    }    
  }

  vec2 shadowmapSize = rpShadowMapSize[i].zw;

  softness = softness * vec2(referenceSize / shadowmapSize.x, referenceSize / shadowmapSize.y);
  return mix(getParallelShadow(pos, i, softness), nextCascade, nextfraction);
}

vec4 pointlightShadow(vec4 pos[6], vec3 toLightOrigin, vec2 softness)
{  
  vec3 d = toLightOrigin;

  int side = 0;
  float l = d.x;


  if( d.y > l ) {
    side = 2;
    l = d.y;
  }
  if( d.z > l ) {
    side = 4;
    l = d.z;
  }
  if( -d.x > l ) {
    side = 1;
    l = -d.x;
  }
  if( -d.y > l ) {
    side = 3;
    l = -d.y;
  }
  if( -d.z > l ) {
    side = 5;
    l = -d.z;
  }   

  float f = getShadow(pos[side], rpShadowCoords[side].xy, rpShadowCoords[side].zw, softness);  

  f = mix(1, rpShadowParams.y, f);

  return vec4(f,f,f,f);   
}