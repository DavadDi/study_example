# golang interface

## 1. 结构定义

![示例图](http://research.swtch.com/gointer2.png)

[iface define](https://github.com/golang/go/blob/master/src/runtime/runtime2.go#L143)

	type iface struct {
		tab  *itab
		data unsafe.Pointer
	}
	
	
[itab define](https://github.com/golang/go/blob/master/src/runtime/runtime2.go#L634)

	// layout of Itab known to compilers
	// allocated in non-garbage-collected memory
	// Needs to be in sync with
	// ../cmd/compile/internal/gc/reflect.go:/^func.dumptypestructs.
	type itab struct {
		inter  *interfacetype
		_type  *_type
		link   *itab
		bad    int32
		inhash int32      // has this itab been added to hash?
		fun    [1]uintptr // variable sized
	}
	

[interfacetype](https://github.com/golang/go/blob/34c480afa89a4efa917f30592d8c095dbaf7612e/src/runtime/type.go#L342)

	type interfacetype struct {
		typ     _type
		pkgpath name
		mhdr    []imethod
	}
	
[_type define](https://github.com/golang/go/blob/34c480afa89a4efa917f30592d8c095dbaf7612e/src/runtime/type.go#L28)

	// Needs to be in sync with ../cmd/compile/internal/ld/decodesym.go:/^func.commonsize,
	// ../cmd/compile/internal/gc/reflect.go:/^func.dcommontype and
	// ../reflect/type.go:/^type.rtype.
	type _type struct {
		size       uintptr
		ptrdata    uintptr // size of memory prefix holding all pointers
		hash       uint32
		tflag      tflag
		align      uint8
		fieldalign uint8
		kind       uint8
		alg        *typeAlg
		
		// gcdata stores the GC type data for the garbage collector.
		// If the KindGCProg bit is set in kind, gcdata is a GC program.
		// Otherwise it is a ptrmask bitmap. See mbitmap.go for details.
		
		gcdata    *byte
		str       nameOff
		ptrToThis typeOff
	}
	
## 2. 测试源码
	
[main.go](https://github.com/DavadDi/go_study/blob/master/src/interface_test/main.go)


## 3. gdb 调测

	(gdb) info locals
	b = 200
	s = {tab = 0x7ffff7e0f1c0, data = 0xc82000a3b0}
	nothing = {_type = 0x4c1360, data = 0xc82000a430}
	face = {tab = 0x0, data = 0x0}
	any = {_type = 0x4e4a80, data = 0xc82000a3c0}
	
	
	(gdb) p  *s.tab
	$24 = {inter = 0x4dffa0, _type = 0x4e4a80, link = 0x0, bad = 0, unused = 0, fun = {4200784}}
	
	(gdb) p  *s.tab.inter
	$26 = {typ = {size = 16, ptrdata = 16, hash = 3958674519, _unused = 0 '\000', align = 8 '\b', fieldalign = 8 '\b', kind = 20 '\024', alg = 0x592770 <runtime.algarray+240>,
	    gcdata = 0x537142 "\003\004\005\006\a\b\t\n\r\016\017\020\021\022\025\026\031\032\033\037,568>?Ur\236\237\325\365\377", _string = 0x5161d0, x = 0x4e0018, ptrto = 0x4b6e80,
	    zero = 0x539c40 <runtime.zerovalue> ""}, mhdr = {array = 0x4e0000, len = 1, cap = 1}}
	(gdb) p  *s.tab._type
	$27 = {size = 8, ptrdata = 0, hash = 1148337467, _unused = 0 '\000', align = 8 '\b', fieldalign = 8 '\b', kind = 139 '\213', alg = 0x5926d0 <runtime.algarray+80>,
	  gcdata = 0x537140 "\001\002\003\004\005\006\a\b\t\n\r\016\017\020\021\022\025\026\031\032\033\037,568>?Ur\236\237\325\365\377", _string = 0x5161c0, x = 0x4e4ac8, ptrto = 0x4e4d20,
	  zero = 0x539c40 <runtime.zerovalue> ""}
	(gdb) p  *s.tab.inter.typ._string
	$28 = 0x50b930 "main.Stringer"
	(gdb) p  *s.tab._type._string
	$29 = 0x50b920 "main.Binary"
	
	

[see](https://research.swtch.com/interfaces)

	
	
