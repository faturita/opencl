#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <OpenCL/opencl.h>

#define MAX_SOURCE_SIZE 512
#define VECTOR_SIZE 1024


#define CL_CHECK(status) \
  if (status != 0) {       \
    std::cout << "OPENCL ERROR CODE:" << status << std::endl; \
    exit(1); \
  }

#define FREE(p)   \
    if (!p) {     \
      free(p);    \
      p = NULL;   \
    }

void displayPlatformInfo(cl_platform_id id,
                         cl_platform_info param_name,
                         const std::string& param_name_str) {
  cl_int cl_status = 0;
  size_t param_size = 0;

  cl_status = clGetPlatformInfo(/*id=*/0, param_name, 0, NULL, &param_size);
  char* some_info = (char*)malloc(sizeof(char) * param_size);
  cl_status = clGetPlatformInfo(id, param_name, param_size, some_info, NULL);
  if (cl_status != CL_SUCCESS) {
    std::cout << "Unable to find any OpenCL platform information about " << param_name_str << std::endl;
    FREE(some_info);
    CL_CHECK(cl_status);
  }
  std::cout << param_name_str << ": " << some_info << std::endl;
  FREE(some_info);
}
/*
platforms_num:1
CL_PLATFORM_PROFILE: FULL_PROFILE
CL_PLATFORM_VERSION: OpenCL 1.2 (Feb 22 2019 20:16:07)
CL_PLATFORM_NAME: Apple
CL_PLATFORM_VENDOR: Apple
CL_PLATFORM_EXTENSIONS: cl_APPLE_SetMemObjectDestructor cl_APPLE_ContextLoggingFunctions cl_APPLE_clut cl_APPLE_query_kernel_names cl_APPLE_gl_sharing cl_khr_gl_event
*/

//*
void displayDevicesDetails(cl_device_id id, cl_device_info param_name, const std::string& param_name_str) {
  cl_int cl_status = 0;
  size_t param_size = 0;

  cl_status = clGetDeviceInfo(id, param_name, 0, NULL, &param_size);
  CL_CHECK(cl_status);

  switch(param_name) {
    case CL_DEVICE_TYPE: {
      cl_device_type* dtype = (cl_device_type*) malloc(sizeof(cl_device_type) * param_size);
      cl_status = clGetDeviceInfo(id, param_name, param_size, dtype, NULL);
      CL_CHECK(cl_status);
      switch(*dtype) {
        case CL_DEVICE_TYPE_CPU: {
          std::cout << "CPU detected" << std::endl; break; }
        case CL_DEVICE_TYPE_GPU: {
          std::cout << "GPU detected" << std::endl; break; }
        case CL_DEVICE_TYPE_DEFAULT: {
          std::cout << "default detected" << std::endl; break; }
      }
      break;
    }
  }
}


void displayDeviceInfo(cl_platform_id pid, cl_device_type dtype, const std::string dtype_str) {
  // devices
  cl_int cl_status = 0;
  cl_uint devices_num = 0;

  // devices num
  cl_status = clGetDeviceIDs(pid, dtype, 0, NULL, &devices_num);
  CL_CHECK(cl_status);
  std::cout << dtype_str << " devices_num:" << devices_num << std::endl;

  // devices: ids
  cl_device_id* devices = (cl_device_id*) malloc(sizeof(cl_device_id) * devices_num);
  cl_status = clGetDeviceIDs(pid, dtype, devices_num, devices, NULL);
  CL_CHECK(cl_status);

  // devices: detailed info
  for (size_t id = 0; id < devices_num; ++id) {
    displayDevicesDetails(devices[id], CL_DEVICE_TYPE, "CL_DEVICE_TYPE");
  }

  FREE(devices);
}
//*/


int main() {
  std::cout << "hello" << std::endl;

  // platforms
  cl_platform_id* platforms = NULL;
  cl_uint platforms_num = 0;
  cl_uint cl_status = 0;

  // platforms num
#if 0
  cl_status = clGetPlatformIDs(0, NULL, &platforms_num);
  CL_CHECK(cl_status);
  std::cout << "platforms_num:" << platforms_num << std::endl;
#else
  platforms_num = 1;
  // platforms: ids
  platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platforms_num);
  platforms[0] = NULL;
#endif
  for (cl_uint id = 0; id < platforms_num; ++id) {
    displayPlatformInfo(platforms[id], CL_PLATFORM_PROFILE, "CL_PLATFORM_PROFILE");
    displayPlatformInfo(platforms[id], CL_PLATFORM_VERSION, "CL_PLATFORM_VERSION");
    displayPlatformInfo(platforms[id], CL_PLATFORM_NAME, "CL_PLATFORM_NAME");
    displayPlatformInfo(platforms[id], CL_PLATFORM_VENDOR, "CL_PLATFORM_VENDOR");
    displayPlatformInfo(platforms[id], CL_PLATFORM_EXTENSIONS, "CL_PLATFORM_EXTENSIONS");

    displayDeviceInfo(platforms[id], CL_DEVICE_TYPE_CPU, "CL_DEVICE_TYPE_CPU");
    displayDeviceInfo(platforms[id], CL_DEVICE_TYPE_GPU, "CL_DEVICE_TYPE_GPU");
    displayDeviceInfo(platforms[id], CL_DEVICE_TYPE_DEFAULT, "CL_DEVICE_TYPE_DEFAULT");
  }

  // devices

  FREE(platforms);
  return 0;
}

int maind(int argc, char** argv)
{



    float *V1 = (float*)malloc(sizeof (float)*VECTOR_SIZE);
    float *V2 = (float*)malloc(sizeof (float)*VECTOR_SIZE);
    for (int i=0;i<VECTOR_SIZE;i++)
    {
        V1[i] = (float) i / 10;
        V2[i] = -(float) i / 9;
    }

    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("example.cl","r");
    if (!fp)
    {
        fprintf(stderr, "Failed to load kernel\n");
        exit(1);
    }
    source_str = (char*) malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_ALL, 1, &device_id,
                          &ret_num_devices);

    cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);

    cl_command_queue command = clCreateCommandQueue(context, device_id, 0, &ret);

    cl_mem v1_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
                                       VECTOR_SIZE * sizeof(float), NULL, &ret);
    cl_mem v2_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
                                       VECTOR_SIZE * sizeof(float), NULL, &ret);
    cl_mem v3_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                                       VECTOR_SIZE * sizeof(float), NULL, &ret);


    ret = clEnqueueWriteBuffer(command, v1_mem_obj, CL_TRUE, 0,
                               VECTOR_SIZE * sizeof(float), V1, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command, v2_mem_obj, CL_TRUE, 0,
                               VECTOR_SIZE * sizeof(float), V2, 0, NULL, NULL);


    cl_program program = clCreateProgramWithSource(context, 1,
                                                   (const char **)&source_str, (const size_t *)&source_size, &ret);


    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    cl_kernel kernel = clCreateKernel(program, "vector_add", &ret);

    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&v1_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&v2_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&v3_mem_obj);


    size_t global_item_size = VECTOR_SIZE; // Cantidad de Workers a usar
    size_t local_item_size = 64; // Se procesaran el grupos de 64
    ret = clEnqueueNDRangeKernel(command, kernel, 1, NULL, &global_item_size,
                                 &local_item_size, 0, NULL, NULL);


    float *V3 = (float*)malloc(sizeof(float)*VECTOR_SIZE);
    ret = clEnqueueReadBuffer(command, v3_mem_obj, CL_TRUE, 0,VECTOR_SIZE * sizeof(float),
                              V3, 0, NULL, NULL);


    for(int i = 0; i < 100; i++)
        printf("%f + %f = %f\n", V1[i], V2[i], V3[i]);


    ret = clFlush(command);
    ret = clFinish(command);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(v1_mem_obj);
    ret = clReleaseMemObject(v2_mem_obj);
    ret = clReleaseMemObject(v3_mem_obj);
    ret = clReleaseCommandQueue(command);
    ret = clReleaseContext(context);
    free(V1);
    free(V2);
    free(V3);

}
