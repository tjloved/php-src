--TEST--
Generator::getReturn() success cases
--FILE--
<?php

function gen1()
{
    return 42;
    (yield 24);
}
function gen2()
{
    (yield 24);
    return 42;
}
// & for generators specifies by-reference yield, not return
// so it's okay to return a literal
function &gen3()
{
    $var = 24;
    (yield $var);
    return 42;
}
// Return types for generators specify the return of the function,
// not of the generator return value, so this code is okay
function gen4() : Generator
{
    (yield 24);
    return 42;
}
// Has no explicit return, but implicitly return NULL at the end
function gen5()
{
    (yield 24);
}
// Explicit value-less return also results in a NULL generator
// return value and there is no interference with type declarations
function gen6() : Generator
{
    return;
    (yield 24);
}
function fn371120263()
{
    $gen = gen1();
    // Calling getReturn() directly here is okay due to auto-priming
    var_dump($gen->getReturn());
    $gen = gen2();
    var_dump($gen->current());
    $gen->next();
    var_dump($gen->getReturn());
    $gen = gen3();
    var_dump($gen->current());
    $gen->next();
    var_dump($gen->getReturn());
    $gen = gen4();
    var_dump($gen->current());
    $gen->next();
    var_dump($gen->getReturn());
    $gen = gen5();
    var_dump($gen->current());
    $gen->next();
    var_dump($gen->getReturn());
    $gen = gen6();
    var_dump($gen->getReturn());
}
fn371120263();
--EXPECTF--
int(42)
int(24)
int(42)
int(24)
int(42)
int(24)
int(42)
int(24)
NULL
NULL
