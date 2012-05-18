#include <QTime>

#include "executor.h"
#include "z80/z80.h"
#include "z80/general.h"

Executor::Executor(QObject *parent) :
    QObject(parent)
{
}

void Executor::execute(MainWindow* m) {
    int i;
    z80 *z = new_z80(FREQ);
    uchar opcode;
    const double cycles_threshold = (FREQ * THRESHOLD_MS) / 1000;
    char get_time = 1;
    QTime* start_time = new QTime(0,0,0,0);
    int remaining_time = 0;
    memory *mem;

    mem = (memory *)malloc(sizeof(memory) * MEMORY);
    memset(mem, 0, MEMORY * sizeof(memory));

    start_time->start();
    z->allocated_memory = MEMORY;
    z->m = mem;

    mem[0].data = 0x0E; // LD C, 0
    mem[1].data = 0x00; // LD C, 0
    mem[2].data = 0x06; // LD B, 0x10
    mem[3].data = 0x02; // LD B, 0x10
    mem[4].data = 0x0C; // INC C
    mem[5].data = 0x10; // DJNZ -3
    mem[6].data = 0xFD; // DJNZ -3
    mem[7].data = 0x76; // HALT

    for(;;) {

        //if(z->halt != 0) {
        //    break;
        //}

        if(get_time == 1) {
            start_time->restart();
            get_time = 0;
        }

        opcode = z->fetch(z);
        if(z->decode_exec(z, opcode) == ERROR) {
            printf("Invalid opcode [%02X]. PC: [%04X]\n", opcode, z->pc-1);
        }

        // The CPU is capable of executing FREQ cycles per second
        // Every 10 milliseconds, we should delay the execuion, so it does not
        // run too fast
        // 1 second - FREQ
        // 10 millisecond - x
        //
        // x = FREQ * THRESHOLD_MS * 10^-3

        if(z->cycles >= cycles_threshold) {
            printf("i = %d\n", i++);
            while((remaining_time = (THRESHOLD_MS * 1000) - start_time->elapsed()) > 0);
            z->cycles -= cycles_threshold;
            get_time = 1;
        }

        // TODO: Check interrupts here?

        // TODO: Check reset here?

        //

        // Shwo the values in their places
        QString str;
        m->lnAccumulator->setText(str.sprintf("%02X", z->a));
        m->lnFlags->setText(str.sprintf("%02X", z->f.flags));
        m->lnB->setText(str.sprintf("%02X", z->b));
        m->lnC->setText(str.sprintf("%02X", z->c));
        m->lnD->setText(str.sprintf("%02X", z->d));
        m->lnE->setText(str.sprintf("%02X", z->e));
        m->lnH->setText(str.sprintf("%02X", z->h));
        m->lnL->setText(str.sprintf("%02X", z->l));

        m->lnAccumulator2->setText(str.sprintf("%02X", z->a2));
        m->lnFlags2->setText(str.sprintf("%02X", z->f2.flags));
        m->lnB2->setText(str.sprintf("%02X", z->b2));
        m->lnC2->setText(str.sprintf("%02X", z->c2));
        m->lnD2->setText(str.sprintf("%02X", z->d2));
        m->lnE2->setText(str.sprintf("%02X", z->e2));
        m->lnH2->setText(str.sprintf("%02X", z->h2));
        m->lnL2->setText(str.sprintf("%02X", z->l2));

        // lnFlags->setText(&z->f);

    }

    delete start_time;
    free(z);
    free(mem);
}
