#!/usr/bin/env python3
"""Decode a PSY-Q .SYM file (MND v1, SN Systems debug info) into C-friendly output.

usage: symdump.py MAIN.SYM OUTDIR

Writes into OUTDIR:
  include/types.h      structs, unions, enums and typedefs, with offsets and size asserts
  include/globals.h    extern declarations of global variables (address in a comment)
  src/<FILE>.c         one skeleton per source file: signatures, args, locals, line ranges
  symbols.txt          every label, sorted by address
  symbol_addrs.txt     splat-style "name = 0xADDR; // type:func size:0x.."
  functions.json       per-function address/size/frame/args/locals/line table
  globals.json         global and static variables
  types.json           type definitions (structured)
  conflicts.txt        types or typedefs defined differently by different objects

The file is a stream of records: addr(u32) type(u8) payload. Debug records
(0x94/0x96) use COFF class and type numbering: the type word is a 4-bit base
type plus 2-bit derivations (1=pointer, 2=function, 3=array) starting at bit 4,
d1 being the one closest to the identifier.
"""
import collections
import hashlib
import json
import os
import re
import struct
import sys

# ---- record parsing --------------------------------------------------------

def parse(b):
    if b[:4] != b"MND\x01":
        raise ValueError("not a PSY-Q MND v1 sym file")
    p = 8
    recs = []

    def u(fmt):
        nonlocal p
        v = struct.unpack_from(fmt, b, p)
        p += struct.calcsize(fmt)
        return v[0]

    def s():
        nonlocal p
        n = b[p]
        p += 1
        r = b[p:p + n].decode("latin1")
        p += n
        return r

    while p < len(b):
        off = p
        addr = u("<I")
        t = b[p]
        p += 1
        r = dict(off=off, addr=addr, t=t)
        if t < 0x80:
            r["name"] = s()
        elif t == 0x80:
            r["sld"] = ("inc", 1)
        elif t == 0x82:
            r["sld"] = ("inc", u("<B"))
        elif t == 0x84:
            r["sld"] = ("inc", u("<H"))
        elif t == 0x86:
            r["sld"] = ("set", u("<I"))
        elif t == 0x88:
            r["sld"] = ("set", u("<I"))
            r["file"] = s()
        elif t == 0x8a:
            r["sld"] = ("end", 0)
        elif t == 0x8c:
            r["fp"] = u("<H")
            r["fsize"] = u("<I")
            r["retreg"] = u("<H")
            r["mask"] = u("<I")
            r["maskoff"] = u("<i")
            r["line"] = u("<I")
            r["file"] = s()
            r["name"] = s()
        elif t in (0x8e, 0x90, 0x92):
            r["line"] = u("<I")
        elif t == 0x94:
            r.update(cls=u("<H"), type=u("<H"), size=u("<I"), name=s(), dims=[], tag="")
        elif t == 0x96:
            r.update(cls=u("<H"), type=u("<H"), size=u("<I"))
            n = u("<H")
            r["dims"] = [u("<I") for _ in range(n)]
            r["tag"] = s()
            r["name"] = s()
        else:
            raise ValueError(f"unknown record type {t:#x} at {off:#x}")
        recs.append(r)
    return recs


# ---- COFF classes / types --------------------------------------------------

C_AUTO, C_EXT, C_STAT, C_REG, C_MOS, C_ARG, C_STRTAG, C_MOU, C_UNTAG, C_TPDEF = 1, 2, 3, 4, 8, 9, 10, 11, 12, 13
C_ENTAG, C_MOE, C_REGPARM, C_FIELD, C_EOS, C_FILE = 15, 16, 17, 18, 102, 103

BASE = {0: "int /*null*/", 1: "void", 2: "char", 3: "short", 4: "int", 5: "long", 6: "float",
        7: "double", 11: "int /*moe*/", 12: "unsigned char", 13: "unsigned short",
        14: "unsigned int", 15: "unsigned long"}
BASE_SIZE = {0: 4, 2: 1, 3: 2, 4: 4, 5: 4, 6: 4, 7: 8, 11: 4, 12: 1, 13: 2, 14: 4, 15: 4}
DT_PTR, DT_FCN, DT_ARY = 1, 2, 3
KIND = {8: "struct", 9: "union", 10: "enum"}
TAGCLS = {C_STRTAG: "struct", C_UNTAG: "union", C_ENTAG: "enum"}
REGNAMES = ["zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6",
            "t7", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp",
            "fp", "ra"]


def regname(n):
    return "$" + REGNAMES[n] if n < 32 else f"reg{n}"


def derivs(ty):
    out = []
    for i in range(6):
        d = (ty >> (4 + 2 * i)) & 3
        if not d:
            break
        out.append(d)
    return out


def is_def(r):
    return r["t"] in (0x94, 0x96)


class Types:
    """Collects tag/typedef definitions across all objects and renders C types."""

    def __init__(self):
        self.tags = {}        # (kind, canon) -> dict(size, members, sig)
        self.typedefs = {}    # name -> (decl text, sig)
        self.opaque = set()   # referenced but never defined: (kind, name)
        self.conflicts = []

    # name resolution is per object because anonymous tags (".Nfake") are scoped to one
    def begin_object(self, name, segment):
        self.obj = name
        self.stem = re.sub(r"\W", "_", os.path.splitext(name)[0])
        self.fake = {}        # raw fake tag -> canonical name
        self.rawdefs = {}     # raw tag -> (kind, members, size)
        self.typedef_of = {}  # raw tag -> first typedef name
        cur = None
        for r in segment:
            if not is_def(r):
                continue
            c = r["cls"]
            if c in TAGCLS:
                cur = (r["name"], TAGCLS[c], r["size"], [])
                self.rawdefs[r["name"]] = cur
            elif c in (C_MOS, C_MOU, C_FIELD, C_MOE) and cur:
                cur[3].append(r)
            elif c == C_EOS:
                cur = None
            elif c == C_TPDEF and r["tag"] and (r["type"] >> 4) == 0 and (r["type"] & 15) in KIND:
                self.typedef_of.setdefault(r["tag"], r["name"])

    def canon(self, raw, _busy=None):
        if not raw.startswith("."):
            return raw
        if raw in self.fake:
            return self.fake[raw]
        if raw in self.typedef_of:
            self.fake[raw] = self.typedef_of[raw]
            return self.fake[raw]
        d = self.rawdefs.get(raw)
        if d is None:
            return f"anon_{self.stem}_{raw.strip('.')}"
        busy = _busy or set()
        if raw in busy:
            return "anon_cyclic"
        busy.add(raw)
        sig = "|".join(f"{m['name']}:{m['type']}:{self.canon(m['tag'], busy)}:{m['dims']}:{m['addr']}:{m['size']}"
                       for m in d[3])
        name = f"anon_{d[1]}_{hashlib.sha1(sig.encode()).hexdigest()[:6]}"
        self.fake[raw] = name
        return name

    def base_str(self, ty, tag):
        b = ty & 15
        if b in KIND:
            if not tag:
                return f"{KIND[b]} /*notag*/"
            return f"{KIND[b]} {self.canon(tag)}"
        return BASE[b]

    def decl(self, ty, tag, dims, name, params=None):
        s = name
        k = 0
        for d in derivs(ty):
            if d == DT_PTR:
                s = "*" + s
            else:
                if s.startswith("*"):
                    s = f"({s})"
                if d == DT_ARY:
                    n = dims[k] if k < len(dims) else 0
                    k += 1
                    s += f"[{n}]" if n else "[]"
                else:
                    s += f"({params if params is not None else ''})"
                    params = None
        base = self.base_str(ty, tag)
        return f"{base} {s}" if s else base

    def sizeof(self, ty, tag, dims, rec_size=0):
        """size in bytes, or None when unknown (opaque/incomplete type)"""
        k = 0
        n = 1
        for d in derivs(ty):
            if d == DT_PTR:
                return 4 * n
            if d == DT_ARY:
                n *= dims[k] if k < len(dims) else 0
                k += 1
            elif d == DT_FCN:
                return None
        b = ty & 15
        if b in KIND:
            t = self.tags.get((KIND[b], self.canon(tag)))
            return None if t is None else t["size"] * n
        return BASE_SIZE.get(b, 4) * n

    # ---- registering definitions ----
    def add_tag(self, kind, raw, size, members):
        name = self.canon(raw)
        mem = []
        for m in members:
            if m["cls"] == C_MOE:
                mem.append(dict(name=m["name"], value=struct.unpack("<i", struct.pack("<I", m["addr"]))[0]))
                continue
            for t in (m["tag"],):
                if t and (m["type"] & 15) in KIND:
                    self.note_ref(KIND[m["type"] & 15], self.canon(t))
            e = dict(name=m["name"], cls=m["cls"], off=m["addr"], size=m["size"], type=m["type"],
                     tag=self.canon(m["tag"]) if m["tag"] else "", dims=m["dims"])
            e["decl"] = self.decl(m["type"], e["tag"], m["dims"], m["name"])
            mem.append(e)
        sig = json.dumps([size, [sorted(x.items()) for x in mem]], default=str)
        key = (kind, name)
        if key in self.tags:
            if self.tags[key]["sig"] != sig:
                self.conflicts.append(f"{kind} {name}: differs in {self.obj} (kept first, from {self.tags[key]['obj']})")
            return
        self.tags[key] = dict(kind=kind, name=name, size=size, members=mem, sig=sig, obj=self.obj)

    def note_ref(self, kind, name):
        self.opaque.add((kind, name))

    def add_typedef(self, r):
        tag = self.canon(r["tag"]) if r["tag"] else ""
        if tag:
            self.note_ref(KIND.get(r["type"] & 15, "struct"), tag)
        text = self.decl(r["type"], tag, r["dims"], r["name"])
        if r["name"] in self.typedefs:
            if self.typedefs[r["name"]][0] != text:
                self.conflicts.append(f"typedef {r['name']}: '{text}' in {self.obj} vs '{self.typedefs[r['name']][0]}'")
            return
        self.typedefs[r["name"]] = (text, r["size"])


# ---- main decode -----------------------------------------------------------

def decode(recs):
    T = Types()
    labels = [(r["addr"], r["t"], r["name"]) for r in recs if r["t"] < 0x80]

    # split into object segments delimited by FILE records (start and end share a name)
    segs = []
    cur = None
    for r in recs:
        if is_def(r) and r["cls"] == C_FILE:
            if cur is not None and cur[0] == r["name"] and not cur[2]:
                cur[2] = True
            else:
                cur = [r["name"], [], False]
                segs.append(cur)
        elif cur is not None:
            cur[1].append(r)

    funcs, globs, sld = [], [], []
    for objname, seg, _ in segs:
        T.begin_object(objname, seg)
        fn = None
        blocks = []
        last_def = {}
        cur_tag = None
        for r in seg:
            t = r["t"]
            if "sld" in r:
                op, v = r["sld"]
                if op == "set":
                    line = v
                    sld.append((r["addr"], line))
                elif op == "inc":
                    line += v
                    sld.append((r["addr"], line))
                continue
            if t == 0x8c:
                d = last_def.get(r["name"])
                fn = dict(name=r["name"], addr=r["addr"], file=r["file"], line=r["line"], fp=r["fp"],
                          frame=r["fsize"], retreg=r["retreg"], mask=r["mask"], maskoff=r["maskoff"],
                          obj=objname, static=bool(d and d["cls"] == C_STAT), args=[], locals=[], end=None,
                          endline=None)
                fn["rtype"] = d
                blocks = []
                nblocks = 0
                continue
            if t == 0x8e:
                fn["end"] = r["addr"]
                fn["endline"] = r["line"]
                funcs.append(fn)
                fn = None
                continue
            if t in (0x90, 0x92):
                if t == 0x90:
                    nblocks += 1
                    blocks.append(nblocks)
                elif blocks:
                    blocks.pop()
                continue
            if not is_def(r):
                continue
            c = r["cls"]
            if c in TAGCLS:
                cur_tag = (TAGCLS[c], r["name"], r["size"], [])
                continue
            if c in (C_MOS, C_MOU, C_FIELD, C_MOE) and cur_tag:
                cur_tag[3].append(r)
                continue
            if c == C_EOS:
                if cur_tag:
                    T.add_tag(cur_tag[0], cur_tag[1], cur_tag[2], cur_tag[3])
                cur_tag = None
                continue
            if c == C_TPDEF:
                T.add_typedef(r)
                continue
            if c == C_FILE:
                continue
            tag = T.canon(r["tag"]) if r["tag"] else ""
            if tag and (r["type"] & 15) in KIND:
                T.note_ref(KIND[r["type"] & 15], tag)
            v = dict(name=r["name"], cls=c, val=r["addr"], type=r["type"], tag=tag, dims=r["dims"],
                     size=r["size"], decl=T.decl(r["type"], tag, r["dims"], r["name"]), block=tuple(blocks))
            if fn is None:
                last_def[r["name"]] = v
                if c in (C_EXT, C_STAT) and derivs(r["type"])[:1] != [DT_FCN]:
                    v["obj"] = objname
                    globs.append(v)
            elif c in (C_ARG, C_REGPARM):
                fn["args"].append(v)
            else:
                fn["locals"].append(v)
    return T, labels, funcs, globs, sld


def func_signature(T, fn):
    d = fn["rtype"]
    ty = d["type"] if d else 0x21
    tag = d["tag"] if d else ""
    # an ARG may be declared twice (REGPARM spill + ARG); keep declaration order, first wins
    seen = set()
    params = []
    for a in fn["args"]:
        if a["name"] in seen:
            continue
        seen.add(a["name"])
        params.append(a["decl"])
    sig = T.decl(ty, tag, d["dims"] if d else [], fn["name"], ", ".join(params) if params else "void")
    return ("static " if fn["static"] else "") + sig


def where(v, fn=None):
    c = v["cls"]
    val = v["val"]
    if c == C_AUTO:
        sv = val - (1 << 32) if val >= 1 << 31 else val
        return f"fp{sv:+#x}"
    if c in (C_REG, C_REGPARM):
        return regname(val)
    if c == C_ARG:
        return f"arg+{val:#x}"
    if c in (C_STAT, C_EXT):
        return f"{val:#010x}" if val >= 0x80000000 else f"static, section offset {val:#x}"
    return f"cls{c} val {val:#x}"


# ---- output ----------------------------------------------------------------

def emit_types(T, out):
    L = ["/* generated by tools/symdump.py from MAIN.SYM -- do not edit */",
         "#ifndef TYPES_H", "#define TYPES_H", ""]
    kinds = collections.OrderedDict()
    for (k, n) in sorted(set(T.tags) | T.opaque):
        kinds.setdefault(k, []).append(n)
    L.append("/* forward declarations */")
    for k, names in kinds.items():
        for n in names:
            L.append(f"{k} {n};" if k != "enum" else f"/* enum {n} */")
    L.append("")
    L.append("/* enums */")
    for (k, n), t in sorted(T.tags.items()):
        if k != "enum":
            continue
        body = ",\n".join(f"    {m['name']} = {m['value']}" for m in t["members"])
        L.append(f"enum {n} {{\n{body}\n}};")
    L.append("")

    # typedefs; enum/struct/union ones are plain "typedef struct X X;"
    L.append("/* typedefs */")
    for name, (text, _) in sorted(T.typedefs.items()):
        L.append(f"typedef {text};")
    L.append("")

    # order struct/union definitions so by-value members come first
    done, order = set(), []

    def deps(t):
        for m in t["members"]:
            if derivs(m["type"]) and derivs(m["type"])[0] != DT_ARY:
                continue
            if any(d in (DT_PTR, DT_FCN) for d in derivs(m["type"])):
                continue
            b = m["type"] & 15
            if b in (8, 9):
                yield (KIND[b], m["tag"])

    def visit(key, stack=()):
        if key in done or key not in T.tags or key in stack:
            return
        for d in deps(T.tags[key]):
            visit(d, stack + (key,))
        done.add(key)
        order.append(key)

    for key in sorted(T.tags):
        if key[0] != "enum":
            visit(key)

    L.append("/* structs and unions; member offsets in trailing comments */")
    asserts = []
    for key in order:
        t = T.tags[key]
        k, n = key
        L.append(f"{k} {n} {{  /* size {t['size']:#x} */")
        for m in t["members"]:
            if m["cls"] == C_FIELD:
                bt = BASE[m["type"] & 15]
                L.append(f"    {bt} {m['name']} : {m['size']};  /* bit {m['off']} */")
            else:
                L.append(f"    {m['decl']};  /* {m['off']:#x} */")
        L.append("};")
        asserts.append((k, n, t["size"]))
    L += ["", "#endif", ""]
    with open(os.path.join(out, "include", "types.h"), "w") as f:
        f.write("\n".join(L))
    # size checks live in a separate file so a bad assert doesn't break the headers
    with open(os.path.join(out, "include", "type_sizes.h"), "w") as f:
        f.write('/* compile-check: gcc -m32 -malign-double -fsyntax-only */\n#include "types.h"\n')
        for k, n, sz in asserts:
            f.write(f"_Static_assert(sizeof({k} {n}) == {sz}, \"{k} {n}\");\n")


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    sym, out = sys.argv[1:]
    recs = parse(open(sym, "rb").read())
    T, labels, funcs, globs, sld = decode(recs)
    for d in ("include", "src"):
        os.makedirs(os.path.join(out, d), exist_ok=True)

    emit_types(T, out)

    # line table: assign each (addr, line) to the function containing it
    funcs.sort(key=lambda f: f["addr"])
    starts = [f["addr"] for f in funcs]
    import bisect
    for f in funcs:
        f["lines"] = []
    for a, ln in sld:
        i = bisect.bisect_right(starts, a) - 1
        if i >= 0 and funcs[i]["end"] and a < funcs[i]["end"]:
            funcs[i]["lines"].append((a, ln))

    # common (uninitialised) externs carry their size in the value field; the address is in the label
    lab = {n: a for a, t, n in labels if t in (1, 2)}
    for g in globs:
        if g["cls"] == C_EXT and g["val"] < 0x80000000 and g["name"] in lab:
            g["val"] = lab[g["name"]]

    # globals: dedupe by (name, address)
    seen, gl = set(), []
    for g in globs:
        k = (g["name"], g["val"], g["cls"], g["obj"] if g["cls"] == C_STAT else "")
        if k in seen:
            continue
        seen.add(k)
        gl.append(g)
    gl.sort(key=lambda g: (g["val"], g["name"]))

    with open(os.path.join(out, "include", "globals.h"), "w") as f:
        f.write('/* generated by tools/symdump.py -- do not edit */\n#include "types.h"\n\n')
        done = set()
        for g in gl:
            if g["cls"] == C_EXT and g["name"] not in done:
                done.add(g["name"])
                f.write(f"extern {g['decl']};  /* {g['val']:#010x}, size {g['size']:#x} */\n")
        f.write("\n/* statics (file scope or function scope); not valid C to redeclare, for reference */\n")
        for g in gl:
            if g["cls"] == C_STAT:
                f.write(f"/* static {g['decl']};  {where(g)}  [{g['obj']}] */\n")

    # source skeletons grouped by source file
    bysrc = collections.defaultdict(list)
    for fn in funcs:
        bysrc[fn["file"]].append(fn)
    names, used = {}, {}
    for path in sorted(bysrc):
        base = path.replace("/", "\\").split("\\")[-1]
        if base.lower() in used and used[base.lower()] != path:
            base = re.sub(r"\W", "_", path)
        used[base.lower()] = path
        names[path] = base
    for path, fl in bysrc.items():
        with open(os.path.join(out, "src", names[path] + (".c" if not names[path].lower().endswith(".c") else "")), "w") as f:
            f.write(f"/* {path} -- generated by tools/symdump.py */\n#include \"types.h\"\n#include \"globals.h\"\n\n")
            for fn in sorted(fl, key=lambda x: x["addr"]):
                size = fn["end"] - fn["addr"]
                f.write(f"/* {fn['addr']:#010x}..{fn['end']:#010x} ({size:#x} bytes), "
                        f"lines {fn['line']}-{fn['endline']}, frame {fn['frame']:#x}, "
                        f"saved mask {fn['mask']:#010x} @fp{fn['maskoff']:+d} */\n")
                f.write(func_signature(T, fn) + "\n{\n")
                for a in fn["args"]:
                    f.write(f"    /* arg {a['decl']};  {where(a)} */\n")
                argnames = {a["name"] for a in fn["args"]}
                path = ()
                for v in fn["locals"]:
                    if v["name"] in argnames:
                        continue  # parameter kept in a register: already declared
                    common = 0
                    while common < min(len(path), len(v["block"])) and path[common] == v["block"][common]:
                        common += 1
                    for i in range(len(path) - 1, common - 1, -1):
                        f.write("    " + "  " * i + "}\n")
                    for i in range(common, len(v["block"])):
                        f.write("    " + "  " * i + "{\n")
                    path = v["block"]
                    f.write("    " + "  " * len(path) + f"{v['decl']};  /* {where(v)} */\n")
                for i in range(len(path) - 1, -1, -1):
                    f.write("    " + "  " * i + "}\n")
                f.write("    /* TODO: decompile */\n}\n\n")

    # label dumps
    labels.sort()
    with open(os.path.join(out, "symbols.txt"), "w") as f:
        for a, t, n in labels:
            f.write(f"{a:08x} {n}\n")
    with open(os.path.join(out, "symbol_addrs.txt"), "w") as f:
        fa = {fn["addr"]: fn for fn in funcs}
        gv = {}
        for g in gl:
            if g["val"] >= 0x80000000:
                gv.setdefault(g["val"], g)
        for a, t, n in labels:
            if a in fa and fa[a]["name"] == n:
                f.write(f"{n} = {a:#x}; // type:func size:{fa[a]['end'] - a:#x}\n")
            elif a in gv and gv[a]["name"] == n:
                sz = gv[a]["size"]
                f.write(f"{n} = {a:#x}; // type:data" + (f" size:{sz:#x}" if sz else "") + "\n")
            else:
                f.write(f"{n} = {a:#x};\n")

    def jfunc(fn):
        d = dict(fn)
        d["rtype"] = None
        d["signature"] = func_signature(T, fn)
        d["args"] = [dict(name=a["name"], decl=a["decl"], where=where(a)) for a in fn["args"]]
        d["locals"] = [dict(name=a["name"], decl=a["decl"], where=where(a), block=list(a["block"])) for a in fn["locals"]]
        return d
    json.dump([jfunc(fn) for fn in funcs], open(os.path.join(out, "functions.json"), "w"), indent=1)
    json.dump([dict(name=g["name"], decl=g["decl"], addr=g["val"] if g["val"] >= 0x80000000 else None,
                    offset=None if g["val"] >= 0x80000000 else g["val"], size=g["size"],
                    storage="extern" if g["cls"] == C_EXT else "static", obj=g["obj"]) for g in gl],
              open(os.path.join(out, "globals.json"), "w"), indent=1)
    json.dump([{k: v for k, v in t.items() if k != "sig"} for t in T.tags.values()],
              open(os.path.join(out, "types.json"), "w"), indent=1)
    with open(os.path.join(out, "conflicts.txt"), "w") as f:
        f.write("\n".join(T.conflicts) + "\n")

    print(f"{len(labels)} labels, {len(funcs)} functions in {len(bysrc)} source files, "
          f"{len(gl)} globals/statics, {len(T.tags)} tags, {len(T.typedefs)} typedefs, "
          f"{len(T.conflicts)} conflicts")


if __name__ == "__main__":
    main()
