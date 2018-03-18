#include "mdef.h"
#include "op.h"
#include "stringpool.h"

#include "matchc.h"
#include "fnpc.h"

#include "gtm_utf8.h"

void op_fnzpiecediff(mval *arg1, mval *arg2, mval *del, mval *dst) 
{
  const char    *arg1_p, *arg2_p, *delim_p;
  unsigned int    arg1_len, arg2_len, delim_len;
  unsigned int    arg1_i, arg2_i, curr_section, ans_index;
  char    *answer;
  
  DCL_THREADGBL_ACCESS;
	SETUP_THREADGBL_ACCESS;
  
  MV_FORCE_STR(arg1);
  MV_FORCE_STR(arg2);
  MV_FORCE_STR(del);
  
  arg1_len = arg1->str.len;
  arg2_len = arg2->str.len;
  arg1_p = arg1->str.addr;
  arg2_p = arg2->str.addr;
  delim_p = del->str.addr;
  arg1_i = 0;
  arg2_i = 0;
  curr_section = 1;
  ans_index = 0;
  
  ENSURE_STP_FREE_SPACE(arg1_len+arg2_len);
  answer = (char *)stringpool.free;
  
  delim_len = del->str.len;
  if (delim_len == 0) 
  {
    dst->mvtype = MV_STR;
    dst->str.addr = answer;
    dst->str.len = 0;
  }
  else if (delim_len == 1) 
  {
    unsigned int    found_end = 0;
    unsigned int    temp_section, section_len, temp_section_len;
    while (arg1_i < arg1_len || arg2_i < arg2_len)
    {
      if (arg1_i >= arg1_len)
      {
        if (*arg2_p == *delim_p && !found_end)
        {
          arg2_i++;
          arg2_p++;
          curr_section++;
          found_end = 1;
        }
        while (arg2_i < arg2_len)
        {
          if (*arg2_p == *delim_p)
          {
            if (ans_index==0) 
            {
              answer[ans_index] = curr_section + '0';
              ans_index++;
            } else 
            {
              answer[ans_index] = ',';
              ans_index++;
              
              section_len = 0;
              temp_section = curr_section;
              while (temp_section) 
              {
                section_len++;
                temp_section /= 10;
              }
              temp_section_len = section_len;
              temp_section = curr_section;
              while (temp_section_len)
              {
                answer[ans_index+temp_section_len-1] = (temp_section % 10) + '0';
                temp_section /= 10;
                temp_section_len--;
              }
              ans_index += section_len;
            }
            curr_section++;
          }
          arg2_i++;
          arg2_p++;
        }
        if (ans_index==0) 
        {
          answer[ans_index] = curr_section + '0';
          ans_index++;
        } else 
        {
          answer[ans_index] = ',';
          ans_index++;
          
          section_len = 0;
          temp_section = curr_section;
          while (temp_section) 
          {
            section_len++;
            temp_section /= 10;
          }
          temp_section_len = section_len;
          temp_section = curr_section;
          while (temp_section_len)
          {
            answer[ans_index+temp_section_len-1] = (temp_section % 10) + '0';
            temp_section /= 10;
            temp_section_len--;
          }
          ans_index += section_len;
        }
        break;
      }
      if (arg2_i >= arg2_len)
      {
        if (*arg1_p == *delim_p && !found_end)
        {
          arg1_i++;
          arg1_p++;
          curr_section++;
          found_end = 1;
        }
        while (arg1_i < arg1_len)
        {
          if (*arg1_p == *delim_p)
          {
            if (ans_index==0) 
            {
              answer[ans_index] = curr_section + '0';
              ans_index++;
            } else 
            {
              answer[ans_index] = ',';
              ans_index++;
              
              section_len = 0;
              temp_section = curr_section;
              while (temp_section) 
              {
                section_len++;
                temp_section /= 10;
              }
              temp_section_len = section_len;
              temp_section = curr_section;
              while (temp_section_len)
              {
                answer[ans_index+temp_section_len-1] = (temp_section % 10) + '0';
                temp_section /= 10;
                temp_section_len--;
              }
              ans_index += section_len;
            }
            curr_section++;
          }
          arg1_i++;
          arg1_p++;
        }
        if (ans_index==0) 
        {
          answer[ans_index] = curr_section + '0';
          ans_index++;
        } else 
        {
          answer[ans_index] = ',';
          ans_index++;
          
          section_len = 0;
          temp_section = curr_section;
          while (temp_section) 
          {
            section_len++;
            temp_section /= 10;
          }
          temp_section_len = section_len;
          temp_section = curr_section;
          while (temp_section_len)
          {
            answer[ans_index+temp_section_len-1] = (temp_section % 10) + '0';
            temp_section /= 10;
            temp_section_len--;
          }
          ans_index += section_len;
        }
        break;
      }
      
      if (*arg1_p != *arg2_p)
      {
        if (ans_index==0) 
        {
          answer[ans_index] = curr_section + '0';
          ans_index++;
        } else 
        {
          answer[ans_index] = ',';
          ans_index++;
          
          section_len = 0;
          temp_section = curr_section;
          while (temp_section) 
          {
            section_len++;
            temp_section /= 10;
          }
          temp_section_len = section_len;
          temp_section = curr_section;
          while (temp_section_len)
          {
            answer[ans_index+temp_section_len-1] = (temp_section % 10) + '0';
            temp_section /= 10;
            temp_section_len--;
          }
          ans_index += section_len;
        }
        curr_section++;
        while (*arg1_p != *delim_p && arg1_i < arg1_len) {
          arg1_p++;
          arg1_i++;
        }
        arg1_p++;
        arg1_i++;
        while (*arg2_p != *delim_p && arg2_i < arg2_len) {
          arg2_p++;
          arg2_i++;
        }
        arg2_p++;
        arg2_i++;
        if (arg1_i >= arg1_len && *arg2_p == *delim_p) {
          arg2_p--;
          arg2_i--;
          curr_section--;
        }
        if (arg2_i >= arg2_len && *arg1_p == *delim_p) {
          arg1_p--;
          arg1_i--;
          curr_section--;
        }
        continue;
      }
      else
      {
        if (*arg1_p == *delim_p) {
          curr_section++;
        }
        arg1_p++;
        arg2_p++;
        arg1_i++;
        arg2_i++;
        continue;
      }
    }
    dst->mvtype = MV_STR;
    dst->str.addr = answer;
    dst->str.len = ans_index;
    return;
  }
  else
  {
    unsigned int    delim_matches;
    unsigned int    temp_i, temp2_i;
    const char    *temp_p, *temp2_p;
    unsigned int    found_end = 0;
    unsigned int    temp_section, section_len, temp_section_len;
    while (arg1_i < arg1_len || arg2_i < arg2_len) 
    {
      if (arg1_i >= arg1_len)
      {
        delim_matches = 0;
        delim_p = del->str.addr;
        temp_p = arg2_p;
        temp_i = arg2_i;
        while (*temp_p == *delim_p && delim_matches < delim_len && temp_i < arg2_len)
        {
          delim_matches++;
          temp_i++;
          temp_p++;
          delim_p++;
        }
        
        if (delim_matches == delim_len && !found_end)
        {
          arg2_i += delim_len;
          arg2_p += delim_len;
          curr_section++;
          found_end = 1;
        }
        while (arg2_i < arg2_len)
        {
          delim_matches = 0;
          delim_p = del->str.addr;
          temp_p = arg2_p;
          temp_i = arg2_i;
          while (*temp_p == *delim_p && delim_matches < delim_len && temp_i < arg2_len)
          {
            delim_matches++;
            temp_i++;
            temp_p++;
            delim_p++;
          }
          if (delim_matches == delim_len)
          {
            if (ans_index==0) 
            {
              answer[ans_index] = curr_section + '0';
              ans_index++;
            } else 
            {
              answer[ans_index] = ',';
              ans_index++;
              
              section_len = 0;
              temp_section = curr_section;
              while (temp_section) 
              {
                section_len++;
                temp_section /= 10;
              }
              temp_section_len = section_len;
              temp_section = curr_section;
              while (temp_section_len)
              {
                answer[ans_index+temp_section_len-1] = (temp_section % 10) + '0';
                temp_section /= 10;
                temp_section_len--;
              }
              ans_index += section_len;
            }
            curr_section++;
            arg2_i += delim_len;
            arg2_p += delim_len;
            continue;
          }
          arg2_i++;
          arg2_p++;
        }
        if (ans_index==0) 
        {
          answer[ans_index] = curr_section + '0';
          ans_index++;
        } else 
        {
          answer[ans_index] = ',';
          ans_index++;
          
          section_len = 0;
          temp_section = curr_section;
          while (temp_section) 
          {
            section_len++;
            temp_section /= 10;
          }
          temp_section_len = section_len;
          temp_section = curr_section;
          while (temp_section_len)
          {
            answer[ans_index+temp_section_len-1] = (temp_section % 10) + '0';
            temp_section /= 10;
            temp_section_len--;
          }
          ans_index += section_len;
        }
        break;
      }
      
      if (arg2_i >= arg2_len)
      {
        delim_matches = 0;
        delim_p = del->str.addr;
        temp_p = arg1_p;
        temp_i = arg1_i;
        while (*temp_p == *delim_p && delim_matches < delim_len && temp_i < arg1_len)
        {
          delim_matches++;
          temp_i++;
          temp_p++;
          delim_p++;
        }
        
        if (delim_matches == delim_len && !found_end)
        {
          arg1_i += delim_len;
          arg1_p += delim_len;
          curr_section++;
          found_end = 1;
        }
        while (arg1_i < arg1_len)
        {
          delim_matches = 0;
          delim_p = del->str.addr;
          temp_p = arg1_p;
          temp_i = arg1_i;
          while (*temp_p == *delim_p && delim_matches < delim_len && temp_i < arg1_len)
          {
            delim_matches++;
            temp_i++;
            temp_p++;
            delim_p++;
          }
          if (delim_matches == delim_len)
          {
            if (ans_index==0) 
            {
              answer[ans_index] = curr_section + '0';
              ans_index++;
            } else 
            {
              answer[ans_index] = ',';
              ans_index++;
              
              section_len = 0;
              temp_section = curr_section;
              while (temp_section) 
              {
                section_len++;
                temp_section /= 10;
              }
              temp_section_len = section_len;
              temp_section = curr_section;
              while (temp_section_len)
              {
                answer[ans_index+temp_section_len-1] = (temp_section % 10) + '0';
                temp_section /= 10;
                temp_section_len--;
              }
              ans_index += section_len;
            }
            curr_section++;
            arg1_i += delim_len;
            arg1_p += delim_len;
            continue;
          }
          arg1_i++;
          arg1_p++;
        }
        if (ans_index==0) 
        {
          answer[ans_index] = curr_section + '0';
          ans_index++;
        } else 
        {
          answer[ans_index] = ',';
          ans_index++;
          
          section_len = 0;
          temp_section = curr_section;
          while (temp_section) 
          {
            section_len++;
            temp_section /= 10;
          }
          temp_section_len = section_len;
          temp_section = curr_section;
          while (temp_section_len)
          {
            answer[ans_index+temp_section_len-1] = (temp_section % 10) + '0';
            temp_section /= 10;
            temp_section_len--;
          }
          ans_index += section_len;
        }
        break;
      }
      
      if (*arg1_p != *arg2_p)
      {
        if (ans_index==0) 
        {
          answer[ans_index] = curr_section + '0';
          ans_index++;
        } else 
        {
          answer[ans_index] = ',';
          ans_index++;
          
          section_len = 0;
          temp_section = curr_section;
          while (temp_section) 
          {
            section_len++;
            temp_section /= 10;
          }
          temp_section_len = section_len;
          temp_section = curr_section;
          while (temp_section_len)
          {
            answer[ans_index+temp_section_len-1] = (temp_section % 10) + '0';
            temp_section /= 10;
            temp_section_len--;
          }
          ans_index += section_len;
        }
        curr_section++;
        
        delim_matches = 0;
        delim_p = del->str.addr;
        while (delim_matches < delim_len && arg1_i < arg1_len)
        {
          if (*arg1_p == *delim_p)
          {
            delim_matches++;
            delim_p++;
          } else 
          {
            delim_matches = 0;
            delim_p = del->str.addr;
          }
          arg1_i++;
          arg1_p++;
        }
        
        delim_matches = 0;
        delim_p = del->str.addr;
        while (delim_matches < delim_len && arg2_i < arg1_len)
        {
          if (*arg2_p == *delim_p)
          {
            delim_matches++;
            delim_p++;
          } else 
          {
            delim_matches = 0;
            delim_p = del->str.addr;
          }
          arg2_i++;
          arg2_p++;
        }
        
        
        if (arg1_i >= arg1_len)
        {
          delim_matches = 0;
          delim_p = del->str.addr;
          temp_p = arg2_p;
          temp_i = arg2_i;
          while (*temp_p == *delim_p && delim_matches < delim_len && temp_i < arg1_len)
          {
            delim_matches++;
            temp_i++;
            temp_p++;
            delim_p++;
          }
          if (delim_matches == delim_len)
          {
            arg2_p--;
            arg2_i--;
            curr_section--;
          }
        }
        
        if (arg2_i >= arg2_len)
        {
          delim_matches = 0;
          delim_p = del->str.addr;
          temp_p = arg1_p;
          temp_i = arg1_i;
          while (*temp_p == *delim_p && delim_matches < delim_len && temp_i < arg1_len)
          {
            delim_matches++;
            temp_i++;
            temp_p++;
            delim_p++;
          }
          if (delim_matches == delim_len)
          {
            arg1_p--;
            arg1_i--;
            curr_section--;
          }
        }
        continue;
      }
      else
      {
        delim_matches = 0;
        delim_p = del->str.addr;
        temp_p = arg1_p;
        temp_i = arg1_i;
        temp2_p = arg2_p;
        temp2_i = arg2_i;
        while (*temp_p == *delim_p && *temp_p == *temp2_p && temp_i < arg1_len && temp2_i < arg2_len)
        {
          delim_matches++;
          delim_p++;
          temp_i++;
          temp_p++;
          temp2_i++;
          temp2_p++;
        }
        if (delim_matches == delim_len)
        {
          arg1_i = temp_i;
          arg1_p = temp_p;
          arg2_i = temp2_i;
          arg2_p = temp2_p;
          curr_section++;
        } else 
        {
          arg1_p++;
          arg1_i++;
          arg2_p++;
          arg2_i++;
        }
        continue;
      }
    }
    dst->mvtype = MV_STR;
    dst->str.addr = answer;
    dst->str.len = ans_index;
    return;
  }
  
}
