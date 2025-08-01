/*
** LuaJIT VM tags, values and objects.
** Copyright (C) 2005-2025 Mike Pall. See Copyright Notice in luajit.h
**
** Portions taken verbatim or adapted from the Lua interpreter.
** Copyright (C) 1994-2008 Lua.org, PUC-Rio. See Copyright Notice in lua.h
*/

#ifndef _LJ_OBJ_H
#define _LJ_OBJ_H

#include "lua.h"
#include "lj_def.h"
#include "lj_arch.h"

/* -- Memory references --------------------------------------------------- */

/* Memory and GC object sizes. */
typedef uint32_t MSize;
#if LJ_GC64
typedef uint64_t GCSize;
#else
typedef uint32_t GCSize;
#endif

/* Memory reference */
typedef struct MRef {
#if LJ_GC64
  uint64_t ptr64;	/* True 64 bit pointer. */
#else
  uint32_t ptr32;	/* Pseudo 32 bit pointer. */
#endif
} MRef;

#if LJ_GC64
#define mref(r, t)	((t *)(void *)(r).ptr64)
#define mrefu(r)	((r).ptr64)

#define setmref(r, p)	((r).ptr64 = (uint64_t)(void *)(p))
#define setmrefu(r, u)	((r).ptr64 = (uint64_t)(u))
#define setmrefr(r, v)	((r).ptr64 = (v).ptr64)
#else
#define mref(r, t)	((t *)(void *)(uintptr_t)(r).ptr32)
#define mrefu(r)	((r).ptr32)

#define setmref(r, p)	((r).ptr32 = (uint32_t)(uintptr_t)(void *)(p))
#define setmrefu(r, u)	((r).ptr32 = (uint32_t)(u))
#define setmrefr(r, v)	((r).ptr32 = (v).ptr32)
#endif

/* -- GC object references ------------------------------------------------ */

/* GCobj reference */
typedef struct GCRef {
#if LJ_GC64
  uint64_t gcptr64;	/* True 64 bit pointer. */
#else
  uint32_t gcptr32;	/* Pseudo 32 bit pointer. */
#endif
} GCRef;

/* Common GC header for all collectable objects. */
#define GCHeader	GCRef nextgc; uint8_t marked; uint8_t gct
/* This occupies 6 bytes, so use the next 2 bytes for non-32 bit fields. */

#if LJ_GC64
#define gcref(r)	((GCobj *)(r).gcptr64)
#define gcrefp(r, t)	((t *)(void *)(r).gcptr64)
#define gcrefu(r)	((r).gcptr64)
#define gcrefeq(r1, r2)	((r1).gcptr64 == (r2).gcptr64)

#define setgcref(r, gc)	((r).gcptr64 = (uint64_t)&(gc)->gch)
#define setgcreft(r, gc, it) \
  (r).gcptr64 = (uint64_t)&(gc)->gch | (((uint64_t)(it)) << 47)
#define setgcrefp(r, p)	((r).gcptr64 = (uint64_t)(p))
#define setgcrefnull(r)	((r).gcptr64 = 0)
#define setgcrefr(r, v)	((r).gcptr64 = (v).gcptr64)
#else
#define gcref(r)	((GCobj *)(uintptr_t)(r).gcptr32)
#define gcrefp(r, t)	((t *)(void *)(uintptr_t)(r).gcptr32)
#define gcrefu(r)	((r).gcptr32)
#define gcrefeq(r1, r2)	((r1).gcptr32 == (r2).gcptr32)

#define setgcref(r, gc)	((r).gcptr32 = (uint32_t)(uintptr_t)&(gc)->gch)
#define setgcrefp(r, p)	((r).gcptr32 = (uint32_t)(uintptr_t)(p))
#define setgcrefnull(r)	((r).gcptr32 = 0)
#define setgcrefr(r, v)	((r).gcptr32 = (v).gcptr32)
#endif

#define gcnext(gc)	(gcref((gc)->gch.nextgc))

/* IMPORTANT NOTE:
**
** All uses of the setgcref* macros MUST be accompanied with a write barrier.
**
** This is to ensure the integrity of the incremental GC. The invariant
** to preserve is that a black object never points to a white object.
** I.e. never store a white object into a field of a black object.
**
** It's ok to LEAVE OUT the write barrier ONLY in the following cases:
** - The source is not a GC object (NULL).
** - The target is a GC root. I.e. everything in global_State.
** - The target is a lua_State field (threads are never black).
** - The target is a stack slot, see setgcV et al.
** - The target is an open upvalue, i.e. pointing to a stack slot.
** - The target is a newly created object (i.e. marked white). But make
**   sure nothing invokes the GC inbetween.
** - The target and the source are the same object (self-reference).
** - The target already contains the object (e.g. moving elements around).
**
** The most common case is a store to a stack slot. All other cases where
** a barrier has been omitted are annotated with a NOBARRIER comment.
**
** The same logic applies for stores to table slots (array part or hash
** part). ALL uses of lj_tab_set* require a barrier for the stored value
** *and* the stored key, based on the above rules. In practice this means
** a barrier is needed if *either* of the key or value are a GC object.
**
** It's ok to LEAVE OUT the write barrier in the following special cases:
** - The stored value is nil. The key doesn't matter because it's either
**   not resurrected or lj_tab_newkey() will take care of the key barrier.
** - The key doesn't matter if the *previously* stored value is guaranteed
**   to be non-nil (because the key is kept alive in the table).
** - The key doesn't matter if it's guaranteed not to be part of the table,
**   since lj_tab_newkey() takes care of the key barrier. This applies
**   trivially to new tables, but watch out for resurrected keys. Storing
**   a nil value leaves the key in the table!
**
** In case of doubt use lj_gc_anybarriert() as it's rather cheap. It's used
** by the interpreter for all table stores.
**
** Note: In contrast to Lua's GC, LuaJIT's GC does *not* specially mark
** dead keys in tables. The reference is left in, but it's guaranteed to
** be never dereferenced as long as the value is nil. It's ok if the key is
** freed or if any object subsequently gets the same address.
**
** Not destroying dead keys helps to keep key hash slots stable. This avoids
** specialization back-off for HREFK when a value flips between nil and
** non-nil and the GC gets in the way. It also allows safely hoisting
** HREF/HREFK across GC steps. Dead keys are only removed if a table is
** resized (i.e. by NEWREF) and xREF must not be CSEd across a resize.
**
** The trade-off is that a write barrier for tables must take the key into
** account, too. Implicitly resurrecting the key by storing a non-nil value
** may invalidate the incremental GC invariant.
*/

/* -- Common type definitions --------------------------------------------- */

/* Types for handling bytecodes. Need this here, details in lj_bc.h. */
typedef uint32_t BCIns;  /* Bytecode instruction. */
typedef uint32_t BCPos;  /* Bytecode position. */
typedef uint32_t BCReg;  /* Bytecode register. */
typedef int32_t BCLine;  /* Bytecode line number. */

/* Internal assembler functions. Never call these directly from C. */
typedef void (*ASMFunction)(void);

/* Resizable string buffer. Need this here, details in lj_buf.h. */
#define SBufHeader	char *w, *e, *b; MRef L
typedef struct SBuf {
  SBufHeader;
} SBuf;

/* -- Tags and values ----------------------------------------------------- */

/* Frame link. */
typedef union {
  int32_t ftsz;		/* Frame type and size of previous frame. */
  MRef pcr;		/* Or PC for Lua frames. */
} FrameLink;

/* Tagged value. */
typedef LJ_ALIGN(8) union TValue {
  uint64_t u64;		/* 64 bit pattern overlaps number. */
  lua_Number n;		/* Number object overlaps split tag/value object. */
#if LJ_GC64
  GCRef gcr;		/* GCobj reference with tag. */
  int64_t it64;
  struct {
    LJ_ENDIAN_LOHI(
      int32_t i;	/* Integer value. */
    , uint32_t it;	/* Internal object tag. Must overlap MSW of number. */
    )
  };
#else
  struct {
    LJ_ENDIAN_LOHI(
      union {
	GCRef gcr;	/* GCobj reference (if any). */
	int32_t i;	/* Integer value. */
      };
    , uint32_t it;	/* Internal object tag. Must overlap MSW of number. */
    )
  };
#endif
#if LJ_FR2
  int64_t ftsz;		/* Frame type and size of previous frame, or PC. */
#else
  struct {
    LJ_ENDIAN_LOHI(
      GCRef func;	/* Function for next frame (or dummy L). */
    , FrameLink tp;	/* Link to previous frame. */
    )
  } fr;
#endif
  struct {
    LJ_ENDIAN_LOHI(
      uint32_t lo;	/* Lower 32 bits of number. */
    , uint32_t hi;	/* Upper 32 bits of number. */
    )
  } u32;
} TValue;

typedef const TValue cTValue;

#define tvref(r)	(mref(r, TValue))

/* More external and GCobj tags for internal objects. */
#define LAST_TT		LUA_TTHREAD
#define LUA_TPROTO	(LAST_TT+1)
#define LUA_TCDATA	(LAST_TT+2)

/* Internal object tags.
**
** Format for 32 bit GC references (!LJ_GC64):
**
** Internal tags overlap the MSW of a number object (must be a double).
** Interpreted as a double these are special NaNs. The FPU only generates
** one type of NaN (0xfff8_0000_0000_0000). So MSWs > 0xfff80000 are available
** for use as internal tags. Small negative numbers are used to shorten the
** encoding of type comparisons (reg/mem against sign-ext. 8 bit immediate).
**
**                  ---MSW---.---LSW---
** primitive types |  itype  |         |
** lightuserdata   |  itype  |  void * |  (32 bit platforms)
** lightuserdata   |ffff|seg|    ofs   |  (64 bit platforms)
** GC objects      |  itype  |  GCRef  |
** int (LJ_DUALNUM)|  itype  |   int   |
** number           -------double------
**
** Format for 64 bit GC references (LJ_GC64):
**
** The upper 13 bits must be 1 (0xfff8...) for a special NaN. The next
** 4 bits hold the internal tag. The lowest 47 bits either hold a pointer,
** a zero-extended 32 bit integer or all bits set to 1 for primitive types.
**
**                     ------MSW------.------LSW------
** primitive types    |1..1|itype|1..................1|
** GC objects         |1..1|itype|-------GCRef--------|
** lightuserdata      |1..1|itype|seg|------ofs-------|
** int (LJ_DUALNUM)   |1..1|itype|0..0|-----int-------|
** number              ------------double-------------
**
** ORDER LJ_T
** Primitive types nil/false/true must be first, lightuserdata next.
** GC objects are at the end, table/userdata must be lowest.
** Also check lj_ir.h for similar ordering constraints.
*/
#define LJ_TNIL			(~0u)
#define LJ_TFALSE		(~1u)
#define LJ_TTRUE		(~2u)
#define LJ_TLIGHTUD		(~3u)
#define LJ_TSTR			(~4u)
#define LJ_TUPVAL		(~5u)
#define LJ_TTHREAD		(~6u)
#define LJ_TPROTO		(~7u)
#define LJ_TFUNC		(~8u)
#define LJ_TTRACE		(~9u)
#define LJ_TCDATA		(~10u)
#define LJ_TTAB			(~11u)
#define LJ_TUDATA		(~12u)
/* This is just the canonical number type used in some places. */
#define LJ_TNUMX		(~13u)

/* Integers have itype == LJ_TISNUM doubles have itype < LJ_TISNUM */
#if LJ_64 && !LJ_GC64
#define LJ_TISNUM		0xfffeffffu
#else
#define LJ_TISNUM		LJ_TNUMX
#endif
#define LJ_TISTRUECOND		LJ_TFALSE
#define LJ_TISPRI		LJ_TTRUE
#define LJ_TISGCV		(LJ_TSTR+1)
#define LJ_TISTABUD		LJ_TTAB

/* Type marker for slot holding a traversal index. Must be lightuserdata. */
#define LJ_KEYINDEX		0xfffe7fffu

#if LJ_GC64
#define LJ_GCVMASK		(((uint64_t)1 << 47) - 1)
#endif

#if LJ_64
/* To stay within 47 bits, lightuserdata is segmented. */
#define LJ_LIGHTUD_BITS_SEG	8
#define LJ_LIGHTUD_BITS_LO	(47 - LJ_LIGHTUD_BITS_SEG)
#endif

/* -- String object ------------------------------------------------------- */

typedef uint32_t StrHash;	/* String hash value. */
typedef uint32_t StrID;		/* String ID. */

/* String object header. String payload follows. */
typedef struct GCstr {
  GCHeader;
  uint8_t reserved;	/* Used by lexer for fast lookup of reserved words. */
  uint8_t hashalg;	/* Hash algorithm. */
  StrID sid;		/* Interned string ID. */
  StrHash hash;		/* Hash of string. */
  MSize len;		/* Size of string. */
} GCstr;

#define strref(r)	(&gcref((r))->str)
#define strdata(s)	((const char *)((s)+1))
#define strdatawr(s)	((char *)((s)+1))
#define strVdata(o)	strdata(strV(o))

/* -- Userdata object ----------------------------------------------------- */

/* Userdata object. Payload follows. */
typedef struct GCudata {
  GCHeader;
  uint8_t udtype;	/* Userdata type. */
  uint8_t unused2;
  GCRef env;		/* Should be at same offset in GCfunc. */
  MSize len;		/* Size of payload. */
  GCRef metatable;	/* Must be at same offset in GCtab. */
  uint32_t align1;	/* To force 8 byte alignment of the payload. */
} GCudata;

/* Userdata types. */
enum {
  UDTYPE_USERDATA,	/* Regular userdata. */
  UDTYPE_IO_FILE,	/* I/O library FILE. */
  UDTYPE_FFI_CLIB,	/* FFI C library namespace. */
  UDTYPE_BUFFER,	/* String buffer. */
  UDTYPE__MAX
};

#define uddata(u)	((void *)((u)+1))
#define sizeudata(u)	(sizeof(struct GCudata)+(u)->len)

/* -- C data object ------------------------------------------------------- */

/* C data object. Payload follows. */
typedef struct GCcdata {
  GCHeader;
  uint16_t ctypeid;	/* C type ID. */
} GCcdata;

/* Prepended to variable-sized or realigned C data objects. */
typedef struct GCcdataVar {
  uint16_t offset;	/* Offset to allocated memory (relative to GCcdata). */
  uint16_t extra;	/* Extra space allocated (incl. GCcdata + GCcdatav). */
  MSize len;		/* Size of payload. */
} GCcdataVar;

#define cdataptr(cd)	((void *)((cd)+1))
#define cdataisv(cd)	((cd)->marked & LJ_GC_ISCDATA)
#define cdatav(cd)	((GCcdataVar *)((char *)(cd) - sizeof(GCcdataVar)))
#define cdatavlen(cd)	check_exp(cdataisv(cd), cdatav(cd)->len)
#define sizecdatav(cd)	(cdatavlen(cd) + cdatav(cd)->extra)
#define memcdatav(cd)	((void *)((char *)(cd) - cdatav(cd)->offset))

/* -- Prototype object ---------------------------------------------------- */

#define SCALE_NUM_GCO	((int32_t)sizeof(lua_Number)/sizeof(GCRef))
#define round_nkgc(n)	(((n) + SCALE_NUM_GCO-1) & ~(SCALE_NUM_GCO-1))

typedef struct GCproto {
  GCHeader;
  uint8_t numparams;	/* Number of parameters. */
  uint8_t framesize;	/* Fixed frame size. */
  MSize sizebc;		/* Number of bytecode instructions. */
#if LJ_GC64
  uint32_t unused_gc64;
#endif
  GCRef gclist;
  MRef k;		/* Split constant array (points to the middle). */
  MRef uv;		/* Upvalue list. local slot|0x8000 or parent uv idx. */
  MSize sizekgc;	/* Number of collectable constants. */
  MSize sizekn;		/* Number of lua_Number constants. */
  MSize sizept;		/* Total size including colocated arrays. */
  uint8_t sizeuv;	/* Number of upvalues. */
  uint8_t flags;	/* Miscellaneous flags (see below). */
  uint16_t trace;	/* Anchor for chain of root traces. */
  /* ------ The following fields are for debugging/tracebacks only ------ */
  GCRef chunkname;	/* Name of the chunk this function was defined in. */
  BCLine firstline;	/* First line of the function definition. */
  BCLine numline;	/* Number of lines for the function definition. */
  MRef lineinfo;	/* Compressed map from bytecode ins. to source line. */
  MRef uvinfo;		/* Upvalue names. */
  MRef varinfo;		/* Names and compressed extents of local variables. */
} GCproto;

/* Flags for prototype. */
#define PROTO_CHILD		0x01	/* Has child prototypes. */
#define PROTO_VARARG		0x02	/* Vararg function. */
#define PROTO_FFI		0x04	/* Uses BC_KCDATA for FFI datatypes. */
#define PROTO_NOJIT		0x08	/* JIT disabled for this function. */
#define PROTO_ILOOP		0x10	/* Patched bytecode with ILOOP etc. */
/* Only used during parsing. */
#define PROTO_HAS_RETURN	0x20	/* Already emitted a return. */
#define PROTO_FIXUP_RETURN	0x40	/* Need to fixup emitted returns. */
/* Top bits used for counting created closures. */
#define PROTO_CLCOUNT		0x20	/* Base of saturating 3 bit counter. */
#define PROTO_CLC_BITS		3
#define PROTO_CLC_POLY		(3*PROTO_CLCOUNT)  /* Polymorphic threshold. */

#define PROTO_UV_LOCAL		0x8000	/* Upvalue for local slot. */
#define PROTO_UV_IMMUTABLE	0x4000	/* Immutable upvalue. */

#define proto_kgc(pt, idx) \
  check_exp((uintptr_t)(intptr_t)(idx) >= ~(uintptr_t)(pt)->sizekgc+1u, \
	    gcref(mref((pt)->k, GCRef)[(idx)]))
#define proto_knumtv(pt, idx) \
  check_exp((uintptr_t)(idx) < (pt)->sizekn, &mref((pt)->k, TValue)[(idx)])
#define proto_bc(pt)		((BCIns *)((char *)(pt) + sizeof(GCproto)))
#define proto_bcpos(pt, pc)	((BCPos)((pc) - proto_bc(pt)))
#define proto_uv(pt)		(mref((pt)->uv, uint16_t))

#define proto_chunkname(pt)	(strref((pt)->chunkname))
#define proto_chunknamestr(pt)	(strdata(proto_chunkname((pt))))
#define proto_lineinfo(pt)	(mref((pt)->lineinfo, const void))
#define proto_uvinfo(pt)	(mref((pt)->uvinfo, const uint8_t))
#define proto_varinfo(pt)	(mref((pt)->varinfo, const uint8_t))

/* -- Upvalue object ------------------------------------------------------ */

typedef struct GCupval {
  GCHeader;
  uint8_t closed;	/* Set if closed (i.e. uv->v == &uv->u.value). */
  uint8_t immutable;	/* Immutable value. */
  union {
    TValue tv;		/* If closed: the value itself. */
    struct {		/* If open: double linked list, anchored at thread. */
      GCRef prev;
      GCRef next;
    };
  };
  MRef v;		/* Points to stack slot (open) or above (closed). */
  uint32_t dhash;	/* Disambiguation hash: dh1 != dh2 => cannot alias. */
} GCupval;

#define uvprev(uv_)	(&gcref((uv_)->prev)->uv)
#define uvnext(uv_)	(&gcref((uv_)->next)->uv)
#define uvval(uv_)	(mref((uv_)->v, TValue))

/* -- Function object (closures) ------------------------------------------ */

/* Common header for functions. env should be at same offset in GCudata. */
#define GCfuncHeader \
  GCHeader; uint8_t ffid; uint8_t nupvalues; \
  GCRef env; GCRef gclist; MRef pc

typedef struct GCfuncC {
  GCfuncHeader;
  lua_CFunction f;	/* C function to be called. */
  TValue upvalue[1];	/* Array of upvalues (TValue). */
} GCfuncC;

typedef struct GCfuncL {
  GCfuncHeader;
  GCRef uvptr[1];	/* Array of _pointers_ to upvalue objects (GCupval). */
} GCfuncL;

typedef union GCfunc {
  GCfuncC c;
  GCfuncL l;
} GCfunc;

#define FF_LUA		0
#define FF_C		1
#define isluafunc(fn)	((fn)->c.ffid == FF_LUA)
#define iscfunc(fn)	((fn)->c.ffid == FF_C)
#define isffunc(fn)	((fn)->c.ffid > FF_C)
#define funcproto(fn) \
  check_exp(isluafunc(fn), (GCproto *)(mref((fn)->l.pc, char)-sizeof(GCproto)))
#define sizeCfunc(n)	(sizeof(GCfuncC)-sizeof(TValue)+sizeof(TValue)*(n))
#define sizeLfunc(n)	(sizeof(GCfuncL)-sizeof(GCRef)+sizeof(GCRef)*(n))

/* -- Table object -------------------------------------------------------- */

/* Hash node. */
typedef struct Node {
  TValue val;		/* Value object. Must be first field. */
  TValue key;		/* Key object. */
  MRef next;		/* Hash chain. */
#if !LJ_GC64
  MRef freetop;		/* Top of free elements (stored in t->node[0]). */
#endif
} Node;

//LJ_STATIC_ASSERT(offsetof(Node, val) == 0);

typedef struct GCtab {
  GCHeader;
  uint8_t nomm;		/* Negative cache for fast metamethods. */
  int8_t colo;		/* Array colocation. */
  MRef array;		/* Array part. */
  GCRef gclist;
  GCRef metatable;	/* Must be at same offset in GCudata. */
  MRef node;		/* Hash part. */
  uint32_t asize;	/* Size of array part (keys [0, asize-1]). */
  uint32_t hmask;	/* Hash part mask (size of hash part - 1). */
#if LJ_GC64
  MRef freetop;		/* Top of free elements. */
#endif
} GCtab;

#define sizetabcolo(n)	((n)*sizeof(TValue) + sizeof(GCtab))
#define tabref(r)	((GCtab *)gcref((r)))
#define noderef(r)	(mref((r), Node))
#define nextnode(n)	(mref((n)->next, Node))
#if LJ_GC64
#define getfreetop(t, n)	(noderef((t)->freetop))
#define setfreetop(t, n, v)	(setmref((t)->freetop, (v)))
#else
#define getfreetop(t, n)	(noderef((n)->freetop))
#define setfreetop(t, n, v)	(setmref((n)->freetop, (v)))
#endif

/* -- State objects ------------------------------------------------------- */

/* VM states. */
enum {
  LJ_VMST_INTERP,	/* Interpreter. */
  LJ_VMST_C,		/* C function. */
  LJ_VMST_GC,		/* Garbage collector. */
  LJ_VMST_EXIT,		/* Trace exit handler. */
  LJ_VMST_RECORD,	/* Trace recorder. */
  LJ_VMST_OPT,		/* Optimizer. */
  LJ_VMST_ASM,		/* Assembler. */
  LJ_VMST__MAX
};

#define setvmstate(g, st)	((g)->vmstate = ~LJ_VMST_##st)

/* Metamethods. ORDER MM */
#ifdef LJ_HASFFI
#define MMDEF_FFI(_) _(new)
#else
#define MMDEF_FFI(_)
#endif

#if LJ_52 || LJ_HASFFI
#define MMDEF_PAIRS(_) _(pairs) _(ipairs)
#else
#define MMDEF_PAIRS(_)
#define MM_pairs	255
#define MM_ipairs	255
#endif

#define MMDEF(_) \
  _(index) _(newindex) _(gc) _(mode) _(eq) _(len) \
  /* Only the above (fast) metamethods are negative cached (max. 8). */ \
  _(lt) _(le) _(concat) _(call) \
  /* The following must be in ORDER ARITH. */ \
  _(add) _(sub) _(mul) _(div) _(mod) _(pow) _(unm) \
  /* The following are used in the standard libraries. */ \
  _(metatable) _(tostring) MMDEF_FFI(_) MMDEF_PAIRS(_)

typedef enum {
#define MMENUM(name)	MM_##name,
MMDEF(MMENUM)
#undef MMENUM
  MM__MAX,
  MM____ = MM__MAX,
  MM_FAST = MM_len
} MMS;

/* GC root IDs. */
typedef enum {
  GCROOT_MMNAME,	/* Metamethod names. */
  GCROOT_MMNAME_LAST = GCROOT_MMNAME + MM__MAX-1,
  GCROOT_BASEMT,	/* Metatables for base types. */
  GCROOT_BASEMT_NUM = GCROOT_BASEMT + ~LJ_TNUMX,
  GCROOT_IO_INPUT,	/* Userdata for default I/O input file. */
  GCROOT_IO_OUTPUT,	/* Userdata for default I/O output file. */
#if LJ_HASFFI
  GCROOT_FFI_FIN,	/* FFI finalizer table. */
#endif
  GCROOT_MAX
} GCRootID;

#define basemt_it(g, it)	((g)->gcroot[GCROOT_BASEMT+~(it)])
#define basemt_obj(g, o)	((g)->gcroot[GCROOT_BASEMT+itypemap(o)])
#define mmname_str(g, mm)	(strref((g)->gcroot[GCROOT_MMNAME+(mm)]))

/* Garbage collector state. */
typedef struct GCState {
  GCSize total;		/* Memory currently allocated. */
  GCSize threshold;	/* Memory threshold. */
  uint8_t currentwhite;	/* Current white color. */
  uint8_t state;	/* GC state. */
  uint8_t unused0;
#if LJ_64
  uint8_t lightudnum;	/* Number of lightuserdata segments - 1. */
#else
  uint8_t unused1;
#endif
  MSize sweepstr;	/* Sweep position in string table. */
  GCRef root;		/* List of all collectable objects. */
  MRef sweep;		/* Sweep position in root list. */
  GCRef gray;		/* List of gray objects. */
  GCRef grayagain;	/* List of objects for atomic traversal. */
  GCRef weak;		/* List of weak tables (to be cleared). */
  GCRef mmudata;	/* List of userdata (to be finalized). */
  GCSize debt;		/* Debt (how much GC is behind schedule). */
  GCSize estimate;	/* Estimate of memory actually in use. */
  MSize stepmul;	/* Incremental GC step granularity. */
  MSize pause;		/* Pause between successive GC cycles. */
#if LJ_64
  MRef lightudseg;	/* Upper bits of lightuserdata segments. */
#endif
} GCState;

/* String interning state. */
typedef struct StrInternState {
  GCRef *tab;		/* String hash table anchors. */
  MSize mask;		/* String hash mask (size of hash table - 1). */
  MSize num;		/* Number of strings in hash table. */
  StrID id;		/* Next string ID. */
  uint8_t idreseed;	/* String ID reseed counter. */
  uint8_t second;	/* String interning table uses secondary hashing. */
  uint8_t unused1;
  uint8_t unused2;
  LJ_ALIGN(8) uint64_t seed;	/* Random string seed. */
} StrInternState;

/* Global state, shared by all threads of a Lua universe. */
typedef struct global_State {
  lua_Alloc allocf;	/* Memory allocator. */
  void *allocd;		/* Memory allocator data. */
  GCState gc;		/* Garbage collector. */
  GCstr strempty;	/* Empty string. */
  uint8_t stremptyz;	/* Zero terminator of empty string. */
  uint8_t hookmask;	/* Hook mask. */
  uint8_t dispatchmode;	/* Dispatch mode. */
  uint8_t vmevmask;	/* VM event mask. */
  StrInternState str;	/* String interning. */
  volatile int32_t vmstate;  /* VM state or current JIT code trace number. */
  GCRef mainthref;	/* Link to main thread. */
  SBuf tmpbuf;		/* Temporary string buffer. */
  TValue tmptv, tmptv2;	/* Temporary TValues. */
  Node nilnode;		/* Fallback 1-element hash part (nil key and value). */
  TValue registrytv;	/* Anchor for registry. */
  GCupval uvhead;	/* Head of double-linked list of all open upvalues. */
  int32_t hookcount;	/* Instruction hook countdown. */
  int32_t hookcstart;	/* Start count for instruction hook counter. */
  lua_Hook hookf;	/* Hook function. */
  lua_CFunction wrapf;	/* Wrapper for C function calls. */
  lua_CFunction panic;	/* Called as a last resort for errors. */
  BCIns bc_cfunc_int;	/* Bytecode for internal C function calls. */
  BCIns bc_cfunc_ext;	/* Bytecode for external C function calls. */
  GCRef cur_L;		/* Currently executing lua_State. */
  MRef jit_base;	/* Current JIT code L->base or NULL. */
  MRef ctype_state;	/* Pointer to C type state. */
  PRNGState prng;	/* Global PRNG state. */
  GCRef gcroot[GCROOT_MAX];  /* GC roots. */
} global_State;

#define mainthread(g)	(&gcref(g->mainthref)->th)
#define niltv(L) \
  check_exp(tvisnil(&G(L)->nilnode.val), &G(L)->nilnode.val)
#define niltvg(g) \
  check_exp(tvisnil(&(g)->nilnode.val), &(g)->nilnode.val)

/* Hook management. Hook event masks are defined in lua.h. */
#define HOOK_EVENTMASK		0x0f
#define HOOK_ACTIVE		0x10
#define HOOK_ACTIVE_SHIFT	4
#define HOOK_VMEVENT		0x20
#define HOOK_GC			0x40
#define HOOK_PROFILE		0x80
#define hook_active(g)		((g)->hookmask & HOOK_ACTIVE)
#define hook_enter(g)		((g)->hookmask |= HOOK_ACTIVE)
#define hook_entergc(g) \
  ((g)->hookmask = ((g)->hookmask | (HOOK_ACTIVE|HOOK_GC)) & ~HOOK_PROFILE)
#define hook_vmevent(g)		((g)->hookmask |= (HOOK_ACTIVE|HOOK_VMEVENT))
#define hook_leave(g)		((g)->hookmask &= ~HOOK_ACTIVE)
#define hook_save(g)		((g)->hookmask & ~HOOK_EVENTMASK)
#define hook_restore(g, h) \
  ((g)->hookmask = ((g)->hookmask & HOOK_EVENTMASK) | (h))

/* Per-thread state object. */
struct lua_State {
  GCHeader;
  uint8_t dummy_ffid;	/* Fake FF_C for curr_funcisL() on dummy frames. */
  uint8_t status;	/* Thread status. */
  MRef glref;		/* Link to global state. */
  GCRef gclist;		/* GC chain. */
  TValue *base;		/* Base of currently executing function. */
  TValue *top;		/* First free slot in the stack. */
  MRef maxstack;	/* Last free slot in the stack. */
  MRef stack;		/* Stack base. */
  GCRef openupval;	/* List of open upvalues in the stack. */
  GCRef env;		/* Thread environment (table of globals). */
  void *cframe;		/* End of C stack frame chain. */
  MSize stacksize;	/* True stack size (incl. LJ_STACK_EXTRA). */
  char _GARRY_VARS[22];		/* GMOD FIELD */
  void *luabase;			/* GMOD FIELD */
};

#define G(L)			(mref(L->glref, global_State))
#define registry(L)		(&G(L)->registrytv)

/* Macros to access the currently executing (Lua) function. */
#if LJ_GC64
#define curr_func(L)		(&gcval(L->base-2)->fn)
#elif LJ_FR2
#define curr_func(L)		(&gcref((L->base-2)->gcr)->fn)
#else
#define curr_func(L)		(&gcref((L->base-1)->fr.func)->fn)
#endif
#define curr_funcisL(L)		(isluafunc(curr_func(L)))
#define curr_proto(L)		(funcproto(curr_func(L)))
#define curr_topL(L)		(L->base + curr_proto(L)->framesize)
#define curr_top(L)		(curr_funcisL(L) ? curr_topL(L) : L->top)

#if defined(LUA_USE_ASSERT) || defined(LUA_USE_APICHECK)
LJ_FUNC_NORET void lj_assert_fail(global_State *g, const char *file, int line,
				  const char *func, const char *fmt, ...);
#endif

/* -- GC object definition and conversions -------------------------------- */

/* GC header for generic access to common fields of GC objects. */
typedef struct GChead {
  GCHeader;
  uint8_t unused1;
  uint8_t unused2;
  GCRef env;
  GCRef gclist;
  GCRef metatable;
} GChead;

/* The env field SHOULD be at the same offset for all GC objects. */
//LJ_STATIC_ASSERT(offsetof(GChead, env) == offsetof(GCfuncL, env));
//LJ_STATIC_ASSERT(offsetof(GChead, env) == offsetof(GCudata, env));

/* The metatable field MUST be at the same offset for all GC objects. */
//LJ_STATIC_ASSERT(offsetof(GChead, metatable) == offsetof(GCtab, metatable));
//LJ_STATIC_ASSERT(offsetof(GChead, metatable) == offsetof(GCudata, metatable));

/* The gclist field MUST be at the same offset for all GC objects. */
//LJ_STATIC_ASSERT(offsetof(GChead, gclist) == offsetof(lua_State, gclist));
//LJ_STATIC_ASSERT(offsetof(GChead, gclist) == offsetof(GCproto, gclist));
//LJ_STATIC_ASSERT(offsetof(GChead, gclist) == offsetof(GCfuncL, gclist));
//LJ_STATIC_ASSERT(offsetof(GChead, gclist) == offsetof(GCtab, gclist));

typedef union GCobj {
  GChead gch;
  GCstr str;
  GCupval uv;
  lua_State th;
  GCproto pt;
  GCfunc fn;
  GCcdata cd;
  GCtab tab;
  GCudata ud;
} GCobj;

/* Macros to convert a GCobj pointer into a specific value. */
#define gco2str(o)	check_exp((o)->gch.gct == ~LJ_TSTR, &(o)->str)
#define gco2uv(o)	check_exp((o)->gch.gct == ~LJ_TUPVAL, &(o)->uv)
#define gco2th(o)	check_exp((o)->gch.gct == ~LJ_TTHREAD, &(o)->th)
#define gco2pt(o)	check_exp((o)->gch.gct == ~LJ_TPROTO, &(o)->pt)
#define gco2func(o)	check_exp((o)->gch.gct == ~LJ_TFUNC, &(o)->fn)
#define gco2cd(o)	check_exp((o)->gch.gct == ~LJ_TCDATA, &(o)->cd)
#define gco2tab(o)	check_exp((o)->gch.gct == ~LJ_TTAB, &(o)->tab)
#define gco2ud(o)	check_exp((o)->gch.gct == ~LJ_TUDATA, &(o)->ud)

/* Macro to convert any collectable object into a GCobj pointer. */
#define obj2gco(v)	((GCobj *)(v))

/* -- TValue getters/setters ---------------------------------------------- */

/* Macros to test types. */
#if LJ_GC64
#define itype(o)	((uint32_t)((o)->it64 >> 47))
#define tvisnil(o)	((o)->it64 == -1)
#else
#define itype(o)	((o)->it)
#define tvisnil(o)	(itype(o) == LJ_TNIL)
#endif
#define tvisfalse(o)	(itype(o) == LJ_TFALSE)
#define tvistrue(o)	(itype(o) == LJ_TTRUE)
#define tvisbool(o)	(tvisfalse(o) || tvistrue(o))
#if LJ_64 && !LJ_GC64
#define tvislightud(o)	(((int32_t)itype(o) >> 15) == -2)
#else
#define tvislightud(o)	(itype(o) == LJ_TLIGHTUD)
#endif
#define tvisstr(o)	(itype(o) == LJ_TSTR)
#define tvisfunc(o)	(itype(o) == LJ_TFUNC)
#define tvisthread(o)	(itype(o) == LJ_TTHREAD)
#define tvisproto(o)	(itype(o) == LJ_TPROTO)
#define tviscdata(o)	(itype(o) == LJ_TCDATA)
#define tvistab(o)	(itype(o) == LJ_TTAB)
#define tvisudata(o)	(itype(o) == LJ_TUDATA)
#define tvisnumber(o)	(itype(o) <= LJ_TISNUM)
#define tvisint(o)	(LJ_DUALNUM && itype(o) == LJ_TISNUM)
#define tvisnum(o)	(itype(o) < LJ_TISNUM)

#define tvistruecond(o)	(itype(o) < LJ_TISTRUECOND)
#define tvispri(o)	(itype(o) >= LJ_TISPRI)
#define tvistabud(o)	(itype(o) <= LJ_TISTABUD)  /* && !tvisnum() */
#define tvisgcv(o)	((itype(o) - LJ_TISGCV) > (LJ_TNUMX - LJ_TISGCV))

/* Special macros to test numbers for NaN, +0, -0, +1 and raw equality. */
#define tvisnan(o)	((o)->n != (o)->n)
#if LJ_64
#define tviszero(o)	(((o)->u64 << 1) == 0)
#else
#define tviszero(o)	(((o)->u32.lo | ((o)->u32.hi << 1)) == 0)
#endif
#define tvispzero(o)	((o)->u64 == 0)
#define tvismzero(o)	((o)->u64 == U64x(80000000,00000000))
#define tvispone(o)	((o)->u64 == U64x(3ff00000,00000000))
#define rawnumequal(o1, o2)	((o1)->u64 == (o2)->u64)

/* Macros to convert type ids. */
#if LJ_64 && !LJ_GC64
#define itypemap(o) \
  (tvisnumber(o) ? ~LJ_TNUMX : tvislightud(o) ? ~LJ_TLIGHTUD : ~itype(o))
#else
#define itypemap(o)	(tvisnumber(o) ? ~LJ_TNUMX : ~itype(o))
#endif

/* Macros to get tagged values. */
#if LJ_GC64
#define gcval(o)	((GCobj *)(gcrefu((o)->gcr) & LJ_GCVMASK))
#else
#define gcval(o)	(gcref((o)->gcr))
#endif
#define boolV(o)	check_exp(tvisbool(o), (LJ_TFALSE - itype(o)))
#if LJ_64
#define lightudseg(u) \
  (((u) >> LJ_LIGHTUD_BITS_LO) & ((1 << LJ_LIGHTUD_BITS_SEG)-1))
#define lightudlo(u) \
  ((u) & (((uint64_t)1 << LJ_LIGHTUD_BITS_LO) - 1))
#define lightudup(p) \
  ((uint32_t)(((p) >> LJ_LIGHTUD_BITS_LO) << (LJ_LIGHTUD_BITS_LO-32)))
static LJ_AINLINE void *lightudV(global_State *g, cTValue *o)
{
  uint64_t u = o->u64;
  uint64_t seg = lightudseg(u);
  uint32_t *segmap = mref(g->gc.lightudseg, uint32_t);
  lj_assertG(tvislightud(o), "lightuserdata expected");
  if (seg == (1 << LJ_LIGHTUD_BITS_SEG)-1) return NULL;
  lj_assertG(seg <= g->gc.lightudnum, "bad lightuserdata segment %d", seg);
  return (void *)(((uint64_t)segmap[seg] << 32) | lightudlo(u));
}
#else
#define lightudV(g, o)	check_exp(tvislightud(o), gcrefp((o)->gcr, void))
#endif
#define gcV(o)		check_exp(tvisgcv(o), gcval(o))
#define strV(o)		check_exp(tvisstr(o), &gcval(o)->str)
#define funcV(o)	check_exp(tvisfunc(o), &gcval(o)->fn)
#define threadV(o)	check_exp(tvisthread(o), &gcval(o)->th)
#define protoV(o)	check_exp(tvisproto(o), &gcval(o)->pt)
#define cdataV(o)	check_exp(tviscdata(o), &gcval(o)->cd)
#define tabV(o)		check_exp(tvistab(o), &gcval(o)->tab)
#define udataV(o)	check_exp(tvisudata(o), &gcval(o)->ud)
#define numV(o)		check_exp(tvisnum(o), (o)->n)
#define intV(o)		check_exp(tvisint(o), (int32_t)(o)->i)

/* Macros to set tagged values. */
#if LJ_GC64
#define setitype(o, i)		((o)->it = ((i) << 15))
#define setnilV(o)		((o)->it64 = -1)
#define setpriV(o, x)		((o)->it64 = (int64_t)~((uint64_t)~(x)<<47))
#define setboolV(o, x)		((o)->it64 = (int64_t)~((uint64_t)((x)+1)<<47))
#else
#define setitype(o, i)		((o)->it = (i))
#define setnilV(o)		((o)->it = LJ_TNIL)
#define setboolV(o, x)		((o)->it = LJ_TFALSE-(uint32_t)(x))
#define setpriV(o, i)		(setitype((o), (i)))
#endif

static LJ_AINLINE void setrawlightudV(TValue *o, void *p)
{
#if LJ_GC64
  o->u64 = (uint64_t)p | (((uint64_t)LJ_TLIGHTUD) << 47);
#elif LJ_64
  o->u64 = (uint64_t)p | (((uint64_t)0xffff) << 48);
#else
  setgcrefp(o->gcr, p); setitype(o, LJ_TLIGHTUD);
#endif
}

#if LJ_FR2 || LJ_32
#define contptr(f)		((void *)(f))
#define setcont(o, f)		((o)->u64 = (uint64_t)(uintptr_t)contptr(f))
#else
#define contptr(f) \
  ((void *)(uintptr_t)(uint32_t)((intptr_t)(f) - (intptr_t)lj_vm_asm_begin))
#define setcont(o, f) \
  ((o)->u64 = (uint64_t)(void *)(f) - (uint64_t)lj_vm_asm_begin)
#endif

static LJ_AINLINE void checklivetv(lua_State *L, TValue *o, const char *msg)
{
  UNUSED(L); UNUSED(o); UNUSED(msg);
#if LUA_USE_ASSERT
  if (tvisgcv(o)) {
    lj_assertL(~itype(o) == gcval(o)->gch.gct,
	       "mismatch of TValue type %d vs GC type %d",
	       ~itype(o), gcval(o)->gch.gct);
    /* Copy of isdead check from lj_gc.h to avoid circular include. */
    lj_assertL(!(gcval(o)->gch.marked & (G(L)->gc.currentwhite ^ 3) & 3), msg);
  }
#endif
}

static LJ_AINLINE void setgcVraw(TValue *o, GCobj *v, uint32_t itype)
{
#if LJ_GC64
  setgcreft(o->gcr, v, itype);
#else
  setgcref(o->gcr, v); setitype(o, itype);
#endif
}

static LJ_AINLINE void setgcV(lua_State *L, TValue *o, GCobj *v, uint32_t it)
{
  setgcVraw(o, v, it);
  checklivetv(L, o, "store to dead GC object");
}

#define define_setV(name, type, tag) \
static LJ_AINLINE void name(lua_State *L, TValue *o, const type *v) \
{ \
  setgcV(L, o, obj2gco(v), tag); \
}
define_setV(setstrV, GCstr, LJ_TSTR)
define_setV(setthreadV, lua_State, LJ_TTHREAD)
define_setV(setprotoV, GCproto, LJ_TPROTO)
define_setV(setfuncV, GCfunc, LJ_TFUNC)
define_setV(setcdataV, GCcdata, LJ_TCDATA)
define_setV(settabV, GCtab, LJ_TTAB)
define_setV(setudataV, GCudata, LJ_TUDATA)

#define setnumV(o, x)		((o)->n = (x))
#define setnanV(o)		((o)->u64 = U64x(fff80000,00000000))
#define setpinfV(o)		((o)->u64 = U64x(7ff00000,00000000))
#define setminfV(o)		((o)->u64 = U64x(fff00000,00000000))

static LJ_AINLINE void setintV(TValue *o, int32_t i)
{
#if LJ_DUALNUM
  o->i = (uint32_t)i; setitype(o, LJ_TISNUM);
#else
  o->n = (lua_Number)i;
#endif
}

static LJ_AINLINE void setint64V(TValue *o, int64_t i)
{
  if (LJ_DUALNUM && LJ_LIKELY(i == (int64_t)(int32_t)i))
    setintV(o, (int32_t)i);
  else
    setnumV(o, (lua_Number)i);
}

#if LJ_64
#define setintptrV(o, i)	setint64V((o), (i))
#else
#define setintptrV(o, i)	setintV((o), (i))
#endif

/* Copy tagged values. */
static LJ_AINLINE void copyTV(lua_State *L, TValue *o1, const TValue *o2)
{
  *o1 = *o2;
  checklivetv(L, o1, "copy of dead GC object");
}

/* -- Number to integer conversion ---------------------------------------- */

#if LJ_SOFTFP
LJ_ASMF int32_t lj_vm_tobit(double x);
#if LJ_TARGET_MIPS64
LJ_ASMF int32_t lj_vm_tointg(double x);
#endif
#endif

static LJ_AINLINE int32_t lj_num2bit(lua_Number n)
{
#if LJ_SOFTFP
  return lj_vm_tobit(n);
#else
  TValue o;
  o.n = n + 6755399441055744.0;  /* 2^52 + 2^51 */
  return (int32_t)o.u32.lo;
#endif
}

#define lj_num2int(n)   ((int32_t)(n))

/*
** This must match the JIT backend behavior. In particular for archs
** that don't have a common hardware instruction for this conversion.
** Note that signed FP to unsigned int conversions have an undefined
** result and should never be relied upon in portable FFI code.
** See also: C99 or C11 standard, 6.3.1.4, footnote of (1).
*/
static LJ_AINLINE uint64_t lj_num2u64(lua_Number n)
{
#if LJ_TARGET_X86ORX64 || LJ_TARGET_MIPS
  int64_t i = (int64_t)n;
  if (i < 0) i = (int64_t)(n - 18446744073709551616.0);
  return (uint64_t)i;
#else
  return (uint64_t)n;
#endif
}

static LJ_AINLINE int32_t numberVint(cTValue *o)
{
  if (LJ_LIKELY(tvisint(o)))
    return intV(o);
  else
    return lj_num2int(numV(o));
}

static LJ_AINLINE lua_Number numberVnum(cTValue *o)
{
  if (LJ_UNLIKELY(tvisint(o)))
    return (lua_Number)intV(o);
  else
    return numV(o);
}

/* -- Miscellaneous object handling --------------------------------------- */

/* Names and maps for internal and external object tags. */
LJ_DATA const char *const lj_obj_typename[1+LUA_TCDATA+1];
LJ_DATA const char *const lj_obj_itypename[~LJ_TNUMX+1];

#define lj_typename(o)	(lj_obj_itypename[itypemap(o)])

/* Compare two objects without calling metamethods. */
LJ_FUNC int LJ_FASTCALL lj_obj_equal(cTValue *o1, cTValue *o2);
LJ_FUNC const void * LJ_FASTCALL lj_obj_ptr(global_State *g, cTValue *o);

#if LJ_ABI_PAUTH
#if LJ_TARGET_ARM64
#include <ptrauth.h>
#define lj_ptr_sign(ptr, ctx) \
  ptrauth_sign_unauthenticated((ptr), ptrauth_key_function_pointer, (ctx))
#define lj_ptr_strip(ptr) ptrauth_strip((ptr), ptrauth_key_function_pointer)
#else
#error "No support for pointer authentication for this architecture"
#endif
#else
#define lj_ptr_sign(ptr, ctx) (ptr)
#define lj_ptr_strip(ptr) (ptr)
#endif

#endif
