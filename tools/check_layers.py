#!/usr/bin/env python3
"""Layer/architecture rules for slang-math (the code-quality-matrix "arch rules" cell).

slang-math is a flat, single-module header library - there is no layer matrix
to enforce. The architecture contract here is the second check, applied over
include/slang-math/**:

  no cyclic #include dependencies, detected as file-level strongly-connected
  components (header-only libraries break on true cycles; partial/deferred
  cycles that happen to compile are still an architecture smell and fail here).

Exit codes: 0 = clean, 1 = violations, 77 = skipped (surface not found).
Usage: python tools/check_layers.py [--report]
"""
import os
import re
import sys

sys.stdout.reconfigure(encoding='utf-8', errors='replace')

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# Leaf-first: a module may include only modules EARLIER in this list (or itself).
ORDER = ['slang-math']
ROOTS = {'slang-math/': 'include'}
EXTERNAL = ()
SCAN = ['include']

INC = re.compile(r'^\s*#\s*include\s+"([\w./-]+)"')


def module_of(_rel):
    return 'slang-math'  # flat: one module; the gate is the cycle check


def resolve(inc, from_file):
    for prefix, base in ROOTS.items():
        if inc.startswith(prefix):
            return os.path.normpath(os.path.join(ROOT, base, inc))
    return os.path.normpath(os.path.join(os.path.dirname(from_file), inc))


def scc(nodes, adj):
    """Tarjan, iterative: strongly-connected components over the file graph."""
    idx, low, on, stack, out = {}, {}, set(), [], []
    counter = [0]
    for start in nodes:
        if start in idx:
            continue
        work = [(start, iter(sorted(adj.get(start, ()))))]
        idx[start] = low[start] = counter[0]
        counter[0] += 1
        stack.append(start)
        on.add(start)
        while work:
            v, it = work[-1]
            advanced = False
            for w in it:
                if w not in idx:
                    idx[w] = low[w] = counter[0]
                    counter[0] += 1
                    stack.append(w)
                    on.add(w)
                    work.append((w, iter(sorted(adj.get(w, ())))))
                    advanced = True
                    break
                elif w in on:
                    low[v] = min(low[v], idx[w])
            if advanced:
                continue
            work.pop()
            if work:
                pv = work[-1][0]
                low[pv] = min(low[pv], low[v])
            if low[v] == idx[v]:
                comp = []
                while True:
                    w = stack.pop()
                    on.discard(w)
                    comp.append(w)
                    if w == v:
                        break
                out.append(comp)
    return out


def main():
    report = '--report' in sys.argv
    files = {}
    for scan in SCAN:
        for dp, _, fs in os.walk(os.path.join(ROOT, scan)):
            if any(x in dp for x in ('build', '_deps', '.git')):
                continue
            for f in fs:
                if f.endswith(('.hpp', '.cpp', '.h', '.c')):
                    p = os.path.normpath(os.path.join(dp, f))
                    files[p] = os.path.relpath(p, ROOT)
    if not files:
        print(f'skip: no sources under {SCAN} in {ROOT}')
        return 77
    adj = {p: set() for p in files}
    edge_count = 0
    unresolved = []
    for p, rel in files.items():
        for line in open(p, encoding='utf-8', errors='replace'):
            m = INC.match(line)
            if not m:
                continue
            t = resolve(m.group(1), p)
            if t in adj and t != p:
                adj[p].add(t)
                edge_count += 1
            elif t not in adj:
                unresolved.append(f'{rel}: "{m.group(1)}"')
    violations = []
    cycles = [c for c in scc(list(files), adj) if len(c) > 1]
    for c in cycles:
        names = sorted(files[x] for x in c)
        violations.append('include cycle: ' + ' <-> '.join(names))
    if report:
        print(f'measured include edges: {edge_count}')
        if unresolved:
            print('unresolved internal-looking includes (info only):')
            for u in unresolved[:10]:
                print(f'  {u}')
    if violations:
        print(f'{len(violations)} layer violation(s):')
        for v in violations:
            print('  ' + v)
        return 1
    print(f'layers clean ({len(files)} files, {edge_count} include edges, no cycles)')
    return 0


if __name__ == '__main__':
    sys.exit(main())
