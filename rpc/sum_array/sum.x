struct singal_array
{
   int size;
   int *array;
};

struct double_array
{
   int size;
   int *array1;
   int *array2;
};

program ARRAYSUM {
  version ARRAYSUM_1 {
    singal_array SUM_ARRAY(double_array)=1;
  }=1;
} = 0x0000001; 
