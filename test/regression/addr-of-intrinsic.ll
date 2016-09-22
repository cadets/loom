; \file  addr-of-intrinsic.ll
; \brief Regression test for "Cannot take the address of an intrinsic",
;        based on bsdgrep.full.bc
;
; Commands for llvm-lit:
; RUN: %loom -S %s -loom-file %s.policy -o %t.instr.ll

; ModuleID = 'bugpoint-reduced-simplified.bc'
source_filename = "bugpoint-output-415e9b5.bc"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-freebsd11.0"

module asm ".ident\09\22$FreeBSD$\22"
module asm ".ident\09\22$FreeBSD$\22"
module asm ".ident\09\22$FreeBSD$\22"
module asm ".ident\09\22$FreeBSD$\22"

%struct.lzma_stream.2.31.89.118.176.205.234.495 = type { i8*, i64, i64, i8*, i64, i64, %struct.lzma_allocator.0.29.87.116.174.203.232.493*, %struct.lzma_internal_s.1.30.88.117.175.204.233.494*, i8*, i8*, i8*, i8*, i64, i64, i64, i64, i32, i32 }
%struct.lzma_allocator.0.29.87.116.174.203.232.493 = type { i8* (i8*, i64, i64)*, void (i8*, i8*)*, i8* }
%struct.lzma_internal_s.1.30.88.117.175.204.233.494 = type opaque
%struct.gzFile_s.3.32.90.119.177.206.235.496 = type { i32, i8*, i64 }
%struct.__nl_cat_d.4.33.91.120.178.207.236.497 = type { i8*, i32 }
%struct.fastmatch_t.5.34.92.121.179.208.237.498 = type { i64, i64, i32*, i8*, [256 x i32], i32*, i8*, i8*, i32, i8*, i32*, i8*, i8, i8, i8, i8, i8, i8, i8, i8, i8 }
%struct.re_pattern_buffer.6.35.93.122.180.209.238.499 = type { i8*, i64, i64, i64, i8*, i8*, i64, i8 }
%struct.option.7.36.94.123.181.210.239.500 = type { i8*, i32, i32*, i32 }
%struct.__sFILE.11.40.98.127.185.214.243.504 = type { i8*, i32, i32, i16, i16, %struct.__nl_cat_d.4.33.91.120.178.207.236.497, i32, i8*, i32 (i8*)*, i32 (i8*, i8*, i32)*, i64 (i8*, i64, i32)*, i32 (i8*, i8*, i32)*, %struct.__nl_cat_d.4.33.91.120.178.207.236.497, i8*, i32, [3 x i8], [1 x i8], %struct.__nl_cat_d.4.33.91.120.178.207.236.497, i32, i64, %struct.pthread_mutex.8.37.95.124.182.211.240.501*, %struct.pthread.9.38.96.125.183.212.241.502*, i32, i32, %union.__mbstate_t.10.39.97.126.184.213.242.503, i32 }
%struct.pthread_mutex.8.37.95.124.182.211.240.501 = type opaque
%struct.pthread.9.38.96.125.183.212.241.502 = type opaque
%union.__mbstate_t.10.39.97.126.184.213.242.503 = type { i64, [120 x i8] }
%struct._RuneLocale.14.43.101.130.188.217.246.507 = type { [8 x i8], [32 x i8], i32 (i8*, i64, i8**)*, i32 (i32, i8*, i64, i8**)*, i32, [256 x i64], [256 x i32], [256 x i32], %struct._RuneRange.13.42.100.129.187.216.245.506, %struct._RuneRange.13.42.100.129.187.216.245.506, %struct._RuneRange.13.42.100.129.187.216.245.506, i8*, i32 }
%struct._RuneRange.13.42.100.129.187.216.245.506 = type { i32, %struct._RuneEntry.12.41.99.128.186.215.244.505* }
%struct._RuneEntry.12.41.99.128.186.215.244.505 = type { i32, i32, i32, i64* }
%struct.anon.0.18.47.105.134.192.221.250.511 = type { %struct.qentry.17.46.104.133.191.220.249.510*, %struct.qentry.17.46.104.133.191.220.249.510** }
%struct.qentry.17.46.104.133.191.220.249.510 = type { %struct.anon.15.44.102.131.189.218.247.508, %struct.str.16.45.103.132.190.219.248.509 }
%struct.anon.15.44.102.131.189.218.247.508 = type { %struct.qentry.17.46.104.133.191.220.249.510* }
%struct.str.16.45.103.132.190.219.248.509 = type { i64, i64, i8*, i8*, i32 }
%struct.hashTable.20.49.107.136.194.223.252.513 = type { %struct.hashTableItemRec.19.48.106.135.193.222.251.512** }
%struct.hashTableItemRec.19.48.106.135.193.222.251.512 = type { i8*, i32, i8*, i32, i8*, %struct.hashTableItemRec.19.48.106.135.193.222.251.512* }
%struct.file.21.50.108.137.195.224.253.514 = type { i32, i8 }
%struct.stat.23.52.110.139.197.226.255.516 = type { i32, i32, i16, i16, i32, i32, i32, %struct.timespec.22.51.109.138.196.225.254.515, %struct.timespec.22.51.109.138.196.225.254.515, %struct.timespec.22.51.109.138.196.225.254.515, i64, i64, i32, i32, i32, i32, %struct.timespec.22.51.109.138.196.225.254.515 }
%struct.timespec.22.51.109.138.196.225.254.515 = type { i64, i64 }
%struct.FTS.25.54.112.141.199.228.257.518 = type { %struct._ftsent.24.53.111.140.198.227.256.517*, %struct._ftsent.24.53.111.140.198.227.256.517*, %struct._ftsent.24.53.111.140.198.227.256.517**, i32, i8*, i32, i64, i64, i32 (%struct._ftsent.24.53.111.140.198.227.256.517**, %struct._ftsent.24.53.111.140.198.227.256.517**)*, i32, i8* }
%struct._ftsent.24.53.111.140.198.227.256.517 = type { %struct._ftsent.24.53.111.140.198.227.256.517*, %struct._ftsent.24.53.111.140.198.227.256.517*, %struct._ftsent.24.53.111.140.198.227.256.517*, i64, i8*, i8*, i8*, i32, i32, i64, i64, i32, i32, i16, i64, i32, i32, i32, %struct.stat.23.52.110.139.197.226.255.516*, i8*, %struct.FTS.25.54.112.141.199.228.257.518* }
%struct.regmatch_t.26.55.113.142.200.229.258.519 = type { i32, i32 }
%struct.hashtable.28.57.115.144.202.231.260.521 = type { i64, i64, i64, i64, %struct.hashtable_entry.27.56.114.143.201.230.259.520** }
%struct.hashtable_entry.27.56.114.143.201.230.259.520 = type { i8*, i8* }

@lstrm = external global %struct.lzma_stream.2.31.89.118.176.205.234.495, align 8
@lin_buf = external global [32768 x i8], align 16
@lnbuflen = external global i64, align 8
@lnbuf = external global i8*, align 8
@fsiz = external global i64, align 8
@bufrem = external global i64, align 8
@bufpos = external global i8*, align 8
@buffer = external global i8*, align 8
@bzbufdesc = external global i8*, align 8
@laction = external global i32, align 4
@gzbufdesc = external global %struct.gzFile_s.3.32.90.119.177.206.235.496*, align 8
@.str = external unnamed_addr constant [2 x i8], align 1
@errstr = external global [10 x i8*], align 16
@.str.1 = external unnamed_addr constant [1 x i8], align 1
@.str.1.2 = external unnamed_addr constant [17 x i8], align 1
@.str.2 = external unnamed_addr constant [34 x i8], align 1
@.str.3 = external unnamed_addr constant [18 x i8], align 1
@.str.4 = external unnamed_addr constant [72 x i8], align 1
@.str.5 = external unnamed_addr constant [63 x i8], align 1
@.str.6 = external unnamed_addr constant [71 x i8], align 1
@.str.7 = external unnamed_addr constant [32 x i8], align 1
@.str.8 = external unnamed_addr constant [24 x i8], align 1
@.str.9 = external unnamed_addr constant [18 x i8], align 1
@cflags = external global i32, align 4
@eflags = external global i32, align 4
@grepbehave = external global i32, align 4
@binbehave = external global i32, align 4
@filebehave = external global i32, align 4
@devbehave = external global i32, align 4
@dirbehave = external global i32, align 4
@linkbehave = external global i32, align 4
@first = external global i8, align 1
@catalog = external global %struct.__nl_cat_d.4.33.91.120.178.207.236.497*, align 8
@Aflag = external global i64, align 8
@Bflag = external global i64, align 8
@bflag = external global i8, align 1
@cflag = external global i8, align 1
@Hflag = external global i8, align 1
@hflag = external global i8, align 1
@iflag = external global i8, align 1
@lflag = external global i8, align 1
@Lflag = external global i8, align 1
@mflag = external global i8, align 1
@mcount = external global i64, align 8
@mlimit = external global i64, align 8
@nflag = external global i8, align 1
@oflag = external global i8, align 1
@qflag = external global i8, align 1
@sflag = external global i8, align 1
@vflag = external global i8, align 1
@wflag = external global i8, align 1
@xflag = external global i8, align 1
@color = external global i8*, align 8
@label = external global i8*, align 8
@lbflag = external global i8, align 1
@nullflag = external global i8, align 1
@finclude = external global i8, align 1
@fexclude = external global i8, align 1
@dinclude = external global i8, align 1
@dexclude = external global i8, align 1
@patterns = external global i32, align 4
@fg_pattern = external global %struct.fastmatch_t.5.34.92.121.179.208.237.498*, align 8
@r_pattern = external global %struct.re_pattern_buffer.6.35.93.122.180.209.238.499*, align 8
@pattern = external global %struct.__nl_cat_d.4.33.91.120.178.207.236.497*, align 8
@re_error = external global [513 x i8], align 16
@file_err = external global i8, align 1
@matchall = external global i8, align 1
@fpatterns = external global i32, align 4
@dpatterns = external global i32, align 4
@dpattern = external global %struct.__nl_cat_d.4.33.91.120.178.207.236.497*, align 8
@fpattern = external global %struct.__nl_cat_d.4.33.91.120.178.207.236.497*, align 8
@prev = external global i8, align 1
@tail = external global i32, align 4
@dpattern_sz = external global i32, align 4
@fpattern_sz = external global i32, align 4
@pattern_sz = external global i32, align 4
@long_options = external constant [48 x %struct.option.7.36.94.123.181.210.239.500], align 16
@optstr = external global i8*, align 8
@.str.10 = external unnamed_addr constant [5 x i8], align 1
@.str.11 = external unnamed_addr constant [13 x i8], align 1
@.str.12 = external unnamed_addr constant [2 x i8], align 1
@optarg = external global i8*, align 8
@.str.13 = external unnamed_addr constant [5 x i8], align 1
@.str.14 = external unnamed_addr constant [5 x i8], align 1
@.str.15 = external unnamed_addr constant [10 x i8], align 1
@.str.16 = external unnamed_addr constant [8 x i8], align 1
@.str.17 = external unnamed_addr constant [14 x i8], align 1
@.str.18 = external unnamed_addr constant [2 x i8], align 1
@.str.19 = external unnamed_addr constant [14 x i8], align 1
@.str.20 = external unnamed_addr constant [7 x i8], align 1
@.str.21 = external unnamed_addr constant [14 x i8], align 1
@.str.22 = external unnamed_addr constant [5 x i8], align 1
@.str.23 = external unnamed_addr constant [15 x i8], align 1
@.str.24 = external unnamed_addr constant [5 x i8], align 1
@.str.25 = external unnamed_addr constant [4 x i8], align 1
@.str.26 = external unnamed_addr constant [7 x i8], align 1
@.str.27 = external unnamed_addr constant [5 x i8], align 1
@.str.28 = external unnamed_addr constant [5 x i8], align 1
@.str.29 = external unnamed_addr constant [6 x i8], align 1
@.str.30 = external unnamed_addr constant [7 x i8], align 1
@.str.31 = external unnamed_addr constant [4 x i8], align 1
@.str.32 = external unnamed_addr constant [6 x i8], align 1
@.str.33 = external unnamed_addr constant [6 x i8], align 1
@.str.34 = external unnamed_addr constant [5 x i8], align 1
@.str.35 = external unnamed_addr constant [3 x i8], align 1
@.str.36 = external unnamed_addr constant [8 x i8], align 1
@optind = external global i32, align 4
@.str.37 = external unnamed_addr constant [3 x i8], align 1
@__stdoutp = external global %struct.__sFILE.11.40.98.127.185.214.243.504*, align 8
@.str.38 = external unnamed_addr constant [2 x i8], align 1
@__stderrp = external global %struct.__sFILE.11.40.98.127.185.214.243.504*, align 8
@.str.86 = external unnamed_addr constant [11 x i8], align 1
@.str.85 = external unnamed_addr constant [2 x i8], align 1
@__isthreaded = external global i32, align 4
@__mb_sb_limit = external global i32, align 4
@_ThreadRuneLocale = external thread_local global %struct._RuneLocale.14.43.101.130.188.217.246.507*, align 8
@_CurrentRuneLocale = external global %struct._RuneLocale.14.43.101.130.188.217.246.507*, align 8
@_DefaultRuneLocale = external constant %struct._RuneLocale.14.43.101.130.188.217.246.507, align 8
@.str.39 = external unnamed_addr constant [60 x i8], align 1
@.str.40 = external unnamed_addr constant [13 x i8], align 1
@.str.41 = external unnamed_addr constant [5 x i8], align 1
@.str.42 = external unnamed_addr constant [5 x i8], align 1
@.str.43 = external unnamed_addr constant [14 x i8], align 1
@.str.44 = external unnamed_addr constant [6 x i8], align 1
@.str.45 = external unnamed_addr constant [5 x i8], align 1
@.str.46 = external unnamed_addr constant [6 x i8], align 1
@.str.47 = external unnamed_addr constant [7 x i8], align 1
@.str.48 = external unnamed_addr constant [8 x i8], align 1
@.str.49 = external unnamed_addr constant [8 x i8], align 1
@.str.50 = external unnamed_addr constant [12 x i8], align 1
@.str.51 = external unnamed_addr constant [12 x i8], align 1
@.str.52 = external unnamed_addr constant [14 x i8], align 1
@.str.53 = external unnamed_addr constant [15 x i8], align 1
@.str.54 = external unnamed_addr constant [12 x i8], align 1
@.str.55 = external unnamed_addr constant [8 x i8], align 1
@.str.56 = external unnamed_addr constant [6 x i8], align 1
@.str.57 = external unnamed_addr constant [8 x i8], align 1
@.str.58 = external unnamed_addr constant [12 x i8], align 1
@.str.59 = external unnamed_addr constant [16 x i8], align 1
@.str.60 = external unnamed_addr constant [7 x i8], align 1
@.str.61 = external unnamed_addr constant [14 x i8], align 1
@.str.62 = external unnamed_addr constant [5 x i8], align 1
@.str.63 = external unnamed_addr constant [13 x i8], align 1
@.str.64 = external unnamed_addr constant [12 x i8], align 1
@.str.65 = external unnamed_addr constant [14 x i8], align 1
@.str.66 = external unnamed_addr constant [12 x i8], align 1
@.str.67 = external unnamed_addr constant [14 x i8], align 1
@.str.68 = external unnamed_addr constant [19 x i8], align 1
@.str.69 = external unnamed_addr constant [20 x i8], align 1
@.str.70 = external unnamed_addr constant [10 x i8], align 1
@.str.71 = external unnamed_addr constant [5 x i8], align 1
@.str.72 = external unnamed_addr constant [12 x i8], align 1
@.str.73 = external unnamed_addr constant [14 x i8], align 1
@.str.74 = external unnamed_addr constant [6 x i8], align 1
@.str.75 = external unnamed_addr constant [7 x i8], align 1
@.str.76 = external unnamed_addr constant [10 x i8], align 1
@.str.77 = external unnamed_addr constant [12 x i8], align 1
@.str.78 = external unnamed_addr constant [18 x i8], align 1
@.str.79 = external unnamed_addr constant [13 x i8], align 1
@.str.80 = external unnamed_addr constant [8 x i8], align 1
@.str.81 = external unnamed_addr constant [12 x i8], align 1
@.str.82 = external unnamed_addr constant [12 x i8], align 1
@.str.83 = external unnamed_addr constant [3 x i8], align 1
@.str.84 = external unnamed_addr constant [11 x i8], align 1
@queue = external global %struct.anon.0.18.47.105.134.192.221.250.511, align 8
@count = external global i64, align 8
@linesqueued = external global i32, align 4
@.str.87 = external unnamed_addr constant [9 x i8], align 1
@.str.1.88 = external unnamed_addr constant [7 x i8], align 1
@.str.2.89 = external unnamed_addr constant [38 x i8], align 1
@.str.3.92 = external unnamed_addr constant [2 x i8], align 1
@.str.4.93 = external unnamed_addr constant [3 x i8], align 1
@.str.5.96 = external unnamed_addr constant [4 x i8], align 1
@.str.6.97 = external unnamed_addr constant [4 x i8], align 1
@.str.7.98 = external unnamed_addr constant [5 x i8], align 1
@.str.16.99 = external unnamed_addr constant [4 x i8], align 1
@.str.17.100 = external unnamed_addr constant [4 x i8], align 1
@.str.12.103 = external unnamed_addr constant [3 x i8], align 1
@.str.13.104 = external unnamed_addr constant [5 x i8], align 1
@.str.14.105 = external unnamed_addr constant [9 x i8], align 1
@.str.15.106 = external unnamed_addr constant [7 x i8], align 1
@.str.8.108 = external unnamed_addr constant [7 x i8], align 1
@.str.9.111 = external unnamed_addr constant [7 x i8], align 1
@.str.10.114 = external unnamed_addr constant [8 x i8], align 1
@.str.11.117 = external unnamed_addr constant [7 x i8], align 1
@.str.135 = external unnamed_addr constant [8 x i32], align 4
@.str.1.136 = external unnamed_addr constant [8 x i32], align 4
@xmalloc_table = external global %struct.hashTable.20.49.107.136.194.223.252.513*, align 8
@xmalloc_current_blocks = external global i32, align 4
@xmalloc_current = external global i32, align 4
@xmalloc_fail_after = external global i32, align 4
@xmalloc_peak_blocks = external global i32, align 4
@xmalloc_peak = external global i32, align 4
@__func__.xmalloc_init = external unnamed_addr constant [13 x i8], align 1
@.str.8.141 = external unnamed_addr constant [56 x i8], align 1
@.str.16.142 = external unnamed_addr constant [22 x i8], align 1
@.str.17.143 = external unnamed_addr constant [29 x i8], align 1
@.str.144 = external unnamed_addr constant [37 x i8], align 1
@.str.1.145 = external unnamed_addr constant [18 x i8], align 1
@.str.2.146 = external unnamed_addr constant [52 x i8], align 1
@.str.3.147 = external unnamed_addr constant [66 x i8], align 1
@.str.4.148 = external unnamed_addr constant [10 x i8], align 1
@.str.5.149 = external unnamed_addr constant [3 x i8], align 1
@.str.6.150 = external unnamed_addr constant [4 x i8], align 1
@.str.7.151 = external unnamed_addr constant [20 x i8], align 1
@__func__.xmalloc_impl = external unnamed_addr constant [13 x i8], align 1
@.str.9.152 = external unnamed_addr constant [9 x i8], align 1
@.str.10.153 = external unnamed_addr constant [46 x i8], align 1
@.str.11.154 = external unnamed_addr constant [2 x i8], align 1
@__func__.hash_table_add = external unnamed_addr constant [15 x i8], align 1
@.str.18.155 = external unnamed_addr constant [12 x i8], align 1
@__func__.xcalloc_impl = external unnamed_addr constant [13 x i8], align 1
@.str.12.156 = external unnamed_addr constant [46 x i8], align 1
@.str.19.157 = external unnamed_addr constant [23 x i8], align 1
@__func__.xrealloc_impl = external unnamed_addr constant [14 x i8], align 1
@.str.13.158 = external unnamed_addr constant [12 x i8], align 1
@.str.14.159 = external unnamed_addr constant [13 x i8], align 1
@.str.15.160 = external unnamed_addr constant [47 x i8], align 1

; Function Attrs: nounwind sspstrong uwtable
declare i8* @grep_fgetln(%struct.file.21.50.108.137.195.224.253.514*, i64*) #0

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare i32 @grep_refill(%struct.file.21.50.108.137.195.224.253.514*) #2

; Function Attrs: nounwind readonly
declare i8* @memchr(i8*, i32, i64) #3

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare i32 @grep_lnbufgrow(i64) #2

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #4

declare i32 @gzread(%struct.gzFile_s.3.32.90.119.177.206.235.496*, i8*, i32) #5

declare i32 @BZ2_bzRead(i32*, i8*, i8*, i32) #5

declare void @BZ2_bzReadClose(i32*, i8*) #5

declare i64 @lseek(i32, i64, i32) #5

declare i64 @read(i32, i8*, i64) #5

; Function Attrs: nounwind
declare i32 @lzma_code(%struct.lzma_stream.2.31.89.118.176.205.234.495*, i32) #6

; Function Attrs: nounwind sspstrong uwtable
declare %struct.file.21.50.108.137.195.224.253.514* @grep_open(i8*) #0

; Function Attrs: argmemonly nounwind
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i32, i1) #4

declare i32 @open(i8*, i32, ...) #5

declare i32 @fstat(i32, %struct.stat.23.52.110.139.197.226.255.516*) #5

declare i8* @mmap(i8*, i64, i32, i32, i32, i64) #5

declare i32 @madvise(i8*, i64, i32) #5

declare %struct.gzFile_s.3.32.90.119.177.206.235.496* @gzdopen(i32, i8*) #5

declare i8* @BZ2_bzdopen(i32, i8*) #5

; Function Attrs: nounwind
declare i32 @lzma_stream_decoder(%struct.lzma_stream.2.31.89.118.176.205.234.495*, i64, i32) #6

; Function Attrs: nounwind
declare i32 @lzma_alone_decoder(%struct.lzma_stream.2.31.89.118.176.205.234.495*, i64) #6

declare i32 @close(i32) #5

declare void @free(i8*) #5

; Function Attrs: nounwind sspstrong uwtable
declare void @grep_close(%struct.file.21.50.108.137.195.224.253.514*) #0

declare i32 @munmap(i8*, i64) #5

; Function Attrs: nounwind sspstrong uwtable
declare i32 @main(i32, i8**) #0

declare i8* @setlocale(i32, i8*) #5

declare %struct.__nl_cat_d.4.33.91.120.178.207.236.497* @catopen(i8*, i32) #5

declare i8* @getprogname() #5

declare i8* @getenv(i8*) #5

; Function Attrs: nounwind readonly
declare i64 @strlen(i8*) #3

declare i8* @strsep(i8**, i8*) #5

declare i32 @getopt_long(i32, i8**, i8*, %struct.option.7.36.94.123.181.210.239.500*, i32*) #5

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare i32 @__sbistype(i32, i64) #2

declare i32* @__error() #5

; Function Attrs: noreturn
declare void @err(i32, i8*, ...) #7

declare i64 @strtoull(i8*, i8**, i32) #5

; Function Attrs: nounwind readonly
declare i32 @strcasecmp(i8*, i8*) #3

declare i8* @catgets(%struct.__nl_cat_d.4.33.91.120.178.207.236.497*, i32, i32, i8*) #5

; Function Attrs: noreturn
declare void @errx(i32, i8*, ...) #7

; Function Attrs: nounwind sspstrong uwtable
declare void @add_pattern(i8*, i64) #0

; Function Attrs: nounwind sspstrong uwtable
declare void @read_patterns(i8*) #0

declare i64 @strtoll(i8*, i8**, i32) #5

declare i32 @printf(i8*, ...) #5

; Function Attrs: noreturn
declare void @exit(i32) #7

declare i32 @isatty(i32) #5

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare i8* @init_color(i8*) #2

; Function Attrs: nounwind sspstrong uwtable
declare void @add_fpattern(i8*, i32) #0

; Function Attrs: nounwind sspstrong uwtable
declare void @add_dpattern(i8*, i32) #0

; Function Attrs: nounwind sspstrong uwtable
declare void @usage() #0

declare i32 @regcomp(%struct.re_pattern_buffer.6.35.93.122.180.209.238.499*, i8*, i32) #5

declare i64 @regerror(i32, %struct.re_pattern_buffer.6.35.93.122.180.209.238.499*, i8*, i64) #5

declare i32 @setlinebuf(%struct.__sFILE.11.40.98.127.185.214.243.504*) #5

declare i32 @catclose(%struct.__nl_cat_d.4.33.91.120.178.207.236.497*) #5

declare i32 @fprintf(%struct.__sFILE.11.40.98.127.185.214.243.504*, i8*, ...) #5

declare %struct.__sFILE.11.40.98.127.185.214.243.504* @fopen(i8*, i8*) #5

declare i32 @fileno(%struct.__sFILE.11.40.98.127.185.214.243.504*) #5

declare i32 @fclose(%struct.__sFILE.11.40.98.127.185.214.243.504*) #5

declare i64 @getline(i8**, i64*, %struct.__sFILE.11.40.98.127.185.214.243.504*) #5

declare i32 @ferror(%struct.__sFILE.11.40.98.127.185.214.243.504*) #5

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare i32 @__sbmaskrune(i32, i64) #2

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare %struct._RuneLocale.14.43.101.130.188.217.246.507* @__getCurrentRuneLocale() #2

; Function Attrs: nounwind sspstrong uwtable
declare void @enqueue(%struct.str.16.45.103.132.190.219.248.509*) #0

; Function Attrs: nounwind sspstrong uwtable
declare %struct.qentry.17.46.104.133.191.220.249.510* @dequeue() #0

; Function Attrs: nounwind sspstrong uwtable
declare void @printqueue() #0

; Function Attrs: nounwind sspstrong uwtable
declare void @clearqueue() #0

; Function Attrs: nounwind sspstrong uwtable
declare zeroext i1 @file_matching(i8*) #0

declare i8* @basename(i8*) #5

declare i32 @fnmatch(i8*, i8*, i32) #5

; Function Attrs: nounwind sspstrong uwtable
declare i32 @grep_tree(i8**) #0

declare %struct.FTS.25.54.112.141.199.228.257.518* @fts_open(i8**, i32, i32 (%struct._ftsent.24.53.111.140.198.227.256.517**, %struct._ftsent.24.53.111.140.198.227.256.517**)*) #5

declare %struct._ftsent.24.53.111.140.198.227.256.517* @fts_read(%struct.FTS.25.54.112.141.199.228.257.518*) #5

declare i8* @strerror(i32) #5

declare void @warnx(i8*, ...) #5

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare zeroext i1 @dir_matching(i8*) #2

declare i32 @fts_set(%struct.FTS.25.54.112.141.199.228.257.518*, %struct._ftsent.24.53.111.140.198.227.256.517*, i32) #5

; Function Attrs: nounwind sspstrong uwtable
declare i32 @procfile(i8*) #0

declare i32 @fts_close(%struct.FTS.25.54.112.141.199.228.257.518*) #5

; Function Attrs: nounwind readonly
declare i32 @strcmp(i8*, i8*) #3

declare i32 @stat(i8*, %struct.stat.23.52.110.139.197.226.255.516*) #5

declare void @warn(i8*, ...) #5

; Function Attrs: nounwind sspstrong uwtable
declare i8* @grep_malloc(i64) #0

declare i8* @strcpy(i8*, i8*) #5

; Function Attrs: nounwind sspstrong uwtable
declare i32 @procline(%struct.str.16.45.103.132.190.219.248.509*, i32) #0

declare i32 @regexec(%struct.re_pattern_buffer.6.35.93.122.180.209.238.499*, i8*, i64, %struct.regmatch_t.26.55.113.142.200.229.258.519*, i32) #5

declare i32 @sscanf(i8*, i8*, ...) #5

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare i32 @__istype(i32, i64) #2

; Function Attrs: nounwind sspstrong uwtable
declare void @printline(%struct.str.16.45.103.132.190.219.248.509*, i32, %struct.regmatch_t.26.55.113.142.200.229.258.519*, i32) #0

declare i32 @fputs(i8*, %struct.__sFILE.11.40.98.127.185.214.243.504*) #5

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare i32 @__sputc(i32, %struct.__sFILE.11.40.98.127.185.214.243.504*) #2

declare i32 @putc(i32, %struct.__sFILE.11.40.98.127.185.214.243.504*) #5

declare i64 @fwrite(i8*, i64, i64, %struct.__sFILE.11.40.98.127.185.214.243.504*) #5

declare i32 @__swbuf(i32, %struct.__sFILE.11.40.98.127.185.214.243.504*) #5

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare i32 @__maskrune(i32, i64) #2

; Function Attrs: nounwind readonly
declare i64 @___runetype(i32) #3

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare %struct._RuneLocale.14.43.101.130.188.217.246.507* @__getCurrentRuneLocale.107() #2

declare noalias i8* @malloc(i64) #5

; Function Attrs: nounwind sspstrong uwtable
declare i8* @grep_calloc(i64, i64) #0

declare noalias i8* @calloc(i64, i64) #5

; Function Attrs: nounwind sspstrong uwtable
declare i8* @grep_realloc(i8*, i64) #0

declare i8* @realloc(i8*, i64) #5

; Function Attrs: nounwind sspstrong uwtable
declare i8* @grep_strdup(i8*) #0

declare noalias i8* @strdup(i8*) #5

; Function Attrs: nounwind sspstrong uwtable
declare i32 @tre_fixncomp(%struct.fastmatch_t.5.34.92.121.179.208.237.498*, i8*, i64, i32) #0

; Function Attrs: nounwind sspstrong uwtable
declare i32 @tre_fastncomp(%struct.fastmatch_t.5.34.92.121.179.208.237.498*, i8*, i64, i32) #0

; Function Attrs: nounwind sspstrong uwtable
declare i32 @tre_fixcomp(%struct.fastmatch_t.5.34.92.121.179.208.237.498*, i8*, i32) #0

; Function Attrs: nounwind sspstrong uwtable
declare i32 @tre_fastcomp(%struct.fastmatch_t.5.34.92.121.179.208.237.498*, i8*, i32) #0

; Function Attrs: nounwind sspstrong uwtable
declare i32 @tre_fixwncomp(%struct.fastmatch_t.5.34.92.121.179.208.237.498*, i32*, i64, i32) #0

; Function Attrs: nounwind sspstrong uwtable
declare i32 @tre_fastwncomp(%struct.fastmatch_t.5.34.92.121.179.208.237.498*, i32*, i64, i32) #0

; Function Attrs: nounwind sspstrong uwtable
declare i32 @tre_fixwcomp(%struct.fastmatch_t.5.34.92.121.179.208.237.498*, i32*, i32) #0

; Function Attrs: nounwind readonly
declare i64 @wcslen(i32*) #3

; Function Attrs: nounwind sspstrong uwtable
declare i32 @tre_fastwcomp(%struct.fastmatch_t.5.34.92.121.179.208.237.498*, i32*, i32) #0

; Function Attrs: nounwind sspstrong uwtable
declare void @tre_fastfree(%struct.fastmatch_t.5.34.92.121.179.208.237.498*) #0

; Function Attrs: nounwind sspstrong uwtable
declare i32 @tre_fastnexec(%struct.fastmatch_t.5.34.92.121.179.208.237.498*, i8*, i64, i64, %struct.regmatch_t.26.55.113.142.200.229.258.519*, i32) #0

declare i32 @___mb_cur_max() #5

; Function Attrs: nounwind sspstrong uwtable
declare i32 @tre_fastexec(%struct.fastmatch_t.5.34.92.121.179.208.237.498*, i8*, i64, %struct.regmatch_t.26.55.113.142.200.229.258.519*, i32) #0

; Function Attrs: nounwind sspstrong uwtable
declare i32 @tre_fastwnexec(%struct.fastmatch_t.5.34.92.121.179.208.237.498*, i32*, i64, i64, %struct.regmatch_t.26.55.113.142.200.229.258.519*, i32) #0

; Function Attrs: nounwind sspstrong uwtable
declare i32 @tre_fastwexec(%struct.fastmatch_t.5.34.92.121.179.208.237.498*, i32*, i64, %struct.regmatch_t.26.55.113.142.200.229.258.519*, i32) #0

; Function Attrs: nounwind sspstrong uwtable
declare %struct.hashtable.28.57.115.144.202.231.260.521* @hashtable_init(i64, i64, i64) #0

; Function Attrs: nounwind sspstrong uwtable
declare i32 @hashtable_put(%struct.hashtable.28.57.115.144.202.231.260.521*, i8*, i8*) #0

; Function Attrs: nounwind sspstrong uwtable
declare i32 @hash32_buf(i8*, i64, i32) #0

; Function Attrs: nounwind readonly
declare i32 @memcmp(i8*, i8*, i64) #3

; Function Attrs: nounwind sspstrong uwtable
declare i32 @hashtable_get(%struct.hashtable.28.57.115.144.202.231.260.521*, i8*, i8*) #0

; Function Attrs: nounwind sspstrong uwtable
declare %struct.hashtable_entry.27.56.114.143.201.230.259.520** @hashtable_lookup(%struct.hashtable.28.57.115.144.202.231.260.521*, i8*) #0

; Function Attrs: nounwind sspstrong uwtable
declare i32 @hashtable_remove(%struct.hashtable.28.57.115.144.202.231.260.521*, i8*) #0

; Function Attrs: nounwind sspstrong uwtable
declare void @hashtable_free(%struct.hashtable.28.57.115.144.202.231.260.521*) #0

; Function Attrs: nounwind sspstrong uwtable
declare i32 @tre_convert_pattern(i8*, i64, i32**, i64*) #0

declare i64 @mbrtowc(i32*, i8*, i64, %union.__mbstate_t.10.39.97.126.184.213.242.503*) #5

; Function Attrs: nounwind sspstrong uwtable
declare void @tre_free_pattern(i32*) #0

; Function Attrs: nounwind sspstrong uwtable
declare i32 @tre_compile_literal(%struct.fastmatch_t.5.34.92.121.179.208.237.498*, i32*, i64, i32) #0

declare i64 @wcstombs(i8*, i32*, i64) #5

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare i32 @__sbistype.128(i32, i64) #2

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare i32 @__sbtoupper(i32) #2

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare i32 @__sbtolower(i32) #2

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare i32 @__istype.129(i32, i64) #2

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare i32 @__toupper(i32) #2

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare i32 @__tolower(i32) #2

; Function Attrs: nounwind readonly
declare i32 @___tolower(i32) #3

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare %struct._RuneLocale.14.43.101.130.188.217.246.507* @__getCurrentRuneLocale.130() #2

; Function Attrs: nounwind readonly
declare i32 @___toupper(i32) #3

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare i32 @__maskrune.131(i32, i64) #2

; Function Attrs: inlinehint nounwind sspstrong uwtable
declare i32 @__sbmaskrune.132(i32, i64) #2

; Function Attrs: nounwind sspstrong uwtable
declare i32 @tre_compile_fast(%struct.fastmatch_t.5.34.92.121.179.208.237.498*, i32*, i64, i32) #0

; Function Attrs: nounwind sspstrong uwtable
declare void @tre_free_fast(%struct.fastmatch_t.5.34.92.121.179.208.237.498*) #0

; Function Attrs: nounwind sspstrong uwtable
declare i32 @tre_match_fast(%struct.fastmatch_t.5.34.92.121.179.208.237.498*, i8*, i64, i32, i32, %struct.regmatch_t.26.55.113.142.200.229.258.519*, i32) #0

; Function Attrs: inlinehint nounwind sspstrong uwtable
define internal i32 @fastcmp(%struct.fastmatch_t.5.34.92.121.179.208.237.498* %fg, i8* %data, i32 %type) #2 !dbg !382 {
entry:
  call void @llvm.trap()
  unreachable
}

; Function Attrs: nounwind sspstrong uwtable
declare void @xmalloc_configure(i32) #0

; Function Attrs: nounwind sspstrong uwtable
declare void @xmalloc_init() #0

; Function Attrs: nounwind sspstrong uwtable
declare %struct.hashTable.20.49.107.136.194.223.252.513* @hash_table_new() #0

; Function Attrs: noreturn
declare void @__assert(i8*, i8*, i32, i8*) #7

; Function Attrs: nounwind sspstrong uwtable
declare i32 @xmalloc_dump_leaks() #0

; Function Attrs: nounwind sspstrong uwtable
declare i8* @xmalloc_impl(i64, i8*, i32, i8*) #0

; Function Attrs: nounwind sspstrong uwtable
declare void @hash_table_add(%struct.hashTable.20.49.107.136.194.223.252.513*, i8*, i32, i8*, i32, i8*) #0

; Function Attrs: nounwind sspstrong uwtable
declare i32 @hash_void_ptr(i8*) #0

; Function Attrs: nounwind sspstrong uwtable
declare i8* @xcalloc_impl(i64, i64, i8*, i32, i8*) #0

; Function Attrs: nounwind sspstrong uwtable
declare void @xfree_impl(i8*, i8*, i32, i8*) #0

; Function Attrs: nounwind sspstrong uwtable
declare void @hash_table_del(%struct.hashTable.20.49.107.136.194.223.252.513*, i8*) #0

; Function Attrs: noreturn
declare void @abort() #7

; Function Attrs: nounwind sspstrong uwtable
declare i8* @xrealloc_impl(i8*, i64, i8*, i32, i8*) #0

; Function Attrs: noreturn nounwind
declare void @llvm.trap() #8

attributes #0 = { nounwind sspstrong uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }
attributes #2 = { inlinehint nounwind sspstrong uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind readonly "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { argmemonly nounwind }
attributes #5 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { nounwind "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #7 = { noreturn "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #8 = { noreturn nounwind }

!llvm.dbg.cu = !{!0, !117, !278, !304, !329, !340, !342, !347, !351}
!llvm.ident = !{!379, !379, !379, !379, !379, !379, !379, !379, !379}
!llvm.module.flags = !{!380, !381}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 4.0.0 (http://llvm.org/git/clang.git 3f7a1d0a6dedbbff7cdd804e31063c9c33a837ec) (http://llvm.org/git/llvm.git 20014804ba63d479300bb5c136856d11fbc1bdae)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, retainedTypes: !28, globals: !37)
!1 = !DIFile(filename: "/usr/home/jon/freebsd/loom/usr.bin/grep/file.c", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!2 = !{!3, !18, !25}
!3 = !DICompositeType(tag: DW_TAG_enumeration_type, file: !4, line: 57, size: 32, align: 32, elements: !5)
!4 = !DIFile(filename: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/tmp/usr/include/lzma/base.h", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!5 = !{!6, !7, !8, !9, !10, !11, !12, !13, !14, !15, !16, !17}
!6 = !DIEnumerator(name: "LZMA_OK", value: 0)
!7 = !DIEnumerator(name: "LZMA_STREAM_END", value: 1)
!8 = !DIEnumerator(name: "LZMA_NO_CHECK", value: 2)
!9 = !DIEnumerator(name: "LZMA_UNSUPPORTED_CHECK", value: 3)
!10 = !DIEnumerator(name: "LZMA_GET_CHECK", value: 4)
!11 = !DIEnumerator(name: "LZMA_MEM_ERROR", value: 5)
!12 = !DIEnumerator(name: "LZMA_MEMLIMIT_ERROR", value: 6)
!13 = !DIEnumerator(name: "LZMA_FORMAT_ERROR", value: 7)
!14 = !DIEnumerator(name: "LZMA_OPTIONS_ERROR", value: 8)
!15 = !DIEnumerator(name: "LZMA_DATA_ERROR", value: 9)
!16 = !DIEnumerator(name: "LZMA_BUF_ERROR", value: 10)
!17 = !DIEnumerator(name: "LZMA_PROG_ERROR", value: 11)
!18 = !DICompositeType(tag: DW_TAG_enumeration_type, file: !4, line: 250, size: 32, align: 32, elements: !19)
!19 = !{!20, !21, !22, !23, !24}
!20 = !DIEnumerator(name: "LZMA_RUN", value: 0)
!21 = !DIEnumerator(name: "LZMA_SYNC_FLUSH", value: 1)
!22 = !DIEnumerator(name: "LZMA_FULL_FLUSH", value: 2)
!23 = !DIEnumerator(name: "LZMA_FULL_BARRIER", value: 4)
!24 = !DIEnumerator(name: "LZMA_FINISH", value: 3)
!25 = !DICompositeType(tag: DW_TAG_enumeration_type, file: !4, line: 44, size: 32, align: 32, elements: !26)
!26 = !{!27}
!27 = !DIEnumerator(name: "LZMA_RESERVED_ENUM", value: 0)
!28 = !{!29, !30}
!29 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64, align: 64)
!30 = !DIDerivedType(tag: DW_TAG_typedef, name: "off_t", file: !31, line: 173, baseType: !32)
!31 = !DIFile(filename: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/tmp/usr/include/sys/types.h", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!32 = !DIDerivedType(tag: DW_TAG_typedef, name: "__off_t", file: !33, line: 53, baseType: !34)
!33 = !DIFile(filename: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/tmp/usr/include/sys/_types.h", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!34 = !DIDerivedType(tag: DW_TAG_typedef, name: "__int64_t", file: !35, line: 60, baseType: !36)
!35 = !DIFile(filename: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/tmp/usr/include/x86/_types.h", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!36 = !DIBasicType(name: "long int", size: 64, align: 64, encoding: DW_ATE_signed)
!37 = !{!38, !50, !52, !56, !57, !58, !63, !64, !65, !66, !73}
!38 = distinct !DIGlobalVariable(name: "gzbufdesc", scope: !0, file: !1, line: 65, type: !39, isLocal: true, isDefinition: true, variable: %struct.gzFile_s.3.32.90.119.177.206.235.496** @gzbufdesc)
!39 = !DIDerivedType(tag: DW_TAG_typedef, name: "gzFile", file: !40, line: 1224, baseType: !41)
!40 = !DIFile(filename: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/tmp/usr/include/zlib.h", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!41 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !42, size: 64, align: 64)
!42 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "gzFile_s", file: !40, line: 1670, size: 192, align: 64, elements: !43)
!43 = !{!44, !46, !49}
!44 = !DIDerivedType(tag: DW_TAG_member, name: "have", scope: !42, file: !40, line: 1671, baseType: !45, size: 32, align: 32)
!45 = !DIBasicType(name: "unsigned int", size: 32, align: 32, encoding: DW_ATE_unsigned)
!46 = !DIDerivedType(tag: DW_TAG_member, name: "next", scope: !42, file: !40, line: 1672, baseType: !47, size: 64, align: 64, offset: 64)
!47 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !48, size: 64, align: 64)
!48 = !DIBasicType(name: "unsigned char", size: 8, align: 8, encoding: DW_ATE_unsigned_char)
!49 = !DIDerivedType(tag: DW_TAG_member, name: "pos", scope: !42, file: !40, line: 1673, baseType: !30, size: 64, align: 64, offset: 128)
!50 = distinct !DIGlobalVariable(name: "laction", scope: !0, file: !1, line: 68, type: !51, isLocal: true, isDefinition: true, variable: i32* @laction)
!51 = !DIDerivedType(tag: DW_TAG_typedef, name: "lzma_action", file: !4, line: 345, baseType: !18)
!52 = distinct !DIGlobalVariable(name: "bzbufdesc", scope: !0, file: !1, line: 72, type: !53, isLocal: true, isDefinition: true, variable: i8** @bzbufdesc)
!53 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !54, size: 64, align: 64)
!54 = !DIDerivedType(tag: DW_TAG_typedef, name: "BZFILE", file: !55, line: 137, baseType: null)
!55 = !DIFile(filename: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/tmp/usr/include/bzlib.h", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!56 = distinct !DIGlobalVariable(name: "buffer", scope: !0, file: !1, line: 75, type: !47, isLocal: true, isDefinition: true, variable: i8** @buffer)
!57 = distinct !DIGlobalVariable(name: "bufpos", scope: !0, file: !1, line: 76, type: !47, isLocal: true, isDefinition: true, variable: i8** @bufpos)
!58 = distinct !DIGlobalVariable(name: "bufrem", scope: !0, file: !1, line: 77, type: !59, isLocal: true, isDefinition: true, variable: i64* @bufrem)
!59 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !31, line: 199, baseType: !60)
!60 = !DIDerivedType(tag: DW_TAG_typedef, name: "__size_t", file: !35, line: 106, baseType: !61)
!61 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint64_t", file: !35, line: 61, baseType: !62)
!62 = !DIBasicType(name: "long unsigned int", size: 64, align: 64, encoding: DW_ATE_unsigned)
!63 = distinct !DIGlobalVariable(name: "fsiz", scope: !0, file: !1, line: 78, type: !59, isLocal: true, isDefinition: true, variable: i64* @fsiz)
!64 = distinct !DIGlobalVariable(name: "lnbuf", scope: !0, file: !1, line: 80, type: !47, isLocal: true, isDefinition: true, variable: i8** @lnbuf)
!65 = distinct !DIGlobalVariable(name: "lnbuflen", scope: !0, file: !1, line: 81, type: !59, isLocal: true, isDefinition: true, variable: i64* @lnbuflen)
!66 = distinct !DIGlobalVariable(name: "lin_buf", scope: !0, file: !1, line: 69, type: !67, isLocal: true, isDefinition: true, variable: [32768 x i8]* @lin_buf)
!67 = !DICompositeType(tag: DW_TAG_array_type, baseType: !68, size: 262144, align: 8, elements: !71)
!68 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint8_t", file: !69, line: 54, baseType: !70)
!69 = !DIFile(filename: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/tmp/usr/include/sys/_stdint.h", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!70 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint8_t", file: !35, line: 54, baseType: !48)
!71 = !{!72}
!72 = !DISubrange(count: 32768)
!73 = distinct !DIGlobalVariable(name: "lstrm", scope: !0, file: !1, line: 67, type: !74, isLocal: true, isDefinition: true, variable: %struct.lzma_stream.2.31.89.118.176.205.234.495* @lstrm)
!74 = !DIDerivedType(tag: DW_TAG_typedef, name: "lzma_stream", file: !4, line: 524, baseType: !75)
!75 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !4, line: 485, size: 1088, align: 64, elements: !76)
!76 = !{!77, !80, !81, !83, !85, !86, !87, !102, !106, !107, !108, !109, !110, !111, !112, !113, !114, !116}
!77 = !DIDerivedType(tag: DW_TAG_member, name: "next_in", scope: !75, file: !4, line: 486, baseType: !78, size: 64, align: 64)
!78 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !79, size: 64, align: 64)
!79 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !68)
!80 = !DIDerivedType(tag: DW_TAG_member, name: "avail_in", scope: !75, file: !4, line: 487, baseType: !59, size: 64, align: 64, offset: 64)
!81 = !DIDerivedType(tag: DW_TAG_member, name: "total_in", scope: !75, file: !4, line: 488, baseType: !82, size: 64, align: 64, offset: 128)
!82 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint64_t", file: !69, line: 69, baseType: !61)
!83 = !DIDerivedType(tag: DW_TAG_member, name: "next_out", scope: !75, file: !4, line: 490, baseType: !84, size: 64, align: 64, offset: 192)
!84 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !68, size: 64, align: 64)
!85 = !DIDerivedType(tag: DW_TAG_member, name: "avail_out", scope: !75, file: !4, line: 491, baseType: !59, size: 64, align: 64, offset: 256)
!86 = !DIDerivedType(tag: DW_TAG_member, name: "total_out", scope: !75, file: !4, line: 492, baseType: !82, size: 64, align: 64, offset: 320)
!87 = !DIDerivedType(tag: DW_TAG_member, name: "allocator", scope: !75, file: !4, line: 502, baseType: !88, size: 64, align: 64, offset: 384)
!88 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !89, size: 64, align: 64)
!89 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !90)
!90 = !DIDerivedType(tag: DW_TAG_typedef, name: "lzma_allocator", file: !4, line: 434, baseType: !91)
!91 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !4, line: 372, size: 192, align: 64, elements: !92)
!92 = !{!93, !97, !101}
!93 = !DIDerivedType(tag: DW_TAG_member, name: "alloc", scope: !91, file: !4, line: 407, baseType: !94, size: 64, align: 64)
!94 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !95, size: 64, align: 64)
!95 = !DISubroutineType(types: !96)
!96 = !{!29, !29, !59, !59}
!97 = !DIDerivedType(tag: DW_TAG_member, name: "free", scope: !91, file: !4, line: 421, baseType: !98, size: 64, align: 64, offset: 64)
!98 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !99, size: 64, align: 64)
!99 = !DISubroutineType(types: !100)
!100 = !{null, !29, !29}
!101 = !DIDerivedType(tag: DW_TAG_member, name: "opaque", scope: !91, file: !4, line: 432, baseType: !29, size: 64, align: 64, offset: 128)
!102 = !DIDerivedType(tag: DW_TAG_member, name: "internal", scope: !75, file: !4, line: 505, baseType: !103, size: 64, align: 64, offset: 448)
!103 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !104, size: 64, align: 64)
!104 = !DIDerivedType(tag: DW_TAG_typedef, name: "lzma_internal", file: !4, line: 442, baseType: !105)
!105 = !DICompositeType(tag: DW_TAG_structure_type, name: "lzma_internal_s", file: !4, line: 442, flags: DIFlagFwdDecl)
!106 = !DIDerivedType(tag: DW_TAG_member, name: "reserved_ptr1", scope: !75, file: !4, line: 513, baseType: !29, size: 64, align: 64, offset: 512)
!107 = !DIDerivedType(tag: DW_TAG_member, name: "reserved_ptr2", scope: !75, file: !4, line: 514, baseType: !29, size: 64, align: 64, offset: 576)
!108 = !DIDerivedType(tag: DW_TAG_member, name: "reserved_ptr3", scope: !75, file: !4, line: 515, baseType: !29, size: 64, align: 64, offset: 640)
!109 = !DIDerivedType(tag: DW_TAG_member, name: "reserved_ptr4", scope: !75, file: !4, line: 516, baseType: !29, size: 64, align: 64, offset: 704)
!110 = !DIDerivedType(tag: DW_TAG_member, name: "reserved_int1", scope: !75, file: !4, line: 517, baseType: !82, size: 64, align: 64, offset: 768)
!111 = !DIDerivedType(tag: DW_TAG_member, name: "reserved_int2", scope: !75, file: !4, line: 518, baseType: !82, size: 64, align: 64, offset: 832)
!112 = !DIDerivedType(tag: DW_TAG_member, name: "reserved_int3", scope: !75, file: !4, line: 519, baseType: !59, size: 64, align: 64, offset: 896)
!113 = !DIDerivedType(tag: DW_TAG_member, name: "reserved_int4", scope: !75, file: !4, line: 520, baseType: !59, size: 64, align: 64, offset: 960)
!114 = !DIDerivedType(tag: DW_TAG_member, name: "reserved_enum1", scope: !75, file: !4, line: 521, baseType: !115, size: 32, align: 32, offset: 1024)
!115 = !DIDerivedType(tag: DW_TAG_typedef, name: "lzma_reserved_enum", file: !4, line: 46, baseType: !25)
!116 = !DIDerivedType(tag: DW_TAG_member, name: "reserved_enum2", scope: !75, file: !4, line: 522, baseType: !115, size: 32, align: 32, offset: 1056)
!117 = distinct !DICompileUnit(language: DW_LANG_C99, file: !118, producer: "clang version 4.0.0 (http://llvm.org/git/clang.git 3f7a1d0a6dedbbff7cdd804e31063c9c33a837ec) (http://llvm.org/git/llvm.git 20014804ba63d479300bb5c136856d11fbc1bdae)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !119, retainedTypes: !120, globals: !124)
!118 = !DIFile(filename: "/usr/home/jon/freebsd/loom/usr.bin/grep/grep.c", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!119 = !{}
!120 = !{!29, !121, !59}
!121 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !122, size: 64, align: 64)
!122 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !123, size: 64, align: 64)
!123 = !DIBasicType(name: "char", size: 8, align: 8, encoding: DW_ATE_signed_char)
!124 = !{!125, !131, !133, !134, !135, !136, !137, !138, !139, !140, !142, !150, !151, !152, !159, !180, !216, !217, !218, !224, !225, !229, !231, !232, !233, !234, !235, !236, !237, !238, !239, !240, !242, !243, !244, !245, !246, !247, !248, !249, !250, !251, !252, !253, !254, !255, !256, !257, !258, !259, !260, !261, !262, !275, !276, !277}
!125 = distinct !DIGlobalVariable(name: "errstr", scope: !117, file: !118, line: 65, type: !126, isLocal: false, isDefinition: true, variable: [10 x i8*]* @errstr)
!126 = !DICompositeType(tag: DW_TAG_array_type, baseType: !127, size: 640, align: 64, elements: !129)
!127 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !128, size: 64, align: 64)
!128 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !123)
!129 = !{!130}
!130 = !DISubrange(count: 10)
!131 = distinct !DIGlobalVariable(name: "cflags", scope: !117, file: !118, line: 79, type: !132, isLocal: false, isDefinition: true, variable: i32* @cflags)
!132 = !DIBasicType(name: "int", size: 32, align: 32, encoding: DW_ATE_signed)
!133 = distinct !DIGlobalVariable(name: "eflags", scope: !117, file: !118, line: 80, type: !132, isLocal: false, isDefinition: true, variable: i32* @eflags)
!134 = distinct !DIGlobalVariable(name: "grepbehave", scope: !117, file: !118, line: 124, type: !132, isLocal: false, isDefinition: true, variable: i32* @grepbehave)
!135 = distinct !DIGlobalVariable(name: "binbehave", scope: !117, file: !118, line: 125, type: !132, isLocal: false, isDefinition: true, variable: i32* @binbehave)
!136 = distinct !DIGlobalVariable(name: "filebehave", scope: !117, file: !118, line: 126, type: !132, isLocal: false, isDefinition: true, variable: i32* @filebehave)
!137 = distinct !DIGlobalVariable(name: "devbehave", scope: !117, file: !118, line: 127, type: !132, isLocal: false, isDefinition: true, variable: i32* @devbehave)
!138 = distinct !DIGlobalVariable(name: "dirbehave", scope: !117, file: !118, line: 128, type: !132, isLocal: false, isDefinition: true, variable: i32* @dirbehave)
!139 = distinct !DIGlobalVariable(name: "linkbehave", scope: !117, file: !118, line: 129, type: !132, isLocal: false, isDefinition: true, variable: i32* @linkbehave)
!140 = distinct !DIGlobalVariable(name: "first", scope: !117, file: !118, line: 151, type: !141, isLocal: false, isDefinition: true, variable: i8* @first)
!141 = !DIBasicType(name: "_Bool", size: 8, align: 8, encoding: DW_ATE_boolean)
!142 = distinct !DIGlobalVariable(name: "catalog", scope: !117, file: !118, line: 58, type: !143, isLocal: false, isDefinition: true, variable: %struct.__nl_cat_d.4.33.91.120.178.207.236.497** @catalog)
!143 = !DIDerivedType(tag: DW_TAG_typedef, name: "nl_catd", file: !144, line: 84, baseType: !145)
!144 = !DIFile(filename: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/tmp/usr/include/nl_types.h", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!145 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !146, size: 64, align: 64)
!146 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "__nl_cat_d", file: !144, line: 81, size: 128, align: 64, elements: !147)
!147 = !{!148, !149}
!148 = !DIDerivedType(tag: DW_TAG_member, name: "__data", scope: !146, file: !144, line: 82, baseType: !29, size: 64, align: 64)
!149 = !DIDerivedType(tag: DW_TAG_member, name: "__size", scope: !146, file: !144, line: 83, baseType: !132, size: 32, align: 32, offset: 64)
!150 = distinct !DIGlobalVariable(name: "matchall", scope: !117, file: !118, line: 83, type: !141, isLocal: false, isDefinition: true, variable: i8* @matchall)
!151 = distinct !DIGlobalVariable(name: "patterns", scope: !117, file: !118, line: 86, type: !45, isLocal: false, isDefinition: true, variable: i32* @patterns)
!152 = distinct !DIGlobalVariable(name: "pattern", scope: !117, file: !118, line: 88, type: !153, isLocal: false, isDefinition: true, variable: %struct.__nl_cat_d.4.33.91.120.178.207.236.497** @pattern)
!153 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !154, size: 64, align: 64)
!154 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "pat", file: !155, line: 98, size: 128, align: 64, elements: !156)
!155 = !DIFile(filename: "/usr/home/jon/freebsd/loom/usr.bin/grep/grep.h", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!156 = !{!157, !158}
!157 = !DIDerivedType(tag: DW_TAG_member, name: "pat", scope: !154, file: !155, line: 99, baseType: !122, size: 64, align: 64)
!158 = !DIDerivedType(tag: DW_TAG_member, name: "len", scope: !154, file: !155, line: 100, baseType: !132, size: 32, align: 32, offset: 64)
!159 = distinct !DIGlobalVariable(name: "r_pattern", scope: !117, file: !118, line: 89, type: !160, isLocal: false, isDefinition: true, variable: %struct.re_pattern_buffer.6.35.93.122.180.209.238.499** @r_pattern)
!160 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !161, size: 64, align: 64)
!161 = !DIDerivedType(tag: DW_TAG_typedef, name: "regex_t", file: !162, line: 424, baseType: !163)
!162 = !DIFile(filename: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/tmp/usr/include/gnu/posix/regex.h", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!163 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "re_pattern_buffer", file: !162, line: 358, size: 512, align: 64, elements: !164)
!164 = !{!165, !166, !167, !168, !170, !171, !172, !173, !174, !175, !176, !177, !178, !179}
!165 = !DIDerivedType(tag: DW_TAG_member, name: "buffer", scope: !163, file: !162, line: 363, baseType: !47, size: 64, align: 64)
!166 = !DIDerivedType(tag: DW_TAG_member, name: "allocated", scope: !163, file: !162, line: 366, baseType: !62, size: 64, align: 64, offset: 64)
!167 = !DIDerivedType(tag: DW_TAG_member, name: "used", scope: !163, file: !162, line: 369, baseType: !62, size: 64, align: 64, offset: 128)
!168 = !DIDerivedType(tag: DW_TAG_member, name: "syntax", scope: !163, file: !162, line: 372, baseType: !169, size: 64, align: 64, offset: 192)
!169 = !DIDerivedType(tag: DW_TAG_typedef, name: "reg_syntax_t", file: !162, line: 43, baseType: !62)
!170 = !DIDerivedType(tag: DW_TAG_member, name: "fastmap", scope: !163, file: !162, line: 377, baseType: !122, size: 64, align: 64, offset: 256)
!171 = !DIDerivedType(tag: DW_TAG_member, name: "translate", scope: !163, file: !162, line: 383, baseType: !47, size: 64, align: 64, offset: 320)
!172 = !DIDerivedType(tag: DW_TAG_member, name: "re_nsub", scope: !163, file: !162, line: 386, baseType: !59, size: 64, align: 64, offset: 384)
!173 = !DIDerivedType(tag: DW_TAG_member, name: "can_be_null", scope: !163, file: !162, line: 392, baseType: !45, size: 1, align: 32, offset: 448, flags: DIFlagBitField, extraData: i64 448)
!174 = !DIDerivedType(tag: DW_TAG_member, name: "regs_allocated", scope: !163, file: !162, line: 403, baseType: !45, size: 2, align: 32, offset: 449, flags: DIFlagBitField, extraData: i64 448)
!175 = !DIDerivedType(tag: DW_TAG_member, name: "fastmap_accurate", scope: !163, file: !162, line: 407, baseType: !45, size: 1, align: 32, offset: 451, flags: DIFlagBitField, extraData: i64 448)
!176 = !DIDerivedType(tag: DW_TAG_member, name: "no_sub", scope: !163, file: !162, line: 411, baseType: !45, size: 1, align: 32, offset: 452, flags: DIFlagBitField, extraData: i64 448)
!177 = !DIDerivedType(tag: DW_TAG_member, name: "not_bol", scope: !163, file: !162, line: 415, baseType: !45, size: 1, align: 32, offset: 453, flags: DIFlagBitField, extraData: i64 448)
!178 = !DIDerivedType(tag: DW_TAG_member, name: "not_eol", scope: !163, file: !162, line: 418, baseType: !45, size: 1, align: 32, offset: 454, flags: DIFlagBitField, extraData: i64 448)
!179 = !DIDerivedType(tag: DW_TAG_member, name: "newline_anchor", scope: !163, file: !162, line: 421, baseType: !45, size: 1, align: 32, offset: 455, flags: DIFlagBitField, extraData: i64 448)
!180 = distinct !DIGlobalVariable(name: "fg_pattern", scope: !117, file: !118, line: 90, type: !181, isLocal: false, isDefinition: true, variable: %struct.fastmatch_t.5.34.92.121.179.208.237.498** @fg_pattern)
!181 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !182, size: 64, align: 64)
!182 = !DIDerivedType(tag: DW_TAG_typedef, name: "fastmatch_t", file: !183, line: 35, baseType: !184)
!183 = !DIFile(filename: "/usr/home/jon/freebsd/loom/usr.bin/grep/regex/fastmatch.h", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!184 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !183, line: 11, size: 9024, align: 64, elements: !185)
!185 = !{!186, !187, !188, !193, !195, !199, !201, !202, !203, !204, !205, !206, !207, !208, !209, !210, !211, !212, !213, !214, !215}
!186 = !DIDerivedType(tag: DW_TAG_member, name: "wlen", scope: !184, file: !183, line: 12, baseType: !59, size: 64, align: 64)
!187 = !DIDerivedType(tag: DW_TAG_member, name: "len", scope: !184, file: !183, line: 13, baseType: !59, size: 64, align: 64, offset: 64)
!188 = !DIDerivedType(tag: DW_TAG_member, name: "wpattern", scope: !184, file: !183, line: 14, baseType: !189, size: 64, align: 64, offset: 128)
!189 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !190, size: 64, align: 64)
!190 = !DIDerivedType(tag: DW_TAG_typedef, name: "wchar_t", file: !191, line: 54, baseType: !192)
!191 = !DIFile(filename: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/tmp/usr/include/stdlib.h", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!192 = !DIDerivedType(tag: DW_TAG_typedef, name: "___wchar_t", file: !35, line: 147, baseType: !132)
!193 = !DIDerivedType(tag: DW_TAG_member, name: "wescmap", scope: !184, file: !183, line: 15, baseType: !194, size: 64, align: 64, offset: 192)
!194 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !141, size: 64, align: 64)
!195 = !DIDerivedType(tag: DW_TAG_member, name: "qsBc", scope: !184, file: !183, line: 16, baseType: !196, size: 8192, align: 32, offset: 256)
!196 = !DICompositeType(tag: DW_TAG_array_type, baseType: !45, size: 8192, align: 32, elements: !197)
!197 = !{!198}
!198 = !DISubrange(count: 256)
!199 = !DIDerivedType(tag: DW_TAG_member, name: "bmGs", scope: !184, file: !183, line: 17, baseType: !200, size: 64, align: 64, offset: 8448)
!200 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !45, size: 64, align: 64)
!201 = !DIDerivedType(tag: DW_TAG_member, name: "pattern", scope: !184, file: !183, line: 18, baseType: !122, size: 64, align: 64, offset: 8512)
!202 = !DIDerivedType(tag: DW_TAG_member, name: "escmap", scope: !184, file: !183, line: 19, baseType: !194, size: 64, align: 64, offset: 8576)
!203 = !DIDerivedType(tag: DW_TAG_member, name: "defBc", scope: !184, file: !183, line: 20, baseType: !45, size: 32, align: 32, offset: 8640)
!204 = !DIDerivedType(tag: DW_TAG_member, name: "qsBc_table", scope: !184, file: !183, line: 21, baseType: !29, size: 64, align: 64, offset: 8704)
!205 = !DIDerivedType(tag: DW_TAG_member, name: "sbmGs", scope: !184, file: !183, line: 22, baseType: !200, size: 64, align: 64, offset: 8768)
!206 = !DIDerivedType(tag: DW_TAG_member, name: "re_endp", scope: !184, file: !183, line: 23, baseType: !127, size: 64, align: 64, offset: 8832)
!207 = !DIDerivedType(tag: DW_TAG_member, name: "hasdot", scope: !184, file: !183, line: 26, baseType: !141, size: 8, align: 8, offset: 8896)
!208 = !DIDerivedType(tag: DW_TAG_member, name: "bol", scope: !184, file: !183, line: 27, baseType: !141, size: 8, align: 8, offset: 8904)
!209 = !DIDerivedType(tag: DW_TAG_member, name: "eol", scope: !184, file: !183, line: 28, baseType: !141, size: 8, align: 8, offset: 8912)
!210 = !DIDerivedType(tag: DW_TAG_member, name: "word", scope: !184, file: !183, line: 29, baseType: !141, size: 8, align: 8, offset: 8920)
!211 = !DIDerivedType(tag: DW_TAG_member, name: "icase", scope: !184, file: !183, line: 30, baseType: !141, size: 8, align: 8, offset: 8928)
!212 = !DIDerivedType(tag: DW_TAG_member, name: "newline", scope: !184, file: !183, line: 31, baseType: !141, size: 8, align: 8, offset: 8936)
!213 = !DIDerivedType(tag: DW_TAG_member, name: "nosub", scope: !184, file: !183, line: 32, baseType: !141, size: 8, align: 8, offset: 8944)
!214 = !DIDerivedType(tag: DW_TAG_member, name: "matchall", scope: !184, file: !183, line: 33, baseType: !141, size: 8, align: 8, offset: 8952)
!215 = !DIDerivedType(tag: DW_TAG_member, name: "reversed", scope: !184, file: !183, line: 34, baseType: !141, size: 8, align: 8, offset: 8960)
!216 = distinct !DIGlobalVariable(name: "fpatterns", scope: !117, file: !118, line: 93, type: !45, isLocal: false, isDefinition: true, variable: i32* @fpatterns)
!217 = distinct !DIGlobalVariable(name: "dpatterns", scope: !117, file: !118, line: 93, type: !45, isLocal: false, isDefinition: true, variable: i32* @dpatterns)
!218 = distinct !DIGlobalVariable(name: "dpattern", scope: !117, file: !118, line: 95, type: !219, isLocal: false, isDefinition: true, variable: %struct.__nl_cat_d.4.33.91.120.178.207.236.497** @dpattern)
!219 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !220, size: 64, align: 64)
!220 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "epat", file: !155, line: 103, size: 128, align: 64, elements: !221)
!221 = !{!222, !223}
!222 = !DIDerivedType(tag: DW_TAG_member, name: "pat", scope: !220, file: !155, line: 104, baseType: !122, size: 64, align: 64)
!223 = !DIDerivedType(tag: DW_TAG_member, name: "mode", scope: !220, file: !155, line: 105, baseType: !132, size: 32, align: 32, offset: 64)
!224 = distinct !DIGlobalVariable(name: "fpattern", scope: !117, file: !118, line: 95, type: !219, isLocal: false, isDefinition: true, variable: %struct.__nl_cat_d.4.33.91.120.178.207.236.497** @fpattern)
!225 = distinct !DIGlobalVariable(name: "re_error", scope: !117, file: !118, line: 98, type: !226, isLocal: false, isDefinition: true, variable: [513 x i8]* @re_error)
!226 = !DICompositeType(tag: DW_TAG_array_type, baseType: !123, size: 4104, align: 8, elements: !227)
!227 = !{!228}
!228 = !DISubrange(count: 513)
!229 = distinct !DIGlobalVariable(name: "Aflag", scope: !117, file: !118, line: 101, type: !230, isLocal: false, isDefinition: true, variable: i64* @Aflag)
!230 = !DIBasicType(name: "long long unsigned int", size: 64, align: 64, encoding: DW_ATE_unsigned)
!231 = distinct !DIGlobalVariable(name: "Bflag", scope: !117, file: !118, line: 102, type: !230, isLocal: false, isDefinition: true, variable: i64* @Bflag)
!232 = distinct !DIGlobalVariable(name: "Hflag", scope: !117, file: !118, line: 103, type: !141, isLocal: false, isDefinition: true, variable: i8* @Hflag)
!233 = distinct !DIGlobalVariable(name: "Lflag", scope: !117, file: !118, line: 104, type: !141, isLocal: false, isDefinition: true, variable: i8* @Lflag)
!234 = distinct !DIGlobalVariable(name: "bflag", scope: !117, file: !118, line: 105, type: !141, isLocal: false, isDefinition: true, variable: i8* @bflag)
!235 = distinct !DIGlobalVariable(name: "cflag", scope: !117, file: !118, line: 106, type: !141, isLocal: false, isDefinition: true, variable: i8* @cflag)
!236 = distinct !DIGlobalVariable(name: "hflag", scope: !117, file: !118, line: 107, type: !141, isLocal: false, isDefinition: true, variable: i8* @hflag)
!237 = distinct !DIGlobalVariable(name: "iflag", scope: !117, file: !118, line: 108, type: !141, isLocal: false, isDefinition: true, variable: i8* @iflag)
!238 = distinct !DIGlobalVariable(name: "lflag", scope: !117, file: !118, line: 109, type: !141, isLocal: false, isDefinition: true, variable: i8* @lflag)
!239 = distinct !DIGlobalVariable(name: "mflag", scope: !117, file: !118, line: 110, type: !141, isLocal: false, isDefinition: true, variable: i8* @mflag)
!240 = distinct !DIGlobalVariable(name: "mcount", scope: !117, file: !118, line: 111, type: !241, isLocal: false, isDefinition: true, variable: i64* @mcount)
!241 = !DIBasicType(name: "long long int", size: 64, align: 64, encoding: DW_ATE_signed)
!242 = distinct !DIGlobalVariable(name: "mlimit", scope: !117, file: !118, line: 112, type: !241, isLocal: false, isDefinition: true, variable: i64* @mlimit)
!243 = distinct !DIGlobalVariable(name: "nflag", scope: !117, file: !118, line: 113, type: !141, isLocal: false, isDefinition: true, variable: i8* @nflag)
!244 = distinct !DIGlobalVariable(name: "oflag", scope: !117, file: !118, line: 114, type: !141, isLocal: false, isDefinition: true, variable: i8* @oflag)
!245 = distinct !DIGlobalVariable(name: "qflag", scope: !117, file: !118, line: 115, type: !141, isLocal: false, isDefinition: true, variable: i8* @qflag)
!246 = distinct !DIGlobalVariable(name: "sflag", scope: !117, file: !118, line: 116, type: !141, isLocal: false, isDefinition: true, variable: i8* @sflag)
!247 = distinct !DIGlobalVariable(name: "vflag", scope: !117, file: !118, line: 117, type: !141, isLocal: false, isDefinition: true, variable: i8* @vflag)
!248 = distinct !DIGlobalVariable(name: "wflag", scope: !117, file: !118, line: 118, type: !141, isLocal: false, isDefinition: true, variable: i8* @wflag)
!249 = distinct !DIGlobalVariable(name: "xflag", scope: !117, file: !118, line: 119, type: !141, isLocal: false, isDefinition: true, variable: i8* @xflag)
!250 = distinct !DIGlobalVariable(name: "lbflag", scope: !117, file: !118, line: 120, type: !141, isLocal: false, isDefinition: true, variable: i8* @lbflag)
!251 = distinct !DIGlobalVariable(name: "nullflag", scope: !117, file: !118, line: 121, type: !141, isLocal: false, isDefinition: true, variable: i8* @nullflag)
!252 = distinct !DIGlobalVariable(name: "label", scope: !117, file: !118, line: 122, type: !122, isLocal: false, isDefinition: true, variable: i8** @label)
!253 = distinct !DIGlobalVariable(name: "color", scope: !117, file: !118, line: 123, type: !127, isLocal: false, isDefinition: true, variable: i8** @color)
!254 = distinct !DIGlobalVariable(name: "dexclude", scope: !117, file: !118, line: 131, type: !141, isLocal: false, isDefinition: true, variable: i8* @dexclude)
!255 = distinct !DIGlobalVariable(name: "dinclude", scope: !117, file: !118, line: 131, type: !141, isLocal: false, isDefinition: true, variable: i8* @dinclude)
!256 = distinct !DIGlobalVariable(name: "fexclude", scope: !117, file: !118, line: 132, type: !141, isLocal: false, isDefinition: true, variable: i8* @fexclude)
!257 = distinct !DIGlobalVariable(name: "finclude", scope: !117, file: !118, line: 132, type: !141, isLocal: false, isDefinition: true, variable: i8* @finclude)
!258 = distinct !DIGlobalVariable(name: "prev", scope: !117, file: !118, line: 152, type: !141, isLocal: false, isDefinition: true, variable: i8* @prev)
!259 = distinct !DIGlobalVariable(name: "tail", scope: !117, file: !118, line: 153, type: !132, isLocal: false, isDefinition: true, variable: i32* @tail)
!260 = distinct !DIGlobalVariable(name: "file_err", scope: !117, file: !118, line: 154, type: !141, isLocal: false, isDefinition: true, variable: i8* @file_err)
!261 = distinct !DIGlobalVariable(name: "optstr", scope: !117, file: !118, line: 169, type: !127, isLocal: true, isDefinition: true, variable: i8** @optstr)
!262 = distinct !DIGlobalVariable(name: "long_options", scope: !117, file: !118, line: 171, type: !263, isLocal: true, isDefinition: true, variable: [48 x %struct.option.7.36.94.123.181.210.239.500]* @long_options)
!263 = !DICompositeType(tag: DW_TAG_array_type, baseType: !264, size: 12288, align: 64, elements: !273)
!264 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !265)
!265 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "option", file: !266, line: 46, size: 256, align: 64, elements: !267)
!266 = !DIFile(filename: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/tmp/usr/include/getopt.h", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!267 = !{!268, !269, !270, !272}
!268 = !DIDerivedType(tag: DW_TAG_member, name: "name", scope: !265, file: !266, line: 48, baseType: !127, size: 64, align: 64)
!269 = !DIDerivedType(tag: DW_TAG_member, name: "has_arg", scope: !265, file: !266, line: 53, baseType: !132, size: 32, align: 32, offset: 64)
!270 = !DIDerivedType(tag: DW_TAG_member, name: "flag", scope: !265, file: !266, line: 55, baseType: !271, size: 64, align: 64, offset: 128)
!271 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !132, size: 64, align: 64)
!272 = !DIDerivedType(tag: DW_TAG_member, name: "val", scope: !265, file: !266, line: 57, baseType: !132, size: 32, align: 32, offset: 192)
!273 = !{!274}
!274 = !DISubrange(count: 48)
!275 = distinct !DIGlobalVariable(name: "pattern_sz", scope: !117, file: !118, line: 87, type: !45, isLocal: true, isDefinition: true, variable: i32* @pattern_sz)
!276 = distinct !DIGlobalVariable(name: "fpattern_sz", scope: !117, file: !118, line: 94, type: !45, isLocal: true, isDefinition: true, variable: i32* @fpattern_sz)
!277 = distinct !DIGlobalVariable(name: "dpattern_sz", scope: !117, file: !118, line: 94, type: !45, isLocal: true, isDefinition: true, variable: i32* @dpattern_sz)
!278 = distinct !DICompileUnit(language: DW_LANG_C99, file: !279, producer: "clang version 4.0.0 (http://llvm.org/git/clang.git 3f7a1d0a6dedbbff7cdd804e31063c9c33a837ec) (http://llvm.org/git/llvm.git 20014804ba63d479300bb5c136856d11fbc1bdae)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !119, retainedTypes: !280, globals: !281)
!279 = !DIFile(filename: "/usr/home/jon/freebsd/loom/usr.bin/grep/queue.c", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!280 = !{!29}
!281 = !{!282, !283}
!282 = distinct !DIGlobalVariable(name: "count", scope: !278, file: !279, line: 52, type: !230, isLocal: true, isDefinition: true, variable: i64* @count)
!283 = distinct !DIGlobalVariable(name: "queue", scope: !278, file: !279, line: 51, type: !284, isLocal: true, isDefinition: true, variable: %struct.anon.0.18.47.105.134.192.221.250.511* @queue)
!284 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !279, line: 51, size: 128, align: 64, elements: !285)
!285 = !{!286, !302}
!286 = !DIDerivedType(tag: DW_TAG_member, name: "stqh_first", scope: !284, file: !279, line: 51, baseType: !287, size: 64, align: 64)
!287 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !288, size: 64, align: 64)
!288 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "qentry", file: !279, line: 46, size: 384, align: 64, elements: !289)
!289 = !{!290, !294}
!290 = !DIDerivedType(tag: DW_TAG_member, name: "list", scope: !288, file: !279, line: 47, baseType: !291, size: 64, align: 64)
!291 = distinct !DICompositeType(tag: DW_TAG_structure_type, scope: !288, file: !279, line: 47, size: 64, align: 64, elements: !292)
!292 = !{!293}
!293 = !DIDerivedType(tag: DW_TAG_member, name: "stqe_next", scope: !291, file: !279, line: 47, baseType: !287, size: 64, align: 64)
!294 = !DIDerivedType(tag: DW_TAG_member, name: "data", scope: !288, file: !279, line: 48, baseType: !295, size: 320, align: 64, offset: 64)
!295 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "str", file: !155, line: 90, size: 320, align: 64, elements: !296)
!296 = !{!297, !298, !299, !300, !301}
!297 = !DIDerivedType(tag: DW_TAG_member, name: "off", scope: !295, file: !155, line: 91, baseType: !30, size: 64, align: 64)
!298 = !DIDerivedType(tag: DW_TAG_member, name: "len", scope: !295, file: !155, line: 92, baseType: !59, size: 64, align: 64, offset: 64)
!299 = !DIDerivedType(tag: DW_TAG_member, name: "dat", scope: !295, file: !155, line: 93, baseType: !122, size: 64, align: 64, offset: 128)
!300 = !DIDerivedType(tag: DW_TAG_member, name: "file", scope: !295, file: !155, line: 94, baseType: !122, size: 64, align: 64, offset: 192)
!301 = !DIDerivedType(tag: DW_TAG_member, name: "line_no", scope: !295, file: !155, line: 95, baseType: !132, size: 32, align: 32, offset: 256)
!302 = !DIDerivedType(tag: DW_TAG_member, name: "stqh_last", scope: !284, file: !279, line: 51, baseType: !303, size: 64, align: 64, offset: 64)
!303 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !287, size: 64, align: 64)
!304 = distinct !DICompileUnit(language: DW_LANG_C99, file: !305, producer: "clang version 4.0.0 (http://llvm.org/git/clang.git 3f7a1d0a6dedbbff7cdd804e31063c9c33a837ec) (http://llvm.org/git/llvm.git 20014804ba63d479300bb5c136856d11fbc1bdae)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !306, retainedTypes: !326, globals: !327)
!305 = !DIFile(filename: "/usr/home/jon/freebsd/loom/usr.bin/grep/util.c", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!306 = !{!307}
!307 = !DICompositeType(tag: DW_TAG_enumeration_type, file: !162, line: 309, size: 32, align: 32, elements: !308)
!308 = !{!309, !310, !311, !312, !313, !314, !315, !316, !317, !318, !319, !320, !321, !322, !323, !324, !325}
!309 = !DIEnumerator(name: "REG_NOERROR", value: 0)
!310 = !DIEnumerator(name: "REG_NOMATCH", value: 1)
!311 = !DIEnumerator(name: "REG_BADPAT", value: 2)
!312 = !DIEnumerator(name: "REG_ECOLLATE", value: 3)
!313 = !DIEnumerator(name: "REG_ECTYPE", value: 4)
!314 = !DIEnumerator(name: "REG_EESCAPE", value: 5)
!315 = !DIEnumerator(name: "REG_ESUBREG", value: 6)
!316 = !DIEnumerator(name: "REG_EBRACK", value: 7)
!317 = !DIEnumerator(name: "REG_EPAREN", value: 8)
!318 = !DIEnumerator(name: "REG_EBRACE", value: 9)
!319 = !DIEnumerator(name: "REG_BADBR", value: 10)
!320 = !DIEnumerator(name: "REG_ERANGE", value: 11)
!321 = !DIEnumerator(name: "REG_ESPACE", value: 12)
!322 = !DIEnumerator(name: "REG_BADRPT", value: 13)
!323 = !DIEnumerator(name: "REG_EEND", value: 14)
!324 = !DIEnumerator(name: "REG_ESIZE", value: 15)
!325 = !DIEnumerator(name: "REG_ERPAREN", value: 16)
!326 = !{!29, !241, !59, !123}
!327 = !{!328}
!328 = distinct !DIGlobalVariable(name: "linesqueued", scope: !304, file: !305, line: 55, type: !132, isLocal: true, isDefinition: true, variable: i32* @linesqueued)
!329 = distinct !DICompileUnit(language: DW_LANG_C99, file: !330, producer: "clang version 4.0.0 (http://llvm.org/git/clang.git 3f7a1d0a6dedbbff7cdd804e31063c9c33a837ec) (http://llvm.org/git/llvm.git 20014804ba63d479300bb5c136856d11fbc1bdae)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !331, retainedTypes: !339)
!330 = !DIFile(filename: "/usr/home/jon/freebsd/loom/usr.bin/grep/regex/fastmatch.c", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!331 = !{!332, !307}
!332 = !DICompositeType(tag: DW_TAG_enumeration_type, file: !333, line: 41, size: 32, align: 32, elements: !334)
!333 = !DIFile(filename: "/usr/home/jon/freebsd/loom/usr.bin/grep/regex/glue.h", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!334 = !{!335, !336, !337, !338}
!335 = !DIEnumerator(name: "STR_WIDE", value: 0)
!336 = !DIEnumerator(name: "STR_BYTE", value: 1)
!337 = !DIEnumerator(name: "STR_MBS", value: 2)
!338 = !DIEnumerator(name: "STR_USER", value: 3)
!339 = !{!59, !241}
!340 = distinct !DICompileUnit(language: DW_LANG_C99, file: !341, producer: "clang version 4.0.0 (http://llvm.org/git/clang.git 3f7a1d0a6dedbbff7cdd804e31063c9c33a837ec) (http://llvm.org/git/llvm.git 20014804ba63d479300bb5c136856d11fbc1bdae)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !119, retainedTypes: !280)
!341 = !DIFile(filename: "/usr/home/jon/freebsd/loom/usr.bin/grep/regex/hashtable.c", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!342 = distinct !DICompileUnit(language: DW_LANG_C99, file: !343, producer: "clang version 4.0.0 (http://llvm.org/git/clang.git 3f7a1d0a6dedbbff7cdd804e31063c9c33a837ec) (http://llvm.org/git/llvm.git 20014804ba63d479300bb5c136856d11fbc1bdae)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !306, retainedTypes: !344)
!343 = !DIFile(filename: "/usr/home/jon/freebsd/loom/usr.bin/grep/regex/tre-compile.c", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!344 = !{!29, !345}
!345 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !346, size: 64, align: 64)
!346 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !48)
!347 = distinct !DICompileUnit(language: DW_LANG_C99, file: !348, producer: "clang version 4.0.0 (http://llvm.org/git/clang.git 3f7a1d0a6dedbbff7cdd804e31063c9c33a837ec) (http://llvm.org/git/llvm.git 20014804ba63d479300bb5c136856d11fbc1bdae)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !349, retainedTypes: !350)
!348 = !DIFile(filename: "/usr/home/jon/freebsd/loom/usr.bin/grep/regex/tre-fastmatch.c", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!349 = !{!307, !332}
!350 = !{!29, !59, !48, !62, !45, !345}
!351 = distinct !DICompileUnit(language: DW_LANG_C99, file: !352, producer: "clang version 4.0.0 (http://llvm.org/git/clang.git 3f7a1d0a6dedbbff7cdd804e31063c9c33a837ec) (http://llvm.org/git/llvm.git 20014804ba63d479300bb5c136856d11fbc1bdae)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !119, retainedTypes: !353, globals: !355)
!352 = !DIFile(filename: "/usr/home/jon/freebsd/loom/usr.bin/grep/regex/xmalloc.c", directory: "/home/jon/freebsd/obj/usr/home/jon/freebsd/loom/usr.bin/grep")
!353 = !{!29, !354, !132, !62}
!354 = !DIBasicType(name: "double", size: 64, align: 64, encoding: DW_ATE_float)
!355 = !{!356, !357, !358, !359, !360, !361}
!356 = distinct !DIGlobalVariable(name: "xmalloc_peak", scope: !351, file: !352, line: 41, type: !132, isLocal: true, isDefinition: true, variable: i32* @xmalloc_peak)
!357 = distinct !DIGlobalVariable(name: "xmalloc_peak_blocks", scope: !351, file: !352, line: 43, type: !132, isLocal: true, isDefinition: true, variable: i32* @xmalloc_peak_blocks)
!358 = distinct !DIGlobalVariable(name: "xmalloc_fail_after", scope: !351, file: !352, line: 45, type: !132, isLocal: true, isDefinition: true, variable: i32* @xmalloc_fail_after)
!359 = distinct !DIGlobalVariable(name: "xmalloc_current", scope: !351, file: !352, line: 42, type: !132, isLocal: true, isDefinition: true, variable: i32* @xmalloc_current)
!360 = distinct !DIGlobalVariable(name: "xmalloc_current_blocks", scope: !351, file: !352, line: 44, type: !132, isLocal: true, isDefinition: true, variable: i32* @xmalloc_current_blocks)
!361 = distinct !DIGlobalVariable(name: "xmalloc_table", scope: !351, file: !352, line: 166, type: !362, isLocal: true, isDefinition: true, variable: %struct.hashTable.20.49.107.136.194.223.252.513** @xmalloc_table)
!362 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !363, size: 64, align: 64)
!363 = !DIDerivedType(tag: DW_TAG_typedef, name: "hashTable", file: !352, line: 39, baseType: !364)
!364 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !352, line: 37, size: 64, align: 64, elements: !365)
!365 = !{!366}
!366 = !DIDerivedType(tag: DW_TAG_member, name: "table", scope: !364, file: !352, line: 38, baseType: !367, size: 64, align: 64)
!367 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !368, size: 64, align: 64)
!368 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !369, size: 64, align: 64)
!369 = !DIDerivedType(tag: DW_TAG_typedef, name: "hashTableItem", file: !352, line: 35, baseType: !370)
!370 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "hashTableItemRec", file: !352, line: 28, size: 384, align: 64, elements: !371)
!371 = !{!372, !373, !374, !375, !376, !377}
!372 = !DIDerivedType(tag: DW_TAG_member, name: "ptr", scope: !370, file: !352, line: 29, baseType: !29, size: 64, align: 64)
!373 = !DIDerivedType(tag: DW_TAG_member, name: "bytes", scope: !370, file: !352, line: 30, baseType: !132, size: 32, align: 32, offset: 64)
!374 = !DIDerivedType(tag: DW_TAG_member, name: "file", scope: !370, file: !352, line: 31, baseType: !127, size: 64, align: 64, offset: 128)
!375 = !DIDerivedType(tag: DW_TAG_member, name: "line", scope: !370, file: !352, line: 32, baseType: !132, size: 32, align: 32, offset: 192)
!376 = !DIDerivedType(tag: DW_TAG_member, name: "func", scope: !370, file: !352, line: 33, baseType: !127, size: 64, align: 64, offset: 256)
!377 = !DIDerivedType(tag: DW_TAG_member, name: "next", scope: !370, file: !352, line: 34, baseType: !378, size: 64, align: 64, offset: 320)
!378 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !370, size: 64, align: 64)
!379 = !{!"clang version 4.0.0 (http://llvm.org/git/clang.git 3f7a1d0a6dedbbff7cdd804e31063c9c33a837ec) (http://llvm.org/git/llvm.git 20014804ba63d479300bb5c136856d11fbc1bdae)"}
!380 = !{i32 2, !"Dwarf Version", i32 2}
!381 = !{i32 2, !"Debug Info Version", i32 3}
!382 = distinct !DISubprogram(name: "fastcmp", scope: !348, file: !348, line: 998, type: !383, isLocal: true, isDefinition: true, scopeLine: 999, flags: DIFlagPrototyped, isOptimized: false, unit: !347, variables: !119)
!383 = !DISubroutineType(types: !384)
!384 = !{!132, !385, !411, !413}
!385 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !386, size: 64, align: 64)
!386 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !387)
!387 = !DIDerivedType(tag: DW_TAG_typedef, name: "fastmatch_t", file: !183, line: 35, baseType: !388)
!388 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !183, line: 11, size: 9024, align: 64, elements: !389)
!389 = !{!390, !391, !392, !393, !394, !395, !396, !397, !398, !399, !400, !401, !402, !403, !404, !405, !406, !407, !408, !409, !410}
!390 = !DIDerivedType(tag: DW_TAG_member, name: "wlen", scope: !388, file: !183, line: 12, baseType: !59, size: 64, align: 64)
!391 = !DIDerivedType(tag: DW_TAG_member, name: "len", scope: !388, file: !183, line: 13, baseType: !59, size: 64, align: 64, offset: 64)
!392 = !DIDerivedType(tag: DW_TAG_member, name: "wpattern", scope: !388, file: !183, line: 14, baseType: !189, size: 64, align: 64, offset: 128)
!393 = !DIDerivedType(tag: DW_TAG_member, name: "wescmap", scope: !388, file: !183, line: 15, baseType: !194, size: 64, align: 64, offset: 192)
!394 = !DIDerivedType(tag: DW_TAG_member, name: "qsBc", scope: !388, file: !183, line: 16, baseType: !196, size: 8192, align: 32, offset: 256)
!395 = !DIDerivedType(tag: DW_TAG_member, name: "bmGs", scope: !388, file: !183, line: 17, baseType: !200, size: 64, align: 64, offset: 8448)
!396 = !DIDerivedType(tag: DW_TAG_member, name: "pattern", scope: !388, file: !183, line: 18, baseType: !122, size: 64, align: 64, offset: 8512)
!397 = !DIDerivedType(tag: DW_TAG_member, name: "escmap", scope: !388, file: !183, line: 19, baseType: !194, size: 64, align: 64, offset: 8576)
!398 = !DIDerivedType(tag: DW_TAG_member, name: "defBc", scope: !388, file: !183, line: 20, baseType: !45, size: 32, align: 32, offset: 8640)
!399 = !DIDerivedType(tag: DW_TAG_member, name: "qsBc_table", scope: !388, file: !183, line: 21, baseType: !29, size: 64, align: 64, offset: 8704)
!400 = !DIDerivedType(tag: DW_TAG_member, name: "sbmGs", scope: !388, file: !183, line: 22, baseType: !200, size: 64, align: 64, offset: 8768)
!401 = !DIDerivedType(tag: DW_TAG_member, name: "re_endp", scope: !388, file: !183, line: 23, baseType: !127, size: 64, align: 64, offset: 8832)
!402 = !DIDerivedType(tag: DW_TAG_member, name: "hasdot", scope: !388, file: !183, line: 26, baseType: !141, size: 8, align: 8, offset: 8896)
!403 = !DIDerivedType(tag: DW_TAG_member, name: "bol", scope: !388, file: !183, line: 27, baseType: !141, size: 8, align: 8, offset: 8904)
!404 = !DIDerivedType(tag: DW_TAG_member, name: "eol", scope: !388, file: !183, line: 28, baseType: !141, size: 8, align: 8, offset: 8912)
!405 = !DIDerivedType(tag: DW_TAG_member, name: "word", scope: !388, file: !183, line: 29, baseType: !141, size: 8, align: 8, offset: 8920)
!406 = !DIDerivedType(tag: DW_TAG_member, name: "icase", scope: !388, file: !183, line: 30, baseType: !141, size: 8, align: 8, offset: 8928)
!407 = !DIDerivedType(tag: DW_TAG_member, name: "newline", scope: !388, file: !183, line: 31, baseType: !141, size: 8, align: 8, offset: 8936)
!408 = !DIDerivedType(tag: DW_TAG_member, name: "nosub", scope: !388, file: !183, line: 32, baseType: !141, size: 8, align: 8, offset: 8944)
!409 = !DIDerivedType(tag: DW_TAG_member, name: "matchall", scope: !388, file: !183, line: 33, baseType: !141, size: 8, align: 8, offset: 8952)
!410 = !DIDerivedType(tag: DW_TAG_member, name: "reversed", scope: !388, file: !183, line: 34, baseType: !141, size: 8, align: 8, offset: 8960)
!411 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !412, size: 64, align: 64)
!412 = !DIDerivedType(tag: DW_TAG_const_type, baseType: null)
!413 = !DIDerivedType(tag: DW_TAG_typedef, name: "tre_str_type_t", file: !333, line: 41, baseType: !332)
