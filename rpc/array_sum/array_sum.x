struct data{
  int *array;
  int *array2;
  int size;
};

program ARRAYSUM {
  version ARRAYSUM_1 {
    data ARRAY_SUM(data)=1;
  }=1;
} = 0x0000001;
