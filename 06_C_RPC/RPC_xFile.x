const DIM = 5;

struct Input{
  char a;
  int b;
};

struct Output{
  char a[DIM];
};


program PROGRAM {
  version PROGRAMVERS{
    int  FUNZ_UNO(Input) = 1;
    Output FUNZ_DUE(void) = 2;
  } = 1;
} = 0x20000013;