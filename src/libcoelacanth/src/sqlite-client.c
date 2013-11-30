/*****************************************************************************
 *  This file is part of the OpenDomo project.
 *  Copyright(C) 2011 OpenDomo Services SL
 *
 *  Isidro Catalan Ramos <icatalan@opendomo.com>
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

#define BUFF_SIZE 512

#define QUERY_KO 2
#define END_OF_RESULTSET 1
#define QUERY_OK 0

#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <coelacanth/str.h>

char filename[255];

/**
 * Gets one line from text file
 */
int freadline(char *line, size_t len, FILE * fd)
{
	int read = 0;

	if (!feof(fd))
	{
		fgets(line, len, fd);

		if (feof(fd))
           return 0;

		read = 1;
	}
	return read;
}

/**
 * Check if a file exists
 */
int exists(const char *fname)
{
	FILE *file;
	file = fopen(fname, "r");
	if (file)
	{
		fclose(file);
		return 1;
	}
	return 0;
}

/**
 * Writes a number in a text file
 */
int write_num(const char *fname, int num)
{
	FILE *file;
	file = fopen(fname, "w");
	if (file)
	{
		fprintf(file, "%d\n", num);
		fclose(file);
		return 1;
	}
	return 0;
}

/**
 * Gets the previously stored number
 */
int get_num(const char *fname)
{
	FILE *file;
	char line[64];
	file = fopen(fname, "r");
	int res = 0;

	int n = freadline(line, sizeof(line), file);

	if (n)
	{
		fclose(file);
		res = atoi(line);
		return res;
	}
	return -1;
}

/**
 * Executes a sql statement
 */
int sql_stmt(char * command, char * db_path)
{
	sqlite3 *db;
	sqlite3_stmt *stmt;
	int rc = 0;
	int col, cols;
	sqlite3_open(db_path, &db);
	FILE *file;

	int row_number = 0;

	// prepare the SQL statement from the command line
	rc = sqlite3_prepare_v2(db, command, -1, &stmt, 0);

	if (rc)
	{
		fprintf(stderr, "SQL error: %d : %s\n", rc, sqlite3_errmsg(db));
		return -1;
	}
	else
	{
		cols = sqlite3_column_count(stmt);
		// execute the statement
		if(cols > 0)
		{
			file = fopen(filename, "a+");
		}

		do
		{
			rc = sqlite3_step(stmt);

			switch (rc)
			{
			case SQLITE_DONE:
				break;

			case SQLITE_ROW:

				fprintf(file, "%d:%s\n", row_number, "ROW_DELIM");
				row_number++;

				// print results for this row
				for (col = 0; col < cols; col++)
				{
					const char *txt = (const char*) sqlite3_column_text(stmt,
							col);

					fprintf(file, "%s_COL_SEP_%s\n",
							sqlite3_column_name(stmt, col), txt ? txt : "NULL");

				}

				break;

			default:
				fprintf(stderr, "Error: %d : %s\n", rc, sqlite3_errmsg(db));
				break;
			}

		} while (rc == SQLITE_ROW);

		// finalize the statement to release resources
		sqlite3_finalize(stmt);

		if(cols > 0)
		{
			fclose(file);
		}

	}

	sqlite3_close(db);
	return QUERY_OK;
}


/**
 * Gets a row
 * Process of getting a row:
 * 1- Find /tmp/ID_QUERY.pos
 * 2- Read /tmp/ID_QUERY.pos
 * 3- Locate line on /tmp/ID_QUERY
 * 4- Parse and set in memory values
 * 5- Update /tmp/ID_QUERY.pos
 * 6- If finished, remove both files
 */
int get_row(char * rows_file)
{
	// 1- Find /tmp/ID_QUERY.pos
	char pos_filename[255];
	char rows_filename[255];
	snprintf(pos_filename, sizeof(pos_filename), "/tmp/sql_%s.pos", rows_file);
	snprintf(rows_filename, sizeof(rows_filename), "/tmp/sql_%s", rows_file);

	int current_pos = 0;

	if (!exists(rows_filename))
	{
		//si el resultset no existe nos vamos
		return 2;
	}

	// 2- Read /tmp/ID_QUERY.pos
	if (exists(pos_filename))
	{
		current_pos = get_num(pos_filename);
	}
	else //if not exists we create it and set position 1
	{
		write_num(pos_filename, current_pos);
	}

	//3- Locate line on /tmp/ID_QUERY
	FILE * file;
	char line[BUFF_SIZE];
	int read = -1;
	int n;
	char str_ptr[64];
	//, *field_name, *field_value
	char field_name[BUFF_SIZE],field_value[BUFF_SIZE];
	int position = -1;

	file = fopen(rows_filename, "r");
	memset(line, 0, sizeof(line));
	while ((n = freadline(line, sizeof(line), file)))
	{
		if (strstr(line, "ROW_DELIM"))
		{
			str_split(line, ":", str_ptr, sizeof(str_ptr));
			position = atoi(str_ptr);

			// hemos encontrado el registro al cual apunta el indice
			if (current_pos == position)
			{
				// actualizamos el puntero
				current_pos++;
				write_num(pos_filename, current_pos);

				// leemos hasta encontrar el siguiente delimitador
				memset(line, 0, sizeof(line));

				while ((n = freadline(line, sizeof(line), file)))
				{
					if(strstr(line, "_COL_SEP_"))
					{
						read = 1;
						char *p = str_split(line, "_COL_SEP_", field_name, sizeof(field_name));
						strncpy(field_value, p, sizeof(field_value));
						// mostramos el valor para pasarselo al eval
						str_trim(field_value);
						printf("%s='%s';", field_name, field_value);
					}

					//cuando llegamos a la siguiente linea, salimos
					if (strstr(line, "ROW_DELIM"))
					{
						read = 1;
						break;
					}
					else
					{
						read = -1;
					}
				}
			}
		}
		memset(line, 0, sizeof(line));
	}


	// si no hemos obtenido informacion, eliminamos el fichero
	// miramos de eliminar el fichero si se trata del ultimo registro
	if (read == -1)
	{
		unlink(rows_filename);
		unlink(pos_filename);
		return END_OF_RESULTSET;
	}

	fclose(file);

	return QUERY_OK;
}


/**
 * MAIN
 */
int main(int argc, char *argv[])
{
	int res = 0;
	char random_value[128];
	memset(filename, 0, sizeof(filename));

	if (argc < 3)
	{
		/* We print argv[0] assuming it is the program name */
		printf("usage: %s [options] [QUERY] [sqlitedb]", argv[0]);
	}
	else
	{
		int opt = *argv[1];

		switch (opt)
		{
		case 'q':


			// guardamos el nombre aleatorio generado
			str_random(random_value, sizeof(random_value), 20,
					"abcdefghijklmnopqrstuvwxyz123456789");


			snprintf(filename, sizeof(filename), "/tmp/sql_%s", random_value);
			while(exists(filename))
			{
				// guardamos el nombre aleatorio generado
				str_random(random_value, sizeof(random_value), 20,
						"abcdefghijklmnopqrstuvwxyz123456789");
				snprintf(filename, sizeof(filename), "/tmp/sql_%s", random_value);
				usleep(5000);
			}
			res = sql_stmt(argv[2], argv[3]);

			// una vez finalizado mostramos el nombre del fichero
			// donde se ha almacenado la query

			if(res == QUERY_OK)
			{
				printf("%s\n", random_value);
			}
			else
			{
				return QUERY_KO;
			}
			break;

		case 'r':
			res = get_row(argv[2]);
			break;
		}
	}

	return res;
}
