/*****************************************************************************
 *  This file is part of the OpenDomo project.
 *  Copyright(C) 2011 OpenDomo Services SL
 *  
 *  Daniel Lerch Hostalot <dlerch@opendomo.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <jpeglib.h>
#include <ai/perceiving/vision/jpeg.h>

#define OUTPUT_BUF_SIZE 4096

typedef struct 
{
	struct jpeg_destination_mgr pub; /* public fields */
	unsigned char ** outbuffer; /* target buffer */
	unsigned long * outsize;
	unsigned char * newbuffer; /* newly allocated buffer */
	JOCTET * buffer; /* start of buffer */
	size_t bufsize;
} 
memory_destination_mgr;

typedef memory_destination_mgr* mem_dest_ptr;


static void init_source(j_decompress_ptr cinfo)
{
}

static unsigned char EOI_data[2] = { 0xFF, 0xD9 };

static boolean fill_input_buffer(j_decompress_ptr cinfo)
{
    cinfo->src->next_input_byte = EOI_data;
    cinfo->src->bytes_in_buffer = 2;
    return TRUE;
}

static void skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
    if (num_bytes > 0) {
        if (num_bytes > (long) cinfo->src->bytes_in_buffer)
            num_bytes = (long) cinfo->src->bytes_in_buffer;
        cinfo->src->next_input_byte += (size_t) num_bytes;
        cinfo->src->bytes_in_buffer -= (size_t) num_bytes;
    }
}

static void term_source(j_decompress_ptr cinfo)
{
}

static void init_destination(j_compress_ptr cinfo)
{
    /* No work necessary here */
}

static boolean empty_output_buffer(j_compress_ptr cinfo)
{
	size_t nextsize;
	JOCTET * nextbuffer;
	
	mem_dest_ptr dest = (mem_dest_ptr) cinfo->dest;

	/* Try to allocate new buffer with double size */
	nextsize = dest->bufsize * 2;
	nextbuffer = (JOCTET *)malloc(nextsize);

	if (nextbuffer == NULL)
	{
		syslog(LOG_ERR, "empty_output_buffer(): out of memory\n");
		return FALSE;
	}

	memcpy(nextbuffer, dest->buffer, dest->bufsize);

	/* Free old buffer */
	if (dest->newbuffer != NULL)
		free(dest->newbuffer);

	dest->newbuffer = nextbuffer;

	dest->pub.next_output_byte = nextbuffer + dest->bufsize;
	dest->pub.free_in_buffer = dest->bufsize;

	dest->buffer = nextbuffer;
	dest->bufsize = nextsize;

	return TRUE;
}

static void term_destination(j_compress_ptr cinfo)
{
	mem_dest_ptr dest = (mem_dest_ptr) cinfo->dest;

	*dest->outbuffer = dest->buffer;
	*dest->outsize = dest->bufsize - dest->pub.free_in_buffer;
}



// {{{ jpeg_buffer_src()
static void jpeg_buffer_src(
	j_decompress_ptr cinfo, 
	unsigned char *buffer, 
	long num)
{
	if (cinfo->src == NULL) 
	{
		cinfo->src = (struct jpeg_source_mgr *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			sizeof (struct jpeg_source_mgr));
	}

	cinfo->src->init_source = init_source;
	cinfo->src->fill_input_buffer = fill_input_buffer;
	cinfo->src->skip_input_data = skip_input_data;
	cinfo->src->resync_to_restart = jpeg_resync_to_restart;
	cinfo->src->term_source = term_source;
	cinfo->src->bytes_in_buffer = num;
	cinfo->src->next_input_byte = (JOCTET *) buffer;
}
// }}}

// {{{ jpeg_buffer_dest()
static void jpeg_buffer_dest(j_compress_ptr cinfo, 
	unsigned char ** outbuffer, unsigned long * outsize)
{
	mem_dest_ptr dest;

	if (outbuffer == NULL || outsize == NULL) /* sanity check */
	{
		syslog(LOG_ERR, "jpeg_buffer_dest() incorrect input\n");
		return;
	}

	/* The destination object is made permanent so that multiple JPEG images
	 * can be written to the same buffer without re-executing jpeg_mem_dest.
	 */
	if (cinfo->dest == NULL) { /* first time for this JPEG object? */
		cinfo->dest = (struct jpeg_destination_mgr *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
					sizeof(memory_destination_mgr));
	}

	dest = (mem_dest_ptr) cinfo->dest;
	dest->pub.init_destination = init_destination;
	dest->pub.empty_output_buffer = empty_output_buffer;
	dest->pub.term_destination = term_destination;
	dest->outbuffer = outbuffer;
	dest->outsize = outsize;
	dest->newbuffer = NULL;

	if (*outbuffer == NULL || *outsize == 0) 
	{
		/* Allocate initial buffer */
		dest->newbuffer = *outbuffer = (unsigned char*)malloc(OUTPUT_BUF_SIZE);
		if (dest->newbuffer == NULL)
		{
			syslog(LOG_ERR, "jpeg_buffer_dest(): out of memory\n");
			return;
		}

		*outsize = OUTPUT_BUF_SIZE;
	}

	dest->pub.next_output_byte = dest->buffer = *outbuffer;
	dest->pub.free_in_buffer = dest->bufsize = *outsize;
}
// }}}

// {{{ jpeg_write()
void jpeg_write(image_t *img, int quality, char * input)
{
   struct jpeg_compress_struct cinfo;
   struct jpeg_error_mgr jerr;
   FILE * outfile;   
   JSAMPROW row_pointer[1]; 
   int row_stride;
	unsigned long blen=0;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	if ((outfile = fopen(input, "wb")) == NULL) 
	{
		fprintf(stderr, "jpeg_write() can't open %s\n", input);
		exit(1);
	}

	jpeg_stdio_dest(&cinfo, outfile);


   cinfo.image_width = img->width;  
   cinfo.image_height = img->height;
   cinfo.input_components = img->components;    
   cinfo.in_color_space = JCS_RGB;   
   jpeg_set_defaults(&cinfo);
   jpeg_set_quality(&cinfo, quality, TRUE);
   jpeg_start_compress(&cinfo, TRUE);
   row_stride = img->width * 3;


   while (cinfo.next_scanline < cinfo.image_height) 
   {
      row_pointer[0] = & img->rgb[cinfo.next_scanline * row_stride];
      jpeg_write_scanlines(&cinfo, row_pointer, 1);
   }

   jpeg_finish_compress(&cinfo);
  	fclose(outfile);
   jpeg_destroy_compress(&cinfo);
}
// }}}

// {{{ jpeg_write_alloc()
char* jpeg_write_alloc(image_t *img, int quality, unsigned long *buflen)
{
   struct jpeg_compress_struct cinfo;
   struct jpeg_error_mgr jerr;
   FILE * outfile;   
   JSAMPROW row_pointer[1]; 
   int row_stride;
	unsigned long blen=0;
	unsigned char *outbuffer = NULL;

   cinfo.err = jpeg_std_error(&jerr);
   jpeg_create_compress(&cinfo);

	jpeg_buffer_dest(&cinfo, &outbuffer, buflen);


   cinfo.image_width = img->width;  
   cinfo.image_height = img->height;
   cinfo.input_components = img->components;    
   cinfo.in_color_space = JCS_RGB;   
   jpeg_set_defaults(&cinfo);
   jpeg_set_quality(&cinfo, quality, TRUE);
   jpeg_start_compress(&cinfo, TRUE);
   row_stride = img->width * 3;

   while (cinfo.next_scanline < cinfo.image_height) 
   {
      row_pointer[0] = & img->rgb[cinfo.next_scanline * row_stride];
      jpeg_write_scanlines(&cinfo, row_pointer, 1);
   }

   jpeg_finish_compress(&cinfo);
   jpeg_destroy_compress(&cinfo);

	return outbuffer;
}
// }}}

struct my_error_mgr 
{
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer; 
};

typedef struct my_error_mgr * my_error_ptr;

void my_error_exit (j_common_ptr cinfo)
{
  my_error_ptr myerr = (my_error_ptr) cinfo->err;
  (*cinfo->err->output_message) (cinfo);
  longjmp(myerr->setjmp_buffer, 1);
}

// {{{ jpeg_read()
image_t *jpeg_read(char *input, int buflen)
{
   struct jpeg_decompress_struct cinfo;
   struct my_error_mgr jerr;
   FILE * infile = NULL;     
   JSAMPARRAY buffer;    
   int row_stride;   
   unsigned char *image_buffer = NULL;

   cinfo.err = jpeg_std_error(&jerr.pub);
   jerr.pub.error_exit = my_error_exit;
   if (setjmp(jerr.setjmp_buffer)) 
   {
      jpeg_destroy_decompress(&cinfo);
      return NULL;
   }

	if(buflen==0)
	{
   	if ((infile = fopen(input, "rb")) == NULL) 
   	{
      	fprintf(stderr, "jpeg_read() can't open %s\n", input);
      	return NULL;
   	}
	}



   jpeg_create_decompress(&cinfo);

	if(buflen>0)
		jpeg_buffer_src(&cinfo, (unsigned char*)input, buflen);
	else
   	jpeg_stdio_src(&cinfo, infile);

   jpeg_read_header(&cinfo, TRUE);
   jpeg_start_decompress(&cinfo);

   row_stride = cinfo.output_width * cinfo.output_components;
   buffer = (*cinfo.mem->alloc_sarray)
      ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

   if((image_buffer = malloc(cinfo.output_width * cinfo.output_height *
               cinfo.output_components)) == NULL)
   {
      perror("Couldn't allocate memory");
      return NULL;
   }

   JSAMPLE *upto = image_buffer;


   while (cinfo.output_scanline < cinfo.output_height) 
   {
      jpeg_read_scanlines(&cinfo, buffer, 1);
      memcpy(upto, *buffer, cinfo.output_width * cinfo.output_components);
      upto += cinfo.output_width * cinfo.output_components;
   }


   image_t *img = image_alloc(image_buffer, cinfo.output_width, 
                     cinfo.output_height, cinfo.output_components);

   free(image_buffer);

   jpeg_finish_decompress(&cinfo);
   jpeg_destroy_decompress(&cinfo);
	if(buflen==0)
   	fclose(infile);

   return img;
}
// }}}



