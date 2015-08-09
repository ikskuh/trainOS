#pragma once

#define STUB(num) extern void intr_stub_ ## num ()

STUB(0);
STUB(1);
STUB(2);
STUB(3);
STUB(4);
STUB(5);
STUB(6);
STUB(7);
STUB(8);
STUB(9);
STUB(10);
STUB(11);
STUB(12);
STUB(13);
STUB(14);
STUB(15);
STUB(16);
STUB(17);
STUB(18);

STUB(32);
STUB(33);
STUB(34);
STUB(35);
STUB(36);
STUB(37);
STUB(38);
STUB(39);
STUB(40);
STUB(41);
STUB(42);
STUB(43);
STUB(44);
STUB(45);
STUB(46);
STUB(47);

STUB(48);

STUB(80);

#undef STUB
