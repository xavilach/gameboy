#ifndef CPU_H_
#define CPU_H_

typedef struct cpu_s cpu_t;

int cpu_init(void);

int cpu_run(cpu_t *p_cpu);

#endif /*CPU_H_*/
