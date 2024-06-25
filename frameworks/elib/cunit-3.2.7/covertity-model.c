void deallocate_memory(void *ptr, unsigned int uiLine, const char* szFileName)
{
  __coverity_escape__(ptr);
}

void CU_free(void *ptr)
{
  __coverity_escape__(ptr);
}
