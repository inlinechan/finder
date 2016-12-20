int increment(int v);
int add(int v1, int v2);

int main(int, char *[])
{
  int v1, v2, v3, v4;
  v1 = 10;
  v2 = 15;

  v3 = add(v1, v2);
  v4 = increment(v3);

  return 0;
}

int increment(int v) {
  return v + 1;
}

int add(int v1, int v2) {
  return v1 + v2;
}
