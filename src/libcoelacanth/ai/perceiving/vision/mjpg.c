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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <fcntl.h>


#include <coelacanth/str.h>
#include "mjpg.h"


/// Controlled abort
#define goto_error_if_fail(x) if(x<0) goto error;
#define return_if_fail(x) if(x<0) return x;

/// Script to launch when stop recording
#define STOP_RECORD_SCRIPT (const char *)("/usr/local/opendomo/services/syscript/convert.sh")

// Max Values
#define BLOCK_SIZE 128
#define WAIT_IF_NO_MOTION 10

static int stop = 0;

/* Internal Functions */

// {{{ recvline() 
/// Read a line from the data source
int recvline(char *line, size_t len, int sd)
{
	char c;
	int i=0;
	int n;
	while(i<len)
	{
		//n=recv(sd, &c, 1, 0);
		n=read(sd, &c, 1);

		if (n<=0)
			return -1;
		
		if(c=='\n')
			break;
		
		line[i++]=c;
	}
	
	if(i>0 && line[i-1]=='\r')
	{
		line[i-1]=0;
		i--;
	}
	else
		line[i]=0;
	
	line[len-1]=0;
	

	return i;
}
// }}}

// {{{ write_frame()
int write_frame(
				int fd, 
				const char* l_blank,
				const char* l_bound,
				const char* l_contt,
				const char* l_contl,
				const char* img,
				size_t img_len)
{
	int nbytes = 0;

	char buffer[256];
	snprintf(buffer, sizeof(buffer), 
		"\r\n"
		"%s\r\n"
		"%s\r\n"
		"Content-Length: %d\r\n"
		"\r\n"
		, l_bound, l_contt, img_len);

	nbytes=write(fd, buffer, strlen(buffer));
	return_if_fail(nbytes);

	nbytes = write(fd, img, img_len);
	return_if_fail(nbytes);

	return 0;
}
// }}}

// {{{ find_next_frame()
int find_next_frame(int fd,
	char *boundary, size_t boundary_len, 
	char *ct, size_t ct_len, 
	char *cl, size_t cl_len)
{
	int nbytes = 0;
	char buffer[128];
	int len = 0;
	
	// Get initial position
	while(1)
	{
	
		buffer[0]=0;
	
		nbytes = recvline(buffer, sizeof(buffer), fd);
		if(nbytes<0)
		{
			syslog(LOG_NOTICE, "Cannot find frame: no more frames\n");
			return -1;
		}
		
		if(nbytes==0)
			continue;


		char *p = NULL;

		// read stream header
		if((p=strstr(buffer, "boundary=")))
		{

			snprintf(boundary, boundary_len, "%s", p+9);
			syslog(LOG_NOTICE, "found boundary: %s\n", boundary);

			// empty line
			nbytes = recvline(buffer, sizeof(buffer), fd);
			if(nbytes<0)
			{
				syslog(LOG_ERR, "Cannot find frame: error reading empty line\n");
				return -1;
			}

			// boundary
			nbytes = recvline(buffer, sizeof(buffer), fd);
			if(nbytes<0)
			{
				syslog(LOG_ERR, "Cannot find frame: error reading boundary\n");
				return -1;
			}

			if(!strstr(buffer, boundary))
			{
				syslog(LOG_DEBUG, "Cannot find frame: incorrect boundary. Waiting "
						 "for %s but %s has been received\n", boundary, buffer);
				//return -1;
			}

			break;
		}

		// read frame header
		if((p=strstr(buffer, boundary)))
		{
			// frame inside file
			break;
		}
	}

	//syslog(LOG_DEBUG, "boundary before: %s\n", boundary);
	str_replace(boundary, boundary_len, "\"", "");
	str_replace(boundary, boundary_len, "'", "");
	str_replace(boundary, boundary_len, "-", "");
	//syslog(LOG_DEBUG, "boundary after: %s\n", boundary);

	// Read headers
	for(;;)
	{
		// read headers
		if(strncasecmp(buffer, "Content-Type", 12)==0)
			snprintf(ct, ct_len, "%s", buffer);

		if(strstr(buffer, "Content-Length"))
		{
			snprintf(cl, cl_len, "%s", buffer);
			sscanf(buffer, "Content-Length: %d", &len);
		}

		if(strstr(buffer, "Content-length"))
		{
			snprintf(cl, cl_len, "%s", buffer);
			sscanf(buffer, "Content-length: %d", &len);
		}

		if(strstr(buffer, "DataLen"))
		{
			sscanf(buffer, "DataLen: %d", &len);
			snprintf(cl, cl_len, "Content-Length: %d", len);
		}

		nbytes = recvline(buffer, sizeof(buffer), fd);
		if(nbytes<0)
		{
			syslog(LOG_ERR, "Cannot find frame: error reading headers\n");
			return -1;
		}

		//syslog(LOG_NOTICE, "Content-Length: %d\n", ct_len);

		if(nbytes==0)
			break;
		
	}

	return len;
}
// }}}

// {{{ get_frame()
int get_frame(int sockfd, 
			  char* l_blank,
			  char* l_bound,
			  char* l_contt,
			  char* l_contl)
{
	int nbytes = 0;
	int len = 0;
	/* read blank */
	nbytes = recvline(l_blank, BLOCK_SIZE, sockfd);
	return_if_fail(nbytes);
	
	if(strncmp(l_blank, "Content-Type", strlen("Content-Type"))!=0)
	{
		/* read --BoundaryString */
		nbytes = recvline(l_bound, BLOCK_SIZE, sockfd);
		return_if_fail(nbytes);

		/* read Content-Type */
		nbytes = recvline(l_contt, BLOCK_SIZE, sockfd);
		return_if_fail(nbytes);
	}	
	else
	{
		snprintf(l_contt, sizeof(l_contt), "%s", l_blank);
		strcpy(l_blank, "-");
		strcpy(l_bound, "-");
	}

	/* read Content-Length */
	nbytes =recvline(l_contl, BLOCK_SIZE, sockfd);
	return_if_fail(nbytes);

	sscanf(l_contl, "Content-Length: %d", &len);
	return len;
}
// }}}

// {{{ signal_handler()
void signal_handler(int signo)
{
	stop=1;
}
// }}}

// {{{ socket_open()
int socket_open(const char *host, const char *url, size_t port, 
				 const char *credentials)
{
	struct hostent *hn;
	if ((hn = gethostbyname(host))==0)
		return -1;

	char *ip = inet_ntoa(*(struct in_addr *)*hn->h_addr_list);

	struct sockaddr_in pin;
	memset (&pin, 0, sizeof(pin));
	pin.sin_family =  AF_INET;
	pin.sin_addr.s_addr = htonl(INADDR_ANY);
	pin.sin_port = htons(port);
	inet_pton(AF_INET, ip, &pin.sin_addr);
	
	int sockfd = socket (AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		return -2;
	
	if(connect(sockfd, (void *)&pin, sizeof(pin))==-1)
		return -3;
	
	char buffer[2048];  
	char b64[256];
	

	if(strlen(credentials)>0)
	{
		base64(credentials, b64, sizeof(b64));

    	//syslog(LOG_NOTICE, "send credentials: %s|%s\n", credentials, b64);
		snprintf(buffer, sizeof(buffer), 
				"GET %s HTTP/1.1\r\n"
				"Authorization: Basic %s\r\n\r\n"
				, url, b64);
	}
	else
	{
    	//syslog(LOG_NOTICE, "no credentials\n");
		snprintf(buffer, sizeof(buffer), 
				"GET %s HTTP/1.1\r\n\r\n", url);
	}


	write(sockfd, buffer, strlen(buffer));

	return sockfd;
	
}
// }}}

// {{{ file_open()
int file_open(const char *file)
{
  int fd = open(file, O_RDONLY);
   if(fd<0)
   {
      syslog(LOG_ERR, "Cannot open file: %s\n", file);
      return -1;
   }

	return fd;
}
// }}}

// {{{ read_raw_image()
int read_raw_image(char * img, int len, int sockfd)
{
   char buffer[len];  
	int nbytes=0;	
	size_t readed_bytes = 0;
	
	img[0]=0;
	
	int buffsize = len;
	while(readed_bytes < len && buffsize > 0)
	{
		nbytes=read(sockfd, buffer, buffsize);
		
		if(nbytes>0)
		{
			memcpy(img+readed_bytes, buffer, nbytes); 
			readed_bytes += nbytes;
			buffsize -= nbytes;
		}	
		if(nbytes<=0)
			return -1;
	}
	
	return readed_bytes;
}
// }}}

// {{{ jump_to()
void jump_to(int fd, int offset)
{
	struct stat buf;
	fstat(fd, &buf);

	off_t pos = (off_t)offset * (buf.st_size/100);
	syslog(LOG_NOTICE, "size: %d\n", buf.st_size);
	syslog(LOG_NOTICE, "position: %d\n", pos);
	lseek(fd, pos, SEEK_SET);
}
// }}}



/* Data Type Methods */

// {{{ mjpg_streaming()
int mjpg_streaming(mjpg_t *p)
{
	if(!p)
		return -1;

	int recording_fd=0;
	int fps_fd=0;
	int frames_cnt=0;
	time_t frames_tm=time(NULL);
	char line[128];
	int nbytes=0;
	int r;
	int len = 0;
	int fd = -1;
	int flags = p->flags;
	char * base = p->base;
	int threshold = p->threshold;
	float fps=0;
	char buf[256];
	/* true if are set zones*/
	int set_zones = 0;
	int i = 0;

	if(signal(SIGTERM, signal_handler)==SIG_ERR)
		return -1;
	
	if(signal(SIGINT, signal_handler)==SIG_ERR)
		return -1;


	// Use socket as source of streaming
	if(flags & M_READ_FROM_SOCKET)
	{
	  	syslog(LOG_NOTICE, "Read streaming from socket\n");
		fd = socket_open(p->host, p->url, p->port, p->auth);
		if(fd<0)
	  		syslog(LOG_NOTICE, "error %d opening socket\n", fd);
	
	}		

	// Use file as source of streaming
	if(flags & M_READ_FROM_FILE)
	{
	  	syslog(LOG_NOTICE, "Read streaming from file\n");
		fd = file_open(p->src_file);

		// Read frames per second
		char filename[128];
		str_assign_str(filename, sizeof(filename), p->src_file);
		str_replace(filename, sizeof(filename), ".mjpg", ".fps");
		int fdtmp = open(filename, O_RDONLY);
		if(fdtmp<0)
		{
	  		syslog(LOG_ERR, "Cannot open file: %s\n", filename);
			goto error;
		}
		char buffer[32];
		if((read(fdtmp, buffer, sizeof(buffer)))>0)
		{
			fps=atof(buffer);
	  		syslog(LOG_NOTICE, "fps:%s=%f\n", buffer, fps);
		}
		
		close(fdtmp);

	}

	if(fd<=0)
	{
	  	syslog(LOG_ERR, "Cannot read streaming source\n");
		return -1;
	}


	// Move reading pointer to specified position
	if((flags & M_READ_FROM_FILE) && (flags & M_OFFSET))
	{	
		syslog(LOG_DEBUG, "play from position: %d\n", p->offset);
		jump_to(fd, p->offset);
	}

	/* Check set zones */
	for (i = 0; i < MAX_ZONES; i++) {
		if(p->zones[i].x_top_left != 0 || p->zones[i].x_bottom_right != 0 || 
		   p->zones[i].y_top_left != 0 || p->zones[i].y_bottom_right != 0)
		{
			set_zones = 1;
			break;
		}
	}
	
	/* image frames */
	image_t *frm1 = NULL;
	image_t *frm2 = NULL;
	int cnt_images=0;
	int record_in_file=0;
	char path_mjpg[128];
	char path_jpg[128];
	char path_fps[128];
	time_t t=0;
	int has_header=0;

	char boundary[64] = "--odboundary";



	while(1)
	{
		if(stop)
		{
			syslog(LOG_NOTICE, "stopping...\n");
			goto error;
		}

		/* find initial position */
		char ct[64] = "";
		char cl[64] = "";

		len = find_next_frame(fd, boundary, sizeof(boundary), 
				ct, sizeof(ct), cl, sizeof(cl));

		if(len==0)
		{
			syslog(LOG_DEBUG, "unknown content length. force to %d\n", 50000);
			len=50000;
		}
	


		/* if video position is too long, find last frame */
		int try=0;
		off_t offset = p->offset;
			
		if(flags & M_READ_FROM_FILE)
		{
			while(len<0 && cnt_images==0 && try<20)
			{
				offset -= 5;
				syslog(LOG_DEBUG, "play from another position: %d\n", offset);
				jump_to(fd, offset);
				try++;
				len = find_next_frame(fd, boundary, sizeof(boundary), 
						ct, sizeof(ct), cl, sizeof(cl));
			}
		}

		goto_error_if_fail(len);

		// write header
		if((flags & M_STDOUT) && !has_header)
		{
			char *http_str = 
				"Content-Type: multipart/x-mixed-replace; boundary=";
			write(1, http_str, strlen(http_str));		
			write(1, boundary, strlen(boundary));		
			write(1, "\r\n", 2);
			write(1, "\r\n", 2);
			has_header=1;
		}

		/* read image */
		//len += 2;
		char img[len*2];
		size_t total_bytes = read_raw_image(img, len, fd);		
		//syslog(LOG_DEBUG, "image read with %d bytes\n", total_bytes);
		goto_error_if_fail(total_bytes);

		cnt_images++;		

		if(flags & M_READ_FROM_SOCKET)
		{
			// Draw timestamp
			image_t *frm = jpeg_read(img, total_bytes);
			if(!frm)
				goto error;

			point_t pt;
			pt.x = 10;
			pt.y = 10;
			time_t t = time(NULL);
			//image_draw_text(frm1, "OpenDomo Video System", &pt);	
			//pt.y = 25;			
			image_draw_text(frm, ctime(&t), &pt);				
			unsigned long imgbuf_len = 0;
			char *imgbuf = jpeg_write_alloc(frm, 75, &imgbuf_len);

			if(len*2<imgbuf_len)	
			{
				syslog(LOG_ERR, "Cannot draw text. Buffer too short\n");
			}
			else
			{
				memcpy(img, imgbuf, imgbuf_len);
				len = imgbuf_len;
			}

			free(imgbuf);
			image_free(frm);
		}


		// get separated frames
		int diff=0;
		if(threshold>0)
		{
			if(cnt_images%2==1)
			{
				frm1 = jpeg_read(img, total_bytes);
				if(!frm1)
					goto error;
			}
			else
			{
				frm2 = jpeg_read(img, total_bytes);

				if(!frm2)
				{
					image_free(frm1);
					goto error;
				}

				// processing between frames
				if(flags & M_DRAW_SHAPE)	
				{		
					// TODO
				}
				if(set_zones == 0)
					diff = image_num_diff(frm1, frm2);
				else
				{
					for (i = 0; i < MAX_ZONES; i++) {
						if(p->zones[i].x_top_left != 0 || 
						   p->zones[i].x_bottom_right != 0 || 
						   p->zones[i].y_top_left != 0 || 
						   p->zones[i].y_bottom_right != 0)
						{
							diff = image_num_diff_zone(frm1, frm2,  &p->zones[i]);
							if (diff>threshold)
								break;
						}
					}
				}


				image_free(frm1);
				image_free(frm2);
			}
		}


		// When recording begins we need to create an image with the first 
		// frame of the streaming and to write the header of the mjpg stream.
		if(record_in_file == 0)
		{
			if((flags & M_RECORD))
				record_in_file = 1;

			if((diff>threshold) && (flags & M_RECORD_THRESHOLD))
				record_in_file = 1;

			if(record_in_file==1)
			{
				t = time(NULL);
				syslog(LOG_NOTICE, "recording: %d\n", t);
				char timestamp[16];
				strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", 
						localtime(&t));
				snprintf(path_mjpg, sizeof(path_mjpg), "%s-%s.mjpg",base,timestamp);
				snprintf(path_jpg, sizeof(path_jpg), "%s-%s.jpg", base, timestamp);
				snprintf(path_fps, sizeof(path_jpg), "%s-%s.fps", base, timestamp);
				record_in_file = 1;

				int fd = open(path_jpg, O_CREAT | O_TRUNC| O_WRONLY, S_IRWXU | S_IRWXG);
				write(fd, img, total_bytes);
				close(fd);
			
				if(diff>threshold)
				{
					char buf[128];
					snprintf(buf, sizeof(buf),
						"/bin/logevent notice odvision 'Motion in [%s]'", path_jpg);
					system(buf);
					syslog(LOG_NOTICE, "%s\n", buf);
				}

				fps_fd = open(path_fps, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU | S_IRWXG);
				if(fps_fd<0)
					perror("open");

				recording_fd = open(path_mjpg, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU | S_IRWXG);
				if(recording_fd<0)
					perror("open");

				char *http_str = 
		"Content-Type: multipart/x-mixed-replace; boundary=--odboundary\r\n";
				write(recording_fd, http_str, strlen(http_str));		
			}
		}

		// In recording by threshold, we stop recording when there are not
		// significant differences detected. We wait X seconds.
		//printf("diff: %d, threshold: %d, time(t): %d\n", diff, threshold, t);
		if(record_in_file && diff<=threshold && flags & M_RECORD_THRESHOLD)
		{
			// motion detected and recording after, X seconds it stops
			if(time(NULL)-t > WAIT_IF_NO_MOTION)
			{
				syslog(LOG_NOTICE, "stop recording: %d\n", time(NULL));
				// we stop recording
				record_in_file = 0;
				char str[16];
				char buf[256];
				float frames = 0;
				if(time(NULL)-frames_tm > 0)
					frames = (float)frames_cnt/(time(NULL)-frames_tm);
				snprintf(str, sizeof(str), "%f", frames);
				write(fps_fd, str, strlen(str));
				close(fps_fd);
				close(recording_fd);
				chmod(path_mjpg, 00777);
				chown(path_mjpg, 1000, 1000);
				chmod(path_jpg, 00777);
				chown(path_jpg, 1000, 1000);
				chmod(path_fps, 00777);
				chown(path_fps, 1000, 1000);
				snprintf(buf, sizeof(buf),
					"/bin/logevent filesave odvision 'Video recorded' %s", path_mjpg);
				system(buf);
				syslog(LOG_NOTICE, "%s\n", buf);
			}
		}
		else if(diff > threshold)
		{
			// reset timer
			t = time(NULL);
		}
		

		// write frame in file
		if(record_in_file==1)
		{
			r=write_frame(recording_fd, "", "--odboundary", ct, cl, img, len);
			goto_error_if_fail(r);
		}


		// write frame in standard output
		if(flags & M_STDOUT)
		{

			// TODO: improve total time
			float usec=0;
			if(fps>0)
				usec = 1000000 * (float)1/fps;
				
			usleep(usec);

			r=write_frame(1, "", boundary, ct, cl, img, len);
			goto_error_if_fail(r);

		}

		frames_cnt++;


		if((flags & M_READ_FROM_FILE) && (flags & M_OFFSET))
			if(p->offset_stop)
			{
				syslog(LOG_DEBUG, "stream stopped\n");
				break;
			}
	}


	close(fd);
	return 0;

error:
	if(stop)
		syslog(LOG_ERR, "Stop signal has been received\n");
	else
		syslog(LOG_ERR, "mjpg_streaming() end\n");

	close(fd);

	// save recording
	if(recording_fd>0)
	{
		syslog(LOG_ERR, "saving recording...\n");

		char str[16];
		float frames = 0;
		if(time(NULL)-frames_tm > 0)
			frames = (float)frames_cnt/(time(NULL)-frames_tm);

		snprintf(str, sizeof(str), "%f", frames);
		write(fps_fd, str, strlen(str));
		close(fps_fd);
		close(recording_fd);
		chmod(path_mjpg, 00777);
		chown(path_mjpg, 1000, 1000);
		chmod(path_jpg, 00777);
		chown(path_jpg, 1000, 1000);
		chmod(path_fps, 00777);
		chown(path_fps, 1000, 1000);
		snprintf(buf, sizeof(buf),
			"/bin/logevent filesave odvision 'Video recorded' %s", path_mjpg);
		system(buf);
		syslog(LOG_NOTICE, "%s\n", buf);
	}
	return -10;

}
// }}}

// {{{ mjpg_verify()
int mjpg_verify(const char *host, const char *url, size_t port, 
				const char *credentials)
{
	
	char line[128];
	int nbytes=0;
	int res = 0;
	
	int sockfd = socket_open(host, url, port, credentials);
	
	if (sockfd <= 0)
		return -1;
	
	/* find mjpeg strings for camera verification */
	while((nbytes = recvline(line, sizeof(line), sockfd))>=0)
	{
		if(strstr(line, "boundary=") || strstr(line, "--BoundaryString"))
		{
			//printf("GOOD:%s\n",line);
			res = 0;
			break;
		}
		else {
			res = -1;
			//printf("BAD:%s\n",line);
			
		}
		
	}
	
	if (nbytes < 0 || res != 0) {
		res = -1;
	}else {
		res = 0;
	}
	
	close(sockfd);
	return res;
}
// }}}

// {{{ mjpg_cam_covered()
int mjpg_cam_covered(const char *host, const char *url, size_t port, 
					 const char *credentials, int threshold, int cnt_imgs)
{
	int sockfd = socket_open(host, url, port, credentials);
	
    if (sockfd <= 0)
		return -1;
	
    char line[128];
	int nbytes=0;
	int len = 0;
	
	/* find initial position */
	while((nbytes = recvline(line, sizeof(line), sockfd))>=0)
	{
		if(strstr(line, "Content-Type") || strstr(line, "--BoundaryString"))
		{
		//	write(1, line, strlen(line));
		//	write(1, "\r\n", 2);
			break;
		}
	}
	return_if_fail(nbytes);
	
	/* image frames */
	image_t *frm1 = NULL;
	int cnt_images=0;

	int cnt = 0;
	
	while(cnt < cnt_imgs)
	{
		char boundary[64] = "";
		char ct[64] = "";
		char cl[64] = "";
		len = find_next_frame(sockfd, boundary, sizeof(boundary), 
				ct, sizeof(ct), cl, sizeof(cl));
		goto_error_if_fail(len);


		/* read image */
		//len += 2;
		char img[len];
		size_t total_bytes = read_raw_image(img, len, sockfd);		
		goto_error_if_fail(total_bytes);
		
		cnt_images++;		
		int res=0;

		frm1 = jpeg_read(img, total_bytes);
		if(!frm1)
			goto error;
		
		/* check if the camera is covered */
		res = image_pixel_diff(frm1, threshold);
		
		/* if the camera is covered we return 1 */
		if(res > 0)
		{
			image_free(frm1);
			close(sockfd);
			return res;
		}
			
		image_free(frm1);

		cnt++;
	}
		
	close(sockfd);
	return 0;

error:
	close(sockfd);
	return -10;
	
}
// }}}





