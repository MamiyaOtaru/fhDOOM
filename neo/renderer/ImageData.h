/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 2016 Johannes Ohlemacher

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

#pragma once

enum class PixelFormat {
	None,
	RGBA,
	BGRA,
	RGB,
	BGR,
	DXT1_RGB,
	DXT1_RGBA,
	DXT3_RGBA,
	DXT5_RGBA
};

template<typename T>
class fhStaticBuffer {
public:
	fhStaticBuffer()
	 : buffer(nullptr)
	 , num(0) {
	}

	explicit fhStaticBuffer(uint num) {
		this->buffer = (T*)R_StaticAlloc(sizeof(T)*num);
		this->num = num;
	}

	~fhStaticBuffer() {
		Free();
	}

	const T& operator[](uint32 index) const {
		assert(buffer);
		return buffer[index];
	}

	T& operator[](uint32 index) {
		assert(buffer);
		return buffer[index];
	}

	const T* Get() const {
		return buffer;
	}

	T* Get() {
		return buffer;
	}

	void Allocate(uint num) {
		assert(!buffer);
		this->buffer = (T*)R_StaticAlloc(sizeof(T)*num);
		this->num = num;
	}

	void Free() {
		if (buffer) {
			R_StaticFree(buffer);
			buffer = nullptr;
			num = 0;
		}
	}

	T* Release() {
		T* ret = buffer;
		buffer = nullptr;
		num = 0;
		return ret;
	}

	uint32 Num() const {
		return num;
	}

private:
	uint32 num;
	T* buffer;
};

class fhImageData {
public:
	            fhImageData();
	            ~fhImageData();

	bool        LoadFile(const char* filename, bool toRgba = false);
	bool        LoadDDS(const char* filename);
	bool        LoadTGA(const char* filename, bool toRgba = false);	
	bool        LoadProgram(const char* program);

	uint32      GetSize(uint32 level = 0) const;
	uint32      GetWidth(uint32 level = 0) const;
	uint32      GetHeight(uint32 level = 0) const;
	uint32      GetNumFaces() const;
	uint32      GetNumLevels() const;
	PixelFormat GetPixelFormat() const;

	const byte* GetData(uint32 face = 0, uint32 level = 0) const;
	ID_TIME_T   GetTimeStamp() const;
	const char* GetName() const;

	bool        IsValid() const;
	void        Clear();

private:
	bool        IsRGBA() const;
	byte*       GetCanonicalData();

	bool        LoadFileIntoBuffer(const char* filename, fhStaticBuffer<byte>& buffer);
	bool        LoadTGA(fhStaticBuffer<byte>& buffer, bool toRgba);
	bool        LoadDDS(fhStaticBuffer<byte>& buffer);

	bool        ParseImageProgram_r(idLexer& src, bool noload, bool toRgba);

	struct level_t {
		uint32 width;
		uint32 height;
		uint32 size; //size in bytes
		uint32 offset;
	};

	struct face_t {
		level_t levels[16];
	};

	uint32      numFaces;
	uint32      numLevels;
	face_t      faces[6];
	PixelFormat format;
	char        name[256];
	ID_TIME_T   timestamp;
	byte*       data;
};

ID_INLINE bool fhImageData::IsRGBA() const {
	return GetPixelFormat() == PixelFormat::RGBA;
}