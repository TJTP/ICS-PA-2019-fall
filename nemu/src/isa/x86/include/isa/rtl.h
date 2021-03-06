#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include "rtl/rtl.h"

/* RTL pseudo instructions */

static inline void rtl_lr(rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(dest, &reg_l(r)); return;
    case 1: rtl_host_lm(dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline void rtl_sr(int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(&reg_l(r), src1); return;
    case 1: rtl_host_sm(&reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(&reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

static inline void rtl_push(const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  //TODO();
  cpu.esp -=4;
  //vaddr_write(cpu.esp,*src1,4);
  rtl_sm(&cpu.esp,src1,4);
  
}

static inline void rtl_pop(rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  //TODO();
  //*dest = vaddr_read(cpu.esp, 4); 
  rtl_lm(dest,&cpu.esp,4);
  cpu.esp += 4;

}

static inline void rtl_is_sub_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  //TODO();
  //判断无符号数进行减法后的差的最高位与被减数的最高位是否相同
  //rtl_msb(&t0,res,width);
  //rtl_msb(&t1,src1,width);
  //if (t0 != t1) *dest = 1;
  //else *dest = 0;
  rtl_xor(&t0,src1,src2);
  rtl_xor(&t1,src1,res);
  rtl_and(&t0,&t0,&t1);
  rtl_msb(dest,&t0,width);
}

static inline void rtl_is_sub_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 - src2)
  //TODO();
  rtl_setrelop(RELOP_LTU,dest,src1,res);
}

static inline void rtl_is_add_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  //TODO();
  rtl_xor(&t0,src1,src2);
  rtl_not(&t0,&t0);
  rtl_xor(&t1,src1,res);
  rtl_and(&t0,&t0,&t1);
  rtl_msb(dest,&t0,width);
}

static inline void rtl_is_add_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  //TODO();
  rtl_setrelop(RELOP_GTU,dest,src1,res);
}

#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (const rtlreg_t* src) { \
    /*TODO();*/ cpu.eflags.f = *src;\
  } \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    /*TODO();*/ *dest = cpu.eflags.f;\
  }

make_rtl_setget_eflags(CF)
make_rtl_setget_eflags(OF)
make_rtl_setget_eflags(ZF)
make_rtl_setget_eflags(SF)

static inline void rtl_update_ZF(const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  //TODO();
  rtl_shli(&t1,result,32-width*8);
  if (t1 == 0) cpu.eflags.ZF = 1;
  else cpu.eflags.ZF = 0;
  //rtl_set_ZF(&t1);
  //if (width*8-10000)
}

static inline void rtl_update_SF(const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  //TODO();
  rtl_msb(&t0,result,width);
  cpu.eflags.SF = t0;
}

static inline void rtl_update_ZFSF(const rtlreg_t* result, int width) {
  rtl_update_ZF(result, width);
  rtl_update_SF(result, width);
}

#endif
