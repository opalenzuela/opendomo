
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<coelacanth/vector.h>

TYPEDEF_VECTOR(vint, int);
TYPEDEF_VECTOR(vdec, float);
TYPEDEF_VECTOR(vstr, char*);

#define vstr_set_m(vs, i, val) \
	free(vstr_get(vs, 0)); \
	vstr_set(vs, i, val);

void add_int_elements(vint_t *v)
{
	vint_add(v, 1);
	vint_add(v, 2);
	vint_add(v, 3);
	vint_add(v, 4);
}

void add_str_elements(vstr_t *v)
{
	vstr_add(v, strdup("abc"));
	vstr_add(v, strdup("def"));
	vstr_add(v, strdup("ghi"));
}


int main()
{
   int i;

	/* Test int vector */
   vint_t *vi = vint_create();

	add_int_elements(vi);
   vint_add(vi, 5);
   vint_add(vi, 7);
   vint_add(vi, 6);

   vint_set(vi, 0, 11);
   vint_set(vi, 1, 12);
	
	vint_sort(vi);

   for(i=0;i<vint_size(vi); i++)
      printf("%d\n", vint_get(vi, i));
	

   vint_destroy(vi);

	/* Test decimal vector */
   vdec_t *vd = vdec_create();

   vdec_add(vd, 5.4);
   vdec_add(vd, 7.1);
   vdec_add(vd, 6.7);

   vdec_set(vd, 0, 1.1);
   vdec_set(vd, 1, 1.2);
	
   vdec_add(vd, 1.7);
   vdec_add(vd, 5.7);

	vdec_sort(vd);

   for(i=0;i<vdec_size(vd); i++)
      printf("%f\n", vdec_get(vd, i));
	
   vdec_destroy(vd);


	/* Test str vector */
   vstr_t *vs = vstr_create();

	add_str_elements(vs);
   vstr_add(vs, strdup("za"));
   vstr_add(vs, strdup("no"));
   vstr_add(vs, strdup("bi"));

	vstr_set_m(vs, 0, strdup("aarg"));
	//free(vstr_get(vs, 0));	
	//vstr_set(vs, 0, strdup("aarg"));

	vstr_set_cmp_fn(vs, strcmp);
	vstr_sort(vs);

   for(i=0; i<vstr_size(vs); i++)
      printf("%s\n", vstr_get(vs, i));

   for(i=0; i<vstr_size(vs); i++)
		free(vstr_get(vs, i));

   vstr_destroy(vs);

   return 0;
}




