// Multi-threaded memory bandwidth test (STREAM-like, work-stealing so slow
// efficiency cores don't gate the result).
// Build: clang -O3 -o membw membw.c -lpthread
// Run:   ./membw [extra_thread_count] [buffer_MB]   (defaults: P-cores and all cores, 1024 MB)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sysctl.h>
#include <mach/mach_time.h>

#define MB (1024ull*1024ull)
#define CHUNK (16ull*MB)
static size_t total = 1024ull*MB;   // per buffer; two buffers allocated
static int iters = 6;

static uint64_t *A, *B; static size_t nchunks;
static atomic_size_t next_chunk; static atomic_uint_fast64_t sink; static int op;

static void *worker(void *unused) {
    (void)unused; uint64_t s = 0; size_t c;
    while ((c = atomic_fetch_add(&next_chunk, 1)) < nchunks) {
        uint64_t *a = A + c*(CHUNK/8), *b = B + c*(CHUNK/8); size_t n = CHUNK/8;
        if (op == 0) { uint64_t s0=0,s1=0,s2=0,s3=0;
            for (size_t i = 0; i < n; i += 4) { s0+=a[i]; s1+=a[i+1]; s2+=a[i+2]; s3+=a[i+3]; }
            s += s0+s1+s2+s3; }
        else if (op == 1) memset(a, (int)c, CHUNK);
        else memcpy(b, a, CHUNK);
    }
    atomic_fetch_add(&sink, s); return NULL;
}
static double now(void) { static mach_timebase_info_data_t tb; if (!tb.denom) mach_timebase_info(&tb);
    return mach_absolute_time() * (double)tb.numer / tb.denom / 1e9; }

static double run(int nt, int o) {
    pthread_t th[256]; double t0 = now(); op = o;
    for (int it = 0; it < iters; it++) {
        atomic_store(&next_chunk, 0);
        for (int i = 0; i < nt; i++) pthread_create(&th[i], NULL, worker, NULL);
        for (int i = 0; i < nt; i++) pthread_join(th[i], NULL);
    }
    double bytes = (double)total * iters * (o == 2 ? 2 : 1);
    return bytes / (now() - t0) / 1e9;
}

int main(int argc, char **argv) {
    int ncpu = (int)sysconf(_SC_NPROCESSORS_ONLN), pcores = 0; size_t sz = sizeof(pcores);
    sysctlbyname("hw.perflevel0.physicalcpu", &pcores, &sz, NULL, 0);
    if (argc > 2) total = (size_t)atoll(argv[2]) * MB;
    nchunks = total / CHUNK; total = nchunks * CHUNK;
    A = malloc(total); B = malloc(total);
    if (!A || !B) { fprintf(stderr, "alloc failed\n"); return 1; }
    memset(A, 1, total); memset(B, 2, total);
    int counts[3] = { pcores ? pcores : ncpu, ncpu, argc > 1 ? atoi(argv[1]) : 0 }; int nc = argc > 1 ? 3 : 2;
    if (counts[0] == counts[1]) nc--, counts[1] = counts[2];
    printf("buffers=2 x %llu MB  iters=%d  (P-cores=%d, all cores=%d)\n\n", (unsigned long long)(total/MB), iters, pcores, ncpu);
    printf("%-8s %-14s %-14s %-14s\n", "threads", "READ GB/s", "WRITE GB/s", "COPY GB/s");
    double peak = 0;
    for (int k = 0; k < nc; k++) { int nt = counts[k]; if (nt <= 0) continue;
        run(nt, 0); double best[3] = {0};
        for (int rep = 0; rep < 3; rep++) for (int o = 0; o < 3; o++) { double bw = run(nt, o); if (bw > best[o]) best[o] = bw; if (bw > peak) peak = bw; }
        printf("%-8d %-14.1f %-14.1f %-14.1f\n", nt, best[0], best[1], best[2]); }
    printf("\nPeak measured: %.0f GB/s. CPU-side tests typically reach 55-75%% of the chip's rated bandwidth.\n", peak);
    printf("COPY counts bytes read + written. Best of 3 per cell.\n");
    return 0 + (int)(atomic_load(&sink) & 0);
}
