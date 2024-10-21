int foo() { return 42; }
int bar() { return 0; }
int main(int argc, char *argv[]) {
  return (argc % 2 == 0) ? foo() : bar();
}
