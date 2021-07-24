/* GCC needs this defined.*/
extern int LLVMFuzzerInitialize(int* argc, char*** argv) {
  (void)argc;
  (void)argv;
  return 0;
}
