#include "carsvm.h"
#include "svm.h"
#include <ctype.h>
#include <dlfcn.h>
#include <errno.h>

struct 	svm_model* model;
struct 	svm_node *x;
struct 	svm_model* (*load_model)(char *name);
double 	(*predict)(const struct svm_model *model, const struct svm_node *x);
int 	(*check_probability_model)(const struct svm_model *model);
int 	max_nr_attr = 64;

cJSON *data,*parameter,*tag;
/*
   return 0: failed.
   return 1: success.
   */
int gsh_formula_carsvm_init(void *arg,void *ret)
{
		char *name = "./formula/carsvm/lib/libsvm.so.2";
		void *handle = dlopen(name,RTLD_LAZY);
		if (!handle) 
		{
				fprintf(stderr, "%s\n", dlerror());
				return 0;
		}

		if(!(load_model=dlsym(handle,"svm_load_model")))
		{
				fprintf(stderr,"export load_model failed.\r\n");
				return 0;
		}
		if(!(check_probability_model = dlsym(handle,"svm_check_probability_model")))
		{
				fprintf(stderr,"export check_probability_model failed.\r\n");
				return 0;
		}
		if(!(predict = dlsym(handle,"svm_predict")))
		{
				fprintf(stderr,"export svm_predict failed.\r\n");
				return 0;
		}

		if(!(model=load_model("./formula/carsvm/data/car.train.model")))
		{
				fprintf(stderr,"export load_model failed.\r\n");
				return 0;
		}
		x = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));
		if(!x)
		{
				fprintf(stderr,"malloc svm_node failed.\r\n");
				return 0;
		}
		if(check_probability_model(model)!=0)
				fprintf(stderr,"Model supports probability estimates, but disabled in prediction.\n");
		return 1;
}

int gsh_formula_carsvm_run(void *arg,void *ret)
{
		data = (cJSON*)arg;
		parameter = cJSON_GetObjectItem(data,"parameter");
		if(!parameter || parameter->type!=cJSON_String)
		{
				fprintf(stderr,"parameter is null.\r\n");
				goto err;
		}
		tag = cJSON_GetObjectItem(data,"tag");
		if(!tag || tag->type!=cJSON_String)
		{
				fprintf(stderr,"tag is null.\r\n");
				goto err;
		}
		char *line = (char*)parameter->valuestring;
		int i = 0;
		double target_label, predict_label;
		char *idx, *val, *label, *endptr;
		int inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0

		label = strtok(line," \t\n");
		if(label == NULL) // empty line
				goto err;

		target_label = strtod(label,&endptr);
		if(endptr == label || *endptr != '\0')
				goto err;

		while(1)
		{
				if(i>=max_nr_attr-1)	// need one more for index = -1
				{
						max_nr_attr *= 2;
						x = (struct svm_node *) realloc(x,max_nr_attr*sizeof(struct svm_node));
				}

				idx = strtok(NULL,":");
				val = strtok(NULL," \t");

				if(val == NULL)
						break;
				errno = 0;
				x[i].index = (int)strtol(idx,&endptr,10);
				if(endptr == idx || errno != 0 || *endptr != '\0' || x[i].index <= inst_max_index)
						goto err;
				else
						inst_max_index = x[i].index;

				errno = 0;
				x[i].value = strtod(val,&endptr);
				if(endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
						goto err;

				++i;
		}
		x[i].index = -1;

		predict_label = predict(model,x);
		int len;
		len  = sprintf(ret,"%g",predict_label);
		if( len >= FORMULA_BUFLEN)
				goto err;
		return 1;
err:
		return 0;
}

