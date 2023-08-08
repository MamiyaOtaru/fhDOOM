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

#include "ImageData.h"
#include "ImageProgram.h"
#include "../framework/FileSystem.h"
#include "../framework/File.h"
#include "tr_local.h"

// DG: replace libjpeg with stb_image.h because it causes fewer headaches
#include "stb_image.h"

namespace {
	//The dwCaps2 member of the DDSCAPS2 structure can be set to one or more of the following values.Flag	Value
	const uint32 DDSCAPS2_CUBEMAP = 0x00000200;
	const uint32 DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400;
	const uint32 DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800;
	const uint32 DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000;
	const uint32 DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000;
	const uint32 DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000;
	const uint32 DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000;
	const uint32 DDSCAPS2_VOLUME = 0x00200000;


	uint32 DataSize( uint32 width, uint32 height, pixelFormat_t format ) {
		switch (format)	{
		case pixelFormat_t::BGR:
		case pixelFormat_t::RGB:
			return width * height * 3;
		case pixelFormat_t::RGBA:
		case pixelFormat_t::BGRA:
			return width * height * 4;
		case pixelFormat_t::DXT1_RGBA:
		case pixelFormat_t::DXT1_RGB:
			return ((width + 3) / 4) * ((height + 3) / 4) * 8;
		case pixelFormat_t::DXT3_RGBA:
		case pixelFormat_t::DXT5_RGBA:
		case pixelFormat_t::DXT5_RxGB:
		case pixelFormat_t::RGTC:
			return ((width + 3) / 4) * ((height + 3) / 4) * 16;
		default:
			assert( false && "unknown pixel format!?" );
		}

		return 0;
	}

}

bool fhImageData::TryLoadFile( const char* filename, const char* ext, fhImageData* imageData, ID_TIME_T* timestamp, bool (fhImageData::*f)(fhStaticBuffer<byte>&, bool) ) {

	idStr filenameExt = filename;

	if (ext) {
		filenameExt.SetFileExtension( ext );
	}

	fhFileHandle file = fileSystem->OpenFileRead( filenameExt );
	if (!file) {
		return false;
	}

	int	len = file->Length();

	if (timestamp) {
		*timestamp = file->Timestamp();
	}

	if (imageData) {
		fhStaticBuffer<byte> buffer;
		buffer.Allocate( len );
		file->Read( buffer.Get(), len );

		bool ret = ((*imageData).*f)(buffer, false);

		if (ret){
			idStr::Copynz( imageData->name, filenameExt.c_str(), MAX_IMAGE_NAME );
			imageData->timestamp = file->Timestamp();
		}

		return ret;
	}

	return true;
}


bool fhImageData::LoadFile(const char* filename, fhImageData* imageData, bool forceRgba, ID_TIME_T* timestamp) {
	// JW: jpg *should* only be in !forceRGBA - shouldn't use jpg for normal maps but D3HDP does
	if (TryLoadFile( filename, "jpg", imageData, timestamp, &fhImageData::LoadJPG )){
		return true;
	}
	
	if (!forceRgba) {
		if (TryLoadFile(filename, "dds", imageData, timestamp, &fhImageData::LoadDDS)){
			return true;
		}
	}

	if (TryLoadFile( filename, "tga", imageData, timestamp, &fhImageData::LoadTGA )){
		return true;
	}

	return false;
}

fhImageData::fhImageData()
	: data(nullptr)
	, format(pixelFormat_t::None)
	, numFaces(0)
	, numLevels(0)
	, timestamp(0) {
	name[0] = '\0';
}

fhImageData::fhImageData(fhImageData&& other)
	: data( other.data )
	, format( other.format )
	, numFaces( other.numFaces )
	, numLevels( other.numLevels )
	, timestamp( other.timestamp ) {

	memcpy( &this->faces, &other.faces, sizeof( this->faces ) );

	other.data = nullptr;
	other.format = pixelFormat_t::None;
	other.numFaces = 0;
	other.numLevels = 0;
	other.timestamp = 0;
}

fhImageData::~fhImageData() {
	Clear();
}

fhImageData& fhImageData::operator=(fhImageData&& other) {
	Clear();

	this->data = other.data;
	this->format = other.format;
	this->numFaces = other.numFaces;
	this->numLevels = other.numLevels;
	this->timestamp = other.timestamp;

	memcpy( &this->faces, &other.faces, sizeof( this->faces ) );

	other.data = nullptr;
	other.format = pixelFormat_t::None;
	other.numFaces = 0;
	other.numLevels = 0;
	other.timestamp = 0;

	return *this;
}

void fhImageData::Clear() {
	if (data) {
		R_StaticFree(data);
		data = nullptr;
	}

	format = pixelFormat_t::None;
	numFaces = 0;
	numLevels = 0;
	timestamp = 0;

	assert(!IsValid());
}

bool fhImageData::IsValid() const {
	if (!data)
		return false;

	if (numFaces == 0)
		return false;

	if (numLevels == 0)
		return false;

	if (format == pixelFormat_t::None)
		return false;

	return true;
}

bool fhImageData::LoadRgbaFromMemory( const byte* pic, uint32 width, uint32 height ) {
	Clear();

	const int numBytes = width * height * 4;

	this->format = pixelFormat_t::RGBA;
	this->numFaces = 1;
	this->numLevels = 1;
	this->data = (byte*)R_StaticAlloc( numBytes );
	memcpy( this->data, pic, numBytes );

	auto& level = this->faces[0].levels[0];
	level.height = height;
	level.width = width;
	level.size = numBytes;
	level.offset = 0;

	return true;
}

bool fhImageData::LoadFile(const char* filename, bool toRgba /* = false */) {
	idStr name = filename;
	name.DefaultFileExtension(".tga");

	if (name.Length()<5) {
		return false;
	}

	name.ToLower();
	idStr ext;
	name.ExtractFileExtension(ext);

	bool ok = false;
	if (ext == "tga") {
		ok = LoadTGA(name.c_str(), toRgba);            // try tga first
		if (!ok) {
			name.StripFileExtension();
			name.DefaultFileExtension( ".jpg" );
			ok = LoadJPG( name.c_str(), toRgba );
		}
	}
	else if (ext == "dds") {
		if (toRgba) {
			common->Warning("Cannot convert compressed data to RGBA");
			return false;
		}
		ok = LoadDDS(name.c_str());
	}
	else if (ext == "pcx") {
		assert( false && "pcx loading not implemented yet" );
		ok = false;
		//ok = LoadPCX32( name.c_str(), pic, width, height, timestamp );
	}
	else if (ext == "bmp") {
		assert( false && "bmp loading not implemented yet" );
		ok = false;
		//ok = LoadBMP( name.c_str(), pic, width, height, timestamp );
	}
	else if (ext == "jpg") {
		ok = LoadJPG( name.c_str(), toRgba );
	}

	//
	// convert to exact power of 2 sizes
	//
	if (ok && this->data && this->numFaces == 1 && this->numLevels == 1 && this->format == pixelFormat_t::RGBA) {

		int	scaled_width, scaled_height;

		int w = GetWidth();
		int h = GetHeight();

		for (scaled_width = 1; scaled_width < w; scaled_width <<= 1)
			;
		for (scaled_height = 1; scaled_height < h; scaled_height <<= 1)
			;

		if (scaled_width != w || scaled_height != h) {
			if (globalImages->image_roundDown.GetBool() && scaled_width > w) {
				scaled_width >>= 1;
			}
			if (globalImages->image_roundDown.GetBool() && scaled_height > h) {
				scaled_height >>= 1;
			}

			byte* resampledBuffer = R_ResampleTexture(GetData(), w, h, scaled_width, scaled_height);
			R_StaticFree(this->data);
			this->data = resampledBuffer;
			this->faces[0].levels[0].width = scaled_width;
			this->faces[0].levels[0].height = scaled_height;
			this->faces[0].levels[0].offset = 0;
			this->faces[0].levels[0].size = scaled_width * scaled_height * 4;
		}
	}

	return ok;
}

bool fhImageData::LoadDDS( const char* filename, bool toRgba ) {
	fhStaticBuffer<byte> buffer;
	if (LoadFileIntoBuffer(filename, buffer)) {
		return LoadDDS(buffer, toRgba);
	}

	return false;
}

bool fhImageData::LoadTGA(const char* filename, bool toRgba) {
	fhStaticBuffer<byte> buffer;
	if (LoadFileIntoBuffer(filename, buffer)) {
		return LoadTGA(buffer, toRgba);
	}

	return false;
}

bool fhImageData::LoadJPG(const char* filename, bool toRgba) {
	fhStaticBuffer<byte> buffer;
	if (LoadFileIntoBuffer(filename, buffer)) {
		return LoadJPG(buffer, toRgba);
	}

	return false;
}

bool fhImageData::LoadDDS(fhStaticBuffer<byte>& buffer, bool toRgba) {

	if (toRgba) {
		//TODO(johl): currently we just assume, that data from dds files can not be
		//            converted to RGBA.
		return false;
	}

	if (buffer.Num() < sizeof(ddsFileHeader_t)) {
		return false;
	}

	data = buffer.Release();

	unsigned long magic = LittleLong(*(unsigned long *)data);
	ddsFileHeader_t	*header = (ddsFileHeader_t *)(data + 4);

	// ( not byte swapping dwReserved1 dwReserved2 )
	header->dwSize = LittleLong(header->dwSize);
	header->dwFlags = LittleLong(header->dwFlags);
	header->dwHeight = LittleLong(header->dwHeight);
	header->dwWidth = LittleLong(header->dwWidth);
	header->dwPitchOrLinearSize = LittleLong(header->dwPitchOrLinearSize);
	header->dwDepth = LittleLong(header->dwDepth);
	header->dwMipMapCount = LittleLong(header->dwMipMapCount);
	header->dwCaps1 = LittleLong(header->dwCaps1);
	header->dwCaps2 = LittleLong(header->dwCaps2);

	header->ddspf.dwSize = LittleLong(header->ddspf.dwSize);
	header->ddspf.dwFlags = LittleLong(header->ddspf.dwFlags);
	header->ddspf.dwFourCC = LittleLong(header->ddspf.dwFourCC);
	header->ddspf.dwRGBBitCount = LittleLong(header->ddspf.dwRGBBitCount);
	header->ddspf.dwRBitMask = LittleLong(header->ddspf.dwRBitMask);
	header->ddspf.dwGBitMask = LittleLong(header->ddspf.dwGBitMask);
	header->ddspf.dwBBitMask = LittleLong(header->ddspf.dwBBitMask);
	header->ddspf.dwABitMask = LittleLong(header->ddspf.dwABitMask);

	const char* fourcc = (const char*)&header->ddspf.dwFourCC;

	if (header->ddspf.dwFlags & DDSF_FOURCC) {
		switch (header->ddspf.dwFourCC) {
		case DDS_MAKEFOURCC('D', 'X', 'T', '1'):
			if (header->ddspf.dwFlags & DDSF_ALPHAPIXELS) {
				format = pixelFormat_t::DXT1_RGBA;
			}
			else {
				format = pixelFormat_t::DXT1_RGB;
			}
			break;
		case DDS_MAKEFOURCC('D', 'X', 'T', '3'):
			format = pixelFormat_t::DXT3_RGBA;
			break;
		case DDS_MAKEFOURCC('D', 'X', 'T', '5'):
			format = pixelFormat_t::DXT5_RGBA;
			break;
		case DDS_MAKEFOURCC( 'A', 'T', 'I', '2' ):
			format = pixelFormat_t::RGTC;
			break;
		case DDS_MAKEFOURCC('R', 'X', 'G', 'B'):
			format = pixelFormat_t::DXT5_RxGB;
			break;
		default:
			common->Warning("Invalid compressed internal format\n");
			return false;
		}
	}
	else if ((header->ddspf.dwFlags & DDSF_RGBA) && header->ddspf.dwRGBBitCount == 32) {
		format = pixelFormat_t::BGRA;
	}
	else if ((header->ddspf.dwFlags & DDSF_RGB) && header->ddspf.dwRGBBitCount == 32) {
		format = pixelFormat_t::BGRA;
	}
	else if ((header->ddspf.dwFlags & DDSF_RGB) && header->ddspf.dwRGBBitCount == 24) {
		if (header->ddspf.dwFlags & DDSF_ID_INDEXCOLOR) {
			common->Warning( "Invalid uncompressed internal format\n" );
			return false;
		}
		else {
			format = pixelFormat_t::BGR;
		}
	}
	else if (header->ddspf.dwRGBBitCount == 8) {
		assert( false && "not supported" );
		common->Warning( "Invalid uncompressed internal format\n" );
		return false;
	}
	else {
		common->Warning( "Invalid uncompressed internal format\n" );
		return false;
	}

	if (header->dwCaps2 & DDSCAPS2_CUBEMAP) {
		const char* missingFace = nullptr;
		if (!(header->dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEX))
			missingFace = "positiv x";
		else if (!(header->dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEX))
			missingFace = "negative x";
		else if (!(header->dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEY))
			missingFace = "positiv y";
		else if (!(header->dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEY))
			missingFace = "negative y";
		else if (!(header->dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEZ))
			missingFace = "positiv z";
		else if (!(header->dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEZ))
			missingFace = "negative z";

		if (missingFace) {
			common->Warning( "not a valid cube map, face '%s' is missing", missingFace );
			return false;
		}

		this->numFaces = 6;
	}
	else {
		this->numFaces = 1;
	}

	this->numLevels = header->dwMipMapCount;

	byte *imagedata = this->data + sizeof(ddsFileHeader_t) + 4;

	for (uint32 j = 0; j < this->numFaces; ++j) {
		int uw = header->dwWidth;
		int uh = header->dwHeight;

		for (uint32 i = 0; i < numLevels; i++) {
			uint32 numBytes = DataSize( uw, uh, format );

			uint32 faceIndex = j;// ddsFace2doomFace[j];

			faces[faceIndex].levels[i].offset = static_cast<uint32>((uintptr_t)imagedata - (uintptr_t)this->data);
			faces[faceIndex].levels[i].width = uw;
			faces[faceIndex].levels[i].height = uh;
			faces[faceIndex].levels[i].size = numBytes;

			imagedata += numBytes;
			uw /= 2;
			uh /= 2;
			if (uw < 1) {
				uw = 1;
			}
			if (uh < 1) {
				uh = 1;
			}
		}
	}

	return true;
}

bool fhImageData::LoadTGA(fhStaticBuffer<byte>& buffer, bool toRgba) {

	struct TargaHeader {
		unsigned char 	id_length, colormap_type, image_type;
		unsigned short	colormap_index, colormap_length;
		unsigned char	colormap_size;
		unsigned short	x_origin, y_origin, width, height;
		unsigned char	pixel_size, attributes;
	};

	if (buffer.Num() < sizeof(TargaHeader)) {
		return false;
	}

	int numBytes = 0;
	int	columns = 0;
	int rows = 0;
	int numPixels = 0;
	byte* buf_p = buffer.Get();

	TargaHeader	targa_header;
	targa_header.id_length = *buf_p++;
	targa_header.colormap_type = *buf_p++;
	targa_header.image_type = *buf_p++;

	targa_header.colormap_index = LittleShort(*(short *)buf_p);
	buf_p += 2;
	targa_header.colormap_length = LittleShort(*(short *)buf_p);
	buf_p += 2;
	targa_header.colormap_size = *buf_p++;
	targa_header.x_origin = LittleShort(*(short *)buf_p);
	buf_p += 2;
	targa_header.y_origin = LittleShort(*(short *)buf_p);
	buf_p += 2;
	targa_header.width = LittleShort(*(short *)buf_p);
	buf_p += 2;
	targa_header.height = LittleShort(*(short *)buf_p);
	buf_p += 2;
	targa_header.pixel_size = *buf_p++;
	targa_header.attributes = *buf_p++;

	if (targa_header.image_type != 2 && targa_header.image_type != 10 && targa_header.image_type != 3) {
		common->Error("LoadTGA( %s ): Only type 2 (RGB), 3 (gray), and 10 (RGB) TGA images supported\n", name);
	}

	if (targa_header.colormap_type != 0) {
		common->Error("LoadTGA( %s ): colormaps not supported\n", name);
	}

	if ((targa_header.pixel_size != 32 && targa_header.pixel_size != 24) && targa_header.image_type != 3) {
		common->Error("LoadTGA( %s ): Only 32 or 24 bit images supported (no colormaps)\n", name);
	}

	if (targa_header.image_type == 2 || targa_header.image_type == 3) {
		numBytes = targa_header.width * targa_header.height * (targa_header.pixel_size >> 3);
		if (numBytes > static_cast<int>(buffer.Num()) - 18 - targa_header.id_length) {
			common->Error("LoadTGA( %s ): incomplete file\n", name);
		}
	}

	columns = targa_header.width;
	rows = targa_header.height;
	numPixels = columns * rows;

	level_t level;
	level.width = columns;
	level.height = rows;
	level.size = numPixels * 4;
	level.offset = 0;

	fhStaticBuffer<byte> rgba(numPixels * 4);

	if (targa_header.id_length != 0) {
		buf_p += targa_header.id_length;  // skip TARGA image comment
	}

	if (targa_header.image_type == 2 || targa_header.image_type == 3)
	{
		// Uncompressed RGB or gray scale image
		for (int row = rows - 1; row >= 0; row--)
		{
			byte* pixbuf = rgba.Get() + row*columns * 4;
			for (int column = 0; column < columns; column++)
			{
				unsigned char red, green, blue, alphabyte;
				switch (targa_header.pixel_size)
				{

				case 8:
					blue = *buf_p++;
					green = blue;
					red = blue;
					*pixbuf++ = red;
					*pixbuf++ = green;
					*pixbuf++ = blue;
					*pixbuf++ = 255;
					break;

				case 24:
					blue = *buf_p++;
					green = *buf_p++;
					red = *buf_p++;
					*pixbuf++ = red;
					*pixbuf++ = green;
					*pixbuf++ = blue;
					*pixbuf++ = 255;
					break;
				case 32:
					blue = *buf_p++;
					green = *buf_p++;
					red = *buf_p++;
					alphabyte = *buf_p++;
					*pixbuf++ = red;
					*pixbuf++ = green;
					*pixbuf++ = blue;
					*pixbuf++ = alphabyte;
					break;
				default:
					common->Error("LoadTGA( %s ): illegal pixel_size '%d'\n", name, targa_header.pixel_size);
					break;
				}
			}
		}
	}
	else if (targa_header.image_type == 10) {   // Runlength encoded RGB images
		unsigned char red, green, blue, alphabyte, packetHeader, packetSize, j;

		red = 0;
		green = 0;
		blue = 0;
		alphabyte = 0xff;

		for (int row = rows - 1; row >= 0; row--) {
			auto pixbuf = rgba.Get() + row*columns * 4;
			for (int column = 0; column < columns;) {
				packetHeader = *buf_p++;
				packetSize = 1 + (packetHeader & 0x7f);
				if (packetHeader & 0x80) {        // run-length packet
					switch (targa_header.pixel_size) {
					case 24:
						blue = *buf_p++;
						green = *buf_p++;
						red = *buf_p++;
						alphabyte = 255;
						break;
					case 32:
						blue = *buf_p++;
						green = *buf_p++;
						red = *buf_p++;
						alphabyte = *buf_p++;
						break;
					default:
						common->Error("LoadTGA( %s ): illegal pixel_size '%d'\n", name, targa_header.pixel_size);
						break;
					}

					for (j = 0; j < packetSize; j++) {
						*pixbuf++ = red;
						*pixbuf++ = green;
						*pixbuf++ = blue;
						*pixbuf++ = alphabyte;
						column++;
						if (column == columns) { // run spans across rows
							column = 0;
							if (row > 0) {
								row--;
							}
							else {
								goto breakOut;
							}
							pixbuf = rgba.Get() + row*columns * 4;
						}
					}
				}
				else {                            // non run-length packet
					for (j = 0; j < packetSize; j++) {
						switch (targa_header.pixel_size) {
						case 24:
							blue = *buf_p++;
							green = *buf_p++;
							red = *buf_p++;
							*pixbuf++ = red;
							*pixbuf++ = green;
							*pixbuf++ = blue;
							*pixbuf++ = 255;
							break;
						case 32:
							blue = *buf_p++;
							green = *buf_p++;
							red = *buf_p++;
							alphabyte = *buf_p++;
							*pixbuf++ = red;
							*pixbuf++ = green;
							*pixbuf++ = blue;
							*pixbuf++ = alphabyte;
							break;
						default:
							common->Error("LoadTGA( %s ): illegal pixel_size '%d'\n", name, targa_header.pixel_size);
							break;
						}
						column++;
						if (column == columns) { // pixel packet run spans across rows
							column = 0;
							if (row > 0) {
								row--;
							}
							else {
								goto breakOut;
							}
							pixbuf = rgba.Get() + row*columns * 4;
						}
					}
				}
			}
		breakOut:;
		}
	}

	if ((targa_header.attributes & (1 << 5))) {			// image flp bit
		R_VerticalFlip(rgba.Get(), level.width, level.height);
	}

	this->faces[0].levels[0] = level;
	this->numFaces = 1;
	this->numLevels = 1;
	this->data = rgba.Release();
	this->format = pixelFormat_t::RGBA;

	return true;
}

bool fhImageData::LoadJPG(fhStaticBuffer<byte>& buffer, bool toRgba) {
	int len = buffer.Num();
	int w=0, h=0, comp=0;
	byte* decodedImageData = stbi_load_from_memory( buffer.Get(), len, &w, &h, &comp, 4 );

	if ( decodedImageData == NULL ) {
		common->Warning( "stb_image was unable to load JPG %s : %s\n",
		                 "filename", stbi_failure_reason() );
		return false;
	}

	// *pic must be allocated with R_StaticAlloc(), but stb_image allocates with malloc()
	// (and as there is no R_StaticRealloc(), #define STBI_MALLOC etc won't help)
	// so the decoded data must be copied once
	int size = w*h*4;
	fhStaticBuffer<byte> rgba(size);
	memcpy( rgba.Get(), decodedImageData, size );
	// now that decodedImageData has been copied into *pic, it's not needed anymore
	stbi_image_free( decodedImageData );
	
	level_t level;
	level.width = w;
	level.height = h;
	level.size = size;

	level.offset = 0;
	this->faces[0].levels[0] = level;
	this->numFaces = 1;
	this->numLevels = 1;
	this->data = rgba.Release();
	this->format = pixelFormat_t::RGBA;
	return true;
}

uint32 fhImageData::GetWidth(uint32 level) const {
	assert(numFaces > 0);
	assert(numLevels > level);
	return faces[0].levels[level].width;
}
uint32 fhImageData::GetHeight(uint32 level) const {
	assert(numFaces > 0);
	assert(numLevels > level);
	return faces[0].levels[level].height;
}

uint32 fhImageData::GetSize(uint32 level) const {
	assert(numFaces > 0);
	assert(numLevels > level);
	return faces[0].levels[level].size;
}

uint32 fhImageData::GetNumFaces() const {
	return numFaces;
}

uint32 fhImageData::GetNumLevels() const {
	return numLevels;
}

uint32 fhImageData::GetMaxNumLevels() const {
	uint32 levels = 0;
	uint32 w = GetWidth();
	uint32 h = GetHeight();

	while (w > 0 || h > 0) {
		levels += 1;
		w /= 2;
		h /= 2;
	}

	return levels;
}

pixelFormat_t fhImageData::GetPixelFormat() const {
	return format;
}

ID_TIME_T fhImageData::GetTimeStamp() const {
	return timestamp;
}

const byte* fhImageData::GetData(uint32 face, uint32 level) const {
	if (face >= numFaces)
		return nullptr;

	if (level >= numLevels)
		return nullptr;

	return data + faces[face].levels[level].offset;
}

byte* fhImageData::GetData( uint32 face, uint32 level ) {
	if (face >= numFaces)
		return nullptr;

	if (level >= numLevels)
		return nullptr;

	return data + faces[face].levels[level].offset;
}

bool fhImageData::LoadFileIntoBuffer(const char* filename, fhStaticBuffer<byte>& buffer) {

	fhFileHandle file = fileSystem->OpenFileRead(filename);
	if (!file) {
		return false;
	}

	int	len = file->Length();

	ID_TIME_T time = file->Timestamp();
	if (time > this->timestamp) {
		timestamp = time;
	}

	strncpy(this->name, filename, Min(strlen(filename), sizeof(this->name) - 1));

	buffer.Free();
	buffer.Allocate(len);
	file->Read(buffer.Get(), len);
	file.Close();

	return true;
}

bool fhImageData::LoadCubeMap( const fhImageData sides[6], const char* name ) {
	Clear();
	strncpy( this->name, name, MAX_IMAGE_NAME );

	int	size = 0;
	int bytesPerSide = 0;
	ID_TIME_T timestamp = 0;
	pixelFormat_t format = pixelFormat_t::None;
	fhStaticBuffer<byte> buffer;

	for (int i = 0; i < 6; i++) {
		const fhImageData& side = sides[i];

		if (i == 0) {
			timestamp = side.GetTimeStamp();
			size = side.GetWidth();
			bytesPerSide = size * size * 4; //4 bytes, due to RGBA
			format = side.GetPixelFormat();
			buffer.Allocate( bytesPerSide * 6 );
		}

		if (side.GetWidth() != size || side.GetHeight() != size) {
			common->Warning( "Mismatched sizes on cube map '%s'", side.GetName() );
			return false;
		}

		if (side.GetPixelFormat() != format) {
			common->Warning( "Mismatched pixel format on cube map '%s'", side.GetName() );
			return false;
		}

		if (side.GetSize( 0 ) != bytesPerSide) {
			common->Warning( "Unexpected size of cube map side '%s'", side.GetName() );
			return false;
		}

		memcpy( &buffer.Get()[i * bytesPerSide], side.GetData(), bytesPerSide );
	}

	this->data = buffer.Release();
	this->numFaces = 6;
	this->numLevels = 1;
	this->format = format;
	this->timestamp = timestamp;

	for (int i = 0; i < 6; ++i) {
		auto& level = this->faces[i].levels[0];
		level.width = size;
		level.height = size;
		level.offset = bytesPerSide * i;
		level.size = bytesPerSide;
	}

	return true;
}

bool fhImageData::LoadProgram( const char* program ) {
	Clear();

	fhImageProgram imageProgram;
	idStr::Copynz( name, program, sizeof( name ) - 1 );
	return imageProgram.LoadImageProgram( program, this, nullptr );
}
