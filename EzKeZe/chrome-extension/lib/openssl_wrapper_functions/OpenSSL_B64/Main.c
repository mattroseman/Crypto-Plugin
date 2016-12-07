#include <stdio.h>
#include <string.h>

int main() {
  //Encode To Base64
  char* base64EncodeOutput, *text="Hello World";

  Base64Encode(text, strlen(text), &base64EncodeOutput);
  printf("Output (base64): %s\n", base64EncodeOutput);

  //Decode From Base64
  char* base64DecodeOutput;
  size_t test;
  Base64Decode("SGVsbG8gV29ybGQ=", &base64DecodeOutput, &test);
  printf("Output: %s %d\n", base64DecodeOutput, test);
  
  return(0);
}
