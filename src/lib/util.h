namespace libsac {

int inline clamp(int x) {
  if (x < -32768)
    return -32768;
  if (x > 32767)
    return 32767;
  return x;
}

} // namespace libsac

