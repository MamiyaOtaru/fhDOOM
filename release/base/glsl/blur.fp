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

in vs_output
{
  vec2 texcoord;
} frag;

out vec4 result;

void main(void)
{
  vec2 stepSize = shaderParm0.xy;
  float d = 2.3;

  vec4 color = vec4(0,0,0,0);

  float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

  color += texture2D(texture1, frag.texcoord) * weight[0];

  for (int i=1; i<5; ++i) {
    color += texture2D(texture1, frag.texcoord + stepSize * i * d) * weight[i];
    color += texture2D(texture1, frag.texcoord + stepSize * -i * d) * weight[i];
  }

  result = texture2D(texture1, frag.texcoord) * 0.05 + vec4(color.rgb, 1);
}