#include <iostream>

int *z_function(char *text, const int &len) {
  int *zf = (int *)malloc(len * sizeof(int));
  zf[0] = 0;
  int lb = 0, rb = 0;
  for (int i = 1; i < len; ++i) {
    zf[i] = std::max(std::min(zf[i - lb], rb - i), 0);
    while (zf[i] + i < len && text[zf[i]] == text[zf[i] + i])
      ++zf[i];
    if (zf[i] + i > rb) {
      rb = zf[i] + i;
      lb = i;
    }
  }
  return zf;
}

void scan_string(char **str, int *len) {
  int capacity = *len + 1;
  char c = getchar();
  while (c != '\n') {
    (*str)[(*len)++] = c;
    if (*len >= capacity) {
      capacity *= 2;
      (*str) = (char*) realloc((*str), (capacity + 1) * sizeof(char)); 
    }
    c = getchar();        
  }
}

char *get_z_string(int *len, int *p_len) {
  *len = 0, *p_len = 0;
  char *str = (char*) malloc(sizeof(char)), *new_str = (char*) malloc(sizeof(char));
  scan_string(&str, len);
  scan_string(&new_str, p_len);
  new_str = (char*) realloc(new_str, (*p_len + *len + 2) * sizeof(char));
  new_str[(*p_len)] = '$';
  for(int i = *p_len + 1, i_s = 0; i < *p_len + 1 + *len; ++i, ++i_s) {
    new_str[i] = str[i_s];
  }
  *len += *p_len + 1;
  new_str[*len] = '\0';
  return new_str; 
}

int main() {
  int string_len, pattern_len;
  char *text = get_z_string(&string_len, &pattern_len);
  int *z_array = z_function(text, string_len);
  for (int i = pattern_len + 1; i < string_len; ++i) {
    if (z_array[i] == pattern_len) {
      std::cout << i - pattern_len - 1 << "\n";
    }
  }
  return 0;
}