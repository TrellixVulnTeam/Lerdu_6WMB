SET(OS_CONTRIB_V8_SRC
./v8/test/cctest/test-accessors.cc
./v8/test/cctest/test-ast.cc
./v8/test/cctest/test-threads.cc
./v8/test/cctest/test-disasm-x64.cc
./v8/test/cctest/test-assembler-x64.cc
./v8/test/cctest/test-deoptimization.cc
./v8/test/cctest/test-date.cc
./v8/test/cctest/test-compiler.cc
./v8/test/cctest/test-platform-tls.cc
./v8/test/cctest/test-disasm-ia32.cc
./v8/test/cctest/test-lock.cc
./v8/test/cctest/test-bignum.cc
./v8/test/cctest/test-regexp.cc
./v8/test/cctest/test-fixed-dtoa.cc
./v8/test/cctest/test-random.cc
./v8/test/cctest/test-disasm-mips.cc
./v8/test/cctest/test-dtoa.cc
./v8/test/cctest/test-fast-dtoa.cc
./v8/test/cctest/test-func-name-inference.cc
./v8/test/cctest/test-mark-compact.cc
./v8/test/cctest/test-serialize.cc
./v8/test/cctest/test-weakmaps.cc
./v8/test/cctest/test-macro-assembler-x64.cc
./v8/test/cctest/test-assembler-arm.cc
./v8/test/cctest/test-reloc-info.cc
./v8/test/cctest/test-dataflow.cc
./v8/test/cctest/test-debug.cc
./v8/test/cctest/test-alloc.cc
./v8/test/cctest/test-dictionary.cc
./v8/test/cctest/test-hashmap.cc
./v8/test/cctest/test-diy-fp.cc
./v8/test/cctest/test-log.cc
./v8/test/cctest/test-unbound-queue.cc
./v8/test/cctest/test-platform-nullos.cc
./v8/test/cctest/test-version.cc
./v8/test/cctest/test-lockers.cc
./v8/test/cctest/gay-fixed.cc
./v8/test/cctest/test-decls.cc
./v8/test/cctest/test-heap-profiler.cc
./v8/test/cctest/test-utils.cc
./v8/test/cctest/test-platform-win32.cc
./v8/test/cctest/test-conversions.cc
./v8/test/cctest/test-assembler-ia32.cc
./v8/test/cctest/test-liveedit.cc
./v8/test/cctest/test-double.cc
./v8/test/cctest/cctest.cc
./v8/test/cctest/test-bignum-dtoa.cc
./v8/test/cctest/test-list.cc
./v8/test/cctest/test-thread-termination.cc
./v8/test/cctest/test-cpu-profiler.cc
./v8/test/cctest/test-profile-generator.cc
./v8/test/cctest/test-sockets.cc
./v8/test/cctest/test-strings.cc
./v8/test/cctest/test-heap.cc
./v8/test/cctest/test-disasm-arm.cc
./v8/test/cctest/test-assembler-mips.cc
./v8/test/cctest/gay-precision.cc
./v8/test/cctest/test-hashing.cc
./v8/test/cctest/test-circular-queue.cc
./v8/test/cctest/gay-shortest.cc
./v8/test/cctest/test-platform-linux.cc
./v8/test/cctest/test-strtod.cc
./v8/test/cctest/test-api.cc
./v8/test/cctest/test-parsing.cc
./v8/test/cctest/test-platform-macos.cc
./v8/test/cctest/test-spaces.cc
./v8/test/cctest/test-flags.cc
./v8/test/cctest/test-log-stack-tracer.cc
./v8/build/gyp/test/same-gyp-name/src/subdir1/main1.cc
./v8/build/gyp/test/same-gyp-name/src/subdir2/main2.cc
./v8/build/gyp/test/toolsets/toolsets.cc
./v8/build/gyp/test/toolsets/main.cc
./v8/build/gyp/test/mac/archs/my_file.cc
./v8/build/gyp/test/mac/archs/my_main_file.cc
./v8/build/gyp/test/mac/libraries/subdir/hello.cc
./v8/build/gyp/test/mac/sdkroot/file.cc
./v8/build/gyp/test/mac/prefixheader/file.cc
./v8/build/gyp/test/win/linker-flags/opt-icf.cc
./v8/build/gyp/test/win/linker-flags/opt-ref.cc
./v8/build/gyp/test/win/linker-flags/additional-deps.cc
./v8/build/gyp/test/win/linker-flags/library-directories-reference.cc
./v8/build/gyp/test/win/linker-flags/default-libs.cc
./v8/build/gyp/test/win/linker-flags/hello.cc
./v8/build/gyp/test/win/linker-flags/subsystem-windows.cc
./v8/build/gyp/test/win/linker-flags/delay-load.cc
./v8/build/gyp/test/win/linker-flags/library-directories-define.cc
./v8/build/gyp/test/win/compiler-flags/warning-level3.cc
./v8/build/gyp/test/win/compiler-flags/warning-level2.cc
./v8/build/gyp/test/win/compiler-flags/warning-as-error.cc
./v8/build/gyp/test/win/compiler-flags/warning-level1.cc
./v8/build/gyp/test/win/compiler-flags/buffer-security.cc
./v8/build/gyp/test/win/compiler-flags/runtime-library-mdd.cc
./v8/build/gyp/test/win/compiler-flags/additional-options.cc
./v8/build/gyp/test/win/compiler-flags/runtime-library-mt.cc
./v8/build/gyp/test/win/compiler-flags/runtime-library-md.cc
./v8/build/gyp/test/win/compiler-flags/hello.cc
./v8/build/gyp/test/win/compiler-flags/warning-level4.cc
./v8/build/gyp/test/win/compiler-flags/runtime-library-mtd.cc
./v8/build/gyp/test/win/compiler-flags/function-level-linking.cc
./v8/build/gyp/test/win/compiler-flags/character-set-unicode.cc
./v8/build/gyp/test/win/compiler-flags/exception-handling-on.cc
./v8/build/gyp/test/win/compiler-flags/rtti-on.cc
./v8/build/gyp/test/win/compiler-flags/runtime-checks.cc
./v8/build/gyp/test/win/compiler-flags/character-set-mbcs.cc
./v8/build/gyp/test/win/long-command-line/hello.cc
./v8/build/gyp/test/relative/foo/b/b.cc
./v8/build/gyp/test/relative/foo/a/a.cc
./v8/build/gyp/test/relative/foo/a/c/c.cc
./v8/build/gyp/test/make/main.cc
./v8/build/gyp/test/rules-dirname/src/subdir/main.cc
./v8/build/gyp/test/cxxflags/cxxflags.cc
./v8/tools/oom_dump/oom_dump.cc
./v8/tools/gcmole/gcmole.cc
./v8/src/string-stream.cc
./v8/src/liveedit.cc
./v8/src/x64/codegen-x64.cc
./v8/src/x64/lithium-x64.cc
./v8/src/x64/assembler-x64.cc
./v8/src/x64/debug-x64.cc
./v8/src/x64/stub-cache-x64.cc
./v8/src/x64/full-codegen-x64.cc
./v8/src/x64/frames-x64.cc
./v8/src/x64/code-stubs-x64.cc
./v8/src/x64/builtins-x64.cc
./v8/src/x64/lithium-gap-resolver-x64.cc
./v8/src/x64/regexp-macro-assembler-x64.cc
./v8/src/x64/disasm-x64.cc
./v8/src/x64/deoptimizer-x64.cc
./v8/src/x64/macro-assembler-x64.cc
./v8/src/x64/cpu-x64.cc
./v8/src/x64/ic-x64.cc
./v8/src/x64/lithium-codegen-x64.cc
./v8/src/x64/simulator-x64.cc
./v8/src/snapshot-common.cc
./v8/src/debug.cc
./v8/src/snapshot-empty.cc
./v8/src/objects-debug.cc
./v8/src/conversions.cc
./v8/src/handles.cc
./v8/src/preparse-data.cc
./v8/src/v8-counters.cc
./v8/src/debug-agent.cc
./v8/src/lithium-allocator.cc
./v8/src/bignum-dtoa.cc
./v8/src/objects-printer.cc
./v8/src/counters.cc
./v8/src/platform-nullos.cc
./v8/src/disassembler.cc
./v8/src/d8-windows.cc
./v8/src/fixed-dtoa.cc
./v8/src/contexts.cc
./v8/src/runtime.cc
./v8/src/platform-macos.cc
./v8/src/utils.cc
./v8/src/mark-compact.cc
./v8/src/objects-visiting.cc
./v8/src/dateparser.cc
./v8/src/codegen.cc
./v8/src/scopeinfo.cc
./v8/src/serialize.cc
./v8/src/assembler.cc
./v8/src/regexp-macro-assembler.cc
./v8/src/checks.cc
./v8/src/jsregexp.cc
./v8/src/inspector.cc
./v8/src/elements.cc
./v8/src/zone.cc
./v8/src/v8preparserdll-main.cc
./v8/src/preparser.cc
./v8/src/platform-freebsd.cc
./v8/src/log-utils.cc
./v8/src/platform-solaris.cc
./v8/src/v8threads.cc
./v8/src/elements-kind.cc
./v8/src/func-name-inferrer.cc
./v8/src/profile-generator.cc
./v8/src/scanner-character-streams.cc
./v8/src/win32-math.cc
./v8/src/messages.cc
./v8/src/transitions.cc
./v8/src/bootstrapper.cc
./v8/src/v8utils.cc
./v8/src/flags.cc
./v8/src/platform-posix.cc
./v8/src/strtod.cc
./v8/src/prettyprinter.cc
./v8/src/store-buffer.cc
./v8/src/factory.cc
./v8/src/cached-powers.cc
./v8/src/runtime-profiler.cc
./v8/src/allocation.cc
./v8/src/extensions/statistics-extension.cc
./v8/src/extensions/gc-extension.cc
./v8/src/extensions/externalize-string-extension.cc
./v8/src/api.cc
./v8/src/preparser-api.cc
./v8/src/frames.cc
./v8/src/isolate.cc
./v8/src/interface.cc
./v8/src/builtins.cc
./v8/src/hydrogen.cc
./v8/src/incremental-marking.cc
./v8/src/d8.cc
./v8/src/property.cc
./v8/src/regexp-macro-assembler-tracer.cc
./v8/src/ast.cc
./v8/src/compilation-cache.cc
./v8/src/ia32/ic-ia32.cc
./v8/src/ia32/lithium-ia32.cc
./v8/src/ia32/regexp-macro-assembler-ia32.cc
./v8/src/ia32/lithium-gap-resolver-ia32.cc
./v8/src/ia32/lithium-codegen-ia32.cc
./v8/src/ia32/stub-cache-ia32.cc
./v8/src/ia32/full-codegen-ia32.cc
./v8/src/ia32/deoptimizer-ia32.cc
./v8/src/ia32/code-stubs-ia32.cc
./v8/src/ia32/simulator-ia32.cc
./v8/src/ia32/cpu-ia32.cc
./v8/src/ia32/frames-ia32.cc
./v8/src/ia32/debug-ia32.cc
./v8/src/ia32/codegen-ia32.cc
./v8/src/ia32/disasm-ia32.cc
./v8/src/ia32/macro-assembler-ia32.cc
./v8/src/ia32/builtins-ia32.cc
./v8/src/ia32/assembler-ia32.cc
./v8/src/liveobjectlist.cc
./v8/src/log.cc
./v8/src/execution.cc
./v8/src/stub-cache.cc
./v8/src/global-handles.cc
./v8/src/interpreter-irregexp.cc
./v8/src/scopes.cc
./v8/src/heap-profiler.cc
./v8/src/ic.cc
./v8/src/safepoint-table.cc
./v8/src/lithium.cc
./v8/src/compiler.cc
./v8/src/unicode.cc
./v8/src/diy-fp.cc
./v8/src/scanner.cc
./v8/src/deoptimizer.cc
./v8/src/regexp-macro-assembler-irregexp.cc
./v8/src/platform-cygwin.cc
./v8/src/arm/codegen-arm.cc
./v8/src/arm/ic-arm.cc
./v8/src/arm/assembler-arm.cc
./v8/src/arm/constants-arm.cc
./v8/src/arm/full-codegen-arm.cc
./v8/src/arm/deoptimizer-arm.cc
./v8/src/arm/regexp-macro-assembler-arm.cc
./v8/src/arm/simulator-arm.cc
./v8/src/arm/cpu-arm.cc
./v8/src/arm/lithium-codegen-arm.cc
./v8/src/arm/debug-arm.cc
./v8/src/arm/code-stubs-arm.cc
./v8/src/arm/builtins-arm.cc
./v8/src/arm/disasm-arm.cc
./v8/src/arm/lithium-gap-resolver-arm.cc
./v8/src/arm/frames-arm.cc
./v8/src/arm/macro-assembler-arm.cc
./v8/src/arm/lithium-arm.cc
./v8/src/arm/stub-cache-arm.cc
./v8/src/atomicops_internals_x86_gcc.cc
./v8/src/v8.cc
./v8/src/mips/constants-mips.cc
./v8/src/mips/ic-mips.cc
./v8/src/mips/lithium-gap-resolver-mips.cc
./v8/src/mips/stub-cache-mips.cc
./v8/src/mips/code-stubs-mips.cc
./v8/src/mips/full-codegen-mips.cc
./v8/src/mips/assembler-mips.cc
./v8/src/mips/builtins-mips.cc
./v8/src/mips/cpu-mips.cc
./v8/src/mips/disasm-mips.cc
./v8/src/mips/debug-mips.cc
./v8/src/mips/lithium-mips.cc
./v8/src/mips/frames-mips.cc
./v8/src/mips/deoptimizer-mips.cc
./v8/src/mips/macro-assembler-mips.cc
./v8/src/mips/codegen-mips.cc
./v8/src/mips/lithium-codegen-mips.cc
./v8/src/mips/regexp-macro-assembler-mips.cc
./v8/src/mips/simulator-mips.cc
./v8/src/token.cc
./v8/src/platform-win32.cc
./v8/src/platform-openbsd.cc
./v8/src/version.cc
./v8/src/accessors.cc
./v8/src/once.cc
./v8/src/d8-readline.cc
./v8/src/variables.cc
./v8/src/code-stubs.cc
./v8/src/date.cc
./v8/src/spaces.cc
./v8/src/parser.cc
./v8/src/type-info.cc
./v8/src/data-flow.cc
./v8/src/mksnapshot.cc
./v8/src/full-codegen.cc
./v8/src/heap.cc
./v8/src/hydrogen-instructions.cc
./v8/src/string-search.cc
./v8/src/d8-posix.cc
./v8/src/optimizing-compiler-thread.cc
./v8/src/dtoa.cc
./v8/src/rewriter.cc
./v8/src/circular-queue.cc
./v8/src/d8-debug.cc
./v8/src/fast-dtoa.cc
./v8/src/objects.cc
./v8/src/cpu-profiler.cc
./v8/src/regexp-stack.cc
./v8/src/v8conversions.cc
./v8/src/gdb-jit.cc
./v8/src/bignum.cc
./v8/src/platform-linux.cc
./v8/src/v8dll-main.cc
./v8/preparser/preparser-process.cc
./v8/samples/lineprocessor.cc
./v8/samples/shell.cc
./v8/samples/process.cc



./v8/CMakeFiles/CompilerIdC/CMakeCCompilerId.c
./v8/build/gyp/test/assembly/src/lib1.c
./v8/build/gyp/test/assembly/src/program.c
./v8/build/gyp/test/ninja/chained-dependency/chained.c
./v8/build/gyp/test/ninja/action_dependencies/src/a.c
./v8/build/gyp/test/ninja/action_dependencies/src/b.c
./v8/build/gyp/test/ninja/action_dependencies/src/c.c
./v8/build/gyp/test/configurations/x64/configurations.c
./v8/build/gyp/test/configurations/basics/configurations.c
./v8/build/gyp/test/configurations/inheritance/configurations.c
./v8/build/gyp/test/configurations/target_platform/right.c
./v8/build/gyp/test/configurations/target_platform/front.c
./v8/build/gyp/test/configurations/target_platform/left.c
./v8/build/gyp/test/library/src/lib1.c
./v8/build/gyp/test/library/src/lib2.c
./v8/build/gyp/test/library/src/program.c
./v8/build/gyp/test/library/src/lib2_moveable.c
./v8/build/gyp/test/library/src/lib1_moveable.c
./v8/build/gyp/test/variants/src/variants.c
./v8/build/gyp/test/exclusion/hello.c
./v8/build/gyp/test/generator-output/src/prog1.c
./v8/build/gyp/test/generator-output/src/subdir3/prog3.c
./v8/build/gyp/test/generator-output/src/subdir2/deeper/deeper.c
./v8/build/gyp/test/generator-output/src/subdir2/prog2.c
./v8/build/gyp/test/generator-output/actions/subdir1/program.c
./v8/build/gyp/test/generator-output/rules/subdir1/program.c
./v8/build/gyp/test/scons_tools/tools.c
./v8/build/gyp/test/toplevel-dir/src/sub2/prog2.c
./v8/build/gyp/test/toplevel-dir/src/sub1/prog1.c
./v8/build/gyp/test/same-name/src/func.c
./v8/build/gyp/test/same-name/src/prog1.c
./v8/build/gyp/test/same-name/src/prog2.c
./v8/build/gyp/test/same-name/src/subdir1/func.c
./v8/build/gyp/test/same-name/src/subdir2/func.c
./v8/build/gyp/test/subdirectory/src/prog1.c
./v8/build/gyp/test/subdirectory/src/subdir/prog2.c
./v8/build/gyp/test/subdirectory/src/subdir/subdir2/prog3.c
./v8/build/gyp/test/home_dot_gyp/src/printfoo.c
./v8/build/gyp/test/link-objects/base.c
./v8/build/gyp/test/link-objects/extra.c
./v8/build/gyp/test/mac/non-strs-flattened-to-env/main.c
./v8/build/gyp/test/mac/postbuild-multiple-configurations/main.c
./v8/build/gyp/test/mac/missing-cfbundlesignature/file.c
./v8/build/gyp/test/mac/copy-dylib/empty.c
./v8/build/gyp/test/mac/libraries/subdir/mylib.c
./v8/build/gyp/test/mac/postbuild-fail/file.c
./v8/build/gyp/test/mac/postbuild-copy-bundle/empty.c
./v8/build/gyp/test/mac/postbuild-copy-bundle/main.c
./v8/build/gyp/test/mac/xcode-env-order/main.c
./v8/build/gyp/test/mac/framework/empty.c
./v8/build/gyp/test/mac/debuginfo/file.c
./v8/build/gyp/test/mac/postbuilds/file.c
./v8/build/gyp/test/mac/app-bundle/empty.c
./v8/build/gyp/test/mac/loadable-module/module.c
./v8/build/gyp/test/mac/type_envvars/file.c
./v8/build/gyp/test/mac/postbuild-defaults/main.c
./v8/build/gyp/test/mac/depend-on-bundle/bundle.c
./v8/build/gyp/test/mac/depend-on-bundle/executable.c
./v8/build/gyp/test/mac/infoplist-process/main.c
./v8/build/gyp/test/mac/rebuild/empty.c
./v8/build/gyp/test/mac/rebuild/main.c
./v8/build/gyp/test/mac/sourceless-module/empty.c
./v8/build/gyp/test/mac/prefixheader/file.c
./v8/build/gyp/test/mac/strip/file.c
./v8/build/gyp/test/mac/postbuild-static-library/empty.c
./v8/build/gyp/test/product/hello.c
./v8/build/gyp/test/multiple-targets/src/prog1.c
./v8/build/gyp/test/multiple-targets/src/prog2.c
./v8/build/gyp/test/multiple-targets/src/common.c
./v8/build/gyp/test/dependencies/a.c
./v8/build/gyp/test/dependencies/b/b3.c
./v8/build/gyp/test/dependencies/b/b.c
./v8/build/gyp/test/dependencies/c/d.c
./v8/build/gyp/test/dependencies/c/c.c
./v8/build/gyp/test/dependencies/main.c
./v8/build/gyp/test/hello/hello2.c
./v8/build/gyp/test/hello/hello.c
./v8/build/gyp/test/actions/src/subdir1/program.c
./v8/build/gyp/test/make/noload/lib/shared.c
./v8/build/gyp/test/make/noload/main.c
./v8/build/gyp/test/module/src/lib1.c
./v8/build/gyp/test/module/src/lib2.c
./v8/build/gyp/test/module/src/program.c
./v8/build/gyp/test/rules-rebuild/src/main.c
./v8/build/gyp/test/rules/src/subdir4/program.c
./v8/build/gyp/test/rules/src/subdir3/program.c
./v8/build/gyp/test/rules/src/subdir1/program.c
./v8/build/gyp/test/cflags/cflags.c
./v8/build/gyp/test/actions-multiple/src/foo.c
./v8/build/gyp/test/actions-multiple/src/main.c
./v8/build/gyp/test/defines-escaping/defines-escaping.c
./v8/build/gyp/test/include_dirs/src/includes.c
./v8/build/gyp/test/include_dirs/src/subdir/subdir_includes.c
./v8/build/gyp/test/additional-targets/src/dir1/lib1.c
./v8/build/gyp/test/builddir/src/prog1.c
./v8/build/gyp/test/builddir/src/func2.c
./v8/build/gyp/test/builddir/src/func4.c
./v8/build/gyp/test/builddir/src/func3.c
./v8/build/gyp/test/builddir/src/func1.c
./v8/build/gyp/test/builddir/src/subdir2/subdir3/subdir4/subdir5/prog5.c
./v8/build/gyp/test/builddir/src/subdir2/subdir3/subdir4/prog4.c
./v8/build/gyp/test/builddir/src/subdir2/subdir3/prog3.c
./v8/build/gyp/test/builddir/src/subdir2/prog2.c
./v8/build/gyp/test/builddir/src/func5.c
./v8/build/gyp/test/hard_dependency/src/a.c
./v8/build/gyp/test/hard_dependency/src/b.c
./v8/build/gyp/test/hard_dependency/src/d.c
./v8/build/gyp/test/hard_dependency/src/c.c
./v8/build/gyp/test/dependency-copy/src/file2.c
./v8/build/gyp/test/dependency-copy/src/file1.c
./v8/build/gyp/test/rules-variables/src/input_path/subdir/test.c
./v8/build/gyp/test/rules-variables/src/subdir/test.c
./v8/build/gyp/test/rules-variables/src/subdir/input_dirname.c
./v8/build/gyp/test/rules-variables/src/test.input_root.c
./v8/build/gyp/test/rules-variables/src/input_name/test.c
./v8/build/gyp/test/rules-variables/src/input_ext.c
./v8/build/gyp/test/defines/defines.c
./v8/build/gyp/test/msvs/precompiled/hello2.c
./v8/build/gyp/test/msvs/precompiled/hello.c
./v8/build/gyp/test/msvs/precompiled/precomp.c
./v8/build/gyp/test/msvs/uldi2010/hello2.c
./v8/build/gyp/test/msvs/uldi2010/hello.c
./v8/build/gyp/test/msvs/props/hello.c
./v8/build/gyp/test/msvs/config_attrs/hello.c
./v8/build/gyp/test/sibling/src/prog1/prog1.c
./v8/build/gyp/test/sibling/src/prog2/prog2.c
./v8/build/gyp/gyp_dummy.c


)
