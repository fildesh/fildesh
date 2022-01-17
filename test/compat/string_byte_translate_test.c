
#include "fildesh_compat_string.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char* s;
  const char xml_needles[] = "&<>\"'";
  const char* const xml_replacements[] = {
    "&amp;", "&lt;", "&gt;", "&quot;", "&apos;",
  };
  s = fildesh_compat_string_byte_translate(
      "Student's code: \"x > z && x < y\"", 
      xml_needles, xml_replacements,
      "<example>", "</example>");
  fprintf(stderr, "Got: %s\n", s);
  assert(0 == strcmp(s, "<example>Student&apos;s code: &quot;x &gt; z &amp;&amp; x &lt; y&quot;</example>"));
  free(s);
  return 0;
}

